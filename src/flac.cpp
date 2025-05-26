#include "main.h"
#include "helpers.h"
#include "flac.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "taglib/fileref.h"
#include "taglib/tag.h"
#include "taglib/flacfile.h"
#include "taglib/vorbisfile.h"
#include "taglib/vorbisproperties.h"
#include "taglib/oggflacfile.h"
#include "taglib/tpropertymap.h"

int tagFLAC(TagLib::FLAC::File* flac, const Options& opts) {
    bool modified = false;
    auto* vc = flac->xiphComment(true);
    if (opts.remov.size() > 0) {
        for (auto& tagCmd : opts.remov) {
            auto tags = splitOnEquals(tagCmd);
            if (tags.second == "") vc->removeFields(tags.first.c_str());
            else vc->removeFields(tags.first.c_str(), tags.second.c_str());
            if (opts.verbose) {
                std::cout << "Removing " << tags.first;
                if (tags.second != "") std::cout << " = " << tags.second;
                std::cout << "\n";
            }
            modified = true;
        }
    }
    if (opts.tag.size() > 0) {
        for (auto& tagCmd : opts.tag) {
            auto tags = splitOnEquals(tagCmd);
            vc->addField(tags.first.c_str(), tags.second.c_str());
            if (opts.verbose) std::cout << "Setting " << tags.first << " = " << tags.second << "\n";
            modified = true;
        }
    }
    if (opts.add.size() > 0) {
        for (auto& tagCmd : opts.add) {
            auto tags = splitOnEquals(tagCmd);
            if (tags.second == "") { 
                std::cout << "Cannot add empty tag value\n";
                return 1;
            }
            vc->addField(tags.first.c_str(), tags.second.c_str(), false);
            if (opts.verbose) std::cout << "Adding " << tags.first << " = " << tags.second << "\n";
            modified = true;
        }
    }
    if (opts.show.size() > 0) {
        TagLib::PropertyMap const props = vc->properties();
        for (auto& tagCmd : opts.show) {
            auto const tags = splitOnEquals(tagCmd);
            if (tags.second == "") {
                auto const itProp = props.find(tags.first.c_str());
                if (itProp != props.end()) {
                    std::cout << itProp->first.to8Bit() << ":";
                    TagLib::StringList values = itProp->second;
                    for (auto const& val : values) {
                        std::cout << "\t" << val.to8Bit() << "\n";
                    }
                }
            }
            else {
                auto const itProp = props.find(tags.first.c_str());
                if (itProp != props.end()) {
                    TagLib::StringList values = itProp->second;
                    for (auto const& val : values) {
                        if (val == tags.second.c_str()) {
                            std::cout << val.to8Bit() << "\n";
                        }
                    }
                }
            }
        }
    }
    if (opts.list) {
        TagLib::PropertyMap const props = vc->properties();
        for (auto const& prop : props) {
            std::cout << prop.first.to8Bit() << ":";
            for (auto const& val : prop.second) {
                std::cout << "\t" << val.to8Bit() << "\n";
            }
        }
    }
    if (modified) {
        flac->save();
    }
    return 0;
}