#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include "struct.hpp"

class Conf
{
	private:

	public:
	ServerConfig c;
	
	Conf(); //no deberíamos inicializar la clase sin un archivo de configuración. Pero este va a ser el de pruebas.
	
	Conf(const std::string fileName);
	Conf(const Conf &other);
	Conf& operator=(const Conf& other);
	~Conf();

	void Read(std::string fileName);

	const Conf* getServerConf() const;
};
