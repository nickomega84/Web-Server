`.
├── Makefile
├── config
│   ├── configNiko.conf
│   ├── cositas.conf
│   ├── default.conf
│   ├── multiple.conf
│   └── simpe.conf
├── include
│   ├── Config.hpp
│   ├── ReadConfig.hpp
│   ├── cgi
│   │   └── CGIHandler.hpp
│   ├── config
│   │   ├── ConfigParser.hpp
│   │   └── configStruct.hpp
│   ├── core
│   │   ├── HTTPStatus.hpp
│   │   ├── Request.hpp
│   │   └── Response.hpp
│   ├── factory
│   │   ├── CGIHandlerFactory.hpp
│   │   ├── IHandlerFactory.hpp
│   │   ├── StaticHandlerFactory.hpp
│   │   └── UploadHandlerFactory.hpp
│   ├── handler
│   │   ├── IRequestHandler.hpp
│   │   ├── StaticFileHandler.hpp
│   │   ├── UploadHandler.hpp
│   │   └── WebSocketHandler.hpp
│   ├── http
│   │   ├── CookieManager.hpp
│   │   └── SessionManager.hpp
│   ├── libraries.hpp
│   ├── logger
│   │   ├── ConsoleLogger.hpp
│   │   └── ILogger.hpp
│   ├── middleware
│   │   ├── AllowMethodMiddleware.hpp
│   │   ├── CookieMiddleware.hpp
│   │   ├── IMiddleware.hpp
│   │   └── MiddlewareStack.hpp
│   ├── response
│   │   ├── DefaultResponseBuilder.hpp
│   │   └── IResponseBuilder.hpp
│   ├── router
│   │   └── Router.hpp
│   ├── server
│   │   ├── ConfigTEMPORAL.hpp
│   │   └── Server.hpp
│   ├── utils
│   │   ├── ErrorPageHandler.hpp
│   │   ├── MimeType.hpp
│   │   └── Utils.hpp
│   ├── utils.cpp

├── logs
│   └── access.log
├── src
│   ├── cgi
│   │   └── CGIHandler.cpp
│   ├── config
│   │   └── ConfigParser.cpp
│   ├── core
│   │   ├── HTTPStatus.cpp
│   │   ├── Request.cpp
│   │   └── Response.cpp
│   ├── factory
│   │   └── StaticHandlerFactory.cpp
│   ├── handler
│   │   ├── StaticFileHandler.cpp
│   │   └── UploadHandler.cpp
│   ├── htpp
│   │   ├── CookieManager.cpp
│   │   └── SessionManager.cpp
│   ├── logger
│   │   └── ConsoleLogger.cpp
│   ├── main.cpp
│   ├── middleware
│   │   ├── AllowMethodMiddleware.cpp
│   │   ├── CookieMiddleware.cpp
│   │   └── MiddlewareStack.cpp
│   ├── response
│   │   └── DefaultResponseBuilder.cpp
│   ├── server
│   │   ├── ConfigTEMPORAL.cpp
│   │   └── Server.cpp
│   └── utils
│       ├── ErrorPageHandler.cpp
│       ├── MimeType.cpp
│       ├── Utils.cpp
│       └── router
│           └── Router.cpp
├── styles
│   └── style.css
|
├── uploads
│   ├── Checkboard.png
│   └── logo.png
└── www
    ├── MontBlancGigapixel-01.jpg
    ├── cgi-bin
    │   ├── pythonGET.py
    │   ├── pythonPOST.py
    │   ├── shellGET.sh
    │   └── shellPOST.sh
    ├── error_pages
    │   ├── 200.html
    │   ├── 403.html
    │   ├── 404.html
    │   ├── 500.html
    │   ├── 502.html
    │   └── assets
    │       ├── default_100_percent
    │       │   ├── 100-disabled.png
    │       │   ├── 100-error-offline.png
    │       │   └── 100-offline-sprite.png
    │       ├── default_200_percent
    │       │   ├── 200-disabled.png
    │       │   ├── 200-error-offline.png
    │       │   └── 200-offline-sprite.png
    │       ├── hello-kugou.gif
    │       ├── kumamon-runner.gif
    │       ├── novas-coisas.gif
    │       ├── offline-sprite-1x.png
    │       ├── offline-sprite-2x.png
    │       ├── screenshot.gif
    │       ├── t-rex-runner-19janil.gif
    │       └── t-rex-runner-bot.gif
    ├── index.html
    ├── logo.png
    └── style.css