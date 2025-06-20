#include "main.h"
#include "magic.h"
#include "helpers.h"
#include "ape.h"
#include "fn2tag.h"
#include "tag2fn.h"
#include <taglib/wavpackfile.h>
#include <taglib/id3v1tag.h>
#include <taglib/apetag.h>
#include <taglib/tpropertymap.h>
#include <iostream>
#include <string>

Result tagWV(TagLib::WavPack::File* wv, const Options& opts, const fs::path& path) {

    bool hasApe = wv->hasAPETag();
    bool hasID3v1 = wv->hasID3v1Tag();

    auto* apeTag = wv->APETag(true);
    auto* id3v1Tag = wv->ID3v1Tag(true);
    bool modified = false;

    Result res;
    res.newPath = path;

    if (!apeTag) {
        std::cout << "No APE tag found\n";
        return res;
    }

    if (opts.clear) {
        wv->strip();
        if (opts.verbose) std::cout << "Clearing all tags\n";
        modified = true;
    }

    if (opts.remov.size() > 0) {
        for (auto tagCmd : opts.remov) {
            auto cmds = splitOnEquals(tagCmd);
            apeTag->removeItem(cmds.first.c_str());
            if (opts.verbose) std::cout << "Removing " << cmds.first << "\n";
            modified = true;
        }
    }

    modified = addApeTag(apeTag, opts);

    for (auto& tagCmd : opts.binary) {
        auto cmds = splitOnEquals(tagCmd);
        if (!addBinary(apeTag, cmds.second, cmds.first, opts)) continue;
        else modified = true;
    }

    if (opts.fn2tag != "") {
        if (tagFromFn(apeTag, opts, path.string())) modified = true;
    }

    std::string newFname = path.string();
    if (opts.tag2fn != "") {
        newFname = tag2fn(apeTag->properties(), opts.tag2fn, opts.verbose);
        if (newFname != "" && newFname != path.string()) {
            if (opts.verbose) std::cout << "Renaming to " << newFname << "\n";
            modified = true;
        }
    }

    fs::path newPath(newFname);
    if (newPath.is_absolute()) {
        std::cout << "Cannot rename to absolute path\nAborting name change!\n";
        newPath = path;
    }
    res.newPath = newPath;
        
    if (opts.noact) {
        const auto& props = apeTag->properties();
        for (auto const& prop : props) {
            std::cout << prop.first.to8Bit() << ":\t" << prop.second.toString() << "\n";
        }
        if (path != newPath) std::cout << path.string() << " -> " << newPath.string() << "\n";
        modified = false;
    }

    listApeTag(apeTag, opts);
    
    if (modified) {
        if (wv->save()) {
            if (opts.verbose) std::cout << "Saved\n";
            res.success = true;
        }
        else {
            if (opts.verbose) std::cout << "Failed to save..";
            res.success = false;
        }
    }
    return res;
}

