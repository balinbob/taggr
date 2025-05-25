#include "main.h"
#include <iostream>
#include <filesystem>
#include "taglib/fileref.h"
#include "taglib/tag.h"
#include "taglib/flacfile.h"

int tagFLAC(TagLib::FLAC::File* flac, const Options& opts);