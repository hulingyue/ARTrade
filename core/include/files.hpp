#pragma once
#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>

namespace core {
namespace Private {

bool create_folder(const std::string path, const std::string name) {
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

} // namespace Private
} // namespace core
