#include "main.h"
#include "magic.h"
#include <filesystem>
#include <taglib/wavpackfile.h>

namespace fs = std::filesystem;

int tagWV(TagLib::WavPack::File* wv, const Options& opts, const fs::path& path);