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

TagLib::String IDToKey(TagLib::ByteVector id) {
    return TagLib::ID3v2::Frame::frameIDToKey(id);
}

TagLib::ByteVector keyToID(TagLib::String key) {
    return TagLib::ID3v2::Frame::keyToFrameID(key);
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
                  bool overwrite)
{
    using namespace TagLib::ID3v2;

    auto id = keyToID(key);
    if (id.isEmpty()) {
        std::cerr << "Unknown frame key: " << key << "\n";
        return nullptr;  // Invalid key, cannot create frame.
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


int tagMP3(TagLib::MPEG::File* mp3, const Options& opts) {
    TagLib::ID3v2::Tag* id3v2 = mp3->ID3v2Tag(true);
    bool modified = false;
    bool frameModified = false;
    auto const& frameMap = id3v2->frameListMap();

    
    if (opts.remov.size() > 0) {
        mp3->strip(TagLib::MPEG::File::ID3v1, true);
        for (const auto& tagCmd : opts.remov) {
            const auto& tags = splitOnEquals(tagCmd);
            for (const auto& entry : frameMap) {
                TagLib::String key = IDToKey(entry.first);
                // std::cout << key << "\n";
                auto frame = reinterpret_cast<TagLib::ID3v2::TextIdentificationFrame*>(entry.second.front());
                if (tags.second == "") {
                    if (toLower(key.to8Bit()) == toLower(tags.first)) {
        //                if (opts.verbose) std::cout  << "Removing all frames for " << keyToID(key) << "\n";
                        id3v2->removeFrames(entry.first);
                        if (opts.verbose) std::cout << "Removing " << key << "\n";
                        modified = true;
                    }
                }
                else {   // field specified
                    if (toLower(key.to8Bit()) == toLower(tags.first)) {
                        for (auto const& frame : entry.second) {
                            TagLib::StringList txtList = reinterpret_cast<TagLib::ID3v2::TextIdentificationFrame*>(frame)
                                    ->toStringList();
                            TagLib::StringList filtered;
                            for (auto const& txt : txtList) {
                                if (txt != tags.second) {
                                    filtered.append(txt);
                                }
                                else {
                                    frameModified = true;
                                }
                            }
                            if (filtered.isEmpty()) {
                                id3v2->removeFrame(frame);
                                if (opts.verbose) std::cout << "Removing " << key << "\n";
                                modified = true;
                            }
                            else if (frameModified) {
                                reinterpret_cast<TagLib::ID3v2::TextIdentificationFrame*>(frame)->setText(filtered);
                                if (opts.verbose) std::cout << "Removing " << key << " = " << tags.second << "\n";
                                modified = true;
                            }
                            
                        }
                    }
                }
            }
        }
    }

    if (opts.tag.size() > 0) {
        for (auto& tagCmd : opts.tag) {
            auto tags = splitOnEquals(tagCmd);
            if (tags.second == "") {
                std::cerr << "No value specified for tag: " << tags.first << "\n";
                continue;
            }

            TagLib::ID3v2::Frame* frame = createTextFrame(id3v2, tags.first, tags.second, true);
            if (frame == nullptr) {
                std::cerr << "Failed to create frame for " << tags.first << "\n";
                continue;
            }
            id3v2->addFrame(frame);

            if (opts.verbose) std::cout << "Setting " << tags.first << " = " << tags.second << "\n";
            modified = true;
        }
    }
    
    if (opts.add.size() > 0) {
        for (auto& tagCmd : opts.add) {
            auto tags = splitOnEquals(tagCmd);
            if (tags.second == "") {
                std::cerr << "No value specified for tag: " << tags.first << "\n";
                continue;
            }

            TagLib::ID3v2::Frame* frame = createTextFrame(id3v2, tags.first, tags.second, false);
            if (frame == nullptr) {
                std::cerr << "Failed to create frame for " << tags.first << "\n";
                continue;
            }
            id3v2->addFrame(frame);

            if (opts.verbose) std::cout << "Adding " << tags.first << " = " << tags.second << "\n";
            modified = true;
        }
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
        std::cout << "ID3v1 tag:\n";
        if (mp3->ID3v1Tag()) {
            auto id3v1 = mp3->ID3v1Tag();
            std::cout << "Title: " << id3v1->title().to8Bit() << "\n";
            std::cout << "Artist: " << id3v1->artist().to8Bit() << "\n";
            std::cout << "Album: " << id3v1->album().to8Bit() << "\n";
            std::cout << "Year: " << id3v1->year() << "\n";
            std::cout << "Comment: " << id3v1->comment().to8Bit() << "\n";
            std::cout << "Genre: " << id3v1->genre().to8Bit() << "\n";
        }
        else {
            std::cout << "No ID3v1 tag found\n";
        }   
        for (const auto& entry : frameMap) {
            const auto& frameID = entry.first;
            const auto& frames = entry.second;
            auto key = IDToKey(frameID);
            for (auto& frame : frames) {
                auto txt = reinterpret_cast<TagLib::ID3v2::TextIdentificationFrame*>(frame)
                    ->toString();
                std::cout << key << ":\t" << txt.to8Bit() << "\n";
            }
        }
    }

    if (modified) mp3->save();
    return 0;
}



