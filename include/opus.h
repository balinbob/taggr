#pragma once

#include <taglib/opusfile.h>
#include "main.h"
#include "flac.h"
#include "helpers.h"
#include "fn2tag.h"
#include "tag2fn.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/opusproperties.h>
#include <taglib/oggfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/tpropertymap.h>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
namespace op = TagLib::Ogg::Opus;

Result tagOPUS(op::File* opus, const Options& opts, const fs::path& path);
