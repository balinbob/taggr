#include "main.h"
#include "collectfiles.h"
#include "glob/glob.hpp"
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::vector<fs::path> collectFiles(const Options& opts) {
    std::vector<std::string> globs;
    std::vector<fs::path> literals;
    for (const auto& pattern : opts.files) {
        if (fs::exists(pattern)) {
            // If it's a real, existing file, add it to the list
            fs::path fsPath(pattern);
            literals.push_back(fsPath);
            // else assume it's a glob 
        }
        else {
            globs.push_back(pattern);
        }

    }

    std::vector<fs::path> paths;
    
    if (opts.recurse) {
        paths = glob::rglob(globs);
    } else {
        paths = glob::glob(globs);
    }
    
    paths.insert(paths.end(), literals.begin(), literals.end());

    return paths;
}