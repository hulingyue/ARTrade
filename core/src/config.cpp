#include "core/config.h"


#define LOGHEAD "[Config::" + std::string(__func__) + "]"

namespace core::config {

// static init
nlohmann::json Config::config;
std::string Config::file_path;

void Config::init(nlohmann::json config) {
    Config::config = config;
}

bool Config::read(std::string file_path) {
    Config::file_path = file_path;
        
    std::ifstream file(file_path);
    if (file.is_open()) {
        // 获取文件长度
        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // 创建字符串并读取文件内容
        std::string content(fileSize, ' ');
        file.read(&content[0], fileSize);
        file.close();

        // 字符串反序列化
        try {
            Config::config = nlohmann::json::parse(content); 
        } catch (const nlohmann::json::parse_error& e) {
            spdlog::error("{} parse json string failure! error msg: {}", LOGHEAD, e.what());
            Config::save();
            return false;
        }
        
        return true;
    }
    
    spdlog::error("{} cannot open {}", LOGHEAD, Config::file_path);
    Config::save();
    return false;
}

bool Config::save() {
    std::ofstream file(Config::file_path);

    if (file.is_open()) {
        file << Config::config.dump(4);
        file.close();
        spdlog::info("{} save config success!", LOGHEAD);
        return true;
    }

    spdlog::error("{} cannot open {}", LOGHEAD, Config::file_path);
    return false;
}

nlohmann::json& Config::get() {
    return Config::config;
}

} // namespace core::config

#undef LOGHEAD