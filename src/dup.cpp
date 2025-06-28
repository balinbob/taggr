#include <iostream>
#include <taglib/fileref.h>
#include <taglib/tpropertymap.h>
#include "dup.h"

bool dupTags(const std::string& src, const std::string& dest, const Options& opts) {
    TagLib::FileRef srcFile(src.c_str());
    TagLib::FileRef destFile(dest.c_str());
    if (srcFile.isNull() || destFile.isNull()) {
        std::cerr << "Could not open file.\n";
        return false;
    }
    TagLib::PropertyMap srcProps = srcFile.file()->properties();
    if (opts.verbose || opts.noact) {
        std::cout << "Copying tags from " << src << " to " << dest << ":\n";
        for (const auto& prop : srcProps) {
            std::cout << "  " << prop.first << " = " << prop.second << "\n";
        }
    }
    if (!opts.noact) {
        destFile.file()->setProperties(srcProps);
        destFile.file()->save();
        return true;
    }
    return false;
}
