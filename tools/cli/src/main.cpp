
#include "spdlog_common.hpp"

#include "config.hpp"

#include "../storage/src/Storage.hpp"
#include "../storage/src/application.hpp"

#include "../common/src/csv.hpp"

#include <face_id_cli/version.hpp>

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

#include <memory>
#include <strstream>
#include <vector>

std::vector<float> GenTestFeature(void) {
    std::vector<float> result;
    for (int i = 0; i < 1024; i++) {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.01;
        result.emplace_back(r);
    }
    return result;
}

int main(int argc, char *argv[]) {
    // Disabling Internal Logging
    //    spdlog::set_level(spdlog::level::off);

    auto printVersionFunc = [](const auto & /*unused*/) {
        std::cout << "verison: " << FACE_ID_CLI_VERSION << "\n"
                  << "git hash: " << FACE_ID_CLI_GIT_HASH << "\n"
                  << "git branch: " << FACE_ID_CLI_GIT_BRANCH << "\n";
        std::exit(EXIT_SUCCESS);
    };

    argparse::ArgumentParser program("face_id_cli", FACE_ID_CLI_VERSION, argparse::default_arguments::help);

    program.add_argument("-c", "--config")
        .required()
        .help("config file.");

    int verbosity = -1;
    program.add_argument("-V", "--verbose")
        .action([&](const auto &) { ++verbosity; })
        .append()
        .default_value(false)
        .implicit_value(true)
        .nargs(0);

    program.add_argument("-v", "--version")
        .action(printVersionFunc)
        .default_value(false)
        .help("prints version information and exits")
        .implicit_value(true)
        .nargs(0);

    argparse::ArgumentParser app_add_command("app_add", FACE_ID_CLI_VERSION, argparse::default_arguments::help);
    app_add_command.add_description("add application");
    app_add_command.add_argument("--appid")
        .help("application id.")
        .required();

    app_add_command.add_argument("--name")
        .help("application name.")
        .required();

    argparse::ArgumentParser app_list_command("app_list", FACE_ID_CLI_VERSION, argparse::default_arguments::help);
    app_list_command.add_description("list application");
    app_list_command.add_argument("--count")
        .scan<'i', std::int64_t>()
        .default_value<std::int64_t>(10);

    program.add_subparser(app_add_command);
    program.add_subparser(app_list_command);

    argparse::ArgumentParser gen_test_command("gen_test", FACE_ID_CLI_VERSION, argparse::default_arguments::help);
    gen_test_command.add_description("generate test data");
    gen_test_command.add_argument("--appid")
        .help("application id.")
        .required();

    gen_test_command.add_argument("--start")
        .help("specifies the start index of the user ID")
        .scan<'i', std::int64_t>()
        .required();

    gen_test_command.add_argument("--count")
        .help("generate the amount of test data")
        .scan<'i', std::int64_t>()
        .required();

    program.add_subparser(gen_test_command);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << program << std::endl;
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (verbosity < 0) {
        spdlog::set_level(spdlog::level::err);
    } else {
        spdlog::set_level(static_cast<spdlog::level::level_enum>(verbosity));
    }

    auto conf = program.get<std::string>("--config");

    YAML::Node node = YAML::LoadFile(conf);
    face::cli::MainConfig config;
    YAML::convert<face::cli::MainConfig>::decode(node, config);

    std::shared_ptr<face::storage::Storage> storage = nullptr;
    if (!config.storage.data_dir.empty()) {
        storage = std::make_shared<face::storage::Storage>(config.storage);
    }

    if (!storage) {
        SPDLOG_ERROR("storage setup fail");
        return EXIT_FAILURE;
    }

    //
    //    do {
    //        std::ifstream file("/Users/king/WorkSpace/dev_face.csv");
    //        for (auto &row : face::common::CSVRange(file)) {
    //
    //            std::cout << "size: " << row.size() << "\n"
    //                      << "url: " << row[0] << "\n"
    //                      << "userid: " << row[1] << "\n\n";
    //        }
    //
    //        return EXIT_SUCCESS;
    //    } while (0);

    if (program.is_subcommand_used(app_add_command)) {

        auto appid = app_add_command.get<std::string>("--appid");
        auto name = app_add_command.get<std::string>("--name");
        auto result = storage->AddApplication(appid, name);
        if (result.is_ok()) {
            auto appInfo = result.take_ok_value();
            nlohmann::json json{
                {"appId", appInfo.appId},
                {"name", appInfo.name},
                {"createDate", appInfo.createDate},
                {"updateDate", appInfo.updateDate},
            };

            std::cout << json.dump(4) << std::endl;
        } else {
            auto msg = result.take_err_value();
            SPDLOG_ERROR("add fail msg: {}", msg);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (program.is_subcommand_used(app_list_command)) {

        std::int64_t count = app_list_command.get<std::int64_t>("--count");
        auto apps = storage->GetApplications(count);
        nlohmann::json json_list;
        for (const auto &app : apps) {
            json_list.emplace_back(nlohmann::json{
                {"appId", app.appId},
                {"name", app.name},
                {"createDate", app.createDate},
                {"updateDate", app.updateDate},
            });
        }

        std::cout << json_list.dump(4) << std::endl;
    }

    if (program.is_subcommand_used(gen_test_command)) {
        auto appid = gen_test_command.get<std::string>("--appid");
        auto start = gen_test_command.get<std::int64_t>("--start");
        auto count = gen_test_command.get<std::int64_t>("--count");

        auto existApp = storage->GetApplication(appid);
        if (!existApp) {
            SPDLOG_ERROR("corresponding appid application does not exist: {}", appid);
            return EXIT_FAILURE;
        }
        for (size_t idx = 0; idx < count; idx++) {
            auto feature = GenTestFeature();
            auto userid = std::to_string(start + idx);
            auto added = storage->AddFaceRecord(appid, userid, "", feature);
            if (!added) {
                SPDLOG_ERROR("add failure userid: {}", userid);
                return EXIT_FAILURE;
            }
        }
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}
