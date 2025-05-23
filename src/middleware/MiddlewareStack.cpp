#include "../../include/middleware/MiddlewareStack.hpp"

MiddlewareStack::MiddlewareStack()
{
    std::cout << "MiddlewareStack constructor called" << std::endl;
}

MiddlewareStack::MiddlewareStack(const MiddlewareStack& other) 
{
    for (size_t i = 0; i < other._middlewares.size(); ++i) {
		_middlewares.push_back(other._middlewares[i]->clone());
	}
	*this = other;
}

MiddlewareStack& MiddlewareStack::operator=(const MiddlewareStack &other) {
	if (this != &other) 
    {
		_middlewares = other._middlewares;
	}
	return (*this);
}

MiddlewareStack::~MiddlewareStack() 
{
    std::cout << "MiddlewareStack destructor called" << std::endl;
}

void MiddlewareStack::add(IMiddleware* middleware) 
{
	_middlewares.push_back(middleware);
}

bool MiddlewareStack::handle(const Request& req, Response& res) 
{
	for (size_t i = 0; i < _middlewares.size(); ++i) 
    {
		if (!_middlewares[i]->handle(req, res))
			return (false);  // si algún middleware falla, se detiene
	}
	return (true);
}
