#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP
#include <string>  // ✅ Esto es lo que falta

// Declaraciones de extensiones como constantes
extern const char ext_html[];
extern const char ext_css[];
extern const char ext_js[];
extern const char ext_png[];
extern const char ext_jpg[];
extern const char ext_txt[];
extern const char ext_json[];
extern const char ext_pdf[];

// template<const char* Ext>
// struct MimeTypeTraits {
// 	static const char* value() { return "application/octet-stream"; }
// };
// // Especializaciones por extensión
// template<> struct MimeTypeTraits<ext_html> { static const char* value() { return "text/html"; } };
// template<> struct MimeTypeTraits<ext_css>  { static const char* value() { return "text/css"; } };
// template<> struct MimeTypeTraits<ext_js>   { static const char* value() { return "application/javascript"; } };
// template<> struct MimeTypeTraits<ext_png>  { static const char* value() { return "image/png"; } };
// template<> struct MimeTypeTraits<ext_jpg>  { static const char* value() { return "image/jpeg"; } };
// template<> struct MimeTypeTraits<ext_txt>  { static const char* value() { return "text/plain"; } };
// template<> struct MimeTypeTraits<ext_json> { static const char* value() { return "application/json"; } };
// template<> struct MimeTypeTraits<ext_pdf>  { static const char* value() { return "application/pdf"; } };

// Clase pública de acceso
class MimeTypes 
{
    public:
        static const char* getContentType(const std::string& path);
};
    
#endif

