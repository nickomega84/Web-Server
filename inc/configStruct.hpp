#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include <sys/ioctl.h>
#include <csignal>
#include <cstring>
#include <vector>
#include <map>
#include <sstream>
#include <string>

struct locationConfig 
{
	std::string location_name;				 	// Ruta del bloque location (ej. "/images/")
	std::string location_root;				 	// Ruta raíz del sistema de archivos para esta location
	std::string index;							// Archivo por defecto a servir (ej. "index.html")
	bool		autoindex;						// Si está activo, el path solicitado corresponde a un directorio y no se encuentra el archivo indicado por 'index', se muestra un listado del contenido del directorio solicitado
	bool		getOn;							// Habilita método GET
	bool		postOn;							// Habilita método POST
	bool		deleteOn;						// Habilita método DELETE
	std::map<std::string, std::string> extPath;	// Mapeo extensión o path → ejecutable (usado en CGI, etc.)
};

typedef struct configStruct 
{
	int			numServs;						// Cantidad total de servidores (uso interno)
	int			iter;							// Índice o iterador dentro de la lista de servidores (uso interno)
	std::string server_name;					// Nombre del servidor (directiva server_name)
	std::string host;							// IP o hostname donde escucha, puede estar vacío para que escuche a cualquier dirección IP
	std::string	port;							// Puerto donde escucha
	std::string root;							// Ruta raíz general del servidor
	std::string init_root;						// Copia de la raíz original (antes de ser modificada)
	std::string activeDirectory;				// Directorio activo en uso (puede cambiar en ejecución)
	std::string user;							// Usuario del proceso o sesión del servidor
	std::string index;							// Archivo por defecto a servir si no se especifica uno
	size_t		body_size;						// Tamaño máximo del cuerpo de la petición (en bytes)
	bool		get_allowed;					// GET permitido a nivel global
	bool		post_allowed;					// POST permitido a nivel global
	bool		delete_allowed;					// DELETE permitido a nivel global
	bool		autoindex;						// Autoindex permitido a nivel global
	std::map<int, std::string>	errors;			// Mapeo de código de error HTTP a página personalizada
	std::vector<locationConfig>	locations;		// Lista de bloques location definidos

}	configStruct;
