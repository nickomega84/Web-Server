#include "../../include/utils/Utils.hpp"
#include <sstream>
#include <map>
#include <cctype>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <limits.h>
#include <cstdlib>

std::string Utils::intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string Utils::trim(const std::string& str) {
    if (str.empty()) return str;

    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    if (start == std::string::npos || end == std::string::npos)
        return "";
    return str.substr(start, end - start + 1);
}

// void Utils::createDirectoriesIfNotExist(const std::string& path) {
//     struct stat st;
//     if (stat(path.c_str(), &st) != 0) {
//         if (mkdir(path.c_str(), 0777) != 0) {
//             std::cerr << "[ERROR] No se pudo crear directorio: " << path << ": " << strerror(errno) << std::endl;
//         } else {
//             std::cout << "[DEBUG] Directorio creado: " << path << std::endl;
//         }
//     } else {
//         std::cout << "[DEBUG] Directorio ya existe: " << path << std::endl;
//     }
// }
std::string Utils::normalisePath(std::string p)
{
    std::vector<std::string> stk;
    std::stringstream ss(p);
    std::string part;

    while (std::getline(ss, part, '/'))
    {
        if (part.empty() || part == ".") continue;
        if (part == "..") { if (!stk.empty()) stk.pop_back(); }
        else stk.push_back(part);
    }
    std::string out = "/";
    for (size_t i = 0; i < stk.size(); ++i)
    {
        out += stk[i];
        if (i + 1 != stk.size()) out += "/";
    }
    return out;
}

/* ---- A ------------------------------------------------------------ */
std::string Utils::mapUriToPath(const std::string& absRoot, const std::string& uri)
{
    std::cout << "[DEBUG] [Utils::mapUriToPath] absRoot: " << absRoot << std::endl;
    std::cout << "[DEBUG] [Utils::mapUriToPath] uri: " << uri << std::endl;

    std::string joined = absRoot;

    if (!joined.empty() && joined[joined.size() - 1] != '/')
        joined += "/";

    if (!uri.empty() && uri[0] == '/')
        joined += uri.substr(1);
    else
        joined += uri;

    std::cout << "[DEBUG] [Utils::mapUriToPath] joined: " << joined << std::endl;

    std::string norm = normalisePath(joined);
    std::cout << "[DEBUG] [Utils::mapUriToPath] norm: " << norm << std::endl;

    if (norm.compare(0, absRoot.size(), absRoot) != 0)
        throw std::runtime_error("Path-traversal: " + uri);

    return norm;
}


// std::string Utils::mapUriToPath(const std::string& absRoot,
//     const std::string& uri)
// {
// 	std::cout << "[DEBUG] [Utils::mapUriToPath] " << absRoot << std::endl;
// std::string joined = absRoot;
// if (joined[joined.size()-1] != '/' && uri[0] != '/')
// {
//     joined += "/";
//     joined += (uri[0] == '/') ? uri.substr(1) : uri;
//     std::cout << "[DEBUG] Utils::mapUriToPath joined: " << joined << std::endl;
// }

// std::string norm = normalisePath(joined);
// std::cout << "[DEBUG] Utils::mapUriToPath norm: " << norm << std::endl;
// if (norm.compare(0, absRoot.size(), absRoot) != 0)
// throw std::runtime_error("Path-traversal: " + uri);

// return norm;                 // puede ser dir, file, etc.
// }

/* ---- B ------------------------------------------------------------ */
std::string Utils::validateFilesystemEntry(const std::string& absPath)
{
    struct stat sb;

    if (::lstat(absPath.c_str(), &sb) == -1)
    throw std::runtime_error("Not found: " + absPath);

    std::string finalPath = absPath;

    if (S_ISDIR(sb.st_mode)) {
    if (finalPath[finalPath.size()-1] != '/') finalPath += "/";
    finalPath += "index.html";

    if (::lstat(finalPath.c_str(), &sb) == -1 || !S_ISREG(sb.st_mode))
    throw std::runtime_error("Directory listing forbidden: " + absPath);
    }
    else if (!S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))
    throw std::runtime_error("Forbidden type: " + absPath);

    int fd = ::open(finalPath.c_str(), O_RDONLY | O_NOFOLLOW);
    if (fd == -1)   throw std::runtime_error("Symlink or I/O error: " + finalPath);
    ::close(fd);

    return finalPath;            // fichero listo para servir
}

/* -------------------------------------------------------------------------- */
/* 2. Directorio válido – devuelve ABSOLUTO y normalizado                      */
/*    Nota: el caller ya debe haber hecho chdir(activeDirectory) una sola vez. */
/* -------------------------------------------------------------------------- */
std::string Utils::resolveAndValidateDir(const std::string& raw)
{
    /* 1. Si es absoluta ya está bien */
    if (!raw.empty() && raw[0] == '/')
        return normalisePath(raw);

    /* 2. Obtener cwd y concatenar */
    char cwdBuf[PATH_MAX];
    if (!::getcwd(cwdBuf, sizeof(cwdBuf)))
        throw std::runtime_error("getcwd() failed");

    std::string abs = std::string(cwdBuf) + "/" + raw;
    std::string norm = normalisePath(abs);

    /* 3. Verificar directorio */
    DIR* d = ::opendir(norm.c_str());
    if (!d)
        throw std::runtime_error("Not a directory: " + norm);
    ::closedir(d);
    return norm;
}

// std::string Utils::resolveAndValidateDir(const std::string& path) {
//     std::string p = path;
//     std::cout << "[DEBUG] Utils::resolveAndValidateDir Resolviendo ruta absoluta: " << p << std::endl;
//     // realpath(p.c_str(), &p[0]); // Resuelve la ruta absoluta
//     // pathconf(p.c_str(), _PC_PATH_MAX); // Verifica el tamaño máximo de la ruta

//     if (p.size() > 1 && p[p.size() - 1] == '/') {
//         p.erase(p.size() - 1);
//     }
//     struct stat sb;
//     if (stat(p.c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode)) {
//         std::cerr << "[ERROR] la ruta no es un directorio válido " << p
//                   << ": " << strerror(errno) << std::endl;
//         // std::exit(EXIT_FAILURE);
//         return ("\0");
//     }

//     return std::string(p);
// }


// std::string Utils::resolveAndValidateFile(const std::string& absRoot,
//     const std::string& uri)
// {
// /* ------------------------------------------------------------------ */
// /* 1. Concatenar root + uri (mantenemos una sola ‘/’)                 */
// /* ------------------------------------------------------------------ */
// std::string joined = absRoot;
// if (joined[joined.size()-1] != '/' && uri[0] != '/')
// joined += "/";
// joined += (uri[0] == '/') ? uri.substr(1) : uri;

// /* ------------------------------------------------------------------ */
// /* 2. Normalizar texto: quita "."  ".."  "//"                         */
// /* ------------------------------------------------------------------ */
// std::string abs = normalisePath(joined);

// /* ------------------------------------------------------------------ */
// /* 3. Path-traversal → la ruta final DEBE empezar por absRoot         */
// /* ------------------------------------------------------------------ */
// if (abs.compare(0, absRoot.size(), absRoot) != 0)
// throw std::runtime_error("Path-traversal detected: " + uri);

// /* ------------------------------------------------------------------ */
// /* 4. Comprobar metadatos con lstat()                                 */
// /* ------------------------------------------------------------------ */
// struct stat sb;
// if (::lstat(abs.c_str(), &sb) == -1)
// throw std::runtime_error("File not found: " + uri);      // 404

// /* symlink final → prohibido                                          */
// if (S_ISLNK(sb.st_mode))
// throw std::runtime_error("Symlink not allowed: " + uri); // 403

// /* Sólo ficheros regulares (dir se trata en autoindex)                */
// if (!S_ISREG(sb.st_mode))
// throw std::runtime_error("Unsupported type: " + uri);    // 403

// /* ------------------------------------------------------------------ */
// /* 5. Abrir con O_NOFOLLOW para proteger TOCTOU                       */
// /* ------------------------------------------------------------------ */
// int fd = ::open(abs.c_str(), O_RDONLY | O_NOFOLLOW);
// if (fd == -1)                                // error I/O inesperado
// throw std::runtime_error("Open failed: " + uri);         // 500
// ::close(fd);

// return abs;          // ruta absoluta, validada y lista para el handler
// }


std::string Utils::resolveAndValidateFile(const std::string& absRoot,
    const std::string& uri)
{
/* 3.1 unir root + uri */
std::string joined = absRoot;
if (joined[joined.size()-1] != '/' && uri[0] != '/'){	
	
	joined += "/";
	joined += (uri[0] == '/') ? uri.substr(1) : uri;
}

/* 3.2 normalizar, eliminar .. */
std::string abs = Utils::normalisePath(joined);

/* 3.3 path-traversal: abs debe empezar por root */
if (abs.compare(0, absRoot.size(), absRoot) != 0)
	throw std::runtime_error("Path-traversal detectado: " + uri);

/* 3.4 comprobar que el archivo existe y no es symlink (O_NOFOLLOW) */
int fd = ::open(abs.c_str(), O_RDONLY | O_NOFOLLOW);
if (fd == -1)
{
	if (errno == ELOOP)  
		throw std::runtime_error("Symlink no permitido: " + abs);
	throw std::runtime_error("No se puede abrir archivo: " + abs + " (" + strerror(errno) + ")");
}

::close(fd);   // solo queríamos validar
return abs;
}



// std::string Utils::resolveAndValidateFile(const std::string& path) {
//     std::string p = path;
//     execve(p.c_str(), NULL, NULL); // Resuelve la ruta absoluta
//     getw
//     std::string resolvedPath = execve("
//         ", NULL, NULL);
//     if (resolvedPath.empty()) {
//         std::cerr << "[ERROR] No se pudo resolver la ruta: " << p << std::endl;
//         return ("\0");
//     }
//     struct stat sb;
//     if (stat(p.c_str(), &sb) != 0 || !S_ISREG(sb.st_mode)) {
//         std::cerr << "[ERROR] la ruta no es un archivo válido " << p << std::endl;
//         // std::exit(EXIT_FAILURE);
//         return ("\0");
//     }
//     return std::string(p);
// }

size_t Utils::strToSizeT(const std::string& str)
{
	std::stringstream ss(str);
	size_t nmb;
	
	ss >> nmb;
	if (ss.fail())
		return (-1);
	else
		return (nmb);
}
