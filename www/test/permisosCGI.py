import subprocess
import os

CONFIG_FILES = {
    "config1.conf": """
server {
    server_name config1.com;
    port 8081;
    host 127.0.0.1;
    body_size 100000;
    index index.html;
    root ./www;

    # Permisos por defecto para el server block
    get_allowed false;
    post_allowed false;
    delete_allowed false;
    autoindex true;

    location / {
        root ./www;
        index index.html;
        allow_methods GET POST; # GET y POST permitidos en la raíz
    }

    location /uploads {
        root ./www/uploads;
        autoindex false;
        allow_methods POST; # Solo POST para subidas
        upload_path ./uploads;
    }

    location /cgi-bin {
        root ./cgi-bin;
        cgi_path /usr/bin/python3.10;
        cgi_ext .sh .py;
        allow_methods GET; # Solo GET para CGI
        deleteOn false;
        postOn false;
        autoindex true;
    }

    location /error_pages {
        not_found ./www/error_pages/200.html;
        forbidden ./www/error_pages/403.html;
        bad_request ./www/error_pages/400.html;
        bad_getaway ./www/error_pages/502.html;
    }
}
""",
    "config2.conf": """
server {
    server_name config2.com;
    port 8082;
    host 127.0.0.1;
    body_size 100000;
    index index.html;
    root ./www;

    get_allowed true;
    post_allowed true;
    delete_allowed false;
    autoindex false;

    location / {
        root ./www;
        index index.html;
        allow_methods GET DELETE; # GET y DELETE permitidos en la raíz
    }

    location /uploads {
        root ./www/uploads;
        autoindex true;
        allow_methods GET POST DELETE; # Todos permitidos para subidas
        upload_path ./uploads;
    }

    location /cgi-bin {
        root ./cgi-bin;
        cgi_path /usr/bin/python3.10;
        cgi_ext .sh .py;
        allow_methods POST; # Solo POST para CGI
        deleteOn false;
        postOn true; # Activar POST para CGI en este bloque
        autoindex true;
    }

    location /error_pages {
        not_found ./www/error_pages/404.html;
        forbidden ./www/error_pages/403.html;
        bad_request ./www/error_pages/400.html;
        bad_getaway ./www/error_pages/502.html;
    }
}
""",
    "config3.conf": """
server {
    server_name config3.com;
    port 8083;
    host 127.0.0.1;
    body_size 100000;
    index index.html;
    root ./www;

    get_allowed true;
    post_allowed true;
    delete_allowed true;
    autoindex true;

    location / {
        root ./www;
        index index.html;
        allow_methods; # Hereda del server block (todos permitidos)
    }

    location /uploads {
        root ./www/uploads;
        autoindex false;
        allow_methods GET; # Solo GET para subidas (para verlas)
        upload_path ./uploads;
    }

    location /cgi-bin {
        root ./cgi-bin;
        cgi_path /usr/bin/python3.10;
        cgi_ext .sh .py;
        allow_methods DELETE; # Solo DELETE para CGI (caso inusual, para probar)
        deleteOn true; # Activar DELETE para CGI
        postOn false;
        autoindex true;
    }

    location /error_pages {
        not_found ./www/error_pages/404.html;
        forbidden ./www/error_pages/403.html;
        bad_request ./www/error_pages/400.html;
        bad_getaway ./www/error_pages/502.html;
    }
}
""",
    "config4.conf": """
server {
    server_name config4.com;
    port 8084;
    host 127.0.0.1;
    body_size 100000;
    index index.html;
    root ./www;

    get_allowed false;
    post_allowed false;
    delete_allowed false;
    autoindex false;

    location / {
        root ./www;
        index index.html;
        allow_methods; # Hereda del server block (ninguno permitido)
    }

    location /uploads {
        root ./www/uploads;
        autoindex false;
        allow_methods POST GET; # POST y GET permitidos para subidas
        upload_path ./uploads;
    }

    location /cgi-bin {
        root ./cgi-bin;
        cgi_path /usr/bin/python3.10;
        cgi_ext .sh .py;
        allow_methods GET POST DELETE; # Todos los métodos para CGI
        deleteOn true;
        postOn true;
        autoindex true;
    }

    location /error_pages {
        not_found ./www/error_pages/404.html;
        forbidden ./www/error_pages/403.html;
        bad_request ./www/error_pages/400.html;
        bad_getaway ./www/error_pages/502.html;
    }
}
"""
}

TEST_CASES = {
    "config1.conf": {
        "static_get": [
            "curl -v http://localhost:8081/",
            "curl -v http://localhost:8081/index.html"
        ],
        "static_post": [
            "curl -v -X POST -H \"Content-Type: text/plain\" --data \"Hello\" http://localhost:8081/",
            "curl -v -X POST -F \"file=@/home/dangonz3/Desktop/example.txt\" http://localhost:8081/uploads"
        ],
        "cgi": [
            "curl -v \"http://localhost:8081/cgi-bin/pythonGET.py?nombre=Juan\"",
            "curl -v \"http://localhost:8081/cgi-bin/shellGET.sh?id=123\""
        ]
    },
    "config2.conf": {
        "static_get": [
            "curl -v http://localhost:8082/index.html",
            "curl -v -X GET http://localhost:8082/"
        ],
        "static_post": [
            "curl -v -X POST -F \"file=@/home/dangonz3/Desktop/logo.png\" http://localhost:8082/uploads",
            "curl -v -X POST -H \"Content-Type: text/plain\" --data \"Otro contenido.\" http://localhost:8082/" # Esto debería fallar (POST no permitido en /)
        ],
        "cgi": [
            "curl -v -X POST -H \"Content-Type: text/plain\" --data \"CGI Post Data\" http://localhost:8082/cgi-bin/pythonPOST.py",
            "curl -v -X POST -H \"Content-Type: text/plain\" --data \"CGI Shell Data\" http://localhost:8082/cgi-bin/shellPOST.sh"
        ]
    },
    "config3.conf": {
        "static_get": [
            "curl -v http://localhost:8083/",
            "curl -v http://localhost:8083/uploads/some_uploaded_file.txt" # Asumiendo que existe un archivo para GET
        ],
        "static_post": [
            "curl -v -X POST -F \"file=@/home/dangonz3/Desktop/test_file.txt\" http://localhost:8083/uploads" # Esto debería fallar (POST no permitido en /uploads)
        ],
        "cgi": [
            "curl -v -X DELETE http://localhost:8083/cgi-bin/delete_me.py", # Asumiendo un CGI script que maneja DELETE
            "curl -v -X GET http://localhost:8083/cgi-bin/pythonGET.py?param=value" # Esto debería fallar (GET no permitido en CGI)
        ]
    },
    "config4.conf": {
        "static_get": [
            "curl -v http://localhost:8084/", # Esto debería fallar (GET no permitido en /)
            "curl -v http://localhost:8084/uploads/existing_file.txt" # Asumiendo un archivo existente en /uploads
        ],
        "static_post": [
            "curl -v -X POST -F \"file=@/home/dangonz3/Desktop/new_upload.jpg\" http://localhost:8084/uploads",
            "curl -v -X POST -H \"Content-Type: text/plain\" --data \"Some text\" http://localhost:8084/uploads"
        ],
        "cgi": [
            "curl -v \"http://localhost:8084/cgi-bin/pythonGET.py?query=test\"",
            "curl -v -X POST -H \"Content-Type: application/json\" --data \"{\\\"data\\\":\\\"json\\\"}\" http://localhost:8084/cgi-bin/pythonPOST.py",
            "curl -v -X DELETE http://localhost:8084/cgi-bin/delete_this_cgi.sh"
        ]
    }
}

def run_test(command):
    """Ejecuta un comando curl y captura su estado para la salida de la lista de verificación."""
    try:
        # Usamos capture_output=True para suprimir la salida en la consola
        result = subprocess.run(command, shell=True, capture_output=True, text=True, check=False)
        if result.returncode == 0:
            print(f"✅ {command}")
        else:
            print(f"❌ {command}")
            # Puedes descomentar las siguientes líneas si necesitas ver el stderr/stdout de los fallos
            # print(f"  STDOUT:\n{result.stdout.strip()}")
            # print(f"  STDERR:\n{result.stderr.strip()}")
    except Exception as e:
        print(f"❌ {command} (Error al ejecutar: {e})")

def create_config_file(filename, content):
    """Crea un archivo de configuración con el contenido dado."""
    with open(filename, "w") as f:
        f.write(content)
    # No imprimimos la creación del archivo aquí para mantener la salida limpia

def main():
    print("## Resultados de las Pruebas\n")

    for config_name, config_content in CONFIG_FILES.items():
        print(f"### Archivo de Configuración: `{config_name}`\n")
        create_config_file(config_name, config_content)
        
        # Simulamos el reinicio del servidor
        # print(f"Por favor, reinicia tu servidor con {config_name} y presiona Enter para continuar...")
        # input() 
        
        tests = TEST_CASES[config_name]
        
        print(f"#### Pruebas Estáticas GET")
        for cmd in tests["static_get"]:
            run_test(cmd)
        print() # Salto de línea para separar secciones
        
        print(f"#### Pruebas POST")
        for cmd in tests["static_post"]:
            run_test(cmd)
        print() # Salto de línea para separar secciones

        print(f"#### Pruebas CGI")
        for cmd in tests["cgi"]:
            run_test(cmd)
        print() # Salto de línea para separar secciones
        
        # Opcional: limpiar el archivo de configuración después de las pruebas
        # os.remove(config_name)

if __name__ == "__main__":
    main()
