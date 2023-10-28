#pragma once
#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>

#define LOGHEAD "[util" + std::string(__func__) + "]"

namespace {
    void set_log_level(const std::string_view level);
    int cli_parse(const std::string describe, int argc, char** argv);
}

namespace core::util {

inline bool create_folder(std::filesystem::path folder_path) {
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

extern Arguments arguments;

inline void startup(const std::string describe, int argc, char** argv, std::string_view log_level = "info", size_t log_size = 5 * 1024 * 1024, size_t log_files = 10) {
    // log
    set_log_level(log_level);

    // startup command
    int cli_parse_code = cli_parse(std::move(describe), argc, argv);
    if (cli_parse_code != 0) {
        spdlog::error("{} Parameter parsing failed!", LOGHEAD);
        exit(-1);
    }
}

} // namespace core::util


namespace {

using namespace core::util;

inline void set_log_level(const std::string_view level) {
    if (level == "trace") { spdlog::set_level(spdlog::level::trace); }
    else if (level == "debug") { spdlog::set_level(spdlog::level::debug); }
    else if (level == "info") { spdlog::set_level(spdlog::level::info); }
    else if (level == "warn") { spdlog::set_level(spdlog::level::warn); }
    else if (level == "err") { spdlog::set_level(spdlog::level::err); }
    else if (level == "critical") { spdlog::set_level(spdlog::level::critical); }
    else if (level == "off") { spdlog::set_level(spdlog::level::off); }
    else { spdlog::set_level(spdlog::level::info); }
}

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

} // namespace


#undef LOGHEAD
