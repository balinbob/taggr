#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include "helpers.h"
#include "tag2fn.h"
#include "fn2tag.h"

namespace fs = std::filesystem;

std::map<std::string, std::string> props2tags(const TagLib::PropertyMap& props) {
    std::map<std::string, std::string> tags;
    
    for (const auto& prop : props) {
        tags[toLower(prop.first.to8Bit())] = prop.second.toString().to8Bit();
    }
    return tags;
}

std::string tag2fn(const TagLib::PropertyMap& propMap, const std::string &pattern, bool verbose) {
    std::map<std::string, std::string> tags = props2tags(propMap);
    std::string fn = pattern;
    if (tags.empty()) {
        return fn;
    }
    if (verbose) {
    for (const auto& pair : tags) {
        std::cout << pair.first << ": " << pair.second << "\n";
    }
    }
    for (const auto& pair : codeToTag) {
        const std::string& code = pair.first;         // e.g. "%n"
        const std::string& tagName = pair.second;     // e.g. "tracknumber"
        if (verbose) std::cout << "Trying to replace " << code << " with " << tagName << "\n";
        auto tagIt = tags.find(tagName);
        if (tagIt != tags.end()) {
            if (verbose) std::cout << "Replacing " << code << " with " << tagIt->second << "\n";
            // Use std::regex_replace in case code appears multiple times
            fn = std::regex_replace(fn, std::regex(code), tagIt->second);
        }
    }
    if (verbose) std::cout << fn << "\n";
    return fn;
}
