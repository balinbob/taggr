#include "main.h"
#include "ape.h"
#include "helpers.h"
#include "fn2tag.h"
#include "tag2fn.h"
#include <iostream>
#include <string>
#include <fstream>
#include <taglib/apefile.h>
#include <taglib/apeitem.h>
#include <taglib/apetag.h>
#include <taglib/tpropertymap.h>

namespace fs = std::filesystem;

bool removeApeTag(TagLib::APE::File* ape, const TagLib::String& key, const TagLib::String& value, const Options& opts) { 
    auto* apeTag = ape->APETag(true);
    bool modified = false;

    if (value.isEmpty()) {
        apeTag->removeItem(key);
        if (opts.verbose) std::cout << "Removing " << key << "\n";
        modified = true;
    }
    else {

        auto props = apeTag->properties();
        auto it = props.find(key);
        if (it != props.end()) {
            TagLib::StringList values = it->second;
            TagLib::StringList filtered;
            for (auto const& val : values) {
                if (val != value) {
                    filtered.append(val);
                }
            }
            if (filtered == values) {
                if (opts.verbose) std::cout << "couldn't find value: " << value << "\n";
                return false;
            }
            if (filtered.isEmpty()) {
                apeTag->removeItem(key);
            }
            else {
                props[key] = filtered;
                apeTag->setProperties(props);
                if (opts.verbose) std::cout << "Removing " << key << " = " << value << "\n";
                modified = true;
            }
        }
        else {
            if (opts.verbose) std::cout << "couldn't find key: " << key << "\n";
            return false;
        }
    }

    if (modified) {
        ape->save();
    }
    return true;
}

bool addApeTag(TagLib::APE::Tag* apeTag, const Options& opts) {
    // includes set (--add)

    for (auto const& cmd : opts.tag) {
        auto cmds = splitOnEquals(cmd);
        apeTag->addValue(cmds.first.c_str(), cmds.second.c_str(), true);
        if (opts.verbose) std::cout << "Setting " << cmds.first << " = " << cmds.second << "\n";
    }
    
    for (auto const& cmd : opts.add) {
        auto cmds = splitOnEquals(cmd);
        apeTag->addValue(cmds.first.c_str(), cmds.second.c_str(), false);
        if (opts.verbose) std::cout << "Adding " << cmds.first << " = " << cmds.second << "\n";
    }
    return true;
}
void listApeTag(TagLib::APE::Tag* apeTag, const Options& opts) {
    // includes show
    TagLib::PropertyMap props = apeTag->properties();    

    for (auto& tagCmd : opts.show) {
        auto prop = props.find(tagCmd.c_str());
        if (prop != props.end()) {
            std::cout << prop->first.to8Bit() << ":\t" << prop->second.toString() << "\n";
        }
    }
    
    if (opts.list) {
        for (auto prop : props) {
            std::cout << prop.first.to8Bit() << ":\t" << prop.second.toString() << "\n";
        }
    }
    return;
}

bool addBinary(TagLib::APE::Tag* apeTag, const std::string& path, const std::string& key, const Options& opts) {
    
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Couldn't open file: " << path << "\n";
        return false;
    }
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    TagLib::ByteVector dataVector(data.data(), data.size());
    
    apeTag->setData(key, dataVector);
    if (opts.verbose) std::cout << "Adding " << key << "\n";
    return true;
}

bool tagFromFn(TagLib::APE::Tag* apeTag, const Options& opts, const std::string& path) {
    bool modified = false;
    auto tags = fn2tag(path, opts.fn2tag);
    for (auto const& tag : tags) {
        apeTag->addValue(tag.first.c_str(), tag.second.c_str(), true);
        if (opts.verbose) std::cout << "Setting " << tag.first << " = " << tag.second << "\n";
        modified = true;
    }
    return modified;
}

Result tagAPE(TagLib::APE::File* ape, const Options& opts, const fs::path& path) {
    auto* apeTag = ape->APETag(true);
    bool modified = false;

    if (opts.clear) {
        ape->strip();
        if (opts.verbose) std::cout << "Clearing all tags\n";
        modified = true;
    }

    for (auto& tagCmd : opts.remov) {
        auto cmds = splitOnEquals(tagCmd);
        if (!removeApeTag(ape, cmds.first.c_str(), cmds.second.c_str(), opts)) {
            continue;
        }
        modified = true;
    }
    
    if (addApeTag(apeTag, opts)) modified = true; // takes care of setting and adding
 
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
        newFname = tag2fn(ape->properties(), opts.tag2fn, opts.verbose);
        if (newFname != "" && newFname != path.string()) {
            if (opts.verbose) std::cout << "Renaming to " << newFname << "\n";
            modified = true;
        }
    }
    
    fs::path newPath(newFname);
    Result res;
    res.newPath = newPath;
    
    if (opts.noact) {
        const auto& props = ape->properties();
        for (const auto& prop : props) {
            std::cout << prop.first.to8Bit() << ":\t" << prop.second.toString() << "\n";
        }
        std::cout << path.string() << " -> " << newPath.string() << "\n";
        modified = false;
    }

    listApeTag(apeTag, opts);   // show and list

    if (modified) {
        if (ape->save()) {
            if (opts.verbose) std::cout << "Saved\n";
            res.success = true;;
        }
        else {
            if (opts.verbose) std::cout << "Couldn't save\n";
            res.success = false;
        }
    }

    return res;
}