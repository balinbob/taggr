#include "main.h"
#include "magic.h"
#include "helpers.h"
#include <taglib/tpropertymap.h>
#include "fn2tag.h"
#include "tag2fn.h"
#include <iostream>
#include <filesystem>
#include "taglib/fileref.h"
#include "taglib/mp4coverart.h"
#include <taglib/mp4tag.h>
#include <taglib/mp4item.h>
#include <taglib/mp4file.h>
#include <string>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

std::string getAtomKey(const std::string& key) {
    static const std::map<std::string, std::string> mp4TagAliases = {
        {"TITLE",        "\xA9""nam"},
        {"ARTIST",       "\xA9""ART"},
        {"ALBUM",        "\xA9""alb"},
        {"ALBUMARTIST",  "aART"},
        {"COMPOSER",     "\xA9""wrt"},
        {"GENRE",        "\xA9""gen"},
        {"TRACKNUMBER",  "trkn"},
        {"DISCNUMBER",   "disk"},
        {"DATE",         "\xA9""day"},
        {"YEAR",         "\xA9""day"},
        {"COMMENT",      "\xA9""cmt"},
        {"ENCODER",      "\xA9""too"},
        {"GROUPING",     "\xA9""grp"},
        {"LYRICS",       "\xA9""lyr"},
        {"COMPILATION",  "cpil"},
        {"BPM",          "tmpo"},
        {"COVERART",     "covr"},
    };
    std::string keyUpper = key;
    std::transform(keyUpper.begin(), keyUpper.end(), keyUpper.begin(), ::toupper);
    auto it = mp4TagAliases.find(keyUpper);
    if (it != mp4TagAliases.end())
        return it->second;
    else
        return key; // Fallback
}

bool setPicture(TagLib::MP4::Tag* mp4Tag, const std::string& imagePath) {
    // Read the image file into a buffer
    std::ifstream img(imagePath, std::ios::binary | std::ios::ate);
    if (!img) {
        std::cerr << "Could not open image file: " << imagePath << "\n";
        return false;
    }
    std::streamsize imgSize = img.tellg();
    img.seekg(0, std::ios::beg);
    std::vector<unsigned char> imgData(imgSize);
    if (!img.read(reinterpret_cast<char*>(imgData.data()), imgSize)) {
        std::cerr << "Could not read image file: " << imagePath << "\n";
        return false;
    }

    // Create the CoverArt object
    fs::path p(imagePath);
    std::string ext = p.extension().string();
    bool isPNG = toLower(ext) == ".png";
    TagLib::MP4::CoverArt::Format format = isPNG ?
        TagLib::MP4::CoverArt::PNG : TagLib::MP4::CoverArt::JPEG;

    TagLib::MP4::CoverArt cover(format, TagLib::ByteVector(reinterpret_cast<const char*>(imgData.data()), imgSize));
    TagLib::MP4::CoverArtList covers;
    covers.append(cover);

    // Set the "covr" tag to the list of cover arts
    if (!mp4Tag) {
        std::cerr << "No MP4 tag found in file\n";
        return false;
    }
    mp4Tag->setItem("covr", TagLib::MP4::Item(covers));

    return true;
}


Result tagMP4(TagLib::MP4::File* mp4, const Options& opts, const fs::path& path) {
    Result res;

    bool modified = false;
    auto* mp4Tag = mp4->tag();
    auto props = mp4Tag->properties();

    if (opts.clear) {
        if (!opts.noact && mp4->strip()) {
            if (opts.verbose) std::cout << "All tags cleared\n";
            modified = true;
        }
    }
    
    if (opts.remov.size() > 0) {
            for (auto tagCmd : opts.remov) {
            auto cmds = splitOnEquals(tagCmd);
            if (cmds.first == "frontcover") {
                const auto& cover = mp4Tag->item("covr");
                if (!cover.isValid()) continue; 
                mp4Tag->removeItem("covr");
                if (opts.verbose) std::cout << "Removing cover art\n";
                modified = true;
                continue;
            }
            auto key = getAtomKey(cmds.first);
            const auto& item = mp4Tag->item(key.c_str());
            if (!item.isValid()) continue;
            mp4Tag->removeItem(key.c_str());
            if (opts.verbose) std::cout << "Removing " << cmds.first << "\n";
            modified = true;
        }
    }
    
    for (auto tagCmd : opts.tag) {
        auto cmds = splitOnEquals(tagCmd);
        auto key = getAtomKey(cmds.first);
        if (cmds.first == "tracknumber" || cmds.first == "discnumber") {
            auto item = TagLib::MP4::Item(stoi(cmds.second), 0);
            mp4Tag->setItem(key.c_str(), item);
            if (opts.verbose) std::cout << "Setting " << cmds.first <<  " = " << cmds.second << "\n";
            modified = true;
            continue;
        }
        auto item = TagLib::MP4::Item(TagLib::StringList(TagLib::String(cmds.second)));
        mp4Tag->setItem(key.c_str(), item);
        if (opts.verbose) std::cout << "Setting " << cmds.first <<  " = " << cmds.second << "\n";
        modified = true;
    }

    if (opts.fn2tag != "") {
        auto tags = fn2tag(path.string(), opts.fn2tag);
        for (auto const& tag : tags) {
            auto key = getAtomKey(tag.first);
            if (toLower(tag.first) == "tracknumber" || toLower(tag.first) == "discnumber") {
                auto item = TagLib::MP4::Item(stoi(tag.second), 0);
                mp4Tag->setItem(key.c_str(), item);
            }
            else {
                std::cout << "tag.second: " << tag.second << "\n";
                auto item = TagLib::MP4::Item(TagLib::StringList(TagLib::String(tag.second)));
                mp4Tag->setItem(key.c_str(), item);
            }

            if (opts.verbose) std::cout << "Setting " << tag.first << " = " << tag.second << "\n";
            modified = true;
        }
    }

    
    std::string newFname = path.string();
    
    if (opts.tag2fn != "") {
        newFname = tag2fn(props, opts.tag2fn, opts.verbose);
        if (newFname != "" && newFname != path.string()) {
            modified = true;            
        }
    }

    fs::path newPath(newFname);
    res.newPath = path;
    if (newPath != path) {
        res.newPath = newPath;
    }
    
    std::string imgName = "";

    for (auto tagCmd : opts.binary) {
        auto cmds = splitOnEquals(tagCmd);
        if (cmds.second == "") {
            imgName = cmds.first;
        }
        else imgName = cmds.second;
        if (opts.verbose) std::cout << "Setting picture: " << imgName << "\n";
        if (!setPicture(mp4Tag, imgName)) continue;
        else modified = true;
    }

    if (opts.list) {
        for (auto prop : props) {
            std::cout << prop.first.to8Bit() << ":\t" << prop.second.toString() << "\n";
        }
        const auto& coverItem = mp4Tag->item("covr");
        if (coverItem.isValid()) {
            std::cout << "Cover Art:\t" << coverItem.atomDataType() << "\n";
        }

    }

    if (opts.show.size() > 0) {
        for (auto tagCmd : opts.show) {
            auto cmds = splitOnEquals(tagCmd);
            auto key = getAtomKey(cmds.first);
            auto prop = props.find(key.c_str());
            if (prop != props.end()) {
                std::cout << prop->first.to8Bit() << ":\t" << prop->second.toString() << "\n";
            }
        }
    }

    if (opts.noact) {
        for (const auto& prop : props) {
            std::cout << prop.first.to8Bit() << ":\t" << prop.second.toString() << "\n";
        }
        if (path != res.newPath) {
            std::cout << "New filename: " << res.newPath << "\n";
            res.newPath = path;
        }
        modified = false;
    }


    res.success = false;

    if (modified && mp4->save()) {
        res.success = true;
        std::cout << "Saved\n";
    }
    return res;
}   


