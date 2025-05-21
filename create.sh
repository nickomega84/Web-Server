#!/bin/bash

# Directorios
dirs=(
  "webserv/config"
  "webserv/include/server"
  "webserv/include/handler"
  "webserv/include/response"
  "webserv/include/logger"
  "webserv/include/core"
  "webserv/include/config"
  "webserv/include/router"
  "webserv/include/cgi"
  "webserv/include/utils"
  "webserv/src/server"
  "webserv/src/handler"
  "webserv/src/response"
  "webserv/src/logger"
  "webserv/src/core"
  "webserv/src/config"
  "webserv/src/router"
  "webserv/src/cgi"
  "webserv/www/error_pages"
  "webserv/uploads/files"
  "webserv/logs"
)

# Archivos vacíos iniciales
files=(
  "webserv/Makefile"
  "webserv/README.md"
  "webserv/config/server.conf"
  "webserv/include/server/IServer.hpp"
  "webserv/include/server/EpollServer.hpp"
  "webserv/include/handler/IRequestHandler.hpp"
  "webserv/include/handler/CGIHandler.hpp"
  "webserv/include/handler/StaticFileHandler.hpp"
  "webserv/include/response/IResponseBuilder.hpp"
  "webserv/include/response/DefaultResponseBuilder.hpp"
  "webserv/include/logger/ILogger.hpp"
  "webserv/include/logger/ConsoleLogger.hpp"
  "webserv/include/core/Request.hpp"
  "webserv/include/core/Response.hpp"
  "webserv/include/core/HTTPStatus.hpp"
  "webserv/include/config/ConfigParser.hpp"
  "webserv/include/router/Router.hpp"
  "webserv/include/cgi/CGIHandler.hpp"
  "webserv/include/utils/Utils.hpp"
  "webserv/src/server/EpollServer.cpp"
  "webserv/src/handler/CGIHandler.cpp"
  "webserv/src/handler/StaticFileHandler.cpp"
  "webserv/src/response/DefaultResponseBuilder.cpp"
  "webserv/src/logger/ConsoleLogger.cpp"
  "webserv/src/core/Request.cpp"
  "webserv/src/core/Response.cpp"
  "webserv/src/core/HTTPStatus.cpp"
  "webserv/src/config/ConfigParser.cpp"
  "webserv/src/router/Router.cpp"
  "webserv/src/cgi/CGIHandler.cpp"
  "webserv/src/main.cpp"
  "webserv/www/index.html"
  "webserv/www/error_pages/404.html"
  "webserv/www/error_pages/500.html"
  "webserv/logs/access.log"
)

# Crear directorios
for dir in "${dirs[@]}"; do
  mkdir -p "$dir"
done

# Crear archivos vacíos si no existen
for file in "${files[@]}"; do
  touch "$file"
done

echo "✅ Estructura del proyecto webserv creada correctamente."
