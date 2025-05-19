#include "../../include/middleware/AllowMethodMiddleware.hpp"
#include "../../include/utils/Utils.hpp"

void AllowMethodMiddleware::allow(const std::string& pathPrefix, const std::vector<std::string>& methods) {
	_allowed[pathPrefix] = methods;
}

bool AllowMethodMiddleware::handle(const Request& req, Response& res) 
{
	std::string path = req.getURI();
	std::string method = req.getMethod();

	std::map<std::string, std::vector<std::string> >::const_iterator it;
	for (it = _allowed.begin(); it != _allowed.end(); ++it)
{
		if (path.find(it->first) == 0) {
			const std::vector<std::string>& allowedMethods = it->second;
			for (size_t i = 0; i < allowedMethods.size(); ++i) 
            {
				if (allowedMethods[i] == method)
					return (true);
			}
			// Método no permitido
			res.setStatus(405, "Method Not Allowed");
			res.setHeader("Content-Type", "text/plain");
			res.setBody("405 - Método no permitido");
			res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
			return (false);
		}
	}

	return (true); // si no hay restricción, permitir
}

IMiddleware* AllowMethodMiddleware::clone() const 
{
	return new AllowMethodMiddleware(*this);
}