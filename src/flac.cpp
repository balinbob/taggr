#include "main.h"
#include "helpers.h"
#include "flac.h"
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/flacfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/tpropertymap.h>
#include <taglib/flacpicture.h>

namespace fs = std::filesystem;

bool addPicture(TagLib::FLAC::File* flac, 
                const std::string& path, 
                const std::string& key, 
                const Options& opts) {
    TagLib::FLAC::Picture *pic = new TagLib::FLAC::Picture;
    
    std::string mimetype;
    fs::path p(path);
    std::string ext = p.extension().string();
    if (toLower(ext) == ".jpg") mimetype = "image/jpeg";
    else if (toLower(ext) == ".png") mimetype = "image/png";
    pic->setMimeType(mimetype);

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Couldn't open file: " << path << "\n";
        return false;
    }
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());    
    
    pic->setData(TagLib::ByteVector(data.data(), data.size()));
    pic->setType(static_cast<TagLib::FLAC::Picture::Type>(pictureTypeFromKey(key)));
    flac->addPicture(pic);
    if (opts.verbose) std::cout << "Adding " << key << "\n";
    return true;
}

bool tagFLAC(TagLib::FLAC::File* flac, const Options& opts) {
    bool modified = false;
    auto* vc = flac->xiphComment(true);

    for (auto& cmd : opts.remov) {
        auto cmds = splitOnEquals(cmd);
        if (cmds.second == "") vc->removeFields(cmds.first.c_str());
        else vc->removeFields(cmds.first.c_str(), cmds.second.c_str());
            
        TagLib::List<TagLib::FLAC::Picture*> const pics = flac->pictureList();
        for (auto const& pic : pics) {
            if (pic->type() == static_cast<TagLib::FLAC::Picture::Type>(pictureTypeFromKey(cmds.first))) {
                flac->removePicture(pic);
            }
        }
        
        if (opts.verbose) {
            std::cout << "Removing " << cmds.first;
            if (cmds.second != "") std::cout << " = " << cmds.second;
            std::cout << "\n";
        }
        modified = true;
    }

    for (auto& cmd : opts.tag) {
        auto cmds = splitOnEquals(cmd);
        if (cmds.second == "") {
            if (opts.verbose) std::cout << "Cannot set empty tag value\n";
            return false;
        }
        vc->addField(cmds.first.c_str(), cmds.second.c_str());
        if (opts.verbose) std::cout << "Setting " << cmds.first << " = " << cmds.second << "\n";
        modified = true;
    }
    
    for (auto& cmd : opts.add) {
        auto cmds = splitOnEquals(cmd);
        if (cmds.second == "") { 
            if (opts.verbose) std::cout << "Cannot add empty tag value\n";
            return false;
        }
        vc->addField(cmds.first.c_str(), cmds.second.c_str(), false);
        if (opts.verbose) std::cout << "Adding " << cmds.first << " = " << cmds.second << "\n";
        modified = true;
    }
    
    TagLib::PropertyMap const props = vc->properties();
    for (auto& cmd : opts.show) {
        auto const cmds = splitOnEquals(cmd);
        auto const prop = props.find(cmds.first.c_str());
        TagLib::StringList values = prop->second;
        if (prop != props.end()) {
            if (cmds.second == "") {
                std::cout << prop->first.to8Bit() << ":";
                    for (auto const& val : values) {
                        std::cout << "\t" << val.to8Bit() << "\n";
                    }
            }
            else {
                for (auto const& val : values) {
                    if (val == cmds.second.c_str()) {
                        std::cout << prop->first.to8Bit() << ":\t" << val.to8Bit() << "\n";
                    }
                }
            }
        }
    }

    for (auto& cmd : opts.binary) {
        auto cmds = splitOnEquals(cmd);
        if (addPicture(flac, cmds.second, cmds.first, opts)) {
            modified = true;
        }
    }
    
    if (opts.list) {
        TagLib::PropertyMap const props = vc->properties();
        for (auto const& prop : props) {
            std::cout << prop.first.to8Bit() << ":\t";
            std::cout << prop.second.toString() << "\n";
        }
        TagLib::List<TagLib::FLAC::Picture*> const pics = flac->pictureList();
        for (auto const& pic : pics) {
            std::cout << pictureTypeToString(pic->type()) << "\t" << pic->description().to8Bit() << "\n";
        }
    }
    
    if (opts.clear) {
        flac->strip();
        flac->removePictures();
        if (opts.verbose) std::cout << "Clearing all tags\n";
        modified = true;
    }

    if (modified) {
        if (flac->save()) {
            if (opts.verbose) std::cout << "Saved\n";
            return true;
        }
        else return false;
    }
    return true;
}