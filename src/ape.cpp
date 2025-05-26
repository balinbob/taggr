#include "main.h"
#include "ape.h"
#include "helpers.h"
#include <iostream>
#include <taglib/apefile.h>
#include <taglib/apefooter.h>
#include <taglib/apeitem.h>
#include <taglib/apetag.h>
#include <taglib/tpropertymap.h>



int tagAPE(TagLib::APE::File* ape, const Options& opts) {
    if (!ape->hasAPETag()) {
        std::cout << "Creating APEv2 tag\n";
    }
    
    auto* apeTag = ape->APETag(true);
    bool modified = false;
    if (opts.remov.size() > 0) {
        TagLib::PropertyMap props = apeTag->properties();
        for (auto& tagCmd : opts.remov) {
            auto tags = splitOnEquals(tagCmd);
            if (tags.second == "") {
                apeTag->removeItem(tags.first.c_str());
                if (opts.verbose) std::cout << "Removing " << tags.first << "\n";
                modified = true;
            }
            else {
                auto itProp = props.find(tags.first.c_str());
                if (itProp != props.end()) {
                    TagLib::StringList values = itProp->second;
                    TagLib::StringList filtered;
                    for (auto const& val : values) {
                        if (val != tags.second.c_str()) {
                            filtered.append(val);
                        }
                    }
                    if (filtered.isEmpty()) {
                        apeTag->removeItem(tags.first.c_str());
                    }
                    else {
                        props[tags.first.c_str()] = filtered;
                        apeTag->setProperties(props);
                    }
                    if (opts.verbose) std::cout << "Removing " << tags.first << " = " << tags.second << "\n";
                    modified = true;
                }
                else {
                    std::cout << "Key " << tags.first << " not found\n";
                }
            }
        }
    }
    if (opts.tag.size() > 0) {
        for (auto& tagCmd : opts.tag) {
            auto tags = splitOnEquals(tagCmd);
            apeTag->addValue(tags.first.c_str(), tags.second.c_str(), true);
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
            apeTag->addValue(tags.first.c_str(), tags.second.c_str(), false);
            if (opts.verbose) std::cout << "Adding " << tags.first << " = " << tags.second << "\n";
            modified = true;
        }
    }
    if (opts.show.size() > 0) {
        TagLib::PropertyMap props = apeTag->properties();    
        for (auto& tagCmd : opts.show) {
            auto itProp = props.find(tagCmd.c_str());
            if (itProp != props.end()) {
                for (auto const& val : itProp->second) {
                    std::cout << val.to8Bit() << "\n";
                }
            }
            else {
                std::cout << "Key " << tagCmd << " not found\n";
            }
        }
    }
    if (opts.list) {
        TagLib::PropertyMap props = apeTag->properties();
        for (auto const& prop : props) {
            std::cout << prop.first.to8Bit() << ":";
            for (auto const& val : prop.second) {
                std::cout << "\t" << val.to8Bit() << "\n";
            }
        }
    }
    if (modified) ape->save();
    return 0;
}