#include "Server.hpp"

//SOCKADDR_IN
/* 	struct sockaddr_in {
		sa_family_t    sin_family;   // communication domain in which the socket should be created. AF_INET for IPv4.
		in_port_t      sin_port;     // Port (en orden de red) (most usual is 8080) //La frase "en orden de red" se refiere al formato específico en que las direcciones IP y otros datos se organizan al ser transmitidos a través de una red. //El orden de red sigue el formato big-endian, lo que significa que el byte de mayor peso (el más significativo) se coloca primero, seguido del byte de menor peso. Este formato es independiente de la arquitectura del sistema y asegura que las direcciones IP y puertos sean interpretados correctamente por diferentes dispositivos de red, independientemente de su arquitectura interna.
		struct in_addr sin_addr;     // Dirección IP (en orden de red) //If you’re a client and won’t be receiving incoming connections, you’ll usually just let the operating system pick any available port number by specifying port 0. If you’re a server, you’ll generally pick a specific number since clients will need to know a port number to connect to (EX: servidor.sin_addr.s_addr = inet_addr("127.0.0.1");). //INADDR_ANY se usa cuando quieres que el socket se asocie con todas las direcciones IP disponibles en el host (tiene el valor 0x00000000) (poner 0 o INADDR_ANY es lo mismo) //ALTERNATIVA: servaddr.sin_addr.s_addr = htonl(2130706433); //2130706433 es la representación uint32t de 127.0.0.1 //127.0.0.1 es localhost, lo que significa que a este servidor solo se puede acceder desde el mismo ordenador
		unsigned char  sin_zero[8];  // sin_zero: Un arreglo de 8 bytes usado para relleno, de modo que la estructura tenga el mismo tamaño que struct sockaddr (que es la estrcutura que bind() espera recibir). No se usa y debe llenarse con ceros.
	};

	struct in_addr {
		in_addr_t s_addr;  // Dirección en formato binario (en orden de red)
	}; */
//htons() toma un valor de tipo short (es decir, 16 bits) en el orden de bytes de la máquina local y lo convierte al orden de bytes de la red (big-endian).
	
Server::Server(): server_port(-1), server_fd(-1)
{
	address.sin_family = AF_INET; //No se puede inicializar en la lista de inicialización un miembro dentro de una estructura. Hay que inicializarlos en la función
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;
	bzero(&address.sin_zero, sizeof(address.sin_zero));
}

Server::Server(int port): server_port(port), server_fd(-1)
{
	address.sin_family = AF_INET; //No se puede inicializar en la lista de inicialización un miembro dentro de una estructura. Hay que inicializarlos en la función
	address.sin_port = htons(server);
	address.sin_addr.s_addr = INADDR_ANY;
	bzero(&address.sin_zero, sizeof(address.sin_zero));
}

Server::~Server()
{
	if (server_fd > 0)
		close(server_fd);
	for (std::vector<int>::iterator it = new_sockets.begin(); it != new_sockets.end(); it++)
		close(*it);
	new_sockets.clear();
}


//SOCKET
/* int socket(int domain, int type, int protocol);
- domain (AF_INET): define el tipo de red que se va a utilizar. Responsable de identificar y localizar dispositivos dentro de una red, y establece cómo se deben enrutar los datos de un dispositivo a otro a través de diferentes redes.
    AF_INET se utiliza para una red basada en IPv4 (Internet Protocol versión 4).
- type (SOCK_STREAM): especifica el tipo de socket. Diferentes tipos de sockets están diseñados para diferentes tipos de protocolos de comunicación.
    SOCK_STREAM se utiliza para crear un socket orientado a la conexión: un socket TCP
- protocol (0): Este argumento especifica el protocolo a usar, en la mayoría de los casos, se pone 0 para que el sistema elija automáticamente el protocolo adecuado en función del tipo de socket.
    Si se utiliza AF_INET y SOCK_STREAM, el protocolo seleccionado automáticamente será TCP.
El valor de retorno de la función socket es el descriptor de archivo del socket, o -1 si ocurre un error */

//BIND
/* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
bind() se utiliza para asociar un descriptor de archivo de socket a una dirección específica (en este caso, una dirección IP y un puerto)
sockfd: Es el descriptor de archivo del socket, que generalmente es creado previamente con la función socket().
addr: Es un puntero a una estructura sockaddr que contiene la dirección (IP y puerto) a la que se desea asociar el socket.
addrlen: Es el tamaño de la estructura sockaddr que contiene la dirección. Usualmente, se usa sizeof(struct sockaddr_in). */

//LISTEN
/* int listen(int sockfd, int backlog);
listen() convierte un socket de servidor en un socket de escucha. Hace que el servidor este preparado para aceptar conexiones entrantes de clientes. 
sockfd: el descriptor de archivo del socket.
backlog: el número máximo de conexiones pendientes que el sistema puede mantener en la cola para ser aceptadas por accept(). */

void Server::setUpServer()
{
	if ((server_fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Error creating server socket");
	if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("Error binding server_fd");
	if (::listen(server_fd, 10) < 0)
		throw std::runtime_error("Error in listen()");
}


/* int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
Se usa en servidores para aceptar conexiones entrantes desde un cliente. Una vez que un cliente intenta conectarse al servidor, accept crea un nuevo socket para manejar la comunicación con ese cliente, dejando el socket original (el de escucha) disponible para aceptar otras conexiones.
sockfd: El descriptor de archivo del socket de escucha.
addr: Un puntero a una estructura sockaddr. utilizado para almacenar la información sobre la dirección del cliente que se conecta.
addrlen: El tamaño de la estructura sockaddr que está siendo pasada. Inicialmente, este valor debe ser el tamaño de la estructura que se espera, y después de la llamada a accept, contendrá el tamaño real de la dirección del cliente. */

int Server::accept()
{
	int new_socket;
	int addrLen = sizeof(address);

	if ((new_socket = ::accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrLen)) < 0)
		throw std::runtime_error("Error accepting incoming conexion");
	new_sockets.push_back(new_socket);
	std::cout << "new conexion on socketfd: " << new_socket << std::endl;
	return (new_socket);
}


/* ssize_t recv(int socket, void *buffer, size_t length, int flags);
- socket: Descriptor del socket desde el que se recibirán los datos.
- buffer: Puntero al área de memoria donde se almacenarán los datos recibidos.
- length: Número máximo de bytes a recibir.
- flags: Opciones para modificar el comportamiento de recv() (por ejemplo, MSG_WAITALL, MSG_PEEK*/

std::string Server::recv(int new_socket)
{
	int bytes_read;
	char buffer[1024];
	std::string input;

	bytes_read = ::recv(new_socket, buffer, sizeof(buffer), 0);
	if (bytes_read < 0)
		throw std::runtime_error("In recv");
	return (input.append(buffer));
}


/* ssize_t send(int socket, const void *buffer, size_t length, int flags);
- socket: Descriptor del socket a través del cual se enviarán los datos.
- buffer: Puntero al bloque de datos que se enviará.
- length: Tamaño (en bytes) de los datos que se enviarán.
- flags: Modificadores opcionales para la operación de envío (como MSG_DONTWAIT, MSG_NOSIGNAL, etc.).
Devuelve el número de bytes enviados si tiene éxito.
Si hay un error, devuelve -1 y establece errno. */

void Server::send(std::string message, int new_socket)
{
	::send(new_socket, message.c_str(), message.size(), 0);
}


/* int epoll_create(int size);  // Crea la instancia epoll
- size: Se usaba en versiones antiguas de Linux, pero no es relevante en sistemas modernos. Se puede poner cualquier valor. Se va a ingnorar. Por convención se suele poner 1.
Devuelve: Un descriptor de archivo para la instancia epoll. */

/* int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);  // Añadir o modificar sockets
- epfd: Descriptor de epoll creado con epoll_create1().
- op: Operación a realizar:
    EPOLL_CTL_ADD: Añadir un socket a epoll.
    EPOLL_CTL_MOD: Modificar los eventos asociados a un socket.
    EPOLL_CTL_DEL: Eliminar un socket de epoll.
- fd: Descriptor del socket que queremos administrar.
- event: Puntero a una estructura epoll_event que define los eventos de interés.
Devuelve: 0 si tiene éxito, -1 si hay error.

struct epoll_event 
{
    uint32_t events;   //Tipos de eventos que queremos monitorear para ese socket, es decir cual va a ser la funcion del socket. EPOLLIN para leer y EPOLLOUT para escribir. También puedes combinar varios eventos usando el operador | (ej: ev.events = EPOLLIN | EPOLLOUT;). Puedes cambiar los eventos a monitorear en un fd después de haberlo registrado con epoll_ctl(). Llama a epoll_ctl() nuevamente, pero con la operación EPOLL_CTL_MOD
    epoll_data_t data; //Información adicional, normalmente el descriptor de archivo
};
EPOLLIN indica que el socket tiene datos listos para ser leídos.
EPOLLOUT descriptor de archivo está listo para escritura sin bloquear, lo que significa que puedes enviar datos (send(), write(), etc.) sin preocuparte de que la operación se bloquee mientras el socket espera estar disponible. Cuando un socket TCP está congestionado (por ejemplo, por muchos datos que aún no han sido enviados), la llamada a send() podría bloquear el proceso si el buffer de envío está lleno. EPOLLOUT te permite saber cuándo es seguro escribir sin bloqueo, evitando esperas innecesarias.
EPOLLERR (Error en el descriptor de archivo) Indica que ocurrió un error en el socket (por ejemplo, desconexión abrupta o fallo en la conexión). Siempre se monitorea automáticamente, pero conviene incluirlo en epoll_data_t
EPOLLHUP (Hang-up, cierre de conexión) Se activa cuando el otro extremo del socket ha cerrado la conexión. Siempre se monitorea automáticamente, pero conviene incluirlo en epoll_data_t */


int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);  // Esperar eventos

void Server::epoll()
{
    int epfd = epoll_create(1);  // Crear epoll instance
    if (epfd < 0)
		throw runtime_error("Error on epoll_create");

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;  // EPOLLIN modifica el comportamiento de epoll_wait(), indica que queremos monitorear si hay datos disponibles para lectura en el sockfd. Si epoll_wait() detecta actividad en el socket con EPOLLIN, retorna el descriptor de archivo como listo para lectura. Si no incluimos EPOLLIN, el descriptor de archivo no será monitoreado para lectura y no recibirás notificaciones cuando haya datos disponibles.
    ev.data.fd = sockfd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) < 0)
		throw runtime_error("Error on epoll_ctl");

    struct epoll_event events[10]; // Buffer para eventos
    while (1)
	{
        int num_events = epoll_wait(epfd, events, 10, -1);  // Esperar eventos indefinidamente
        for (int i = 0; i < num_events; i++) 
		{
            if (events[i].events & EPOLLIN) 
			{
                char buffer[1024];
                ssize_t bytes = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (bytes > 0) 
				{
                    buffer[bytes] = '\0';
                    printf("Mensaje recibido: %s\n", buffer);
                }
            }
        }
    }

    close(epfd);
    close(sockfd);
    return 0;
}