#ifndef SYNTAX_SCANNER_HPP
#define SYNTAX_SCANNER_HPP

#include <set>
#include <string>
#include <regex>
#include <vector>

namespace Parcel::Editor {
    class SyntaxScanner {
        inline static const std::set<std::string> JAVA_KEYWORDS = {
            "abstract", "class", "final", "public", "static", "void"
        };

        inline static const std::string MD_PATTERN = "(^#+.*$)|(```[\\s\\S]*?```)";

    public:
        void tokenize(const std::string& source) {
            std::regex re(MD_PATTERN);
        }
    };
}
#endif
