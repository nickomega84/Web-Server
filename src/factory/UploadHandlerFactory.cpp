#include "../../include/factory/UploadHandlerFactory.hpp"
#include "../../include/handler/UploadHandler.hpp"

UploadHandlerFactory::UploadHandlerFactory(const std::string& path, IResponseBuilder* builder, const ConfigParser& cfg) : _uploadsPath(path), _builder(builder), _cfg(cfg)
{
    (void) _cfg; // Assuming _cfg is not used in this constructor
    std::cout << "[DEBUG][UploadHandlerFactory] created with uploads path: " << _uploadsPath << std::endl;
}

UploadHandlerFactory::~UploadHandlerFactory() 
{
    std::cout << "[DEBUG][UploadHandlerFactory] destroyed" << std::endl;
}

IRequestHandler* UploadHandlerFactory::createHandler() const {
    return new UploadHandler(_uploadsPath, _builder, _cfg);
}