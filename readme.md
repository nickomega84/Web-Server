# WEBSERV

### SERVIDOR
Para usar el servidor usa make y ejecuta ./webserv.
#### Inicializar el servidor:

- La clase Server necesita ser instanciada con un puntero a una clase Config.
 
- En el futuro Config deberá leer un archivo de configuración. Provisionalmente se puede instanciar con valores por defecto.
 
#### Usar el servidor:

- void setUpListenSocket(); llamalo una vez por cada servidor que quieras desplegar.

- void setUpEpoll(); de acuerdo al subject solo deberiamos crear una instancia de epoll, llamalo cuando hayas inicializado todos los servidores.

### CLIENTE
Para desplegar el cliente abre otro terminal y usa 'make client && ./client'. Se conectará un servidor con valores por defecto.

 - Si cambias el host y el puerto del servidor deberas cambiar others/client_ALT para que el cliente se conecte.
   
 - El servidor debe ser capaz de manejar varios clientes a la vez.

#### Navegador

Para conectarte al servidor desde el navegador usa: http//localhost:8080

### TEORIA

La teoria del funcionamiento del server esta en src/Server.cpp
