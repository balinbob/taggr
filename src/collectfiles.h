#ifndef COLLECTFILES_H
#define COLLECTFILES_H

#include <vector>
#include <string>
#include <filesystem>
#include "main.h"

namespace fs = std::filesystem;
std::vector<fs::path> collectFiles(const Options& opts);



#endif // COLLECTFILES_H