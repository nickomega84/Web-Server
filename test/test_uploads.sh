#!/bin/bash

# --- Configuración ---
HOST="localhost"
PORT="8081"
BASE_URL="http://${HOST}:${PORT}"
UPLOAD_ENDPOINT="${BASE_URL}/uploads"

# Rutas a los archivos locales (ajusta si es necesario)
TXT_FILE="/home/dangonz3/Desktop/example.txt"
GIF_FILE="/home/dangonz3/Desktop/fat-monkey.gif"
PNG_FILE="/home/dangonz3/Desktop/logo.png"
MP4_FILE="/home/dangonz3/Desktop/mono_bailando.mp4"

# Colores para la salida
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# --- Función para ejecutar una prueba ---
# Argumentos: 1. Descripción de la prueba, 2. Argumentos para curl, 3. Código de estado HTTP esperado
run_test() {
    local description="$1"
    local curl_args_str="$2"
    local expected_status="$3"

    echo -e "\n${YELLOW}--- PRUEBA: $description ---${NC}"
    
    # Construye y muestra el comando que se va a ejecutar
    # Usamos `eval` para manejar correctamente los argumentos con comillas y espacios
    local command="curl -s -o /dev/null -w '%{http_code}' ${curl_args_str}"
    echo -e "${CYAN}Comando: ${NC}${command}"

    # Ejecuta curl y captura el código de estado
    local status_code=$(eval ${command})

    # Comprueba el resultado
    if [ "$status_code" -eq "$expected_status" ]; then
        echo -e "Resultado: ${GREEN}✅ ÉXITO${NC} (Código de estado: $status_code)"
    else
        echo -e "Resultado: ${RED}❌ FALLO${NC} (Código de estado: $status_code, se esperaba: $expected_status)"
    fi
}

# --- Inicio de las pruebas ---
echo "============================================="
echo "  INICIANDO SCRIPT DE PRUEBAS DE SUBIDA POST "
echo "============================================="

# --- Grupo 1: Subidas RAW (handleRawUpload) ---
echo -e "\n${CYAN}### Pruebas de subida RAW (handleRawUpload) ###${NC}"
run_test "Subida de archivo de texto (text/plain)" \
         "-X POST ${UPLOAD_ENDPOINT}/raw_upload.txt -H 'Content-Type: text/plain' --data-binary '@${TXT_FILE}'" \
         200

run_test "Subida de imagen GIF (image/gif)" \
         "-X POST ${UPLOAD_ENDPOINT}/raw_upload.gif -H 'Content-Type: image/gif' --data-binary '@${GIF_FILE}'" \
         200

run_test "Subida de imagen PNG (image/png)" \
         "-X POST ${UPLOAD_ENDPOINT}/raw_upload.png -H 'Content-Type: image/png' --data-binary '@${PNG_FILE}'" \
         200

run_test "Subida de vídeo MP4 (video/mp4)" \
         "-X POST ${UPLOAD_ENDPOINT}/raw_upload.mp4 -H 'Content-Type: video/mp4' --data-binary '@${MP4_FILE}'" \
         200


# --- Grupo 2: Formatos Especiales (urlencoded y multipart) ---
echo -e "\n${CYAN}### Pruebas de formatos especiales ###${NC}"
run_test "Subida con 'application/x-www-form-urlencoded'" \
         "-X POST ${UPLOAD_ENDPOINT} -H 'Content-Type: application/x-www-form-urlencoded' --data 'urlencoded_file.txt=Este+es+el+contenido+del+archivo+con+espacios+y+caracteres+especiales:%C3%B1'" \
         200

run_test "Subida con 'multipart/form-data'" \
         "-X POST ${UPLOAD_ENDPOINT} -F 'file=@${TXT_FILE};filename=multipart_file.txt'" \
         200


# --- Grupo 3: Casos de Error y Situaciones Límite ---
echo -e "\n${CYAN}### Pruebas de casos de error ###${NC}"
run_test "Fallo: Content-Type no soportado (application/json)" \
         "-X POST ${UPLOAD_ENDPOINT}/test.json -H 'Content-Type: application/json' --data '{\"key\":\"value\"}'" \
         415

run_test "Fallo: Método no permitido (PUT en /uploads)" \
         "-X PUT ${UPLOAD_ENDPOINT}/put_test.txt --data 'test'" \
         405

# Curl falla antes de enviar la petición si el archivo no existe. El código HTTP será 000.
run_test "Fallo: Archivo local no existe" \
         "-X POST ${UPLOAD_ENDPOINT}/no_existe.txt -H 'Content-Type: text/plain' --data-binary '@./este_archivo_no_existe.tmp'" \
         0

echo -e "\n============================================="
echo "         PRUEBAS FINALIZADAS"
echo "============================================="