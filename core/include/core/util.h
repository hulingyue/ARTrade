#pragma once
#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <CLI/CLI.hpp>

#define LOGHEAD "[util" + std::string(__func__) + "]"

namespace {
    void set_log_level(const std::string_view level);
    int cli_parse(const std::string describe, int argc, char** argv);
}

namespace core::util {

std::filesystem::path log_path();
std::filesystem::path config_path();

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
    std::string root_path;

    Arguments() : root_path("./") {}
};

extern Arguments arguments;

inline void startup(const std::string describe, int argc, char** argv, std::string_view log_level = "info", size_t log_size = 5 * 1024 * 1024, size_t log_files = 10) {
    // startup command
    int cli_parse_code = cli_parse(std::move(describe), argc, argv);
    if (cli_parse_code != 0) {
        spdlog::error("{} Parameter parsing failed!", LOGHEAD);
        exit(-1);
    }

    // config
    core::util::create_folder(config_path());

    // log
    std::filesystem::path log_file_path = log_path() / std::filesystem::path("log.txt");
    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file_path, log_size, log_files);
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("file_logger", spdlog::sinks_init_list{fileSink, consoleSink});
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    set_log_level(log_level);
}

inline std::filesystem::path log_path() {
    return std::move(std::filesystem::path(arguments.root_path) / std::filesystem::path("log"));
}

inline std::filesystem::path config_path() {
    return std::move(std::filesystem::path(arguments.root_path) / std::filesystem::path("config"));
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
    app.add_option("-r,--root", arguments.root_path, "root path")->required();

    CLI11_PARSE(app, argc, argv);

    return 0;
}

} // namespace


#undef LOGHEAD
