
# 🧱 Proyecto Webserv – Planificación, Arquitectura y Gestión Técnica

## 📐 Arquitectura Elegida: Event-Driven + Middleware + Router Modular

### 📊 Ventajas:
- 🔁 Escalable (ideal para múltiples conexiones simultáneas)
- 🧱 Modular (fácil añadir PUT, HEAD, 301, cookies, etc.)
- 📦 Extensible (compatible con WebSocket y CGI)
- 🧵 Adaptable a multithread o single-thread eficiente

### 🗂 Componentes Principales:

```
[ poll() ]
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

## 🧩 Módulos del Proyecto

| Módulo                  | Responsabilidad principal                                        | Bonus implicados               |
|-------------------------|------------------------------------------------------------------|-------------------------------|
| Core/Event Loop         | poll() + gestión de FDs                                          | WebSockets, multicliente     |
| Connection Manager      | Estado por cliente                                               | Cookies, sesiones            |
| HTTP Parser             | Analiza solicitudes HTTP 1.1                                     | PUT, HEAD, WebSockets        |
| Middleware Engine       | Filtros antes del router                                         | Cookies, sesiones            |
| Router                  | Mapea ruta → handler                                             | Redirecciones, CGI           |
| CGI Handler             | fork() + pipe para scripts                                       | Múltiples CGIs               |
| Static File Server      | Sirve archivos o genera autoindex                                | Autoindex avanzado           |
| Response Builder        | Construye respuestas HTTP                                        | Todos                        |
| Logger/Error Manager    | Logs y manejo de errores                                         | General                      |

---

## 🗓️ Roadmap de Fases

### Fase 1: Infraestructura básica (Días 1-7)
- [ ] epoll() funcional
- [ ] Aceptar conexiones
- [ ] ConnectionManager

### Fase 2: HTTP + Parser + Rutas (Días 8-14)
- [ ] Parser HTTP 1.1
- [ ] Middleware mínimo
- [ ] Router básico

### Fase 3: Configuración estilo NGINX (Días 15-19)
- [ ] Parser de config
- [ ] Multipuerto y redirecciones
- [ ] Aplicación dinámica al router

### Fase 4: CGI + Uploads (Días 20-24)
- [ ] Handler CGI funcional
- [ ] POST con archivos

### Fase 5: Bonus medianos (Días 25-30)
- [ ] PUT / HEAD
- [ ] Cookies / sesiones
- [ ] Redirecciones HTTP 3xx

### Fase 6: Bonus avanzados (Días 31-38)
- [ ] Autoindex dinámico
- [ ] WebSockets (RFC 6455)
- [ ] epoll de threads

### Fase 7: Tests y Defensa (Días 39-42)
- [ ] Pruebas con curl, navegador
- [ ] README + Documentación
- [ ] Defensa técnica

---

## ✅ Checklist Obligatorios

| Requisito                                                    | Estado |
|--------------------------------------------------------------|--------|
| Compila con flags de C++98                                   | [ ]    |
| Usa poll()/select()                                          | [ ]    |
| FDs no bloqueantes                                           | [ ]    |
| Soporte para GET, POST, DELETE                               | [ ]    |
| Servir archivos estáticos                                    | [ ]    |
| Subida de archivos                                           | [ ]    |
| Múltiples puertos                                            | [ ]    |
| Código HTTP correcto (200, 404, 500...)                      | [ ]    |
| Páginas de error configurables                               | [ ]    |
| Archivo de configuración tipo NGINX                          | [ ]    |
| CGI (mínimo uno)                                             | [ ]    |
| Sin leaks de memoria                                         | [ ]    |
| Compatible con navegadores                                   | [ ]    |

---

## ✨ Checklist Bonus

| Bonus                           | Estado |
|--------------------------------|--------|
| Cookies y sesiones             | [ ]    |
| Redirecciones 301/302          | [ ]    |
| PUT y HEAD                     | [ ]    |
| Autoindex avanzado             | [ ]    |
| Soporte a múltiples CGI        | [ ]    |
| WebSockets                     | [ ]    |
| Multithread/multiproceso       | [ ]    |

---

## 🌲 Árbol del Proyecto

```
webserv/
├── Makefile
├── README.md
├── config/
│   └── server.conf
├── data/
│   ├── uploads/
│   └── cgi-bin/
├── www/
│   └── index.html
├── include/
│   ├── core/
│   ├── http/
│   ├── middleware/
│   ├── router/
│   ├── handlers/
│   ├── utils/
│   └── ConfigParser.hpp
├── src/
│   ├── main.cpp
│   ├── core/
│   ├── http/
│   ├── middleware/
│   ├── router/
│   ├── handlers/
│   ├── utils/
│   └── ConfigParser.cpp
├── tests/
│   ├── test_config/
│   ├── test_http/
│   ├── test_parser/
│   └── test_router/
└── docs/
    ├── architecture.md
    ├── api.md
    ├── bonus_checklist.md
    └── rfc/
```
