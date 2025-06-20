#include <string>
#include "helpers.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>

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

/**
 * Splits a string into two substrings at the first occurrence of an
 * equals sign (=). If the string does not contain an equals sign, the
 * first substring is the original string and the second substring is
 * empty.
 *
 * For example, if the input string is "key=value", the first substring
 * is "key" and the second substring is "value". If the input string is
 * "noequals", the first substring is "noequals" and the second substring
 * is an empty string.
 */
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

/**
 * Converts a picture type key to a picture type enumeration value.
 * The enumeration values are consistent with the ID3v2 specification.
 *
 * @param originalKey The key to convert, such as "frontcover" or "backcover".
 * @return The corresponding picture type enumeration value.
 */
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

/**
 * Converts a picture type enumeration value to a string description.
 *
 * The values are consistent with the ID3v2 specification.
 *
 * @param type The picture type enumeration value to convert.
 * @return A string description of the picture type.
 */
std::string pictureTypeToString(int type) {
    switch (type) {
        case 0:  return "Other";
        case 1:  return "32x32 PNG Icon";
        case 2:  return "Other Icon";
        case 3:  return "Front Cover";
        case 4:  return "Back Cover";
        case 5:  return "Leaflet Page";
        case 6:  return "Media (Label Side)";
        case 7:  return "Lead Artist/Performer";
        case 8:  return "Artist/Performer";
        case 9:  return "Conductor";
        case 10: return "Band/Orchestra";
        case 11: return "Composer";
        case 12: return "Lyricist/Text Writer";
        case 13: return "Recording Location";
        case 14: return "During Recording";
        case 15: return "During Performance";
        case 16: return "Movie/Video Screen Capture";
        case 17: return "A Bright Coloured Fish";
        case 18: return "Illustration";
        case 19: return "Band/Artist Logotype";
        case 20: return "Publisher/Studio Logotype";
        default: return "Unknown";
    }
}

std::string sanitize_filename(const std::string& name) {
    // Remove illegal characters: \ / : * ? " < > | and control chars
    static const std::regex illegal(R"([\\/:*?"<>|\x00-\x1F])");
    std::string cleaned = std::regex_replace(name, illegal, "_");

    // Remove trailing spaces and dots (Windows restriction)
    while (!cleaned.empty() && (cleaned.back() == ' ' || cleaned.back() == '.'))
        cleaned.pop_back();

    return cleaned;
}

fs::path sanitize_path(const fs::path& inputPath) {
    fs::path result;
    for (const auto& part : inputPath) {
        // Skip empty components (can happen with leading/trailing slashes)
        if (part.empty()) continue;
        result /= sanitize_filename(part.string());
    }
    return result;
}

bool doRename(const fs::path& oldName, const fs::path& newName, bool verbose) {
    if (oldName == newName) return true;

    // Normalize slashes for cross-platform safety
    fs::path normNewName = newName;
    std::string normStr = normNewName.string();
    std::replace(normStr.begin(), normStr.end(), '\\', '/');
    normNewName = fs::path(normStr);

    try {
        auto parent = normNewName.parent_path();
        if (!parent.empty() && !fs::exists(parent))
            fs::create_directories(parent);

        if (fs::exists(normNewName))
            fs::remove(normNewName);
        fs::rename(oldName, normNewName);

//        if (verbose) std::cout << "Renamed " << oldName << " -> " << normNewName << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Rename failed: " << e.what() << "\n";
        return false;
    }
}
