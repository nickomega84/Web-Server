# 🧱 Arquitectura general

```mermaid
flowchart TD
    A[Web Server (C++)]
    A --> B[Parser]
    B -->|Carga config| C[Server]
    C --> D[Event Loop (epoll)]
    D --> E[Request Parser]
    E --> F[Middleware Layer]
    F --> G[Router]
    G --> H[Action Executor]
    G --> I[Response Builder]
    H --> J[CGI Manager]
```

## 📐 Arquitectura Elegida: Event-Driven + Middleware + Router Modular

Este servidor sigue un enfoque **reactivo y modular** inspirado en servidores modernos como Nginx:

- 🔁 **Event-Driven con epoll():** utilizamos `epoll()` para manejar múltiples conexiones simultáneas de forma eficiente y no bloqueante.
- 🧱 **Middleware:** cada request pasa por capas intermedias (autenticación, logging, validación, etc.).
- 🧭 **Router modular:** selecciona dinámicamente qué lógica ejecutar según método y ruta.

---

## 🧠 Explicación paso a paso

### A[Web Server (C++)]
Nodo principal: el servidor web completo, implementado desde cero en C++.

### A --> B[Parser]
Lee y analiza el archivo de configuración `.conf` (similar a Nginx).

### B -->|Carga config| C[Server]
Configura los sockets, puertos y direcciones según el `.conf`.

### C --> D[Event Loop (epoll)]
Usa `epoll()` para escuchar múltiples conexiones concurrentes eficientemente.

### D --> E[Request Parser]
Interpreta el método HTTP, headers, path, etc.

### E --> F[Middleware Layer]
Cada request atraviesa middlewares (autenticación, validación, logging...).

### F --> G[Router]
Determina qué acción debe ejecutarse según la configuración.

### G --> H[Action Executor]
Realiza la acción: servir archivo, borrar recurso, manejar upload, etc.

### G --> I[Response Builder]
Genera la respuesta HTTP con headers y contenido adecuado.

### H --> J[CGI Manager]
Si la acción lo requiere, se ejecuta un script externo como PHP.

---

## 🔄 Flujo resumido

CONFIGURACIÓN → SOCKETS → EPOLL → PARSEO → MIDDLEWARE → RUTEO → ACCIÓN → RESPUESTA