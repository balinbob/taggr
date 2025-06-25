#include "main.h"
#include "magic.h"
#include "helpers.h"
#include "taglib/fileref.h"
#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>
#include <taglib/tpropertymap.h>
#include <string>

namespace fs = std::filesystem;
//namespace mp4 = TagLib::MP4;

std::string getAtomKey(const std::string& atom);
bool setPicture(TagLib::MP4::Tag* mp4Tag, const std::string& imagePath);
Result tagMP4(TagLib::MP4::File* mp4, const Options& opts, const fs::path& path);

