🧭 Mapa de Ruta del Proyecto Webserv (42)
🧑‍💻 Fase 1 — Análisis y planificación (Días 1-2)
Objetivo: Entender completamente el subject y acordar decisiones clave del equipo.

Acciones:

Leer el subject oficial y extraer los requisitos obligatorios.

Definir los bonus a realizar (solo si el obligatorio estará sólido).

Acordar herramientas, reglas de estilo, Gitflow, y métodos de comunicación.

Crear el repositorio base y organizar los archivos iniciales.

🧱 Fase 2 — Diseño de arquitectura (Días 3-5)
Objetivo: Esquematizar cómo funcionará el servidor.

Acciones:

Diseñar la arquitectura del servidor: poll, módulos HTTP, parser de configuración, etc.

Crear diagramas de flujo de conexiones y solicitudes.

Dividir el proyecto por componentes:

Parser de config (servidores, rutas, métodos, errores).

Socket manager (conexiones, no bloqueo, múltiples puertos).

Gestor HTTP (métodos GET, POST, DELETE, redirecciones, errores).

⚙️ Fase 3 — Implementación del núcleo (Días 6-14)
Objetivo: Lograr un servidor funcional básico.

Acciones:

Servir contenido estático (archivos HTML, imágenes).

Manejar múltiples clientes con poll() no bloqueante.

Implementar GET, POST y DELETE.

Soporte para múltiples puertos y servidores virtuales.

Manejo básico de errores (404, 500...).

🧪 Fase 4 — Funcionalidades avanzadas y CGI (Días 15-22)
Objetivo: Añadir funcionalidades avanzadas exigidas por el subject.

Acciones:

Añadir soporte para scripts CGI (Python o Bash).

Implementar carga de archivos (upload).

Activar redirecciones HTTP.

Integrar límites de tamaño de cuerpo de solicitud (body size).

🚀 Fase 5 — Bonus (opcional) (Días 23-29)
Objetivo: Ampliar el servidor con funcionalidades opcionales.

Acciones:

Cookies y sesiones.

WebSockets o múltiples CGI.

Directorios protegidos.

Pruebas de estrés y rendimiento concurrente.

📋 Fase 6 — Validación, testing y defensa (Días 30-35)
Objetivo: Garantizar la calidad, estabilidad y documentación del proyecto.

Acciones:

Pruebas con navegadores reales y telnet.

Comparaciones con NGINX.

Crear páginas de error por defecto.

Documentar en README: estructura, compilación, ejemplos de config.

Ensayar defensa (uso, código, pruebas).