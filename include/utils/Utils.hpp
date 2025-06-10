#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <map>

class Utils {
    public:
        static std::string intToString(int value);
        bool strToBool(const std::string& str);
        static  std::string trim(const std::string& str);

};
    

#endif
