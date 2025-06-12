#include <string>
#include <regex>
#include <map>


std::string patternToRegex(const std::string& pattern);
std::map<std::string, std::string> extractTags(const std::string& filename, const std::string& regexPattern);
const std::map<std::string, std::string> fn2tag(const std::string& fn, const std::string &pattern);