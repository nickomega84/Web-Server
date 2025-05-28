#ifndef MIMETYPE_HPP
#define MIMETYPE_HPP
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

// Template base
template<const char* Ext>
struct MimeType {
	static const char* value() { return "application/octet-stream"; }
};

// Especializaciones por extensión
template<> struct MimeType<ext_html> { static const char* value() { return "text/html"; } };
template<> struct MimeType<ext_css>  { static const char* value() { return "text/css"; } };
template<> struct MimeType<ext_js>   { static const char* value() { return "application/javascript"; } };
template<> struct MimeType<ext_png>  { static const char* value() { return "image/png"; } };
template<> struct MimeType<ext_jpg>  { static const char* value() { return "image/jpeg"; } };
template<> struct MimeType<ext_txt>  { static const char* value() { return "text/plain"; } };
template<> struct MimeType<ext_json> { static const char* value() { return "application/json"; } };
template<> struct MimeType<ext_pdf>  { static const char* value() { return "application/pdf"; } };

// Función pública para detección dinámica
const char* guessMimeType(const std::string& path);

#endif
