
#include "spdlog_common.hpp"

#include "config.hpp"

#include "../storage/src/Storage.hpp"
#include "../storage/src/application.hpp"

#include <face_id_cli/version.hpp>

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

#include <memory>
#include <strstream>

int main(int argc, char *argv[]) {
    // Disabling Internal Logging
    spdlog::set_level(spdlog::level::off);

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
    app_list_command.add_argument("--count")
        .default_value<std::int64_t>(10);

    program.add_subparser(app_add_command);
    program.add_subparser(app_list_command);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << program << std::endl;
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    auto conf = program.get<std::string>("--config");

    YAML::Node node = YAML::LoadFile(conf);
    face::cli::MainConfig config;
    YAML::convert<face::cli::MainConfig>::decode(node, config);

    std::shared_ptr<face::storage::Storage> storage = nullptr;
    if (!config.storage.data_dir.empty()) {
        storage = std::make_shared<face::storage::Storage>(config.storage);
    }

    if (program.is_subcommand_used(app_add_command)) {
        if (!storage) {
            std::cerr << "storage setup fail" << std::endl;
            return EXIT_SUCCESS;
        }
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
            std::cerr << "add fail msg: " << msg << std::endl;
        }
        return EXIT_SUCCESS;
    }

    if (program.is_subcommand_used(app_list_command)) {
        if (!storage) {
            std::cerr << "storage setup fail" << std::endl;
            return EXIT_SUCCESS;
        }

        std::int64_t count = app_list_command.get<std::int64_t>("--count");
        auto apps = storage->Applocations(count);
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
    return EXIT_SUCCESS;
}
