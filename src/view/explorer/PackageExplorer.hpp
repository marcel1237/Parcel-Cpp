#ifndef PACKAGE_EXPLORER_HPP
#define PACKAGE_EXPLORER_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace Parcel::View {
    class PackageExplorer {
    public:
        PackageExplorer();
        void openProject(const std::string& rootPath);
        bool shouldFilter(const std::string& fileName) const;

    private:
        std::filesystem::path currentRoot;
        inline static const std::vector<std::string> FILTERED_NAMES = {
            ".git", ".gitignore", "target", "node_modules", "build", "bin", "out"
        };

        void listFilteredFiles(const std::filesystem::path& dir, std::vector<std::filesystem::path>& result);
    };
}
#endif
