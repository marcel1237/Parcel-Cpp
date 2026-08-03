#ifndef FILE_SYSTEM_SERVICE_HPP
#define FILE_SYSTEM_SERVICE_HPP

#include <string>
#include <vector>

namespace Parcel::Service {
    class FileSystemService {
    public:
        void saveFile(const std::string& filePath, const std::string& content);
        void saveBinaryFile(const std::string& filePath, const std::vector<char>& data);
    };
}
#endif
