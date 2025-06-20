#include "CLI11/CLI.hpp"
#include "glob/glob.hpp"
#include "main.h"
#include "helpers.h"
#include "collectfiles.h"
#include "magic.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    CLI::App app{"taggr — test"};
    Result res;
    Options opts;
    std::vector<fs::path> fpaths;
    app.add_flag(",--recurse", opts.recurse, "recurse");
    app.add_flag("-v,--verbose", opts.verbose, "extra output");
    app.add_flag("-q,--quiet", opts.quiet, "quiet"
        "(Don't print filenames)");
    app.add_flag("-n,--noact", opts.noact, "no action, just show what would be done");
    app.add_flag("-l,--list", opts.list, "list all tags");
    app.add_flag(",--clear", opts.clear, "clear all tags");
    app.add_option("-s,--show", opts.show, "show tag values"
        "\nEx: --show genre");
    app.add_option("-t,--tag", opts.tag, 
        "set a tag as a key=\"value\" pair"
        "\nEx: -t title=\"Come Together\"  -t album=\"Abbey Road\"");
    app.add_option("-a,--add", opts.add, "add a value to a multivalue tag"
        "\nEx: -a genre=\"rock\" -a genre=\"pop\"");
    app.add_option("-r,--remove", opts.remov, "remove a tag or tag value"
        "\nEx: --remove genre=\"pop\" -r genre");
    app.add_option("-b,--binary", opts.binary, "set a binary tag (cover art)"
        "\nEx: --binary frontcover=\"cover.jpg\"");
    app.add_option(",--fn2tag", opts.fn2tag, "extract tags from filename"
        "\nEx: --fn2tag \"\\%l\\%n %t.flac\"");
    app.add_option(",--tag2fn", opts.tag2fn, "set filename from tags");
    app.add_option("files", opts.files, "filepaths and/or globs")
                ->required()
                ->expected(-1);

    CLI11_PARSE(app, argc, argv);

    if(opts.verbose)
        std::cout << "Verbose mode on!\n";
    if (opts.files.size() > 0) {
        fpaths = collectFiles(opts);
    }
    else {
        std::cout << "Usage:  " << argv[0] << " [options].. -- <files>..\n";
        std::cout << "Try '" << argv[0] << " --help' for more information.\n";
    }

    fs::path newPath;
    for (const auto& path : fpaths) {
        res = doMagic(path, opts);
        if (opts.tag2fn == "") continue;

        if (opts.noact || res.newPath == "" || res.newPath == path) {
            if (opts.verbose) std::cout << "Skipping rename\n";}
        else {
            newPath = sanitize_path(res.newPath);

            if (!doRename(path, newPath, opts.verbose)) {
                std::cerr << "Error renaming " << path << " to " << newPath << "\n";
                res.success = false;
            }
            else {
                if (opts.verbose) std::cout << "Renamed " << path << " to " << newPath << "\n";
            } 
        }
    }
    return res.success;
}
