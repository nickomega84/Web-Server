#!/bin/bash

echo "Content-Type: text/html"
echo ""
echo "<html>"
echo "<head><title>Shell POST - Crear Recurso</title></head>"
echo "<body>"
echo "<h1>POST Request Processed by Shell CGI</h1>"

if [ "$CONTENT_LENGTH" -gt 0 ]; then
    # Lee los datos POST de stdin de forma más robusta
    # Usamos cat para leer toda la entrada estándar y almacenarla en POST_DATA
    POST_DATA=$(cat)

    # 1. Generar un nombre único para el archivo
    TIMESTAMP=$(date +%Y%m%d%H%M%S)
    FILE_NAME="resource_${TIMESTAMP}.txt"

    # 2. Definir el directorio de destino
    # Asegúrate de que esta ruta sea relativa al directorio de ejecución del CGI.
    # Por ejemplo, si tu script está en www/cgi-bin/, y quieres guardar en www/uploads/
    # entonces la ruta debe ser ../uploads o similar dependiendo de la estructura exacta.
    # La ruta '../../uploads' significa dos niveles arriba desde el directorio del script.
    UPLOAD_DIR="../../uploads"

    # Asegúrate de que el directorio de subida exista, créalo si no
    if [ ! -d "$UPLOAD_DIR" ]; then
        mkdir -p "$UPLOAD_DIR"
        if [ $? -ne 0 ]; then
            echo "<p style='color:red;'>Error al crear directorio: $UPLOAD_DIR</p>"
            echo "</body></html>"
            exit 1
        fi
    fi
    
    FILE_PATH="${UPLOAD_DIR}/${FILE_NAME}"

    # 3. Escribir los datos en el archivo
    # Verificamos si POST_DATA tiene contenido antes de escribir
    if [ -n "$POST_DATA" ]; then
        echo "$POST_DATA" > "$FILE_PATH"

        if [ $? -eq 0 ]; then
            echo "<p>Recurso creado con éxito: <strong>${FILE_NAME}</strong></p>"
            echo "<p>Contenido guardado en: <code>${FILE_PATH}</code></p>"
        else
            echo "<p style='color:red;'>Error al guardar el recurso.</p>"
        fi
    else
        echo "<p style='color:orange;'>Advertencia: No se recibieron datos en el cuerpo POST, archivo creado vacío.</p>"
    fi

    echo "<p>Datos POST recibidos:</p>"
    echo "<pre>${POST_DATA}</pre>"
else
    echo "<p>No se recibieron datos POST para crear un recurso (CONTENT_LENGTH es 0).</p>"
fi

echo "</body>"
echo "</html>"

#curl -X POST -H "Content-Type: text/plain" --data "Contenido del archivo via shell." http://localhost:8080/cgi-bin/shellPOST.sh