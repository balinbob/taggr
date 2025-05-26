#include "main.h"
#include "magic.h"
#include "helpers.h"
#include "flac.h"
#include "ape.h"
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include "taglib/fileref.h"
#include "taglib/tag.h"
#include "taglib/flacfile.h"
#include "taglib/vorbisfile.h"
#include "taglib/vorbisproperties.h"
#include "taglib/oggflacfile.h"
#include "taglib/apefile.h"
#include "taglib/mpegfile.h"

namespace fs = std::filesystem;

int doMagic(const fs::path& path, const Options& opts) {
    TagLib::FileRef f(path.string().c_str());
    if (f.isNull() || !f.file()->isValid()) {
        std::cout << "Not a taggable file: " << path << "\n";
        return 1;
    }
    std::cout << "Processing " << path << "\n";
    if (auto* flac = dynamic_cast<TagLib::FLAC::File*>(f.file())) {
        int result = tagFLAC(flac, opts);
        if (result != 0) return result;
    }
    else if (auto* ogg = dynamic_cast<TagLib::Ogg::Vorbis::File*>(f.file())) {

    }
    else if (auto* oggflac = dynamic_cast<TagLib::Ogg::FLAC::File*>(f.file())) {
    
    }
    else if (auto* ape = dynamic_cast<TagLib::APE::File*>(f.file())) {
        int result = tagAPE(ape, opts);
        if (result != 0) return result;   
    }
    else if (auto* mp3 = dynamic_cast<TagLib::MPEG::File*>(f.file())) {
        
    }
    
    else {
        std::cout << "Unknown taggable file: " << path << "\n";
        return 1;
    }
    return 0;
}