#ifndef PERSISTENCE_SERVICE_HPP
#define PERSISTENCE_SERVICE_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <map>

namespace Parcel::Service {

    struct UsageStats {
        int daily;
        int weekly;
        int monthly;
        int yearly;
    };

    class PersistenceService {
    public:
        void saveLastProject(const std::string& path);
        std::string getLastProject();

        void saveOpenFiles(const std::vector<std::string>& filePaths);
        std::vector<std::string> getOpenFiles();

        // Gemini Usage Tracking
        void incrementUsage();
        UsageStats getUsageStats();

        // Gemini Config (Encrypted)
        void saveGeminiApiKey(const std::string& key);
        std::string getGeminiApiKey();

    private:
        std::string encrypt(const std::string& data);
        std::string decrypt(const std::string& data);
        std::filesystem::path getConfigPath();
        std::map<std::string, std::string> loadProperties();
        void saveProperties(const std::map<std::string, std::string>& props);
        std::string getCurrentDate();
        std::string getCurrentWeek();
        std::string getCurrentMonth();
        std::string getCurrentYear();
    };
}
#endif
