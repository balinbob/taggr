#include <string>
#include "helpers.h"
#include <algorithm>
#include <cctype>


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

int pictureTypeFromKey(const std::string& originalKey) {
    const std::string key = toLower(originalKey);
    if (key == "frontcover") return 3;   // Front cover
    else if (key == "backcover") return 4; // Back cover
    else if (key == "other") return 0;
    else if (key == "fileicon") return 1;
    else if (key == "otherfileicon") return 2;
    else if (key == "leafletpage") return 5;
    else if (key == "media") return 6;
    else if (key == "leadartist") return 7;
    else if (key == "artist") return 8;
    else if (key == "conductor") return 9;
    else if (key == "band") return 10;
    else if (key == "composer") return 11;
    else if (key == "lyricist") return 12;
    else if (key == "recordinglocation") return 13;
    else if (key == "duringrecording") return 14;
    else if (key == "duringperformance") return 15;
    else if (key == "moviescreencapture") return 16;
    else if (key == "illustration") return 18;
    else if (key == "bandlogo") return 19;
    else if (key == "publisherlogo") return 20;
    else return 0; // Other (default)
}

