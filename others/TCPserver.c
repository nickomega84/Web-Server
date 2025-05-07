#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

int main(int argc, char *argv[])
{
//SOCKET
	int server_socket;

	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}

//BIND
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;
	//address.sin_addr.s_addr = inet_addr("127.0.0.1"); //para usar local host
	bzero(&address.sin_zero, sizeof(address.sin_zero));

	if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("In bind");
		exit(EXIT_FAILURE);
	}

//LISTEN
	if (listen(server_socket, 10) < 0)
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
		if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
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
	close (server_socket);
	return 0;
}

//RESUMEN
/* Crear un socket de servidor (socket()).

Vincular el socket a una dirección local (bind()).

Escuchar connectiones entrantes (listen()).

Aceptar connectiones (accept()):

Cada vez que accept() se llama, se obtiene un nuevo socket para manejar la comunicación con el cliente.

El servidor puede seguir aceptando más connectiones con más llamadas a accept(). */