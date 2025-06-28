#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include "helpers.h"
#include <filesystem>
#include <taglib/tpropertymap.h>
#include <iostream>
#include <taglib/fileref.h>
#include "main.h"

namespace fs = std::filesystem;

bool dupTags(const std::string& src, const std::string& dest, const Options& opts);