#ifndef STATICFILEHANDLER_HPP
# define STATICFILEHANDLER_HPP

# include "IRequestHandler.hpp"


class StaticFileHandler : public IRequestHandler
{
  private:
	std::string _rootPath;

  public:
	StaticFileHandler(const std::string &root);
	virtual ~StaticFileHandler();

	Response handleRequest(const Request &request);
};

#endif
