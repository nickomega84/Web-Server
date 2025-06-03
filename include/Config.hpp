#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include "ReadConfig.hpp"

class Config
{
	private:
	void	initConfigStruct(configStruct &other);
	void	initLocationConfig(locationConfig &other);
	public:
	configStruct c;
	
	Config(); //no deberíamos inicializar la clase sin un archivo de Configiguración. Pero este va a ser el de pruebas.
	
	Config(const std::string fileName);
	Config(const Config &other);
	Config& operator=(const Config& other);
	~Config();

	void	Read(std::string fileName);
	const	Config* getServerConf() const;
};