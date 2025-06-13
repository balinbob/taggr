#include <string>
#include <iostream>
#include <filesystem>
#include "main.h"
#include "magic.h"
#include "helpers.h"
#include "flac.h"
#include "fn2tag.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/flacfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/oggfile.h>

bool addPicture(TagLib::Ogg::Vorbis::File* ogg, const std::string& path, const std::string& key, const Options& opts);
bool tagOGG(TagLib::Ogg::Vorbis::File* ogg, const Options& opts, const fs::path& path);