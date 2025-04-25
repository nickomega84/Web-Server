#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
//SOCKET
	int server_fd;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}

//BIND
	struct sockaddr_in address; //For IP networking, we use struct sockaddr_in, which is defined in the header netinet/in.h. Before calling bind, we need to fill out this structure.

	address.sin_family = AF_INET; //communication domain in which the socket should be created. AF_INET = IP
	address.sin_port = htons(8080); //8080 es el más habitual
	address.sin_addr.s_addr = INADDR_ANY; //If you’re a client and won’t be receiving incoming connections, you’ll usually just let the operating system pick any available port number by specifying port 0. If you’re a server, you’ll generally pick a specific number since clients will need to know a port number to connect to (EX: servidor.sin_addr.s_addr = inet_addr("127.0.0.1");). //Cuando se utiliza INADDR_ANY en el campo sin_addr.s_addr, significa que el servidor puede recibir conexiones en cualquier dirección IP disponible en la máquina (es decir, no se asocia a una IP específica). //En el contexto de un servidor, si tienes múltiples interfaces de red (como Ethernet y Wi-Fi), el servidor puede aceptar conexiones de cualquier red. //ALTERNATIVA /*servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1 //2130706433 es la representación uint32t de 127.0.0.1 //127.0.0.1 es localhost, lo que significa que a este servidor solo se puede acceder desde el mismo ordenador*/ 
	bzero(&address.sin_zero, sizeof(address.sin_zero)); //sockaddr_in debe llenar ese mismo tamaño para garantizar que puedas pasarla como argumento a las funciones que esperan una struct sockaddr, como bind(), connect()

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("In bind");
		exit(EXIT_FAILURE);
	}

//LISTEN
	if (listen(server_fd, 10) < 0) //(La función listen() convierte un socket de servidor en un socket de escucha. Esto significa que el servidor está preparado para aceptar conexiones entrantes de clientes.). es el numero de conexiones simultaneas que vamos a permitir (cuántas conexiones entrantes pueden estar en espera antes de que el sistema operativo empiece a rechazar nuevas conexiones.) (Cuando el número de conexiones entrantes supera el valor de backlog, las conexiones adicionales pueden ser rechazadas o no aceptadas por el servidor hasta que se libere espacio en la cola de espera.) (Si hay espacio en la cola (es decir, si el número de conexiones en espera es menor que el tamaño especificado por backlog), las conexiones adicionales se almacenarán hasta que el servidor las acepte.)
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
//ACCEPT
		int new_socket;
		int addrlen = sizeof(address);

		printf("\n--------------WAITING FOR CONNECTION--------------\n\n");
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) //La función accept() es la que finalmente permite que el servidor acepte una conexión entrante de un cliente. En otras palabras, accept() extrae una conexión pendiente de la cola y la convierte en un socket de comunicación que el servidor puede utilizar para enviar y recibir datos con el cliente.
		{
            perror("In accept");
            exit(EXIT_FAILURE);
        }

//READ
		char buffer[30000] = {0};
		long valread;
		char* salute = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nNANANANA!";
		valread = read(new_socket, buffer, sizeof(buffer));
        printf("%s\n",buffer );

//WRITE
        write(new_socket , salute , strlen(salute));
        printf("------------------Hello message sent-------------------\n");

        close(new_socket);
	}
	close (server_fd);
	return 0;
}

//SOCKADDR_IN
/* 	struct sockaddr_in {
		sa_family_t    sin_family;   // Familia de direcciones (siempre AF_INET para IPv4)
		in_port_t      sin_port;     // Puerto (en orden de red)
		struct in_addr sin_addr;     // Dirección IP (en orden de red)
	
		// sin_zero: Un arreglo de 8 bytes usado para relleno, de modo que la estructura tenga el mismo tamaño que struct sockaddr. No se usa y debe llenarse con ceros.
		unsigned char  sin_zero[8];
	}; */

/* 	struct in_addr {
		in_addr_t s_addr;  // Dirección en formato binario (también en orden de red)
	}; */

/* int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
sockfd: Es el descriptor de archivo del socket de escucha previamente configurado con listen().

addr: Es un puntero a una estructura sockaddr donde se almacenará la dirección del cliente que se ha conectado. Este parámetro puede ser NULL si no necesitas esta información.

addrlen: Es el tamaño de la estructura sockaddr. Al entrar a la función, este valor debe contener el tamaño máximo que puede tener la estructura, y al salir de la función, contendrá el tamaño de la dirección del cliente que se ha conectado. */

//ACCEPT
//new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)
//Devuelve un nuevo socket: accept() devuelve un nuevo descriptor de archivo que se usa para la comunicación con el cliente. El socket original que fue usado en listen() sigue existiendo y se puede usar para seguir aceptando más conexiones, pero para cada cliente que se conecta, se crea un nuevo socket de comunicación.
//Proporciona la dirección del cliente: La dirección del cliente que se ha conectado se almacena en la estructura sockaddr que se pasa como argumento. Esto puede ser útil para saber desde qué dirección o puerto se ha conectado el cliente.

//RESUMEN
/* Flujo general de listen() y accept()

    Crear un socket de servidor (socket()).

    Vincular el socket a una dirección local (bind()).

    Escuchar conexiones entrantes (listen()).

    Aceptar conexiones (accept()):

        Cada vez que accept() se llama, se obtiene un nuevo socket para manejar la comunicación con el cliente.

        El servidor puede seguir aceptando más conexiones con más llamadas a accept(). */
