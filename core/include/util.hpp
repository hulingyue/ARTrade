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
    std::string log_path;

    Arguments() : log_path("./") {}
};

inline int cli_parse(const std::string describe, Arguments &result, int argc, char** argv) {
    CLI::App app {describe};
    argv = app.ensure_utf8(argv);
    app.add_option("-f,--file", result.log_path, "log/ path")->required();

    CLI11_PARSE(app, argc, argv);
}

} // namespace core::util
