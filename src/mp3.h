#include <iostream>
#include <taglib/mpegfile.h>
#include "main.h"
#include "helpers.h"
#include <taglib/id3v2.h>
#include <taglib/id3v2frame.h>
#include <taglib/tstring.h>
#include <taglib/tstringlist.h>


TagLib::String IDToKey(TagLib::ByteVector id);
TagLib::ByteVector keyToID(TagLib::String key);
// TagLib::ID3v2::Frame* createTextFrame(TagLib::ID3v2::Tag* id3v2, TagLib::String key, TagLib::String value, bool overwrite);
TagLib::ID3v2::Frame* createTextFrame(TagLib::ID3v2::Tag* tag,
                                  const TagLib::String &key,
                                  const TagLib::String &value,
                                  bool overwrite = false);
int tagMP3(TagLib::MPEG::File* mp3, const Options& opts);