# WEBSERV

### SERVER
- Para usar el servidor usa make y ejecuta ./webserv. Por defecto usara el puerto 8080 y cualquier dirección IP.
	- Instancia Server:
		- Server necesita ser instanciado con un puntero a una instancia de Config. 
		- En el futuro Config deberá leer un archivo de configuración para instanciarse. Provisionalmente se puede crear con valores por defecto. 
	- Usa el server con:
		- void setUpListenSocket(); llamalo una vez por cada servidor que queramos desplegar
		- void setUpEpoll(); de acuerdo al subject solo deberiamos crear una instancia de epoll, llamalo cuando hayas inicializado todos los servidores
- Para desplegar el cliente abre otro terminal y usa 'make client && ./client'. Se conectará al servidor.
	- el servidor es capaz de manejar varios clientes a la vez.

### NAVEGADOR
- Para conectarte al servidor desde el navegador usa: http//localhost:8080

### TEORIA
- La teoria del funcionamiento del server esta en src/Server.cpp
