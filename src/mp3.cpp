#include <iostream>
#include "main.h"
#include "helpers.h"
#include "mp3.h"
#include <taglib/mpegfile.h>
#include <taglib/id3v2.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/tbytevector.h>
#include <taglib/tfile.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tstring.h>
#include <taglib/tstringlist.h>
#include <taglib/tpropertymap.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v1genres.h>
#include <taglib/id3v1tag.h>
#include <taglib/attachedpictureframe.h>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

TagLib::String IDToKey(TagLib::ByteVector id) {
    return TagLib::ID3v2::Frame::frameIDToKey(id);
}

TagLib::ByteVector keyToID(TagLib::String key) {
    return TagLib::ID3v2::Frame::keyToFrameID(key);
}

bool removeUserTextFrame(TagLib::ID3v2::Tag* id3v2, const TagLib::String& desc, const TagLib::String& value, bool verbose) {
    TagLib::ID3v2::FrameList txxxFrames = id3v2->frameListMap()["TXXX"];
    bool removed = false;

    for (auto it = txxxFrames.begin(); it != txxxFrames.end(); ) {
        auto userTextFrame = dynamic_cast<TagLib::ID3v2::UserTextIdentificationFrame*>(*it);
        if (userTextFrame) {
            // does the "key" match?
            if ((userTextFrame->description() == desc) &&
                // if there was a value passed, it must match too, else remove the whole frame
                (value.isEmpty() || userTextFrame->fieldList().size() > 1 && userTextFrame->fieldList()[1] == value))
                {
                    if (verbose) std::cout << "Removing TXXX frame: " << userTextFrame->description();
                    if (verbose && !value.isEmpty()) std::cout << value;
                    if (verbose) std::cout << "\n";
                    id3v2->removeFrame(userTextFrame);
                    it = txxxFrames.erase(it);
                    removed = true;
                    continue;
                }
        }
        ++it;
    }
    return removed;
}


/**
 * Creates or updates a text frame in the provided ID3v2 tag.
 *
 * This function generates a new text frame associated with the specified `key` and
 * incorporates the `value` into the list of text values. If a frame with the same
 * `key` already exists and `overwrite` is true, all existing frames with that key
 * are removed before adding the new value. If `overwrite` is false, the new value
 * is appended unless it is a duplicate of an existing value.
 *
 * @param tag The ID3v2 tag to which the frame should be added.
 * @param key The text key identifying the frame type to create or update.
 * @param value The text value to add to the frame's text list.
 * @param overwrite Determines whether existing frames with the same key should be
 *                  cleared before adding the new frame.
 * @return A pointer to the created or updated TextIdentificationFrame, or nullptr
 *         if the operation was unsuccessful (e.g., an invalid key or a duplicate
 *         value when not overwriting).
 * 
 * This function is designed to handle the creation and management of text frames
 * - used in --tag and --add operations
 */
TagLib::ID3v2::Frame* createTextFrame(TagLib::ID3v2::Tag* tag,
                  const TagLib::String &key,
                  const TagLib::String &value,
                  TagLib::MPEG::File* mp3,
                  bool overwrite)
{
    using namespace TagLib::ID3v2;
    
    auto id = keyToID(key);

    if (id.isEmpty() && key != "PICTURE" && key != "GENERALOBJECT") {
        UserTextIdentificationFrame *userTextFrame = new UserTextIdentificationFrame(key,
                                                TagLib::StringList(value),
                                                TagLib::String::UTF8);           
        
        if (overwrite) {
            // --tag ging so remove existing TXXX:Desc frames
            if (removeUserTextFrame(tag, key, "", false)) {
                mp3->save();
            };
            // If overwriting, remove all existing frames with this key
        }
        else {
            // Check if the frame already exists
            auto existingFrames = tag->frameList(id);
            for (auto *f : existingFrames) {
                if (auto *userTextFrame = dynamic_cast<UserTextIdentificationFrame*>(f)) {
                    // If it exists, append the value to the existing frame
                    userTextFrame->setText(userTextFrame->toStringList().append(value));
                }
            }
        }
        return userTextFrame;
    }


    // 1) Collect existing values
    TagLib::StringList values;
    auto frames = tag->frameList(id);
    for (auto *f : frames) {
        if (auto *tf = dynamic_cast<TextIdentificationFrame*>(f)) {
            for (auto &s : tf->toStringList())
                values.append(s);
        }
    }

    // 2) Optionally clear out all old frames of that ID
    if (overwrite) {
        tag->removeFrames(id);
        values.clear();
    }

    // 3) Avoid exact duplicates
    if (!overwrite && values.contains(value))
        return nullptr;  // No need to create a new frame, just return.

    // 4) Append the new value
    values.append(value);

    // 5) Build and add a single new frame carrying the full list
    //    (removes the need to manage multiple frames manually)
    tag->removeFrames(id);  // ensure only one frame remains
    auto *tf = new TextIdentificationFrame(id, TagLib::String::UTF8);
    tf->setText(values);
    return tf;
    // tag->addFrame(tf);
}

bool removeTextFrame(TagLib::ID3v2::Tag* tag, 
                    const TagLib::String &key,
                    const TagLib::String &value,
                    bool verbose) {
    auto props = tag->properties();
    if (!props.contains(key)) {
        if (verbose) std::cout << "couldn't find key: " << key << "\n";
        return false;
    } 
    
    if (!value.isEmpty()) {
        if (!props[key].contains(value)) {
            if (verbose) std::cout << key << " has no value " << value << "\n";
            return false;
        }
        auto it = props[key].find(value);
        props[key].erase(it);
        if (props[key].isEmpty()) {
            props.erase(key);
            if (verbose) std::cout << "Removing " << key << "\n";
        }
        else {
            if (verbose) std::cout << "Removing " << key << " = " << value << "\n";
        }
    } 
    else {
        props.erase(key);
        if (verbose) std::cout << "Removing " << key << "\n";
    }
    tag->setProperties(props);
    return true;
}

bool addPicture(TagLib::MPEG::File* mp3,
                const std::string& path, 
                const std::string& key, 
                const Options& opts) {
    
    std::string mimetype;
    fs::path p(path);
    std::string ext = p.extension().string();
    if (toLower(ext) == ".jpg") mimetype = "image/jpeg";
    else if (toLower(ext) == ".png") mimetype = "image/png";
    else {
        std::cout << "Unsupported file type: " << ext << "\n";
        return false;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Couldn't open file: " << path << "\n";
        return false;
    }
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());    

    TagLib::ID3v2::Tag* id3v2 = mp3->ID3v2Tag(true);
    TagLib::ID3v2::AttachedPictureFrame* frame = new TagLib::ID3v2::AttachedPictureFrame;
    frame->setMimeType(mimetype);
    frame->setDescription(key);
    frame->setType(static_cast<TagLib::ID3v2::AttachedPictureFrame::Type>(pictureTypeFromKey(key)));
    frame->setTextEncoding(TagLib::String::UTF8);
    frame->setPicture(TagLib::ByteVector(data.data(), data.size()));
    id3v2->addFrame(frame);
    if (opts.verbose) std::cout << "Adding " << key << "\n";
    return true;
}

bool removePicture(TagLib::MPEG::File* mp3, 
                   const std::string& key, 
                   const std::string& value, 
                   const Options& opts) {

    bool modified = false;
    TagLib::ID3v2::Tag* tag = mp3->ID3v2Tag();
    if (!tag) return false;
    int type = pictureTypeFromKey(key);  // ie frontcover
    TagLib::ID3v2::FrameList frames = tag->frameList("APIC");
    for (auto it = frames.begin(); it != frames.end(); /* no increment here */) {
        TagLib::ID3v2::AttachedPictureFrame* pic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(*it);
        if (pic) {
            if (pic->type() == type) {
                tag->removeFrame(pic);
                it = frames.erase(it);
                if (opts.verbose) std::cout << "Removing " << pictureTypeToString(type) << "\n";
                modified = true;
            } 
            else {
               ++it;
            }
        }
    }    
    return modified; 
}

bool tagMP3(TagLib::MPEG::File* mp3, const Options& opts) {
    TagLib::ID3v2::Tag* id3v2 = mp3->ID3v2Tag(true);
    bool modified = false;
    bool frameModified = false;
    auto const& frameMap = id3v2->frameListMap();

    for (auto& cmd : opts.remov) {
        mp3->strip(TagLib::MPEG::File::ID3v1, true);
        auto cmds = splitOnEquals(cmd);
        auto const& keyToRemove = cmds.first;
        auto const& frameID = keyToID(keyToRemove);
        if (frameID != "") if (removeTextFrame(id3v2, cmds.first, cmds.second, opts.verbose)) modified = true;
        if (frameID == "") if (removeUserTextFrame(id3v2, cmds.first, cmds.second, opts.verbose)) modified = true;
        
        // try to remove it as a picture
        if (!modified && removePicture(mp3, cmds.first, cmds.second, opts)) modified = true;
 
        if (opts.verbose && modified) {
            std::cout << "Removing " << cmds.first;
            if (cmds.second != "") std::cout << " = " << cmds.second;
            std::cout << "\n";
        }
    }

    if (opts.clear) {
        if (opts.verbose) std::cout << "Clearing all ID3v2 frames\n";
        mp3->strip();
        id3v2 = mp3->ID3v2Tag(true);  // Re-fetch the tag after stripping
        modified = true;
    }

    for (auto& tagCmd : opts.tag) {
        auto tags = splitOnEquals(tagCmd);
        if (tags.second == "") {
            std::cerr << "No value specified for tag: " << tags.first << "\n";
            continue;
        }

        TagLib::ID3v2::Frame* frame = createTextFrame(id3v2, tags.first, tags.second, mp3, true);
        if (frame == nullptr) {
            std::cerr << "Failed to create frame for " << tags.first << "\n";
            continue;
        }
        id3v2->addFrame(frame);

        if (opts.verbose) std::cout << "Setting " << tags.first << " = " << tags.second << "\n";
        modified = true;
    }
    
    for (auto& tagCmd : opts.add) {
        auto tags = splitOnEquals(tagCmd);
        if (tags.second == "") {
            std::cerr << "No value specified for tag: " << tags.first << "\n";
            continue;
        }

        TagLib::ID3v2::Frame* frame = createTextFrame(id3v2, tags.first, tags.second, mp3, false);
        if (frame == nullptr) {
            std::cerr << "Failed to create frame for " << tags.first << "\n";
            continue;
       }
        id3v2->addFrame(frame);

        if (opts.verbose) std::cout << "Adding " << tags.first << " = " << tags.second << "\n";
        modified = true;
   }

    for (auto& tagCmd : opts.binary) {
        auto cmds = splitOnEquals(tagCmd);
        if (!addPicture(mp3, cmds.second, cmds.first, opts)) return 1;
        else modified = true;
    }

    if (opts.show.size() > 0) {
        for (const auto& key : opts.show) {
            TagLib::ByteVector id = keyToID(key);
            if (id.isEmpty()) {
                std::cerr << "Unknown frame key: " << key << "\n";
                continue;
            }
            auto frames = id3v2->frameList(id);
            if (frames.isEmpty()) {
                if (opts.verbose) std::cout << "No frames found for key: " << key << "\n";
                continue;
            }
            if (opts.verbose) std::cout << "Frames for key: " << key << "\n";
            for (const auto& frame : frames) {
                std::cout << frame->toString() << "\n";
            }
        }
    }
    if (opts.list) {
        if (mp3->hasID3v1Tag()) {
            auto id3v1 = mp3->ID3v1Tag();
            std::cout << "ID3v1 tag:\n";
            std::cout << "Title: " << id3v1->title().to8Bit() << "\n";
            std::cout << "Artist: " << id3v1->artist().to8Bit() << "\n";
            std::cout << "Album: " << id3v1->album().to8Bit() << "\n";
            std::cout << "Year: " << id3v1->year() << "\n";
            std::cout << "Comment: " << id3v1->comment().to8Bit() << "\n";
            std::cout << "Genre: " << id3v1->genre().to8Bit() << "\n";
        }

        for (const auto& entry : frameMap) {
            const auto& frameID = entry.first;
            const auto& frames = entry.second;
            auto key = IDToKey(frameID);
            for (auto& frame : frames) {
                if (frame->frameID() == "APIC") {
                    auto val = reinterpret_cast<TagLib::ID3v2::AttachedPictureFrame*>(frame);
                    
                    std::cout << pictureTypeToString(val->type()) << ":\t " << val->toString().to8Bit() << "\n";
                    continue;
                }
                auto txt = reinterpret_cast<TagLib::ID3v2::TextIdentificationFrame*>(frame)
                    ->toString();
                std::cout << key << ":\t" << txt.to8Bit() << "\n";
            }
        }
    }

    if (modified) {
        if (mp3->save()) {
            if (opts.verbose) std::cout << "Saved\n";
            return true;
        }
        else {
            std::cerr << "Failed to save\n";
            return false;
        }
    }
    return false;
}