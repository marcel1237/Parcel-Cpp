#include "FileSystemService.hpp"
#include <fstream>
#include <filesystem>

using namespace Parcel::Service;

void FileSystemService::saveFile(const std::string& filePath, const std::string& content) {
    if (filePath.empty()) return;

    std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);
    if (outFile.is_open()) {
        outFile << content;
        outFile.close();
    }
}

void FileSystemService::saveBinaryFile(const std::string& filePath, const std::vector<char>& data) {
    if (filePath.empty()) return;

    std::ofstream outFile(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (outFile.is_open()) {
        outFile.write(data.data(), data.size());
        outFile.close();
    }
}
