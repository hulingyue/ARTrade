#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>


namespace core::config {

class Config final {
public:
    static void init(nlohmann::json config);

    static bool read(std::string file_path);

    static bool save();

    static nlohmann::json& get();

private:
    Config() = delete;
    ~Config() = delete;
    Config(const Config&) = delete;

private:
    static nlohmann::json config;
    static std::string file_path;
};

} // namespace core::config
