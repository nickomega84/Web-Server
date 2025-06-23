
#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include "handler/IRequestHandler.hpp"
#include "response/IResponseBuilder.hpp"

class UploadHandler : public IRequestHandler {
public:
    UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder);
    virtual ~UploadHandler();

    Response handleRequest(const Request& req);

private:
    std::string _uploadsPath;
    IResponseBuilder* _builder;
};

#endif

// #ifndef UPLOADHANDLER_HPP
// #define UPLOADHANDLER_HPP

// #include "IRequestHandler.hpp"
// #include "../utils/Utils.hpp"
// #include "../include/libraries.hpp"
// #include "../include/response/IResponseBuilder.hpp"

// #include "handler/IRequestHandler.hpp"
// // #include "utils/Utils.hpp"

// class UploadHandler : public IRequestHandler {
// public:
//     UploadHandler(const std::string& uploadsPath);
//     virtual ~UploadHandler();

//     Response handleRequest(const Request& req);

// private:
//     std::string _uploadsPath;
// };

// #endif


// class UploadHandler : public IRequestHandler {
//     private:
//         std::string _uploadsPath;
//         IResponseBuilder* _builder;
//     public:
//         UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder)
//             : _uploadsPath(uploadsPath), _builder(builder) {}
    
//         Response handleRequest(const Request& req);
//     };
    

// class UploadHandler : public IRequestHandler 
// {
//     private:
//         std::string uploadsPath; // Ruta donde se guardarán los archivos subidos

//     public:
// 	    Response handleRequest(const Request& req);
// };

// #endif
