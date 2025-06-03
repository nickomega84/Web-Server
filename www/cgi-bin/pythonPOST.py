#!/usr/bin/python3

import os
import sys
import cgi # Módulo CGI para manejar datos de formularios

print("Content-Type: text/html\n")
print("<html>")
print("<head><title>Python POST - Crear Recurso</title></head>")
print("<body>")
print("<h1>POST Request Processed by Python CGI</h1>")

content_length = int(os.environ.get("CONTENT_LENGTH", 0))

if content_length > 0:
    post_data = sys.stdin.read(content_length)

    # 1. Obtener un nombre para el nuevo recurso
    # Podrías obtenerlo de un campo en los datos POST,
    # o generarlo (por ejemplo, con un timestamp).
    # Aquí, para simplificar, usaremos un nombre fijo o generado por tiempo.
    
    # Ejemplo 1: Usar un nombre fijo (para demostración, no en producción real)
    # file_name = "nuevo_recurso.txt"
    
    # Ejemplo 2: Generar un nombre basado en el tiempo para evitar sobrescribir
    import datetime
    timestamp = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
    file_name = f"recurso_{timestamp}.txt"
    
    # 2. Definir la ruta donde se guardará el recurso
    # Asegúrate de que este directorio exista y el CGI tenga permisos de escritura.
    # Por ejemplo, un directorio 'uploads' dentro de tu carpeta 'www'.
    # Si tu script está en www/cgi-bin/post.py, esto podría ser www/uploads/
    upload_dir = "../../uploads" # Asegúrate de que esta ruta sea relativa al directorio de ejecución del CGI (m["dir"] en tu C++)
    
    # Asegúrate de que el directorio de subida exista
    if not os.path.exists(upload_dir):
        try:
            os.makedirs(upload_dir)
        except OSError as e:
            print(f"<p style='color:red;'>Error al crear directorio: {e}</p>")
            print("</body></html>")
            sys.exit(1) # Salir con error
            
    file_path = os.path.join(upload_dir, file_name)

    try:
        with open(file_path, "w") as f:
            f.write(post_data)
        print(f"<p>Recurso creado con éxito: <strong>{file_name}</strong></p>")
        print(f"<p>Contenido guardado en: <code>{file_path}</code></p>")
    except Exception as e:
        print(f"<p style='color:red;'>Error al guardar el recurso: {e}</p>")
    
    print("<p>Datos POST recibidos:</p>")
    print(f"<pre>{post_data}</pre>")
else:
    print("<p>No se recibieron datos POST para crear un recurso.</p>")

print("</body>")
print("</html>")

#curl -X POST -H "Content-Type: text/plain" --data "Este es el contenido de mi nuevo recurso." http://localhost:8080/cgi-bin/pythonPOST.py