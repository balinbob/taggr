#include "helpers.h"
#include <algorithm>
#include <cctype>
#include <string>

/**
 * Returns a copy of the input string with all characters converted to lower
 * case using the locale-insensitive std::tolower function.
 */
std::string toLower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::pair<std::string, std::string> splitOnEquals(const std::string& input) {
    std::string::size_type pos = input.find('=');
    std::string key;
    std::string value;
    
    if (pos != std::string::npos) {
        key = input.substr(0, pos);
        value = input.substr(pos + 1);
    } else {
        key = input;
        value = "";
    }
    
    return std::make_pair(key, value);
}