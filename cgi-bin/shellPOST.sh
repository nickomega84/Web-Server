#!/bin/bash

echo "Content-Type: text/html"
echo ""
echo "<html>"
echo "<head><title>Shell POST - Crear Recurso</title></head>"
echo "<body>"
echo "<h1>POST Request Processed by Shell CGI</h1>"

if [ "$CONTENT_LENGTH" -gt 0 ]; then

    POST_DATA=$(cat)

    TIMESTAMP=$(date +%Y%m%d%H%M%S)
    FILE_NAME="resource_${TIMESTAMP}.txt"

	UPLOAD_DIR="../../uploads"

    if [ ! -d "$UPLOAD_DIR" ]; then
        mkdir -p "$UPLOAD_DIR"
        if [ $? -ne 0 ]; then
            echo "<p style='color:red;'>Error al crear directorio: $UPLOAD_DIR</p>"
            echo "</body></html>"
            exit 1
        fi
    fi
    
    FILE_PATH="${UPLOAD_DIR}/${FILE_NAME}"

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

#curl -X POST -H "Content-Type: text/plain" --data "Contenido del archivo via shell." http://localhost:8081/cgi-bin/shellPOST.sh