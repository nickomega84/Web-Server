#ifndef STATICHANDLERFACTORY_HPP
#define STATICHANDLERFACTORY_HPP

#include "../../include/factory/IHandlerFactory.hpp"
#include "../../include/handler/IRequestHandler.hpp"
#include "../response/IResponseBuilder.hpp"



class IResponseBuilder;

class StaticHandlerFactory : public IHandlerFactory 
{
    private:
        std::string _rootDir;
        IResponseBuilder* _builder;


        // // Disable copy constructor and assignment operator
        // StaticHandlerFactory(const StaticHandlerFactory&);
        // StaticHandlerFactory& operator=(const StaticHandlerFactory&);
    public:
        // StaticHandlerFactory();
        // explicit StaticHandlerFactory(const std::string& rootDir);
        explicit StaticHandlerFactory(const std::string& rootDir, IResponseBuilder*  builder);
        virtual ~StaticHandlerFactory();
        virtual IRequestHandler* createHandler() const;
};

#endif
