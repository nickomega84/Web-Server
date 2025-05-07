
# Proyecto Webserv – Documentación Técnica

## 🎯 Objetivo SMART

**S**pecific: Implementar un servidor HTTP en C++98 conforme al subject de 42, incluyendo los métodos GET, POST, DELETE, subida de archivos, múltiples puertos, CGI y configuración dinámica. Además, implementar acceso HTTPS mediante NGINX.

**M**easurable:
- Al menos 30 conexiones concurrentes.
- Soporte funcional para GET, POST, DELETE.
- Comprobación de códigos 2xx, 4xx, 5xx.
- HTTPS accesible desde navegador con certificados.

**A**chievable: Usando `poll()`, sockets no bloqueantes, y NGINX con certificados SSL autofirmados.

**R**elevant: Esencial para el entendimiento profundo de protocolos web, arquitectura cliente-servidor y despliegue seguro de aplicaciones.

**T**ime-bound: Estimación de desarrollo:
- Parser config: 3 días
- I/O & loop con poll: 4 días
- Métodos HTTP: 5 días
- CGI y uploads: 3 días
- NGINX + HTTPS: 2 días
- Testing: 2 días

## 📊 Diagrama de Flujo

![Diagrama de Flujo Webserv](webserv_diagrama_flujo.png)

## 🛠️ Confirmación de Estándares

Este proyecto cumple con:

- El estándar **ISO/IEC 14882:1998 (C++98)**, como lo requiere el subject oficial.
- Las normas del proyecto Webserv de 42:
  - No usar `fork()` excepto para CGI.
  - Operación completamente **no bloqueante** mediante `epoll()` o equivalente.
  - Implementación de **métodos GET, POST y DELETE**.
  - **Subida de archivos**, manejo de errores, CGI, múltiples puertos.
  - Configuración desde archivo similar a NGINX.
  - Compatible con navegadores y herramientas como NGINX/telnet.

## 🔒 HTTPS mediante NGINX (Bonus Recomendado)

Para cumplir con HTTPS sin violar las reglas del subject:
1. Configurar NGINX como **proxy inverso** hacia Webserv.
2. Usar **certificados SSL autofirmados**.
3. Redirigir tráfico `https://` → `http://localhost:PORT_WEB_SERV`.

Esto garantiza una navegación segura compatible con los navegadores actuales sin añadir librerías externas.

---

**Autor:** [Tu Nombre]  
**42 Network - Webserv**
