#include <filesystem>
#include "main.h"

namespace fs = std::filesystem;

int doMagic(const fs::path& path, const Options& opts);