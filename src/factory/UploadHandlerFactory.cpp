#include "../../include/factory/UploadHandlerFactory.hpp"
#include "../../include/handler/UploadHandler.hpp"

UploadHandlerFactory::UploadHandlerFactory(const std::string& path, IResponseBuilder* builder, const ConfigParser& cfg) : _uploadsPath(path), _builder(builder), _cfg(cfg)
{
    (void) _cfg;
    #ifndef NDEBUG
    std::cout << "[DEBUG][UploadHandlerFactory] created with uploads path: " << _uploadsPath << std::endl;
    #endif
}

UploadHandlerFactory::~UploadHandlerFactory() 
{
    #ifndef NDEBUG
    std::cout << "[DEBUG][UploadHandlerFactory] destroyed" << std::endl;
    #endif
}

IRequestHandler* UploadHandlerFactory::createHandler() const {
    return new UploadHandler(_uploadsPath, _builder, _cfg);
}