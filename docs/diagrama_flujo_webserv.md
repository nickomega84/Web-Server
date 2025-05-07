# 🔄 Diagrama de Flujo General del Proyecto Webserv

```
[ main.cpp ]
     |
     v
[ ConfigParser ]
     |
     v
[ Core: Event Loop (epoll) ]
     |
     v
[ fd activo ]
     |
     v
[ HTTP Parser (http/) ]
     |
     v
[ Middleware Stack (middleware/) ]
     |
     v
[ Router (router/) ]
     |
     v
[ Handler ]
   |     |      |
[ CGI ] [ Static ] [ Otro ]
     |
     v
[ Construcción de Respuesta (http/) ]
     |
     v
[ Envío a cliente (core/) ]
```

🧠 Este flujo refleja claramente cómo se relacionan los módulos presentes en `src/` e `include/` y cómo se construye el ciclo de vida de una petición HTTP dentro del servidor.