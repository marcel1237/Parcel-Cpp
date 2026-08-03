#include "GitService.hpp"
#include <fstream>

using namespace Parcel::Service;

GitService::GitService(const std::string& rootPath) : projectRoot(rootPath), isGit(false) {
    detectRepository();
}

void GitService::detectRepository() {
    isGit = std::filesystem::exists(projectRoot / ".git");
}

bool GitService::isGitRepository() const { return isGit; }

std::string GitService::getCurrentBranch() const {
    if (!isGit) return "";
    std::ifstream headFile(projectRoot / ".git/HEAD");
    std::string line;
    if (std::getline(headFile, line)) {
        if (line.find("ref: ") == 0) {
            return line.substr(line.find_last_of('/') + 1);
        }
        return line;
    }
    return "unknown";
}
