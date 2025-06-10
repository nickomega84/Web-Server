#!/usr/bin/env bash
set -euo pipefail

# ————————————————————————————————
# RUTAS BASE (ajusta si cambia algo)
# ————————————————————————————————
PROJECT_ROOT="$(cd "$(dirname "$0")/.." >/dev/null 2>&1 && pwd)"
TEST_DIR="$PROJECT_ROOT/test"
WEBSERV_BIN="$PROJECT_ROOT/webserv"                       # tu ejecutable
CONF_FILE="$TEST_DIR/webserv_test/webserv.conf"
TESTER="$TEST_DIR/ubuntu_tester"
WEBROOT="$TEST_DIR/webserv_test/static"                   # directorio static vacío
UPLOADS_DIR="$TEST_DIR/webserv_test/uploads"
YOUPI_DIR="$TEST_DIR/webserv_test/YoupiBanane"
CGI_TEST="$TEST_DIR/webserv_test/cgi_test"
SERVER_URL="http://localhost:8080"

# ————————————————————————————————
# LIMPIAR Y PREPARAR ESTRUCTURA
# ————————————————————————————————
echo "🔨 Limpiando y preparando directorios en $TEST_DIR/webserv_test…"
# preserva ya los que vienen con git, pero por si acaso:
rm -rf "$UPLOADS_DIR" "$WEBROOT"
mkdir -p "$UPLOADS_DIR" "$WEBROOT"

# (Los ficheros de YoupiBanane ya vienen creados bajo test/webserv_test/YoupiBanane)

# ————————————————————————————————
# CGI_TEST YA LO TIENES BAJO test/webserv_test/cgi_test
# ————————————————————————————————
# Asegúrate de que sea ejecutable:
chmod +x "$CGI_TEST"

# ————————————————————————————————
# ARRANCAR WEB SERV
# ————————————————————————————————
echo "🚀 Arrancando tu servidor:"
echo "    $WEBSERV_BIN $CONF_FILE"
"$WEBSERV_BIN" "$CONF_FILE" &
SERVER_PID=$!

# ————————————————————————————————
# ESPERAR A QUE ESTÉ UP
# ————————————————————————————————
echo -n "⏳ Esperando a que $SERVER_URL responda… "
until nc -z localhost 8080; do sleep 0.1; done
echo "OK"

# ————————————————————————————————
# EJECUTAR TESTER (envía los 5 ENTERs)
# ————————————————————————————————
echo "🧪 Ejecutando ubuntu_tester…"
printf '\n\n\n\n\n' | "$TESTER" "$SERVER_URL"
RESULT=$?

# ————————————————————————————————
# STOP Y SALIDA
# ————————————————————————————————
echo "🛑 Deteniendo servidor (PID $SERVER_PID)"
kill $SERVER_PID

if [ $RESULT -eq 0 ]; then
  echo "✅ Todos los tests pasaron."
else
  echo "❌ Fallaron algunos tests (exit code $RESULT)."
fi

exit $RESULT
