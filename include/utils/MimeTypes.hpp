#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include "../include/libraries.hpp"

extern const char ext_html[];
extern const char ext_css[];
extern const char ext_js[];
extern const char ext_png[];
extern const char ext_jpg[];
extern const char ext_txt[];
extern const char ext_json[];
extern const char ext_pdf[];

class MimeTypes 
{
    public:
        static const char* getContentType(const std::string& path);
};
    
#endif

