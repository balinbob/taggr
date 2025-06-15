#include <string>
#include <iostream>
#include <filesystem>
#include "helpers.h"
#include "flac.h"
#include "fn2tag.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/flacfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/flacpicture.h>
#include <taglib/oggflacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/oggfile.h>
#include <fstream>

namespace fs = std::filesystem;
namespace ogg = TagLib::Ogg;

bool addPicture(ogg::Vorbis::File* ogg, const std::string& path, const std::string& key, const Options& opts) {
    TagLib::FLAC::Picture *pic = new TagLib::FLAC::Picture;
    auto* vc = ogg->tag();    
    std::string mimetype;
    fs::path p(path);
    std::string ext = p.extension().string();
    if (toLower(ext) == ".jpg") mimetype = "image/jpeg";
    else if (toLower(ext) == ".png") mimetype = "image/png";
    pic->setMimeType(mimetype);
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        if (opts.verbose) std::cout << "Couldn't open file: " << path << "\n";
        return false;
    }
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    pic->setData(TagLib::ByteVector(data.data(), data.size()));
    pic->setType(static_cast<TagLib::FLAC::Picture::Type>(pictureTypeFromKey(key)));
    vc->addPicture(pic);
    if (opts.verbose) std::cout << "Adding " << key << "\n";
    return true;
}

bool tagOGG(ogg::Vorbis::File* ogg, const Options& opts, const fs::path& path) {
    bool modified = false;
    if (!ogg->isValid()) {
        std::cout << "Not a taggable file: " << path << "\n";
        return false;
    }
    
    auto* vc = ogg->tag();
    for (auto& cmd : opts.remov) {
        auto cmds = splitOnEquals(cmd);
        if (cmds.second == "") vc->removeFields(cmds.first.c_str());
        else vc->removeFields(cmds.first.c_str(), cmds.second.c_str());
        if (opts.verbose) {
            std::cout << "Removing " << cmds.first;
            if (cmds.second != "") std::cout << " = " << cmds.second;
            std::cout << "\n";
        }
        modified = true;
    }
    if (opts.clear) {
        vc->removeAllFields();
        if (opts.verbose) std::cout << "Clearing all tags\n";
        modified = true;
    }

    for (auto& cmd : opts.tag) {
        auto cmds = splitOnEquals(cmd);
        if (cmds.second == "") {
            if (opts.verbose) std::cout << "Tag value required for " << cmds.first << "\n";
            continue;
        }
        vc->addField(cmds.first.c_str(), cmds.second.c_str(), true);
        if (opts.verbose) {
            std::cout << "Setting " << cmds.first << " = " << cmds.second << "\n";
        }
        modified = true;
    }

    for (auto& cmd : opts.add) {
        auto cmds = splitOnEquals(cmd);
        if (cmds.second == "") {
            if (opts.verbose) std::cout << "Tag value required for " << cmds.first << "\n";
            continue;
        }
        vc->addField(cmds.first.c_str(), cmds.second.c_str(), false);
        if (opts.verbose) {
            std::cout << "Adding " << cmds.first << " = " << cmds.second << "\n";
        }
        modified = true;
    }

    for (auto& cmd : opts.binary) {
        auto cmds = splitOnEquals(cmd);
        if (addPicture(ogg, cmds.second, cmds.first, opts)) modified = true;
    }

    for (auto& cmd : opts.fn2tag) {
        auto tags = fn2tag(path.string(), opts.fn2tag);
        for (const auto& tag : tags) {
            vc->addField(tag.first.c_str(), tag.second.c_str(), true);
            if (opts.verbose) std::cout << "Setting " << tag.first << " = " << tag.second << "\n";
            modified = true;
        }
    }
 
    if (opts.show.size() > 0) showTag(vc, opts);
    
    if (opts.list) listTags(vc, opts);
    
    if (modified) {
        if (ogg->save()) {
            if (opts.verbose) std::cout << "Saved\n";
        }
        else {
            if (opts.verbose) std::cout << "Failed to save\n";
            modified = false;
        }
    }
    return modified;
}