#ifndef APE_H
#define APE_H

#include <taglib/apefile.h>
#include <taglib/apefooter.h>
#include <taglib/apeitem.h>
#include <taglib/apetag.h>
#include <filesystem>
#include "main.h"

namespace fs = std::filesystem;

bool addApeTag(TagLib::APE::Tag* apeTag, const Options& opts);
bool removeApeTag(TagLib::APE::File* ape, 
    const TagLib::String& key, 
    const TagLib::String& value, 
    const Options& opts);
void listApeTag(TagLib::APE::Tag* apeTag, const Options& opts);
bool addBinary(TagLib::APE::Tag* apeTag, const std::string& path, const std::string& key, const Options& opts);
bool tagFromFn(TagLib::APE::Tag* apeTag, const Options& opts, const std::string& path);
bool tagAPE(TagLib::APE::File* ape, const Options& opts, const fs::path& path);

#endif