#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <map>

class Utils {
    public:
        static  std::string intToString(int value);
        bool    strToBool(const std::string& str);
        static  std::string trim(const std::string& str);
        static  void createDirectoriesIfNotExist(const std::string& path);
        static  std::string resolveAndValidateDir(const std::string& path);
        static  std::string resolveAndValidateFile(const std::string& path);



};
    

#endif
