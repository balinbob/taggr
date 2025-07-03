#include <string>
#include <iostream>
#include <filesystem>
#include "helpers.h"
#include "flac.h"
#include "fn2tag.h"
#include "tag2fn.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/flacfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/flacpicture.h>
#include <taglib/oggflacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/oggfile.h>
#include <taglib/tpropertymap.h>
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

Result tagOGG(ogg::Vorbis::File* ogg, const Options& opts, const fs::path& path) {
    bool modified = false;
    
    auto* vc = ogg->tag();

    if (removeTags(vc, opts)) modified = true;

    
    
/*    
    for (auto& cmd : opts.remov) {
        auto cmds = splitOnEquals(cmd);
        TagLib::List<TagLib::FLAC::Picture*> const pics = vc->pictureList();
        for (auto const& pic : pics) {
            if (pic->type() == static_cast<TagLib::FLAC::Picture::Type>(pictureTypeFromKey(cmds.first))) {
                vc->removePicture(pic);
                if (opts.verbose) std::cout << "Removing " << cmds.first << "\n";
                modified = true;
            }    
        }    
    }

    // can't remove pictures from ogg vorbis (yet)
*/        
    if (opts.clear) {
        vc->removeAllFields();
        vc->removeAllPictures();
        if (opts.verbose) std::cout << "Clearing all tags\n";
        modified = true;
    }

    if (addTags(vc, opts)) modified = true;

    for (auto& cmd : opts.binary) {
        auto cmds = splitOnEquals(cmd);
        if (addPicture(ogg, cmds.second, cmds.first, opts)) modified = true;
    }

    if (opts.fn2tag != "") {
        auto tags = fn2tag(path.string(), opts.fn2tag);
        if (opts.verbose) std::cout << "Found " << tags.size() << " tags from " << path.string() << "\n";
        for (const auto& tag : tags) {
            vc->addField(tag.first.c_str(), tag.second.c_str(), true);
            if (opts.verbose) std::cout << "Setting " << tag.first << " = " << tag.second << "\n";
            modified = true;
        }
    }
 
    std::string newFname = path.string();

    if (opts.tag2fn != "") {
        newFname = tag2fn(vc->properties(), opts.tag2fn, opts.verbose);
        if (newFname != path.string()) {
            if (opts.verbose) std::cout << "Renaming to " << newFname << "\n";
            modified = true;
        }
    }

    fs::path newPath(newFname);
    if (newPath.is_absolute()) {
        std::cout << "Cannot rename to absolute path\nAborting name change!\n";
        newPath = path;
    }

    Result res;
    res.newPath = newPath;

    if (opts.noact) {
        const auto& props = vc->properties();
        for (const auto& prop : props) {
            std::cout << prop.first.to8Bit() << ": " << prop.second.toString() << "\n";
        }
        if (path != newPath) std::cout << path.string() << " -> " << newPath.string() << "\n";
        modified = false;
    }

    if (opts.show.size() > 0) showTag(vc, opts);
    
    if (opts.list) listTags(vc, opts);
       
    if (modified) {
        if (ogg->save()) {
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