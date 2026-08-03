#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class FileHelper {
public:
    static std::vector<std::string> readLines(const std::string& path) {
        std::vector<std::string> lines;
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) lines.push_back(line);
        return lines;
    }
};

int main() {
    auto lines = FileHelper::readLines("test.txt");
    for(const auto& l : lines) std::cout << l << std::endl;
    return 0;
}
