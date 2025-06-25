#!/bin/bash

# Ruta al ejecutable de tu servidor
SERVER_BIN=../webserv

# Ruta al archivo de configuración
CONF_FILE=../config/configNiko.conf
# CONF_FILE= config/configNiko.conf

# Archivo de log de Valgrind
VALGRIND_LOG=valgrind_report.txt

# Verifica que el binario y el conf existen
if [[ ! -x "$SERVER_BIN" ]]; then
    echo "Error: No se encontró el ejecutable $SERVER_BIN"
    exit 1
fi

if [[ ! -f "$CONF_FILE" ]]; then
    echo "Error: No se encontró el archivo de configuración $CONF_FILE"
    exit 1
fi

# Eliminar log previo si existe
rm -f "$VALGRIND_LOG"

echo "[INFO] Lanzando el servidor bajo Valgrind..."
valgrind --tool=memcheck --leak-check=full --track-origins=yes \
    --show-leak-kinds=all --error-exitcode=42 --log-file="$VALGRIND_LOG" \
    "$SERVER_BIN" "$CONF_FILE" &

SERVER_PID=$!

# Esperamos que el servidor arranque (ajusta si necesita más tiempo)
sleep 2

# Realizamos una petición de prueba (puedes adaptarla)
echo "[INFO] Realizando petición de prueba..."
curl -s http://localhost:8081/ > /dev/null

# Terminamos el servidor
echo "[INFO] Terminando servidor..."
# kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

# Mostramos los resultados
echo "[INFO] Reporte de Valgrind generado en $VALGRIND_LOG"
grep "definitely lost" "$VALGRIND_LOG"
grep "indirectly lost" "$VALGRIND_LOG"
grep "still reachable" "$VALGRIND_LOG"

echo "[INFO] Fin del test"

