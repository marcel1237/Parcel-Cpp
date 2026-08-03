#include "PersistenceService.hpp"
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>

using namespace Parcel::Service;

void PersistenceService::saveLastProject(const std::string& path) {
    auto props = loadProperties();
    props["last.project.path"] = path;
    saveProperties(props);
}

std::string PersistenceService::getLastProject() {
    auto props = loadProperties();
    return props["last.project.path"];
}

void PersistenceService::saveOpenFiles(const std::vector<std::string>& filePaths) {
    auto props = loadProperties();
    std::string files;
    for (size_t i = 0; i < filePaths.size(); ++i) {
        files += filePaths[i];
        if (i < filePaths.size() - 1) files += ",";
    }
    props["open.files"] = files;
    saveProperties(props);
}

std::vector<std::string> PersistenceService::getOpenFiles() {
    auto props = loadProperties();
    std::string files = props["open.files"];
    std::vector<std::string> result;
    if (files.empty()) return result;

    std::stringstream ss(files);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) result.push_back(item);
    }
    return result;
}

void PersistenceService::incrementUsage() {
    auto props = loadProperties();

    std::string dKey = "usage.d." + getCurrentDate();
    std::string wKey = "usage.w." + getCurrentWeek();
    std::string mKey = "usage.m." + getCurrentMonth();
    std::string yKey = "usage.y." + getCurrentYear();

    props[dKey] = std::to_string(std::stoi(props[dKey].empty() ? "0" : props[dKey]) + 1);
    props[wKey] = std::to_string(std::stoi(props[wKey].empty() ? "0" : props[wKey]) + 1);
    props[mKey] = std::to_string(std::stoi(props[mKey].empty() ? "0" : props[mKey]) + 1);
    props[yKey] = std::to_string(std::stoi(props[yKey].empty() ? "0" : props[yKey]) + 1);

    saveProperties(props);
}

UsageStats PersistenceService::getUsageStats() {
    auto props = loadProperties();
    UsageStats stats;

    std::string dKey = "usage.d." + getCurrentDate();
    std::string wKey = "usage.w." + getCurrentWeek();
    std::string mKey = "usage.m." + getCurrentMonth();
    std::string yKey = "usage.y." + getCurrentYear();

    stats.daily = std::stoi(props[dKey].empty() ? "0" : props[dKey]);
    stats.weekly = std::stoi(props[wKey].empty() ? "0" : props[wKey]);
    stats.monthly = std::stoi(props[mKey].empty() ? "0" : props[mKey]);
    stats.yearly = std::stoi(props[yKey].empty() ? "0" : props[yKey]);

    return stats;
}

void PersistenceService::saveGeminiApiKey(const std::string& key) {
    auto props = loadProperties();
    props["gemini.api.key"] = encrypt(key);
    saveProperties(props);
}

std::string PersistenceService::getGeminiApiKey() {
    auto props = loadProperties();
    return decrypt(props["gemini.api.key"]);
}

std::string PersistenceService::encrypt(const std::string& data) {
    if (data.empty()) return "";
    std::string output;
    for (char c : data) {
        unsigned char encrypted = c ^ 0xAF;
        std::stringstream ss;
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)encrypted;
        output += ss.str();
    }
    return output;
}

std::string PersistenceService::decrypt(const std::string& data) {
    if (data.empty()) return "";
    std::string output;
    for (size_t i = 0; i < data.length(); i += 2) {
        std::string part = data.substr(i, 2);
        unsigned char ch = (unsigned char)std::stoul(part, nullptr, 16);
        output += (char)(ch ^ 0xAF);
    }
    return output;
}

std::string PersistenceService::getCurrentDate() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d");
    return ss.str();
}

std::string PersistenceService::getCurrentWeek() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%W");
    return ss.str();
}

std::string PersistenceService::getCurrentMonth() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m");
    return ss.str();
}

std::string PersistenceService::getCurrentYear() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y");
    return ss.str();
}

std::filesystem::path PersistenceService::getConfigPath() {
    const char* home = getenv("HOME");
    std::filesystem::path configDir;
    if (home) {
        configDir = std::filesystem::path(home) / ".config" / "parcelcpp";
    } else {
        configDir = ".config/parcelcpp";
    }
    std::filesystem::create_directories(configDir);
    return configDir / "settings.properties";
}

std::map<std::string, std::string> PersistenceService::loadProperties() {
    std::map<std::string, std::string> props;
    std::ifstream file(getConfigPath());
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            props[line.substr(0, pos)] = line.substr(pos + 1);
        }
    }
    return props;
}

void PersistenceService::saveProperties(const std::map<std::string, std::string>& props) {
    std::ofstream file(getConfigPath());
    for (const auto& [key, value] : props) {
        file << key << "=" << value << "\n";
    }
}
