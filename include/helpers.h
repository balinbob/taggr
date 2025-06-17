#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

struct Result {
    bool success = false;;
    fs::path newPath = fs::path("");
};


std::string toLower(const std::string& input);
std::pair<std::string, std::string> splitOnEquals(const std::string& input);
int pictureTypeFromKey(const std::string& originalKey);
std::string pictureTypeToString(int type);
bool doRename(const fs::path& oldPath, const fs::path& newPath, bool verbose);