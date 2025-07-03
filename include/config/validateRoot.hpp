#ifndef VALIDATE_ROOT_HPP
#define VALIDATE_ROOT_HPP

#include <string>

class validateRoot {    
    private:
        std::string configFilePath; 
    public:
    void validationRoot();
        validateRoot(const std::string& configFilePath);
};

#endif