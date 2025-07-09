#!/usr/bin/env python3
import requests
import os
import time
import subprocess

# --- Configuración ---
BASE_URL = "http://localhost:8081"
# Directorio donde se crearán los archivos de prueba temporales
TEST_FILES_DIR = "./" 
# Nombres de los archivos que se crearán para las pruebas
LOGO_FILENAME = "logo_test.png"
TEXT_FILENAME = "example_test.txt"
DELETE_FILENAME = "DELETEME.txt"

# --- Colores para la Salida en Terminal ---
class colors:
    """Clase para definir colores de texto en la terminal usando códigos ANSI."""
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    ENDC = '\033[0m'

def print_header(title):
    """Imprime un encabezado para una sección de pruebas."""
    print(f"\n{colors.BLUE}--- {title.upper()} ---{colors.ENDC}")

def print_result(description, success, details=""):
    """Imprime el resultado de una prueba individual."""
    status = f"{colors.GREEN}[PASS]{colors.ENDC}" if success else f"{colors.RED}[FAIL]{colors.ENDC}"
    message = f"{description:<70} {status}"
    if not success and details:
        message += f"\n{colors.YELLOW}  └> Detalles: {details}{colors.ENDC}"
    print(message)
    return success

# --- Funciones de Preparación y Limpieza ---

def create_test_files():
    """Crea los archivos necesarios para las pruebas de subida y eliminación."""
    print_header("Preparando entorno de prueba")
    try:
        # Crear un archivo de texto simple
        with open(os.path.join(TEST_FILES_DIR, TEXT_FILENAME), "w") as f:
            f.write("Este es un archivo de texto de ejemplo para la prueba de subida.\n")
        print_result(f"Archivo de texto '{TEXT_FILENAME}' creado.", True)

        # Crear un archivo PNG mínimo (1x1 píxel transparente)
        # Esto evita la necesidad de tener un archivo de imagen real
        png_data = b'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x01\x00\x00\x00\x01\x08\x06\x00\x00\x00\x1f\x15\xc4\x89\x00\x00\x00\nIDATx\x9cc\x00\x01\x00\x00\x05\x00\x01\r\n-\xb4\x00\x00\x00\x00IEND\xaeB`\x82'
        with open(os.path.join(TEST_FILES_DIR, LOGO_FILENAME), "wb") as f:
            f.write(png_data)
        print_result(f"Archivo de imagen '{LOGO_FILENAME}' creado.", True)
        
        # Crear archivo para la prueba de DELETE
        with open(os.path.join(TEST_FILES_DIR, DELETE_FILENAME), "w") as f:
            f.write("Este archivo será eliminado.\n")
        print_result(f"Archivo para DELETE '{DELETE_FILENAME}' creado.", True)

        return True
    except Exception as e:
        print_result("Error al crear archivos de prueba", False, str(e))
        return False

def cleanup_test_files():
    """Elimina los archivos de prueba creados."""
    print_header("Limpiando entorno de prueba")
    for filename in [LOGO_FILENAME, TEXT_FILENAME, DELETE_FILENAME]:
        try:
            path = os.path.join(TEST_FILES_DIR, filename)
            if os.path.exists(path):
                os.remove(path)
                print_result(f"Archivo temporal '{filename}' eliminado.", True)
        except Exception as e:
            print_result(f"Error al eliminar '{filename}'", False, str(e))

# --- Definición de las Pruebas ---

def run_all_tests():
    """Ejecuta el conjunto completo de pruebas."""
    passed_count = 0
    failed_count = 0
    results = []

    # 1. Pruebas de POST (Upload)
    print_header("Pruebas de POST (Upload)")
    # Prueba 1.1: Subir archivo de imagen
    try:
        file_path = os.path.join(TEST_FILES_DIR, LOGO_FILENAME)
        with open(file_path, 'rb') as f:
            files = {'file': (LOGO_FILENAME, f, 'image/png')}
            response = requests.post(f"{BASE_URL}/uploads", files=files, timeout=5)
        # Un POST exitoso a un endpoint de subida suele devolver 201 Created o 200 OK
        success = response.status_code in [200, 201]
        results.append(print_result(f"POST /uploads con '{LOGO_FILENAME}'", success, f"Status: {response.status_code}"))
    except requests.exceptions.RequestException as e:
        results.append(print_result(f"POST /uploads con '{LOGO_FILENAME}'", False, str(e)))

    # Prueba 1.2: Subir archivo de texto
    try:
        file_path = os.path.join(TEST_FILES_DIR, TEXT_FILENAME)
        with open(file_path, 'rb') as f:
            files = {'file': (TEXT_FILENAME, f, 'text/plain')}
            response = requests.post(f"{BASE_URL}/uploads", files=files, timeout=5)
        success = response.status_code in [200, 201]
        results.append(print_result(f"POST /uploads con '{TEXT_FILENAME}'", success, f"Status: {response.status_code}"))
    except requests.exceptions.RequestException as e:
        results.append(print_result(f"POST /uploads con '{TEXT_FILENAME}'", False, str(e)))


    # 2. Pruebas de GET (Download)
    print_header("Pruebas de GET (Download)")
    # Esperamos un momento para que el servidor procese los archivos subidos
    time.sleep(1) 
    try:
        # El servidor podría guardar los archivos en una ruta diferente, ajustamos la URL
        # Asumimos que los archivos subidos a /uploads están disponibles en /
        response = requests.get(f"{BASE_URL}/{LOGO_FILENAME}", timeout=5)
        success = response.status_code == 200
        results.append(print_result(f"GET /{LOGO_FILENAME}", success, f"Status: {response.status_code}"))
    except requests.exceptions.RequestException as e:
        results.append(print_result(f"GET /{LOGO_FILENAME}", False, str(e)))


    # 3. Pruebas de DELETE
    print_header("Pruebas de DELETE")
    # Primero, subimos el archivo que vamos a borrar para asegurar que existe en el servidor
    try:
        file_path = os.path.join(TEST_FILES_DIR, DELETE_FILENAME)
        with open(file_path, 'rb') as f:
            files = {'file': (DELETE_FILENAME, f, 'text/plain')}
            requests.post(f"{BASE_URL}/uploads", files=files, timeout=5)
        
        # Ahora, intentamos borrarlo
        response = requests.delete(f"{BASE_URL}/{DELETE_FILENAME}", timeout=5)
        # Un DELETE exitoso puede devolver 200 OK, 202 Accepted o 204 No Content
        success = response.status_code in [200, 202, 204]
        results.append(print_result(f"DELETE /{DELETE_FILENAME}", success, f"Status: {response.status_code}"))
    except requests.exceptions.RequestException as e:
        results.append(print_result(f"DELETE /{DELETE_FILENAME}", False, str(e)))


    # 4. Pruebas de CGI
    print_header("Pruebas de CGI (Scripts)")
    # Prueba 4.1: Python GET
    try:
        params = {'nombre': 'Juan', 'ciudad': 'Madrid'}
        response = requests.get(f"{BASE_URL}/cgi-bin/pythonGET.py", params=params, timeout=5)
        success = response.status_code == 200 and 'Juan' in response.text and 'Madrid' in response.text
        results.append(print_result("CGI GET pythonGET.py?nombre=Juan&ciudad=Madrid", success, f"Status: {response.status_code}"))
    except requests.exceptions.RequestException as e:
        results.append(print_result("CGI GET pythonGET.py", False, str(e)))

    # Prueba 4.2: Python POST
    try:
        data = "Este es el contenido de mi nuevo recurso."
        headers = {'Content-Type': 'text/plain'}
        response = requests.post(f"{BASE_URL}/cgi-bin/pythonPOST.py", data=data, headers=headers, timeout=5)
        success = response.status_code == 200 and 'Recurso creado con éxito' in response.text
        results.append(print_result("CGI POST pythonPOST.py", success, f"Status: {response.status_code}"))
    except requests.exceptions.RequestException as e:
        results.append(print_result("CGI POST pythonPOST.py", False, str(e)))

    # Prueba 4.3: Shell GET
    try:
        params = {'id': '123', 'categoria': 'electronica'}
        response = requests.get(f"{BASE_URL}/cgi-bin/shellGET.sh", params=params, timeout=5)
        success = response.status_code == 200 and '123' in response.text and 'electronica' in response.text
        results.append(print_result("CGI GET shellGET.sh?id=123&categoria=electronica", success, f"Status: {response.status_code}"))
    except requests.exceptions.RequestException as e:
        results.append(print_result("CGI GET shellGET.sh", False, str(e)))

    # Prueba 4.4: Shell POST
    try:
        data = "Contenido del archivo via shell."
        headers = {'Content-Type': 'text/plain'}
        response = requests.post(f"{BASE_URL}/cgi-bin/shellPOST.sh", data=data, headers=headers, timeout=5)
        success = response.status_code == 200 and 'Recurso creado con éxito' in response.text
        results.append(print_result("CGI POST shellPOST.sh", success, f"Status: {response.status_code}"))
    except requests.exceptions.RequestException as e:
        results.append(print_result("CGI POST shellPOST.sh", False, str(e)))

    # --- Resumen Final ---
    passed_count = sum(1 for r in results if r)
    failed_count = len(results) - passed_count
    
    print_header("Resumen de Pruebas")
    print(f"Total de pruebas ejecutadas: {len(results)}")
    print(f"{colors.GREEN}Aprobadas: {passed_count}{colors.ENDC}")
    print(f"{colors.RED}Fallidas:   {failed_count}{colors.ENDC}")
    
    return failed_count == 0


if __name__ == "__main__":
    # Asegurarse de que el servidor está corriendo antes de empezar
    try:
        requests.get(BASE_URL, timeout=3)
    except requests.ConnectionError:
        print(f"{colors.RED}Error: No se pudo conectar al servidor en {BASE_URL}.{colors.ENDC}")
        print(f"{colors.YELLOW}Por favor, asegúrate de que tu servidor web esté iniciado antes de ejecutar las pruebas.{colors.ENDC}")
        exit(1)
        
    if create_test_files():
        try:
            all_passed = run_all_tests()
        finally:
            # La limpieza se ejecuta siempre, incluso si las pruebas fallan
            cleanup_test_files()
        
        # Salir con un código de estado apropiado
        if not all_passed:
            exit(1)
    else:
        print(f"{colors.RED}No se pudieron ejecutar las pruebas debido a un error en la preparación.{colors.ENDC}")
        exit(1)
