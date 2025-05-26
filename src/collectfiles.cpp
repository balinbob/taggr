#include "main.h"
#include "collectfiles.h"
#include "glob/glob.hpp"
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::vector<fs::path> collectFiles(const Options& opts) {
    std::vector<fs::path> paths;
    if (opts.recurse) {
        paths = glob::rglob(opts.files);
    } else {
        paths = glob::glob(opts.files);
    }
    for (const auto& path : paths) {
//        std::cout << path << "\n";
    }
    return paths;
}