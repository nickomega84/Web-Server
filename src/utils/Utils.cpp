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

    std::cout << "[DEBUG][Utils::validateFilesystemEntry] START absPath: " << absPath << std::endl;

	if (::lstat(absPath.c_str(), &sb) == -1)
		throw std::runtime_error("[ERROR][Utils::validateFilesystemEntry] Not found: " + absPath);

    if (!S_ISREG(sb.st_mode) && !S_ISDIR(sb.st_mode))
    	throw std::runtime_error("[ERROR][Utils::validateFilesystemEntry] Forbidden type: " + absPath);

    if (S_ISREG(sb.st_mode)) {
        int fd = ::open(absPath.c_str(), O_RDONLY | O_NOFOLLOW);
        if (fd == -1)
		    throw std::runtime_error("[ERROR][Utils::validateFilesystemEntry] Symlink or I/O error: " + absPath);
        ::close(fd);
    }

	std::cout << "[DEBUG][Utils::validateFilesystemEntry] END" << std::endl;
    return absPath;
}

std::string Utils::resolveAndValidateDir(const std::string& raw)
{
    if (!raw.empty() && raw[0] == '/')
        return normalisePath(raw);

    char cwdBuf[PATH_MAX];
    if (!::getcwd(cwdBuf, sizeof(cwdBuf)))
        throw std::runtime_error("getcwd() failed");

    std::string abs = std::string(cwdBuf) + "/" + raw;
    std::string norm = normalisePath(abs);

    DIR* d = ::opendir(norm.c_str());
    if (!d)
        throw std::runtime_error("Not a directory: " + norm);
    ::closedir(d);
    return norm;
}

std::string Utils::resolveAndValidateFile(const std::string& absRoot, const std::string& uri)
{
	std::string joined = absRoot;
	if (joined[joined.size()-1] != '/' && uri[0] != '/'){

		joined += "/";
		joined += (uri[0] == '/') ? uri.substr(1) : uri;
	}

	std::string abs = Utils::normalisePath(joined);

	if (abs.compare(0, absRoot.size(), absRoot) != 0)
		throw std::runtime_error("Path-traversal detectado: " + uri);

	int fd = ::open(abs.c_str(), O_RDONLY | O_NOFOLLOW);
	if (fd == -1)
	{
		if (errno == ELOOP)
			throw std::runtime_error("Symlink no permitido: " + abs);
		throw std::runtime_error("No se puede abrir archivo: " + abs + " (" + strerror(errno) + ")");
	}

	::close(fd);
	return abs;
}
