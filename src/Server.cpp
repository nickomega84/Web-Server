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
	
Server::Server(): server_fd(-1)
{
	address.sin_family = AF_INET; //No se puede inicializar en la lista de inicialización un miembro dentro de una estructura. Hay que inicializarlos en la función
	address.sin_port = htons(PORT);
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


/* int socket(int domain, int type, int protocol);

domain (AF_INET): define el tipo de red que se va a utilizar. Responsable de identificar y localizar dispositivos dentro de una red, y establece cómo se deben enrutar los datos de un dispositivo a otro a través de diferentes redes.
    AF_INET se utiliza para una red basada en IPv4 (Internet Protocol versión 4).

type (SOCK_STREAM): especifica el tipo de socket. Diferentes tipos de sockets están diseñados para diferentes tipos de protocolos de comunicación.
    SOCK_STREAM se utiliza para crear un socket orientado a la conexión: un socket TCP

protocol (0): Este argumento especifica el protocolo a usar, en la mayoría de los casos, se pone 0 para que el sistema elija automáticamente el protocolo adecuado en función del tipo de socket.
    Si se utiliza AF_INET y SOCK_STREAM, el protocolo seleccionado automáticamente será TCP.

El valor de retorno de la función socket es el descriptor de archivo del socket, o -1 si ocurre un error */

void Server::socket()
{
	if ((server_fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Error creating server socket");
}


/* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

bind() se utiliza para asociar un descriptor de archivo de socket a una dirección específica (en este caso, una dirección IP y un puerto)
sockfd: Es el descriptor de archivo del socket, que generalmente es creado previamente con la función socket().
addr: Es un puntero a una estructura sockaddr que contiene la dirección (IP y puerto) a la que se desea asociar el socket.
addrlen: Es el tamaño de la estructura sockaddr que contiene la dirección. Usualmente, se usa sizeof(struct sockaddr_in). */

void Server::bind()
{
	if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("Error binding server_fd");
}


/* int listen(int sockfd, int backlog);

listen() convierte un socket de servidor en un socket de escucha. Hace que el servidor este preparado para aceptar conexiones entrantes de clientes. 
sockfd: el descriptor de archivo del socket.
backlog: el número máximo de conexiones pendientes que el sistema puede mantener en la cola para ser aceptadas por accept(). */

void Server::listen()
{
	if (::listen(server_fd, 10) < 0)
		throw std::runtime_error("Error in listen()");
}

void Server::setUpServer()
{
	Server::socket();
	Server::bind();
	Server::listen();
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

std::vector<char> Server::read(int socket)
{
	long bytes_read;
	char buffer[1024];
	std::vector<char> data;

	while ((bytes_read = ::read(socket, buffer, sizeof(buffer))) > 0) //data() devuelve un puntero a la memoria interna del vector
		data.insert(data.end(), buffer, buffer + bytes_read);
	if (bytes_read < 0)
		throw std::runtime_error("Error reading socket");
	return (data);
}

void Server::write(int socket, std::string httpMessage)
{
	::write(socket, httpMessage.c_str(), httpMessage.size());
}
