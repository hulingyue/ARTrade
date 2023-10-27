#pragma once
#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>

namespace core::util {

inline bool create_folder(const std::string path, const std::string name) {
    auto folder_path = std::filesystem::path(path) / std::filesystem::path(name);
    try {
        std::filesystem::create_directory(folder_path);
        spdlog::info("create directory  - success!");
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        spdlog::error("create direcroty - failure! msg: {}", e.what());
    }
    return false;
}

struct Arguments {
    std::string config_path;
    std::string log_path;

    Arguments() : config_path("./"), log_path("") {}
};

extern core::util::Arguments arguments;

inline int cli_parse(const std::string describe, int argc, char** argv) {
    CLI::App app {describe};
    argv = app.ensure_utf8(argv);
    app.add_option("-c,--config", arguments.config_path, "config/ path")->required();
    app.add_option("-l,--log", arguments.log_path, "log/ path");

    CLI11_PARSE(app, argc, argv);

    if (arguments.log_path.empty()) {
        arguments.log_path = arguments.config_path;
    }
    return 0;
}

} // namespace core::util
