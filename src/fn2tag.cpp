#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>

// List of code to tag name
const std::map<std::string, std::string> codeToTag = {
    {"%n", "tracknumber"},
    {"%t", "title"},
    {"%l", "album"},
    {"%a", "artist"},
    {"%d", "date"},
    {"%D", "discnumber"}
    // Add more codes as needed
};

// Converts a pattern to a regex, returns the regex string AND group order
std::pair<std::string, std::vector<std::string>>
patternToRegex(const std::string& pattern) {
    std::string regexStr;
    std::vector<std::string> groupOrder;
    size_t i = 0;

    while (i < pattern.size()) {
        if (pattern[i] == '%') {
            if (i + 1 < pattern.size()) {
                std::string code = pattern.substr(i, 2);
                auto it = codeToTag.find(code);
                if (it != codeToTag.end()) {
                    if (code == "%n" || code == "%D")
                        regexStr += "(0?\\d+)";
                    else
                        regexStr += "(.+?)";
                    groupOrder.push_back(it->second);
                    i += 2;
                    continue;
                }
            }
        }
        // Escape regex metacharacters
        if (std::string(".^$*+?()[]{}|\\").find(pattern[i]) != std::string::npos)
            regexStr += '\\';
        regexStr += pattern[i];
        ++i;
    }
    regexStr = ".*" + regexStr + "$";
    return {regexStr, groupOrder};
}

// Extracts tags using the regex and group order
std::map<std::string, std::string>
extractTags(const std::string& filename, const std::string& regexPattern, const std::vector<std::string>& groupOrder) {
    std::map<std::string, std::string> tags;
    std::regex re(regexPattern);
    std::smatch m;
    if (std::regex_match(filename, m, re)) {
        for (size_t i = 0; i < groupOrder.size(); ++i) {
            tags[groupOrder[i]] = m[i+1];
        }
    }
    return tags;
}

// High-level wrapper
std::map<std::string, std::string> fn2tag(const std::string& fn, const std::string &pattern) {
    auto [regexPattern, groupOrder] = patternToRegex(pattern);
    std::map<std::string, std::string> tags = extractTags(fn, regexPattern, groupOrder);
    if (tags.empty()) {
        std::cout << "Filepath does not match pattern: " << pattern << "\n";
    }
    return tags;
}
