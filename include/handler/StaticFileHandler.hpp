#ifndef STATICFILEHANDLER_HPP
# define STATICFILEHANDLER_HPP

# include "IRequestHandler.hpp"


class StaticFileHandler : public IRequestHandler
{
  private:
	std::string _rootPath;
	Response	doGET(Response& res, std::string uri);
	Response	doPOST(const Request& req, Response& res);
	int			createPOSTfile(const Request& req, std::string& relative_path);
	std::string	createPOSTbody(std::string full_path);
	Response	doDELETE(Response res, std::string uri);
	std::string	get_date();

  public:
	StaticFileHandler(const std::string &root);
	virtual ~StaticFileHandler();

	Response handleRequest(const Request &request);
};

#endif
