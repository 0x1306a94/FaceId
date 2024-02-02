
#include "config.hpp"
#include "router.hpp"
#include "server_context.hpp"

#include <hv/HttpServer.h>
#include <hv/hlog.h>

#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <string_view>

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "spdlog_common.hpp"

#include <filesystem>

namespace fs = std::filesystem;

const char *ENV_DISABLE_CONSOLE_LOG_NAME = "DISABLE_CONSOLE_LOG";

void setup_log(const std::string &log_dir) {
    if (log_dir.empty()) {
        spdlog::set_level(spdlog::level::off);
        return;
    }

    // std::string log_dir = "/var/log/seeta_face_feature_server";
    auto path = fs::path(log_dir);
    bool exists = fs::exists(path);
    if (!exists) {
        if (!fs::create_directories(path)) {
            std::cerr << "create_directories fail: " << log_dir << std::endl;
            exit(EXIT_FAILURE);
            return;
        }
    }

    /// 单个日志文件最大 16M
    size_t max_size = 1024 * 1024 * 16;
    size_t max_files = 10;

    auto log_path = path.append("log.log");
    auto str_path = log_path.string();

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(str_path, max_size, max_files);
    file_sink->set_level(spdlog::level::info);

    std::vector<spdlog::sink_ptr> m_sinks{{file_sink}};
    const char *tmp = std::getenv(ENV_DISABLE_CONSOLE_LOG_NAME);
    std::string env_var(tmp ? tmp : "");
    if (env_var.empty()) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        m_sinks.push_back(console_sink);
    }

    auto logger = std::make_shared<spdlog::logger>("multi_sink", m_sinks.begin(), m_sinks.end());
    logger->flush_on(spdlog::level::info);

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}

void libhv_log_handler(int loglevel, const char *buf, int len) {
    if (len == 0) {
        return;
    }
    std::string_view msg(buf, len);
    if (buf[len - 1] == '\n') {
        msg = std::string_view(buf, len - 1);
    }
    switch (loglevel) {
        case LOG_LEVEL_DEBUG: {
            SPDLOG_DEBUG("{}", msg);
            break;
        }
        case LOG_LEVEL_INFO: {
            SPDLOG_INFO("{}", msg);
            break;
        }
        case LOG_LEVEL_WARN: {
            SPDLOG_WARN("{}", msg);
            break;
        }
        case LOG_LEVEL_ERROR: {
            SPDLOG_ERROR("{}", msg);
            break;
        }
        case LOG_LEVEL_FATAL: {
            SPDLOG_CRITICAL("{}", msg);
            break;
        }
    }
}

static hv::HttpServer *server = nullptr;
void server_on_exit(void) {
    SPDLOG_INFO("server_on_exit");
    spdlog::drop_all();
    spdlog::shutdown();
}

void signal_exit_handler(int sig) {
    SPDLOG_INFO("signal_exit_handler");
    server->stop();
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "USAGE:\n"
                  << argv[0]
                  << " <config.yaml> or --version or -v"
                  << std::endl;

        return EXIT_FAILURE;
    }

    //    if ((strcmp(argv[1], "--version") == 0) || (strcmp(argv[1], "-v") == 0)) {
    //        std::cout << "verison: " << FACE_FEATURE_SERVER_VERSION << "\n"
    //                  << "git hash: " << FACE_FEATURE_SERVER_GIT_HASH << "\n"
    //                  << "git branch: " << FACE_FEATURE_SERVER_GIT_BRANCH << "\n";
    //        return EXIT_SUCCESS;
    //    }

    YAML::Node node = YAML::LoadFile(argv[1]);
    face::server::MainConfig config;
    YAML::convert<face::server::MainConfig>::decode(node, config);

    std::cout << node << std::endl;
    face::server::Context::Init(config);

    const auto &serverConf = config.server;
    const auto &recognizerConf = config.recognizer;

    setup_log(serverConf.http_log);

    atexit(server_on_exit);
    signal(SIGINT, signal_exit_handler);
    signal(SIGKILL, signal_exit_handler);
    signal(SIGTERM, signal_exit_handler);
    // std::cout << config << std::endl;

    //    SPDLOG_INFO("verison: {} git hash: {} git branch: {}", FACE_FEATURE_SERVER_VERSION, FACE_FEATURE_SERVER_GIT_HASH, FACE_FEATURE_SERVER_GIT_BRANCH);
    SPDLOG_INFO("engine_pool_szie: {} http_thread_num: {} http_port: {} http_log: {} face_model_dir: {}", recognizerConf.engine_pool_szie, serverConf.http_thread_num, serverConf.http_port, serverConf.http_log, recognizerConf.face_model_dir);

    hv::HttpService service;
    face::server::Router::Register(service);
    // hlog_disable();
    // if (config.http_log.empty()) {
    //     hlog_disable();
    // } else {
    //     hlog_set_file(config.http_log.c_str());
    // }

    hlog_set_handler(libhv_log_handler);

    SPDLOG_INFO("start http server 0.0.0.0: {}", serverConf.http_port);

    server = new hv::HttpServer();

    server->registerHttpService(&service);
    server->setPort(serverConf.http_port);
    server->setThreadNum(static_cast<int>(serverConf.http_thread_num));
    server->run();

    SPDLOG_INFO("stop http server 0.0.0.0: {}", serverConf.http_port);
    return EXIT_SUCCESS;
}
