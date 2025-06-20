#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <set>
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
    if (tags.empty()) return fn;

    // 1. Find placeholders in pattern
    std::set<std::string> patternPlaceholders;
    std::regex re("%[a-z]");
    for (auto it = std::sregex_iterator(pattern.begin(), pattern.end(), re);
         it != std::sregex_iterator(); ++it) {
        patternPlaceholders.insert(it->str());
    }

    // 2. For each placeholder in pattern, require the tag
    for (const auto& code : patternPlaceholders) {
        auto tagIt = codeToTag.find(code);
        if (tagIt == codeToTag.end()) continue; // unknown placeholder, skip
        const std::string& tagName = tagIt->second;
        auto valueIt = tags.find(tagName);
        if (valueIt != tags.end()) {
            // Tag exists: replace all occurrences
            fn = std::regex_replace(fn, std::regex(code), valueIt->second);
        } else {
            std::cerr << "ABORT: Required tag '" << tagName << "' for " << code
                      << " missing for file pattern '" << pattern << "'\n";
            fn = "";
        }
    }

    return fn;
}
