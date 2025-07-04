#include "../../include/factory/UploadHandlerFactory.hpp"
#include "../../include/handler/UploadHandler.hpp"

UploadHandlerFactory::UploadHandlerFactory(const std::string& path, IResponseBuilder* builder) : _uploadsPath(path), _builder(builder) 
{
    std::cout << "[DEBUG][UploadHandlerFactory] created with uploads path: " << _uploadsPath << std::endl;
}

UploadHandlerFactory::~UploadHandlerFactory() 
{
    std::cout << "[DEBUG][UploadHandlerFactory] destroyed" << std::endl;
}

IRequestHandler* UploadHandlerFactory::createHandler() const {
    return new UploadHandler(_uploadsPath, _builder);
}