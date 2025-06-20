#include "main.h"
#include "magic.h"
#include <filesystem>
#include <taglib/wavpackfile.h>
#include "helpers.h"


namespace fs = std::filesystem;

Result tagWV(TagLib::WavPack::File* wv, const Options& opts, const fs::path& path);