#include "../../include/factory/UploadHandlerFactory.hpp"

UploadHandlerFactory::UploadHandlerFactory(const std::string& path, IResponseBuilder* builder) : _uploadsPath(path), _builder(builder) 
{
    std::cout << "[DEBUG] UploadHandlerFactory created with uploads path: " << _uploadsPath << "\n";
}

UploadHandlerFactory::~UploadHandlerFactory() 
{
    std::cout << "[DEBUG] UploadHandlerFactory destroyed\n";
}

IRequestHandler* UploadHandlerFactory::createHandler() const {
    return new UploadHandler(_uploadsPath, _builder);
}