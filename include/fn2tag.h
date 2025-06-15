#ifndef FN2TAG_H
#define FN2TAG_H

#include <string>
#include <regex>
#include <map>
#include <vector>

// List of code to tag name
const std::map<std::string, std::string> codeToTag = {
    {"%n", "tracknumber"},
    {"%t", "title"},
    {"%l", "album"},
    {"%a", "artist"},
    {"%d", "date"},
    {"%D", "discnumber"}
    // Add more codes as needed
};

std::pair<std::string, std::vector<std::string>> patternToRegex(const std::string& pattern);
std::map<std::string, std::string> extractTags(const std::string& filename, 
                                            const std::string& regexPattern, 
                                            const std::vector<std::string>& groupOrder);
const std::map<std::string, std::string> fn2tag(const std::string& fn, const std::string &pattern);

#endif