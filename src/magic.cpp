#include "main.h"
#include "magic.h"
#include "helpers.h"
#include "flac.h"
#include "ape.h"
#include "mp3.h"
#include "mp4.h"
#include "wv.h"
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/apefile.h>
#include <taglib/mpegfile.h>
#include <taglib/wavpackfile.h>
#include "ogg.h"
#include "taglib/mp4tag.h"
#include "taglib/opusfile.h"
#include "opus.h"

namespace fs = std::filesystem;

Result doMagic(const fs::path& path, const Options& opts) {
    Result res;
    TagLib::FileRef f(path.string().c_str());
    if (f.isNull() || !f.file()->isValid()) {
        std::cout << "Not a taggable file: " << path << "\n";
        res.success = false;
        return res;
    }
    if (!opts.quiet) std::cout << "Processing " << path << "\n";

    if (auto* flac = dynamic_cast<TagLib::FLAC::File*>(f.file())) {

        res = tagFLAC(flac, opts, path);
    }
    else if (auto* ogg = dynamic_cast<TagLib::Ogg::Vorbis::File*>(f.file())) {
        res = tagOGG(ogg, opts, path);
    }
    else if (auto* oggflac = dynamic_cast<TagLib::Ogg::FLAC::File*>(f.file())) {
    
    }
    else if (auto* opus = dynamic_cast<TagLib::Ogg::Opus::File*>(f.file())) {
        res = tagOPUS(opus, opts, path);
    }
    else if (auto* ape = dynamic_cast<TagLib::APE::File*>(f.file())) {
        res = tagAPE(ape, opts, path);
    }
    else if (auto* mp3 = dynamic_cast<TagLib::MPEG::File*>(f.file())) {
        res = tagMP3(mp3, opts, path);
    }
    else if (auto* mp4 = dynamic_cast<TagLib::MP4::File*>(f.file())) {
        res = tagMP4(mp4, opts, path);
    }
    else if (auto* wv = dynamic_cast<TagLib::WavPack::File*>(f.file())) {
        res = tagWV(wv, opts, path);
    }   
    else {
        std::cout << "Unknown taggable file: " << path << "\n";
        res.success = false;
    }
    return res;
}