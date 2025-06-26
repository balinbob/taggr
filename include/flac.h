#ifndef FLAC_H
#define FLAC_H

#include "main.h"
#include "helpers.h"
#include <filesystem>
#include "taglib/fileref.h"
#include "taglib/flacfile.h"

namespace fs = std::filesystem; 
namespace ogg = TagLib::Ogg;

bool addPicture(TagLib::FLAC::File* flac, const std::string& path, const std::string& key, const Options& opts);
void showTag(ogg::XiphComment* vc, const Options& opts);
void listTags(ogg::XiphComment* vc, const Options& opts);
bool addTags(ogg::XiphComment* vc, const Options& opts);
bool removeTags(ogg::XiphComment* vc, const Options& opts);
Result tagFLAC(TagLib::FLAC::File* flac, const Options& opts, const fs::path& path);
#endif