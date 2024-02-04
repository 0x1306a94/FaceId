
#include "spdlog_common.hpp"

#include "config.hpp"

#include "../storage/src/Storage.hpp"
#include "../storage/src/application.hpp"

#include <face_id_cli/version.hpp>

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

#include <argparse/argparse.hpp>

#include <memory>

int main(int argc, char *argv[]) {

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

    argparse::ArgumentParser app_command("app", FACE_ID_CLI_VERSION, argparse::default_arguments::help);
    app_command.add_description("application management.");

    program.add_subparser(app_command);

    argparse::ArgumentParser app_add_command("add", FACE_ID_CLI_VERSION, argparse::default_arguments::help);
    app_add_command.add_argument("--appid")
        .help("application id.")
        .required();

    app_add_command.add_argument("--name")
        .help("application name.")
        .required();

    app_command.add_subparser(app_add_command);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << program << std::endl;
        SPDLOG_ERROR("{}", err.what());
        return EXIT_FAILURE;
    }

    auto conf = program.get<std::string>("--config");

    YAML::Node node = YAML::LoadFile(conf);
    face::cli::MainConfig config;
    YAML::convert<face::cli::MainConfig>::decode(node, config);
    //    std::cerr << node << std::endl;

    std::shared_ptr<face::storage::Storage> storage = nullptr;
    if (!config.storage.data_dir.empty()) {
        storage = std::make_shared<face::storage::Storage>(config.storage);
    }

    if (program.is_subcommand_used(app_command) && app_command.is_subcommand_used(app_add_command)) {
        if (!storage) {
            SPDLOG_ERROR("storage setup fail");
            return EXIT_SUCCESS;
        }
        auto appid = app_add_command.get<std::string>("--appid");
        auto name = app_add_command.get<std::string>("--name");
        auto result = storage->AddApplication(appid, name);
        if (result.is_ok()) {
            auto appInfo = result.take_ok_value();
            SPDLOG_INFO("add succes identifier: {}", appInfo.identifier);
        } else {
            auto msg = result.take_err_value();
            SPDLOG_ERROR("add fail msg: {}", msg);
        }
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}
