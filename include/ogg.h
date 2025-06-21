#pragma once

#include <string>
#include <filesystem>
#include "main.h"
#include "helpers.h"
#include <taglib/vorbisfile.h>
#include <taglib/oggfile.h>

bool addPicture(TagLib::Ogg::Vorbis::File* ogg, const std::string& path, const std::string& key, const Options& opts);
Result tagOGG(TagLib::Ogg::Vorbis::File* ogg, const Options& opts, const fs::path& path);