#include "../../include/middleware/AllowMethodMiddleware.hpp"
#include "../../include/utils/Utils.hpp"

void AllowMethodMiddleware::allow(const std::string& pathPrefix, const std::vector<std::string>& methods) {
	_allowed[pathPrefix] = methods;
}


bool AllowMethodMiddleware::handle(const Request& req, Response& res)
{
    std::string path   = req.getURI();
    std::string method = req.getMethod();

    // 1. Busca el path más largo que coincida como prefijo
    for (std::map<std::string, std::vector<std::string> >::const_iterator it = _allowed.begin();
         it != _allowed.end(); ++it)
    {
        if (path.find(it->first) == 0) {
            const std::vector<std::string>& allowed = it->second;
            if (std::find(allowed.begin(), allowed.end(), method) != allowed.end())
                return true;

            // Bloqueo 405
            res.setStatus(405, "Method Not Allowed");
            res.setHeader("Content-Type", "text/plain");
            res.setBody("405 - Método no permitido");
            return false;
        }
    }

    // 2. Si no hay regla y el método no es seguro => bloquear
    if (method != "GET" && method != "HEAD") {
        res.setStatus(405, "Method Not Allowed");
        res.setHeader("Content-Type", "text/plain");
        res.setBody("405 - Método no permitido");
        return false;
    }
    return true; // GET y HEAD por defecto
}


IMiddleware* AllowMethodMiddleware::clone() const 
{
	return new AllowMethodMiddleware(*this);
}