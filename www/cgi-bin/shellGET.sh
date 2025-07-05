#!/bin/sh

echo "Content-type: text/html"
echo ""

# --- Obtener fecha y hora ---
CURRENT_TIME=$(date +"%Y-%m-%d %H:%M:%S %Z")

# --- Lógica CGI ---
PARAMS_HTML=""
if [ -n "$QUERY_STRING" ]; then
    PARAMS_HTML="<ul>"
    OLDIFS=$IFS
    IFS='&'
    for i in $QUERY_STRING; do
        # Decodificar y separar clave/valor
        KEY=$(echo "$i" | cut -d'=' -f1)
        VALUE=$(echo "$i" | cut -d'=' -f2)
        PARAMS_HTML="${PARAMS_HTML}<li><strong>${KEY}:</strong> ${VALUE}</li>"
    done
    IFS=$OLDIFS
    PARAMS_HTML="${PARAMS_HTML}</ul>"
else
    PARAMS_HTML="<p>No se proporcionó un Query String.</p>"
fi

# --- Imprimir la respuesta HTML ---
cat << EOF
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Shell GET</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f4f4f9; color: #333; margin: 0; padding: 20px; }
        .container { max-width: 800px; margin: auto; background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #2c3e50; }
        .timestamp { background: #e8f6f3; border-left: 5px solid #1abc9c; padding: 10px; margin-bottom: 20px; }
        .data-box { background: #fafafa; border: 1px solid #ddd; padding: 15px; margin-top: 20px; border-radius: 5px; }
        ul { list-style-type: none; padding: 0; }
        li { background: #eee; margin-bottom: 5px; padding: 8px; border-radius: 3px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>GET Request Procesada por Shell 🐚</h1>
        <div class="timestamp">
            <strong>Hora de Ejecución:</strong> ${CURRENT_TIME}
        </div>
        <div class="data-box">
            <h2>Datos Recibidos:</h2>
            ${PARAMS_HTML}
        </div>
    </div>
</body>
</html>
EOF