#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include "../../include/config/configStruct.hpp"

class ConfigTEMPORAL
{
	private:
	void	initConfigStruct(configStruct &other);
	void	initLocationConfig(locationConfig &other);

	public:
	configStruct c;
	
	ConfigTEMPORAL(); //no deberíamos inicializar la clase sin un archivo de ConfigTEMPORALiguración. Pero este va a ser el de pruebas.
	ConfigTEMPORAL(const std::string fileName);
	ConfigTEMPORAL(const ConfigTEMPORAL &other);
	ConfigTEMPORAL& operator=(const ConfigTEMPORAL& other);
	~ConfigTEMPORAL();

	void	Read(std::string fileName);
	const	ConfigTEMPORAL* getServerConf() const;
};
