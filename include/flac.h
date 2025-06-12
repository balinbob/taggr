#include "main.h"
#include <iostream>
#include <filesystem>
#include "taglib/fileref.h"
#include "taglib/tag.h"
#include "taglib/flacfile.h"

namespace fs = std::filesystem; 

bool addPicture(TagLib::FLAC::File* flac, const std::string& path, const std::string& key, const Options& opts);
bool tagFLAC(TagLib::FLAC::File* flac, const Options& opts, const fs::path& path);