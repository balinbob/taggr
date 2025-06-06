#pragma once
#include <string>
#include <vector>


std::string toLower(const std::string& input);
std::pair<std::string, std::string> splitOnEquals(const std::string& input);
int pictureTypeFromKey(const std::string& originalKey);
std::string pictureTypeToString(int type);