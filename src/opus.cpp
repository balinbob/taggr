#include <taglib/opusfile.h>
#include "opus.h"

/*
#include "main.h"
#include "flac.h"
#include "helpers.h"
#include "fn2tag.h"
#include "tag2fn.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/opusproperties.h>
#include <taglib/oggfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/tpropertymap.h>
#include <filesystem>
#include <string>
*/
#include <iostream>

namespace fs = std::filesystem;
namespace op = TagLib::Ogg::Opus;

Result tagOPUS(op::File* opus, const Options& opts, const fs::path& path) {
    Result res;
    bool modified = false;
    TagLib::Ogg::XiphComment* vc = opus->tag();
    if (removeTags(vc, opts)) modified = true;
    if (addTags(vc, opts)) modified = true;
    
    
    
    
    if (opts.fn2tag != "") {
        auto tags = fn2tag(path.string(), opts.fn2tag);
        if (opts.verbose) std::cout << "Found " << tags.size() << " tags from " << path.string() << "\n";
        for (const auto& tag : tags) {
            vc->addField(tag.first.c_str(), tag.second.c_str(), true);
            if (opts.verbose) std::cout << "Setting " << tag.first << " = " << tag.second << "\n";
            modified = true;
        }
    }

    std::string newName = path.string();
    if (opts.tag2fn != "") {
        newName = tag2fn(opus->properties(), opts.tag2fn, opts.verbose);
        if (newName != path.string() && newName != "") {
            if (opts.verbose) std::cout << "Renaming to " << newName << "\n";
            modified = true;
        }
    }

    fs::path newPath(newName);
    res.newPath = newPath;
    if (newPath.is_absolute()) {
        std::cout << "Absolute path not supported\n";
        res.newPath = path;
    }

    if (opts.noact) {
        for (auto prop : opus->properties()) {
            std::cout << prop.first << " = " << prop.second << "\n";
        }
        if (path != newPath) {
            std::cout << "Would rename to " << newName << "\n";
        }
        modified = false;
    }
    if (opts.list) listTags(vc, opts);
    if (opts.show.size() > 0) showTag(vc, opts);
    if (modified) {
        if (opus->save()) {
            if (opts.verbose) std::cout << "Saved\n";
            res.success = true;
        }
        else {
            if (opts.verbose) std::cout << "Failed to save\n";
            res.success = false;
        }
    }
    
    return res;
}
