#include "main.h"
#include "magic.h"
#include "helpers.h"
#include "flac.h"
#include "ape.h"
#include "mp3.h"
#include "wv.h"
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/flacfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/vorbisproperties.h>
#include <taglib/oggflacfile.h>
#include <taglib/apefile.h>
#include <taglib/mpegfile.h>
#include <taglib/wavpackfile.h>
#include "ogg.h"

namespace fs = std::filesystem;

Result doMagic(const fs::path& path, const Options& opts) {
    Result res;
    TagLib::FileRef f(path.string().c_str());
    if (f.isNull() || !f.file()->isValid()) {
        std::cout << "Not a taggable file: " << path << "\n";
        res.success = false;
    }
    if (!opts.quiet) std::cout << "Processing " << path << "\n";

    if (auto* flac = dynamic_cast<TagLib::FLAC::File*>(f.file())) {

        res = tagFLAC(flac, opts, path);
//        if (!result) return result;
    }
    else if (auto* ogg = dynamic_cast<TagLib::Ogg::Vorbis::File*>(f.file())) {
        int result = tagOGG(ogg, opts, path);
//        if (result != 0) return result;
    }
    else if (auto* oggflac = dynamic_cast<TagLib::Ogg::FLAC::File*>(f.file())) {
    
    }
    else if (auto* ape = dynamic_cast<TagLib::APE::File*>(f.file())) {
        res = tagAPE(ape, opts, path);
//        if (result != 0) return result;   
    }
    else if (auto* mp3 = dynamic_cast<TagLib::MPEG::File*>(f.file())) {
        int result = tagMP3(mp3, opts, path);
//        if (result != 0) return result;        
    }
    else if (auto* wv = dynamic_cast<TagLib::WavPack::File*>(f.file())) {
        int result = tagWV(wv, opts, path);
//        if (result != 0) return result;
    }   
    else {
        std::cout << "Unknown taggable file: " << path << "\n";
        res.success = false;
    }
 
    return res;
 
}