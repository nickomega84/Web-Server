#ifndef EPOLLSERVER_HPP
# define EPOLLSERVER_HPP

# include "../../include/core/Response.hpp"
# include "../../include/middleware/MiddlewareStack.hpp"
# include "../include/libraries.hpp"
# include "../middleware/AllowMethodMiddleware.hpp"
# include "../middleware/IMiddleware.hpp"
# include "../router/Router.hpp"
# include "IServer.hpp"

class EpollServer : public IServer
{
  private:
	int _epoll_fd;
	std::vector<int> _listenSockets;
	Router _router;
	MiddlewareStack _middleware;

	std::map<int, std::string> _clients;

	// Helpers
	void setupEpoll();
	void makeSocketNonBlocking(int fd);
	void acceptConnection(int listen_fd);
	void handleClientRead(int client_fd);
	void handleClientWrite(int client_fd);
	void closeClient(int client_fd);
    
    public:
    // Canonical form (non-orthodox)
    EpollServer();
    // Router _router;
    
    EpollServer(const EpollServer &other);
    EpollServer &operator=(const EpollServer &other);
    virtual ~EpollServer();
    
    void	setRouter(const Router &router);                  // ES IMPORTANTE
    void	setMiddlewareStack(const MiddlewareStack &stack); // ES IMPORTANTE
    void    initMiddleware() ;
    // Interface
    void	addListeningSocket(int port);
    void	start(void);
    void	stop(void);
};


#endif
