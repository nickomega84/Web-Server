#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <map>
#include "../libraries.hpp"
#include "../core/Request.hpp"
using std::cout;
using std::endl;
class Utils 
{
    // private:
    //     std::vector<std::string> stk;
    //     std::stringstream ss(p);
    //     std::string part;
    public:
        static std::string resolveAndValidateFile(const std::string& root, const std::string& uri);
        static  std::string intToString(int value);
        bool    strToBool(const std::string& str);
        static  std::string trim(const std::string& str);
        // static  std::string renderAutoindex(const Request& request, const std::string& dirPath);
         static      std::string renderAutoindex(const std::string& uri, const std::string& dirPath);

        // static  void createDirectoriesIfNotExist(const std::string& path);
        // static  std::string resolveAndValidateDir(const std::string& path);
        // static  std::string resolveAndValidateFile(const std::string& path);
        // static  std::string resolveAndValidateFile(const std::string absRoot);
        static  std::string normalisePath(std::string p);
        static  std::string resolveAndValidateDir(const std::string& raw);
        static std::string mapUriToPath(const std::string& absRoot, const std::string& uri);
        static std::string validateFilesystemEntry(const std::string& absPath);

		size_t	strToSizeT(const std::string& str);
};

#endif
