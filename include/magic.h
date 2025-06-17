#pragma once

#include <filesystem>
#include "main.h"
#include "helpers.h"

namespace fs = std::filesystem;

Result doMagic(const fs::path& path, const Options& opts);