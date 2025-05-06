# 🧱 Arquitectura general

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

## 🔄 Flujo resumido

CONFIGURACIÓN → SERVIDOR → PETICIÓN → PARSEO → ACCIÓN (CGI o no) → RESPUESTA