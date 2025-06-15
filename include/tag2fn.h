#ifndef TAG2FN_H
#define TAG2FN_H

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <regex>


namespace fs = std::filesystem;

std::map<std::string, std::string> props2tags(const TagLib::PropertyMap& props);
std::string tag2fn(const TagLib::PropertyMap& tags, const std::string &pattern, bool verbose);

#endif