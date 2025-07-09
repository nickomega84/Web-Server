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
#include <dirent.h>
#include <string>

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

std::string Utils::mapUriToPath(const std::string& absRoot, const std::string& uri)
{
    std::cout << "[DEBUG][Utils::mapUriToPath] absRoot: " << absRoot << std::endl;
    std::cout << "[DEBUG][Utils::mapUriToPath] uri: " << uri << std::endl;

    std::string joined = absRoot;

    if (!joined.empty() && joined[joined.size() - 1] != '/')
        joined += "/";

    if (!uri.empty() && uri[0] == '/')
        joined += uri.substr(1);
    else
        joined += uri;

    std::cout << "[DEBUG][Utils::mapUriToPath] joined: " << joined << std::endl;

    std::string norm = normalisePath(joined);
    std::cout << "[DEBUG][Utils::mapUriToPath] norm: " << norm << std::endl;

    if (norm.compare(0, absRoot.size(), absRoot) != 0)
        throw std::runtime_error("Path-traversal: " + uri);

    return norm;
}

std::string Utils::validateFilesystemEntry(const std::string& absPath)
{
    struct stat sb;

    std::cout << "[DEBUG][Utils::validateFilesystemEntry] absPath: " << absPath << std::endl;

	if (::lstat(absPath.c_str(), &sb) == -1)
		throw std::runtime_error("Not found: " + absPath);

    if (!S_ISREG(sb.st_mode) && !S_ISDIR(sb.st_mode))
    	throw std::runtime_error("Forbidden type: " + absPath);

    // Si es un fichero regular, comprobamos que no sea symlink
    if (S_ISREG(sb.st_mode)) {
        int fd = ::open(absPath.c_str(), O_RDONLY | O_NOFOLLOW);
        if (fd == -1)
		    throw std::runtime_error("Symlink or I/O error: " + absPath);
        ::close(fd);
    }

    return absPath;
}

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

std::string Utils::renderAutoindexPage(const std::string& displayPath, const std::string& physicalPath)
{
    // Asegurarse de que la ruta a mostrar termine con / para construir bien los enlaces
    std::string currentPath = displayPath;
    if (currentPath.empty() || currentPath[currentPath.length() - 1] != '/')
        currentPath += '/';

    // Generar el encabezado del HTML
    std::string html = "<html><head><title>Index of " + currentPath + "</title></head>";
    html += "<body><h1>Index of " + currentPath + "</h1><hr><pre>";

    // Abrir el directorio físico
    DIR* dir = opendir(physicalPath.c_str());
    if (dir)
    {
        struct dirent* entry;
        // Leer cada entrada del directorio
        while ((entry = readdir(dir)))
        {
            std::string entryName = entry->d_name;
            // Omitir los directorios "." y ".."
            if (entryName != "." && entryName != "..")
            {
                // Construir el enlace completo usando la ruta de la URI
                std::string href = currentPath + entryName;
                html += "<a href=\"" + href + "\">" + entryName + "</a><br>";
            }
        }
        closedir(dir);
    }
    else
    {
        // Opcional: manejar el caso en que el directorio no se pueda abrir
        std::cerr << "[ERROR][renderAutoindexPage] No se pudo abrir el directorio: " << physicalPath << std::endl;
        html += "Error: No se puede listar el contenido del directorio.";
    }

    // Cerrar las etiquetas HTML
    html += "</pre><hr></body></html>";

    return (html);
}
