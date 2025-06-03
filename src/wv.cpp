#include "main.h"
#include "magic.h"
#include "helpers.h"
#include "ape.h"
#include <taglib/wavpackfile.h>
#include <taglib/id3v1tag.h>
#include <taglib/apetag.h>
#include <taglib/tpropertymap.h>
#include <iostream>
#include <string>

int tagWV(TagLib::WavPack::File* wv, const Options& opts) {
    bool hasApe = wv->hasAPETag();
    bool hasID3v1 = wv->hasID3v1Tag();

    auto* apeTag = wv->APETag(true);
    auto* id3v1Tag = wv->ID3v1Tag(true);
    bool modified = false;

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

    listApeTag(apeTag, opts);
    
    if (modified) {
        wv->save();
        if (opts.verbose) std::cout << "Saved\n"; 
    }
    return 0;
}

