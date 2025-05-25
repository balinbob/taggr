#include "CLI11/CLI.hpp"
#include "glob/glob.hpp"
#include "main.h"
#include "collectfiles.h"
#include "magic.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    CLI::App app{"taggr — test"};

    Options opts;
    std::vector<fs::path> fpaths;
    app.add_flag(",--recurse", opts.recurse, "recurse");
    app.add_flag("-v,--verbose", opts.verbose, "extra output");
    app.add_option("-t,--tag", opts.tag, "set a tag");
    app.add_option("-a,--add", opts.add, "add a tag value");
    app.add_option("-r,--remove", opts.remov, "remove a tag or tag value");
    app.add_option("-c,--clear", opts.clear, "clear all tags");
    app.add_option("files", opts.files, "filepaths and/or globs")
                ->expected(-1);

    CLI11_PARSE(app, argc, argv);

    if(opts.verbose)
        std::cout << "Verbose mode on!\n";
    if (opts.files.size() > 0) {
        std::cout << "Processing " << opts.files.size() << " files\n";
        fpaths = collectFiles(opts);
        std::cout << "Found " << fpaths.size() << " files\n";
    }
    else {
        std::cout << "Hello, world!\n";
    }
    for (const auto& path : fpaths) {
        doMagic(path, opts);
    }
    return 0;
}
