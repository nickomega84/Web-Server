#ifndef UTILS_HPP
#define UTILS_HPP

#include "../include/libraries.hpp"

using std::cout;
using std::endl;

class Utils 
{
    public:
        static	std::string resolveAndValidateFile(const std::string& root, const std::string& uri);
        static	std::string intToString(int value);
        static	std::string trim(const std::string& str);
        static	std::string renderAutoindex(const std::string& uri, const std::string& dirPath);
        static	std::string normalisePath(std::string p);
        static	std::string resolveAndValidateDir(const std::string& raw);
        static	std::string mapUriToPath(const std::string& absRoot, const std::string& uri);
        static	std::string validateFilesystemEntry(const std::string& absPath);
};

#endif
