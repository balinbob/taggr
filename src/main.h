#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <vector>

struct Options {
    bool recurse = false;
    bool verbose = false;
    std::vector<std::string> tag;
    std::vector<std::string> add;
    std::vector<std::string> remov;
    std::string clear;
    std::vector<std::string> files;
};


#endif
