#pragma once

#include <iostream>
#include <taglib/mpegfile.h>
#include "main.h"
#include "helpers.h"
#include <taglib/id3v2.h>
#include <taglib/id3v2frame.h>
#include <taglib/tstring.h>
#include <taglib/tstringlist.h>
#include <filesystem>

namespace fs = std::filesystem;

TagLib::String IDToKey(TagLib::ByteVector id);
TagLib::ByteVector keyToID(TagLib::String key);
TagLib::ID3v2::Frame* createTextFrame(TagLib::ID3v2::Tag* tag,
                                  const TagLib::String &key,
                                  const TagLib::String &value,
                                  TagLib::MPEG::File* mp3,
                                  bool overwrite = false);
bool removeTextFrame(TagLib::ID3v2::Tag* tag, const TagLib::String& key, const TagLib::String & value, bool verbose);
bool removeUserTextFrame(TagLib::ID3v2::Tag* id3v2, const TagLib::String& desc, const TagLib::String& value, bool verbose);
bool removePicture(TagLib::MPEG::File* mp3, const std::string& key, const std::string& value, const Options& opts);
TagLib::ID3v2::Frame* createCommentFrame(TagLib::ID3v2::Tag* id3v2, const TagLib::String& comment);
bool tagMP3(TagLib::MPEG::File* mp3, const Options& opts, const fs::path& path);