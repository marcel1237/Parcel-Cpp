#ifndef FILE_ICON_PROVIDER_HPP
#define FILE_ICON_PROVIDER_HPP

#include <string>

namespace Parcel::View {
    enum class IconType { FOLDER, SOURCE, SCIENCE, STORAGE, BUILD, GIT, FILE };

    class FileIconProvider {
    public:
        static IconType getIconType(const std::string& fileName, bool isDirectory) {
            if (isDirectory) {
                if (fileName == "src") return IconType::SOURCE;
                if (fileName == "test") return IconType::SCIENCE;
                if (fileName == "resources") return IconType::STORAGE;
                if (fileName == "target" || fileName == "build") return IconType::BUILD;
                if (fileName == ".git") return IconType::GIT;
                return IconType::FOLDER;
            }
            return IconType::FILE;
        }
    };
}
#endif
