#!/usr/bin/env bash
# integration_tests.sh
# Este script automatiza las pruebas de integración para los endpoints del servidor.

# Configuración
BASE_URL="http://localhost:8081"
UPLOAD_PATH="/uploads"
CGI_PATH="/cgi-bin"
# Asegúrate de que el servidor esté corriendo antes de ejecutar este script.

# Función de impresión de resultados
print_result() {
  local status=$1
  local test_name="$2"
  local details="$3"
  if [[ $status -eq 0 ]]; then
    echo -e "[OK]    $test_name"
  else
    echo -e "[ERROR] $test_name (Código: $status)"
    echo -e "$details"
  fi
}

# 1. POST: subir un nuevo archivo
test_upload_post() {
  local url="$BASE_URL$UPLOAD_PATH/nuevo_archivo.txt"
  local http_code
  http_code=$(curl -s -o /dev/null -w "%{http_code}" -X POST \
    -H "Content-Type: text/plain" \
    -d "Este es el contenido del archivo." \
    "$url")
  if [[ $http_code =~ ^2 ]]; then
    return 0
  else
    DETAILS="HTTP status: $http_code"
    echo "$DETAILS"
    return 1
  fi
}

# 2. DELETE: eliminar el recurso
test_upload_delete() {
  local url="$BASE_URL$UPLOAD_PATH/nuevo_archivo.txt"
  local http_code
  http_code=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE "$url")
  if [[ $http_code =~ ^2 ]]; then
    return 0
  else
    DETAILS="HTTP status: $http_code"
    echo "$DETAILS"
    return 1
  fi
}

# 3. CGI Python GET
test_cgi_python_get() {
  local url="$BASE_URL$CGI_PATH/pythonGET.py?nombre=Juan&ciudad=Madrid"
  local response body status DETAILS
  response=$(curl -s -w "\n%{http_code}" "$url")
  status=$(echo "$response" | tail -n1)
  body=$(echo "$response" | sed '$d')
  if [[ $status =~ ^2 ]] && echo "$body" | grep -q "Juan" && echo "$body" | grep -q "Madrid"; then
    return 0
  else
    DETAILS="Response body: $body\nHTTP status: $status"
    echo "$DETAILS"
    return 1
  fi
}

# 4. CGI Python POST
test_cgi_python_post() {
  local url="$BASE_URL$CGI_PATH/pythonPOST.py"
  local response body status DETAILS
  response=$(curl -s -X POST -w "\n%{http_code}" -H "Content-Type: text/plain" --data "Este es el contenido de mi nuevo recurso." "$url")
  status=$(echo "$response" | tail -n1)
  body=$(echo "$response" | sed '$d')
  if [[ $status =~ ^2 ]] || echo "$body" | grep -qE "Success|OK"; then
    return 0
  else
    DETAILS="Response body: $body\nHTTP status: $status"
    echo "$DETAILS"
    return 1
  fi
}

# 5. CGI Shell GET
test_cgi_shell_get() {
  local url="$BASE_URL$CGI_PATH/shellGET.sh?id=123&categoria=electronica"
  local response body status DETAILS
  response=$(curl -s -w "\n%{http_code}" "$url")
  status=$(echo "$response" | tail -n1)
  body=$(echo "$response" | sed '$d')
  if [[ $status =~ ^2 ]] && echo "$body" | grep -q "123" && echo "$body" | grep -q "electronica"; then
    return 0
  else
    DETAILS="Response body: $body\nHTTP status: $status"
    echo "$DETAILS"
    return 1
  fi
}

# 6. CGI Shell POST
test_cgi_shell_post() {
  local url="$BASE_URL$CGI_PATH/shellPOST.s"
  local response body status DETAILS
  response=$(curl -s -X POST -w "
%{http_code}" -H "Content-Type: text/plain" --data "Contenido del archivo via shell." "$url")
  status=$(echo "$response" | tail -n1)
  body=$(echo "$response" | sed '$d')
  if [[ $status =~ ^2 ]] || echo "$body" | grep -qE "Success|OK"; then
    return 0
  else
    DETAILS="Response body: $body
HTTP status: $status"
    echo "$DETAILS"
    return 1
  fi
}

# Ejecutar todos los tests
echo "Iniciando pruebas de integración..."

test_upload_post; print_result $? "POST /uploads/nuevo_archivo.txt"

test_upload_delete; print_result $? "DELETE /uploads/nuevo_archivo.txt"

test_cgi_python_get; print_result $? "GET pythonGET.py?nombre=Juan&ciudad=Madrid"

test_cgi_python_post; print_result $? "POST pythonPOST.py"

test_cgi_shell_get; print_result $? "GET shellGET.sh?id=123&categoria=electronica"

test_cgi_shell_post; print_result $? "POST shellPOST.sh"

echo "Pruebas completadas."
