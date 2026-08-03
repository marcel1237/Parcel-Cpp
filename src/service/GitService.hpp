#ifndef GIT_SERVICE_HPP
#define GIT_SERVICE_HPP

#include <string>
#include <filesystem>

namespace Parcel::Service {
    class GitService {
    public:
        explicit GitService(const std::string& rootPath);
        bool isGitRepository() const;
        std::string getCurrentBranch() const;

    private:
        std::filesystem::path projectRoot;
        bool isGit;
        void detectRepository();
    };
}
#endif
