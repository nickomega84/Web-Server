#include "Server.hpp"

//SOCKADDR_IN // es una variante de sockaddr, hay que rellenarla para usarla en funciones como bind()
/* 	struct sockaddr_in {
		sa_family_t    sin_family;   // communication domain in which the socket should be created. AF_INET for IPv4.
		in_port_t      sin_port;     // Port (en orden de red) (most usual is 8080) //La frase "en orden de red" se refiere al formato específico en el que las direcciones IP y otros datos se organizan al ser transmitidos a través de una red. //El orden de red sigue el formato big-endian, lo que significa que el byte de mayor peso (el más significativo) se coloca primero, seguido del byte de menor peso. Este formato es independiente de la arquitectura del sistema y asegura que las direcciones IP y puertos sean interpretados correctamente por diferentes dispositivos de red, independientemente de su arquitectura interna.
		struct in_addr sin_addr;     // Dirección IP (en orden de red) //If you’re a client and won’t be receiving incoming connections, you’ll usually just let the operating system pick any available port number by specifying port 0. If you’re a server, you’ll generally pick a specific number since clients will need to know a port number to connect to. //INADDR_ANY se usa cuando quieres que el socket se asocie con todas las direcciones IP disponibles en el host (tiene el valor 0x00000000) (poner 0 o INADDR_ANY es lo mismo) //ALTERNATIVA: servaddr.sin_addr.s_addr = htonl(2130706433); //2130706433 es la representación uint32t de 127.0.0.1 //127.0.0.1 es localhost, lo que significa que a este servidor solo se puede acceder desde el mismo ordenador
		unsigned char  sin_zero[8];  // sin_zero: Un arreglo de 8 bytes usado para relleno, de modo que la estructura tenga el mismo tamaño que struct sockaddr (que es la estrcutura que bind() espera recibir). No se usa y debe llenarse con ceros.
	};

	struct in_addr {
		in_addr_t s_addr;  // Dirección en formato binario (en orden de red)
	}; */
//htons() toma un valor de tipo short (es decir, 16 bits) en el orden de bytes de la máquina local y lo convierte al orden de bytes de la red (big-endian).
	
Server::Server(): server_socket(-1)
{
	address.sin_family = AF_INET; //Un miembro dentro de una estructura no se puede inicializar en la lista de inicialización. Hay que inicializarlos en la función
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;
	bzero(&address.sin_zero, sizeof(address.sin_zero));
}

Server::Server(int port): server_socket(-1)
{
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	bzero(&address.sin_zero, sizeof(address.sin_zero));
}

Server::~Server()
{
	if (server_socket > 0)
		close(server_socket);
	for (std::vector<int>::iterator it = client_sockets.begin(); it != client_sockets.end(); it++)
		close(*it);
	client_sockets.clear();
}


//SOCKET
/* int socket(int domain, int type, int protocol);
- domain (AF_INET): define el tipo de red que se va a utilizar. Responsable de identificar y localizar dispositivos dentro de una red, y establece cómo se deben enrutar los datos de un dispositivo a otro a través de diferentes redes.
    AF_INET se utiliza para una red basada en IPv4 (Internet Protocol versión 4).
- type (SOCK_STREAM): especifica el tipo de socket. Diferentes tipos de sockets están diseñados para diferentes tipos de protocolos de comunicación.
    SOCK_STREAM se utiliza para crear un socket orientado a una conexión capaz de enviar y recibir streams de información (el porotocolo más usado para estos sockets es TCP (Transmission Control Protocol)
- protocol (0): Este argumento especifica el protocolo a usar. Habitualmente, se pone 0 para que el sistema elija el protocolo adecuado en función del tipo de socket.
    Si se utilizan AF_INET y SOCK_STREAM, el protocolo seleccionado automáticamente será TCP.
El valor de retorno de la función socket es el descriptor de archivo del socket, o -1 si ocurre un error */

//BIND
/* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
bind() se utiliza para asociar un descriptor de archivo de socket a una dirección específica (en este caso, una dirección IP y un puerto)
sockfd: Es el descriptor de archivo del socket, que generalmente es creado previamente con la función socket().
addr: Es un puntero a una estructura sockaddr que contiene la dirección (IP y puerto) a la que se desea asociar el socket.
addrlen: Es el tamaño de la estructura sockaddr que contiene la dirección. Usualmente, se usa sizeof(struct sockaddr_in). */

//LISTEN
/* int listen(int sockfd, int backlog);
listen() convierte un socket de servidor en un socket de escucha. Hace que el servidor este preparado para aceptar connectiones entrantes de clientes. 
sockfd: el descriptor de archivo del socket.
backlog: el número máximo de connectiones pendientes que el sistema puede mantener en la cola para ser aceptadas por accept(). */

void Server::setUpServer()
{
	if ((server_socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Error creating server socket");
	if (::bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("Error binding server_socket");
	if (::listen(server_socket, MAX_CONN) < 0)
		throw std::runtime_error("Error in listen()");
	Server::epoll();
}

/* 
Epoll is an API (Application Programming Interface, es un puente entre aplicaciones). Monitors multiple file descriptors to see if I/O is possible on any of them.

    The central concept of the epoll API is the epoll instance, an  in-ker‐
    nel data structure which, can be considered as a container for two lists:

    • The interest list (sometimes also called the epoll set): the  set  of
      file descriptors that the process has registered an interest in moni‐
      toring.
    • The ready list: the set of file descriptors that are "ready" for I/O.
      The  ready  list  is a subset of (or, more precisely, a set of refer‐
      ences to) the file descriptors in the interest list.  The ready  list
      is dynamically populated by the kernel as a result of I/O (In and Out) activity on
      those file descriptors.

int epoll_create(int size); // creates  a new epoll instance and returns a file descriptor referring to that instance.
- size: Se usaba en versiones antiguas de Linux, pero no es relevante en sistemas modernos. Se puede poner cualquier valor. Se va a ingnorar. Por convención se suele poner 1.
Devuelve: Un descriptor de archivo para la instancia epoll.

int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout); //waits for I/O events, blocking the calling thread if no events are currently available.

- epfd: El descriptor de archivo devuelto por epoll_create.
- events: Puntero a un arreglo de estructuras epoll_event donde se almacenarán los eventos que el kernel detecte.
- maxevents: Número máximo de eventos que el arreglo events puede contener. Debe ser mayor que 0.
- timeout: Tiempo de espera en milisegundos:
	* 0: retorno inmediato (polling no bloqueante).
	* -1: espera indefinidamente hasta que ocurra un evento.
	* > 0: espera hasta ese número de milisegundos.
Devuelve el número de descriptores de archivo listos (i.e. número de elementos llenados en events) si tiene éxito.
Devuelve -1 si ocurre un error y establece errno.
*/

void Server::epoll()
{	
	int epollfd = epoll_create(1);
    if (epollfd < 0)
		throw std::runtime_error("Error on epoll_create");

	epoll_ctl_call(epollfd, server_socket, EPOLLIN); // agregamos el socket del servidor a epoll, cada vez que haya una nueva conexión lo escucharemos en server_socket

	std::cout << "WAITING FOR CONNECTIONS:" << std::endl << std::endl;
	struct epoll_event events[MAX_EVENTS]; // epoll wait escribe aqui cuando recibe eventos de los sockets que está rastreando. Si el número de eventos recibido sobrepasa MAX_EVENTS, recibira los sobrantes en la siguiente llamada a epoll_wait()
    while (1)
	{
        int event_nmb = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (event_nmb == -1)
			throw std::runtime_error("Error on epoll_wait()");

        for (int i = 0; i < event_nmb; i++)
		{
			if (events[i].data.fd == server_socket) // si registramos una lectura en server_socket, tenemos una nueva conexión
			{
				int client_socket = accept_connection(server_socket);
				epoll_ctl_call(epollfd, client_socket, EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP); // los sockets de los clientes deberan estar atentos a leer y escribir (de acuerdo al subject) // si un mismo socket de cliente recibe varios eventos a la vez, se acumulan todos en epoll_event.events
            }
			else // el evento no es una nueva conexión
			{
				if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & (EPOLLIN | EPOLLOUT))) // !(events[i].events & (EPOLLIN | EPOLLOUT): si el fd ha generado eventos, pero ninguno de ellos es de lectura o escritura tenemos que cerrarlo
				{
					close_client_socket(events[i].data.fd);
					continue;
				}	
				if (events[i].events & EPOLLIN) // .events es un uint32_t, & es el operador: bitwise AND, la condición sera verdad si los bits que corresponden al valor de EPOLLIN están activos
					recv_data(events[i].data.fd);
				if (events[i].events & EPOLLOUT)
				{
					std::cout << "ENVIANDO COSITAS" << std::endl;
					send_data("Hello from the server. I could be in the beach right now ;_;", events[i].data.fd);
				}
			}
		}
    }
	close(epollfd);
}


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
EPOLLHUP (Hang-up, cierre de conexión) Se activa cuando el otro extremo del socket ha cerrado la conexión. Siempre se monitorea automáticamente, pero conviene incluirlo en epoll_data_t 

typedef union epoll_data {
    void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;
- ptr: Un puntero genérico que puedes usar para apuntar a estructuras personalizadas, contextos, buffers, etc.
- fd: El descriptor de archivo directamente.
- u32 y u64: Puedes usarlos para almacenar datos enteros adicionales (por ejemplo, identificadores, flags, etc).
*/

void	Server::epoll_ctl_call(int epollfd, int socket, uint32_t events)
{
	struct epoll_event event_struct;

	event_struct.events = events; // agregamos el socket del servidor a epoll, cada vez que haya una nueva conexión lo escucharemos en server_socket
	event_struct.data.fd = socket;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socket, &event_struct) < 0)
		throw std::runtime_error("Error on epoll_ctl");
}


/* int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
Se usa en servidores para aceptar connectiones entrantes desde un cliente. Una vez que un cliente intenta conectarse al servidor, accept crea un nuevo socket para manejar la comunicación con ese cliente, dejando el socket original (el de escucha) disponible para aceptar otras connectiones.
sockfd: El descriptor de archivo del socket de escucha.
addr: Un puntero a una estructura sockaddr. Utilizado para almacenar la información sobre la dirección del cliente que se conecta. En este caso no nos interesa guardar esa información, así que ponemos NULL.
addrlen: El tamaño de la estructura sockaddr que está siendo pasada. Inicialmente, este valor debe ser el tamaño de la estructura que se espera, y después de la llamada a accept, contendrá el tamaño real de la dirección del cliente. */

int Server::accept_connection(int server_socket)
{
	int client_socket = -1;

	if ((client_socket = ::accept(server_socket, NULL, NULL)) < 0)
		throw std::runtime_error("Error accepting incoming connection");
	client_sockets.push_back(client_socket);
	std::cout << "New connection accepted() fd = " << client_socket << std::endl;
	return (client_socket);
}


/* ssize_t recv(int socket, void *buffer, size_t length, int flags);
- socket: Descriptor del socket desde el que se recibirán los datos.
- buffer: Puntero al área de memoria donde se almacenarán los datos recibidos.
- length: Número máximo de bytes a recibir.
- flags: Opciones para modificar el comportamiento de recv() (por ejemplo, MSG_WAITALL, MSG_PEEK*/

std::string Server::recv_data(const int new_socket) const
{
	char buffer[BUFFER_SIZE];

	int bytes_read = ::recv(new_socket, buffer, sizeof(buffer), 0);
	if (bytes_read < 0)
		throw std::runtime_error("Error on recv");
	if (bytes_read == 0)
		throw std::runtime_error("Error on recv: fd closed, connection lost");
	std::string input(buffer, bytes_read);
	std::cout << "Message from fd " << new_socket << ": " << input << std::endl;  
	return (input);
}


/* ssize_t send(int socket, const void *buffer, size_t length, int flags);
- socket: Descriptor del socket a través del cual se enviarán los datos.
- buffer: Puntero al bloque de datos que se enviará.
- length: Tamaño (en bytes) de los datos que se enviarán.
- flags: Modificadores opcionales para la operación de envío (como MSG_DONTWAIT, MSG_NOSIGNAL, etc.).
Devuelve el número de bytes enviados si tiene éxito.
Si hay un error, devuelve -1 y establece errno. */

void Server::send_data(std::string message, const int new_socket) const
{
	int bytes_sent;
	bytes_sent = ::send(new_socket, message.c_str(), message.size(), 0);
	if (bytes_sent < 0)
		throw std::runtime_error("Error on send");
	if (bytes_sent == 0)
		throw std::runtime_error("Error on send: fd closed, connection lost");
}


// Cerramos el socket y lo eliminamos de la lista de sockets abiertos
void Server::close_client_socket(const int fd)
{
	std::cout << "Something went wrong in client_socket: " << fd << ", closing" << std::endl;
	close(fd);
	std::vector<int>::iterator it = std::find(client_sockets.begin(), client_sockets.end(), fd);
	if (it != client_sockets.end())
		client_sockets.erase(it);
}
