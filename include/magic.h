#pragma once

#include <filesystem>
#include "main.h"

namespace fs = std::filesystem;

bool doMagic(const fs::path& path, const Options& opts);