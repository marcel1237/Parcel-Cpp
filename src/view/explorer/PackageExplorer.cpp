#include "PackageExplorer.hpp"

using namespace Parcel::View;
namespace fs = std::filesystem;

PackageExplorer::PackageExplorer() {}

void PackageExplorer::openProject(const std::string& rootPath) {
    currentRoot = rootPath;
}

bool PackageExplorer::shouldFilter(const std::string& name) const {
    for (const auto& filter : FILTERED_NAMES) {
        if (filter == "*" && name.size() >= filter.size() - 1) {
             // Basic implementation of extension filtering
        } else if (name == filter || name.find(filter) == 0) {
            return true;
        }
    }
    return false;
}

void PackageExplorer::listFilteredFiles(const fs::path& dir, std::vector<fs::path>& result) {
    if (!fs::exists(dir) || !fs::is_directory(dir)) return;

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!shouldFilter(entry.path().filename().string())) {
            result.push_back(entry.path());
        }
    }
    std::sort(result.begin(), result.end());
}
