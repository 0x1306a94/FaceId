
#include "../common/src/AutoBuffer.hpp"
#include "../common/src/base64.hpp"
#include "FeatureEngine.hpp"

#include "spdlog_common.hpp"

#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>
#include <seeta/GenderPredictor.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <chrono>
#include <fstream>
#include <iostream>

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

namespace face {
namespace recognizer {
using std::chrono::duration_cast;
using std::chrono::milliseconds;

class FeatureEngine::Impl {
  public:
    std::shared_ptr<seeta::FaceDetector> FD;
    std::shared_ptr<seeta::FaceLandmarker> FL;
    std::shared_ptr<seeta::FaceRecognizer> FR;
    // std::shared_ptr<seeta::GenderPredictor> GP;

    Impl(const std::string &model_dir) {
        seeta::ModelSetting fd_setting;
        fd_setting.set_device(SEETA_DEVICE_AUTO);
        fd_setting.append(model_dir + "/face_detector.csta");
        FD.reset(new seeta::FaceDetector(fd_setting));
        // 表示人脸检测器可以检测到的最小人脸，该值越小，支持检测到的人脸尺寸越小，检测速度越慢，默认值为20
        FD->set(seeta::FaceDetector::Property::PROPERTY_MIN_FACE_SIZE, 80);
        // 表示人脸检测器过滤阈值，默认为 0.90
        FD->set(seeta::FaceDetector::Property::PROPERTY_THRESHOLD, 0.80);
        // 分别表示支持输入的图像的最大宽度和高
        // FD->set(seeta::FaceDetector::Property::PROPERTY_MAX_IMAGE_WIDTH, 900);
        // FD->set(seeta::FaceDetector::Property::PROPERTY_MAX_IMAGE_HEIGHT, 600);
        // 表示人脸检测器计算线程数，默认为 4
        FD->set(seeta::FaceDetector::Property::PROPERTY_NUMBER_THREADS, 4);

        seeta::ModelSetting fl_setting;
        fl_setting.set_device(SEETA_DEVICE_AUTO);
        fl_setting.append(model_dir + "/face_landmarker_pts5.csta");
        FL.reset(new seeta::FaceLandmarker(fl_setting));

        seeta::ModelSetting fr_setting;
        fr_setting.set_id(0);
        fr_setting.append(model_dir + "/face_recognizer.csta");
        fr_setting.set_device(SEETA_DEVICE_AUTO);
        FR.reset(new seeta::FaceRecognizer(fr_setting));
        // 表示计算线程数，默认为 4
        FR->set(seeta::FaceRecognizer::Property::PROPERTY_NUMBER_THREADS, 4);

        // seeta::ModelSetting gp_setting;
        // gp_setting.set_id(0);
        // gp_setting.append(model_dir + "/gender_predictor.csta");
        // gp_setting.set_device(SEETA_DEVICE_AUTO);
        // GP.reset(new seeta::GenderPredictor(gp_setting));
        // GP->set(seeta::GenderPredictor::Property::PROPERTY_NUMBER_THREADS, 4);
    }

    face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> ExtractFeature(const cv::Mat &img) {
        SeetaImageData simg;
        simg.width = img.cols;
        simg.height = img.rows;
        simg.channels = img.channels();
        simg.data = img.data;

        SeetaFaceInfoArray _faces = FD->detect(simg);

        SPDLOG_TRACE("faces size {}", _faces.size);
        if (_faces.size <= 0) {
            return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::NoFace);
        }

        // if (faces.size > 1) {
        //     return face::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::MultiFace);
        // }

        // 支持多张人脸时取最大人脸
        auto faces = std::vector<SeetaFaceInfo>(_faces.data, _faces.data + _faces.size);
        std::partial_sort(faces.begin(), faces.begin() + 1, faces.end(), [](SeetaFaceInfo a, SeetaFaceInfo b) {
            return a.pos.width > b.pos.width;
        });

        int size = FR->GetExtractFeatureSize();
        std::vector<float> feature(size);

        SeetaPointF points[5];
        FL->mark(simg, faces[0].pos, points);
#if 0
        seeta::ImageData face = FR->CropFaceV2(simg, &points[0]);
        if(FR->ExtractCroppedFace(face, feature.data())) {
            return face::Ok<FeatureEngine::Result>(FeatureEngine::Result{feature});
        }
#else
        if (FR->Extract(simg, points, feature.data())) {
            return face::common::Ok<FeatureEngine::Result>(FeatureEngine::Result{feature});
        }
#endif

        return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::UnableFeature);
    }
};

FeatureEngine::FeatureEngine(const std::string &model_dir)
    : impl(std::make_unique<Impl>(model_dir)) {

    SPDLOG_TRACE("Create FR engine");
}

FeatureEngine::~FeatureEngine() {
    SPDLOG_TRACE("Destroy FR engine");
}

face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> FeatureEngine::ExtractFeatureFromBase64(const std::string &base64_str) const {
    spdlog::stopwatch sw;
    std::string decode_str = common::base64::from_base64(base64_str);
    std::vector<uchar> data(decode_str.begin(), decode_str.end());
    try {

        cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
        SPDLOG_TRACE("从base64加载图片 elapsed: {}", duration_cast<milliseconds>(sw.elapsed()));

        if (img.empty()) {
            return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::ImageFail);
        }

        if (!img.isContinuous()) {
            img = img.clone();
        }

        return impl->ExtractFeature(img);
    } catch (std::exception &e) {
        SPDLOG_ERROR("exception: {}", e.what());
        return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::ImageFail);
    }
}

face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> FeatureEngine::ExtractFeatureFromData(const std::string &source) const {
    try {
        spdlog::stopwatch sw;
        std::vector<uchar> data(source.begin(), source.end());
        cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
        SPDLOG_TRACE("从内存加载图片 elapsed: {}", duration_cast<milliseconds>(sw.elapsed()));

        if (img.empty()) {
            return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::ImageFail);
        }

        if (!img.isContinuous()) {
            img = img.clone();
        }
        return impl->ExtractFeature(img);
    } catch (std::exception &e) {
        SPDLOG_ERROR("exception: {}", e.what());
        return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::ImageFail);
    }
}

face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> FeatureEngine::ExtractFeatureFromData(const common::AutoBuffer &buffer) const {
    try {
        spdlog::stopwatch sw;
        std::vector<uchar> data(buffer.data(), buffer.data() + buffer.size());
        cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
        SPDLOG_TRACE("从内存加载图片 size {} elapsed: {}", buffer.size(), duration_cast<milliseconds>(sw.elapsed()));

        if (img.empty()) {
            return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::ImageFail);
        }

        if (!img.isContinuous()) {
            img = img.clone();
        }
        return impl->ExtractFeature(img);
    } catch (std::exception &e) {
        SPDLOG_ERROR("exception: {}", e.what());
        return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::ImageFail);
    }
}

face::common::Result<FeatureEngine::Result, FeatureEngine::ErrorCode> FeatureEngine::ExtractFeature(const std::string &image_path) const {
    try {
        spdlog::stopwatch sw;
        cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
        SPDLOG_TRACE("从磁盘加载图片 elapsed: {}", duration_cast<milliseconds>(sw.elapsed()));

        if (img.empty()) {
            return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::ImageFail);
        }

        if (!img.isContinuous()) {
            img = img.clone();
        }
        return impl->ExtractFeature(img);
    } catch (std::exception &e) {
        SPDLOG_ERROR("exception: {}", e.what());
        return face::common::Err<FeatureEngine::ErrorCode>(FeatureEngine::ErrorCode::ImageFail);
    }
}

}  // namespace recognizer
}  // namespace face
