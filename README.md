# Webserv - Proyecto 42

Este proyecto consiste en implementar un servidor HTTP desde cero en C++, sin el uso de bibliotecas externas como Boost. El objetivo es comprender y construir la arquitectura fundamental de un servidor web como Nginx o Apache, respetando los estándares HTTP/1.1 y los requerimientos del subject.

---

## ✅ Requisitos obligatorios

- 🧾 Soporte para archivos de configuración tipo nginx (`.conf`)
- 🌐 Servidor capaz de escuchar en múltiples puertos/IPs
- 👥 Manejo simultáneo de al menos 3 clientes
- 📩 Métodos HTTP soportados: `GET`, `POST` y `DELETE`
- 🗂 Autoindex habilitado si no hay archivo index
- 🔁 Redirecciones 301 y 302
- 🧠 Manejo de códigos de estado HTTP correctos (200, 404, 500...)
- 🧨 Páginas de error personalizadas
- ⚙️ Soporte de CGI (por ejemplo: ejecutar scripts PHP o Python)
- 💾 Gestión de uploads via POST (por ejemplo: `multipart/form-data`)
- 🧼 Liberación completa de recursos y sin memory leaks (`valgrind`)

---

## 🧱 Arquitectura general

#### 📐 Arquitectura Elegida: Event-Driven + Middleware + Router Modular

#### 📊 Ventajas:
- 🔁 Escalable (ideal para múltiples conexiones simultáneas)
- 🧱 Modular (fácil añadir PUT, HEAD, 301, cookies, etc.)
- 📦 Extensible (compatible con WebSocket y CGI)
- 🧵 Adaptable a multithread o single-thread eficiente


```mermaid
flowchart TD
    A[Web Server (C++)]
    A --> B[Parser]
    B -->|Carga config| C[Server]
    C --> D[Request Handler]
    D --> E[Request Parsing]
    D --> F[Action Execution]
    D --> G[Response Builder]
    F --> H[CGI Manager]
```

## 🧠 Explicación paso a paso

### A[Web Server (C++)]
Nodo principal: es el servidor web escrito en C++, que contiene todos los componentes del sistema.

### A --> B[Parser]
El servidor primero llama al **Parser**, que se encarga de **leer y analizar el archivo de configuración `.conf`**, como en Nginx.

### B -->|Carga config| C[Server]
Una vez procesada la configuración, se pasa al módulo **Server**, que es el encargado de **abrir sockets, escuchar en puertos/IPs definidos y aceptar conexiones**.

### C --> D[Request Handler]
Por cada conexión que llega, el servidor invoca el **Request Handler**, quien se encarga de **procesar cada petición HTTP**.

### D --> E[Request Parsing]
El handler primero **interpreta la solicitud**: método (`GET`, `POST`...), URI, headers, body, etc.

### D --> F[Action Execution]
Luego **ejecuta la acción correspondiente**: buscar archivo, ejecutar CGI, borrar un recurso, etc.

### D --> G[Response Builder]
Después, **genera la respuesta HTTP** adecuada con su código (`200`, `404`...), headers y cuerpo.

### F --> H[CGI Manager]
Si la acción incluye ejecutar un script (por ejemplo PHP), se delega a **CGI Manager**, que se encarga de **correr el script externo** y devolver su salida como respuesta.

---

### 🔄 Flujo resumido

CONFIGURACIÓN → SERVIDOR → PETICIÓN → PARSEO → ACCIÓN (CGI o no) → RESPUESTA

---


### 🗂 Componentes Principales:
### epoll()
```
[ epoll() ]
    |
    v
[ fd activo ] --> [ Connection Handler ]
                      |
                      v
              [ Parser HTTP ]
                      |
                      v
              [ Middleware Stack ]
                  |       |
          [Session]   [Cookie]
                  |
                  v
               [ Router ]
             /     |     \
         CGI   Static   WebSocket
             \     |     /
              [ Response ]
                  |
               [ write() ]
```
---

## 🌲 Árbol del Proyecto

```
webserv/
│
├── Makefile                           # Compilación principal
├── README.md                          # Documentación general del proyecto
├── LICENSE                            # (opcional) Licencia del proyecto
│
├── config/
│   └── server.conf                    # Configuración tipo NGINX
│
├── data/                              # Recursos dinámicos
│   ├── uploads/                       # Archivos subidos por usuarios
│   └── cgi-bin/                       # Scripts CGI (PHP, Python, etc.)
│
├── www/                               # Archivos estáticos del sitio web
│   └── index.html                     # Página por defecto
│
├── include/                           # Headers del proyecto
│   ├── core/
│   │   ├── Server.hpp
│   │   ├── Poller.hpp
│   │   ├── Connection.hpp
│   │   └── EventLoop.hpp
│   ├── http/
│   │   ├── HttpRequest.hpp
│   │   ├── HttpResponse.hpp
│   │   └── HttpParser.hpp
│   ├── middleware/
│   │   ├── CookieMiddleware.hpp
│   │   ├── SessionMiddleware.hpp
│   │   └── LoggerMiddleware.hpp
│   ├── router/
│   │   ├── Route.hpp
│   │   └── Router.hpp
│   ├── handlers/
│   │   ├── StaticHandler.hpp
│   │   ├── CgiHandler.hpp
│   │   ├── WebSocketHandler.hpp
│   │   └── UploadHandler.hpp
│   ├── utils/
│   │   ├── Logger.hpp
│   │   ├── MimeTypes.hpp
│   │   └── StatusCodes.hpp
│   └── ConfigParser.hpp               # Parser del archivo de configuración
│
├── src/
│   ├── main.cpp                       # Punto de entrada
│   ├── core/
│   │   ├── Server.cpp
│   │   ├── Poller.cpp
│   │   ├── Connection.cpp
│   │   └── EventLoop.cpp
│   ├── http/
│   │   ├── HttpRequest.cpp
│   │   ├── HttpResponse.cpp
│   │   └── HttpParser.cpp
│   ├── middleware/
│   │   ├── CookieMiddleware.cpp
│   │   ├── SessionMiddleware.cpp
│   │   └── LoggerMiddleware.cpp
│   ├── router/
│   │   ├── Route.cpp
│   │   └── Router.cpp
│   ├── handlers/
│   │   ├── StaticHandler.cpp
│   │   ├── CgiHandler.cpp
│   │   ├── WebSocketHandler.cpp
│   │   └── UploadHandler.cpp
│   ├── utils/
│   │   ├── Logger.cpp
│   │   ├── MimeTypes.cpp
│   │   └── StatusCodes.cpp
│   └── ConfigParser.cpp
│
├── tests/
│   ├── test_config/
│   ├── test_http/
│   ├── test_parser/
│   └── test_router/
│
└── docs/
    ├── architecture.md                # Explicación de diseño
    ├── api.md                         # Especificaciones internas
    ├── bonus_checklist.md             # Progreso de bonus
    └── rfc/                           # RFCs de referencia (copias locales opcionales)

```


# 🌐 Documentación Oficial Relevante para el Proyecto Webserv 📘

## 1. Protocolo HTTP
- **HTTP/1.1 (RFC 2616 - Obsoleto)**  
  https://www.w3.org/Protocols/rfc2616/rfc2616.html

- **HTTP/1.1 actualizado (RFC 7230 a RFC 7235)**  
  - [RFC 7230 - Message Syntax and Routing](https://datatracker.ietf.org/doc/html/rfc7230)  
  - [RFC 7231 - Semantics and Content](https://datatracker.ietf.org/doc/html/rfc7231)  
  - [RFC 7235 - Authentication](https://datatracker.ietf.org/doc/html/rfc7235)

## 2. Sockets y Redes en UNIX (POSIX)
- Manual POSIX de `socket`, `bind`, `accept`, `listen`, `poll`, etc.  
  - https://man7.org/linux/man-pages/dir_section_2.html  
  - https://man7.org/linux/man-pages/man2/socket.2.html  
  - https://man7.org/linux/man-pages/man2/poll.2.html

## 3. Estándar C++98
- **ISO/IEC 14882:1998 — Lenguaje de programación C++**  
  - [Consulta PDF proporcionado (tu archivo iso-cpp.pdf)]  
  - Alternativamente: https://cplusplus.com/doc/tutorial/

## 4. Documentación CGI
- **Especificación CGI 1.1 (RFC 3875)**  
  https://datatracker.ietf.org/doc/html/rfc3875  
- **Referencia de variables de entorno CGI**  
  https://www.cgi101.com/book/ch3/text.html

## 5. NGINX
- Documentación oficial:  
  - https://nginx.org/en/docs/  
  - https://nginx.org/en/docs/http/configuring_https_servers.html  
  - https://nginx.org/en/docs/http/ngx_http_proxy_module.html

## 6. Certificados SSL para desarrollo
- **OpenSSL CLI (para generar certificados autofirmados)**  
  - https://www.openssl.org/docs/man1.1.1/man1/openssl.html  
  - https://wiki.openssl.org/index.php/Command_Line_Utilities

## 7. Referencia HTTP de Mozilla Developer Network (MDN)
- https://developer.mozilla.org/en-US/docs/Web/HTTP  
  Ideal para entender los códigos de estado, headers, y flujos de conexión HTTP.

---

# 🧠 Análisis de Arquitecturas Posibles para Webserv

| Arquitectura                      | Descripción breve                                                                 | Ventajas                                                                 | Inconvenientes                                                                 | ¿Viable para Webserv + bonus?      |
|----------------------------------|------------------------------------------------------------------------------------|--------------------------------------------------------------------------|--------------------------------------------------------------------------------|-------------------------------------|
| Monolítica                       | Toda la lógica en un solo binario o módulo                                        | Fácil de desarrollar al principio                                        | Difícil de mantener con múltiples funcionalidades                              | ❌ No recomendado                   |
| Microservicios                   | Cada componente es un servicio independiente                                      | Escalable, robusto, ideal para grandes sistemas                          | Muy complejo, necesitas red interna, contenedores, etc.                       | ❌ Excesivo para Webserv            |
| MVC (Model View Controller)      | Separación entre lógica (Controlador), datos (Modelo) y presentación (Vista)     | Facilita añadir rutas, lógica, vistas, respuestas estructuradas         | No es nativa para servidores HTTP desde 0                                     | ⚠️ Interesante para capas internas  |
| Reactor Pattern (💡)             | Un loop de eventos que distribuye eventos I/O a manejadores (handlers) sin bloqueo| Ideal para poll()/select, escalable, usado en NGINX y Node.js            | Requiere diseño claro de conexiones y estados                                 | ✅ Sí. Ideal como núcleo de Webserv |
| Multithread con Pool de Workers  | Un hilo principal acepta conexiones y distribuye a un pool de threads            | Muy eficiente para CPU-bound o CGI                                       | Complejidad de sincronización                                                | ✅ Sí, si quieres bonus multithread |
| Pipeline/Middleware              | Las solicitudes pasan por etapas (parser, router, lógica, respuesta) en cadena    | Altamente extensible, ideal para agregar funcionalidades como sesiones   | Necesita diseño modular, pero factible                                        | ✅ Altamente recomendado            |
| Event-Driven + Middleware híbrido| Combinación del patrón Reactor + Pipeline interno                                 | Máxima flexibilidad y escalabilidad                                     | Requiere pensar en fases y estado por conexión                               | ✅ Arquitectura recomendada         |

