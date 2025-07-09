#ifndef STATICHANDLERFACTORY_HPP
#define STATICHANDLERFACTORY_HPP

#include "../include/libraries.hpp"
#include "../include/factory/IHandlerFactory.hpp"
#include "../include/response/IResponseBuilder.hpp"

class IResponseBuilder;

class StaticHandlerFactory : public IHandlerFactory 
{
    private:
        std::string _rootDir;
        IResponseBuilder* _builder;
        const ConfigParser& _cfg;


    public:
        StaticHandlerFactory(const std::string& rootDir, IResponseBuilder*  builder, const ConfigParser& cfg);
        virtual ~StaticHandlerFactory();
        virtual IRequestHandler* createHandler() const;
};

#endif
