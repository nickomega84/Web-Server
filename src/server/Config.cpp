#include "../../include/server/Config.hpp"

Config::Config() //solo para pruebas //basado en default.conf
{
    initConfigStruct(c);
	c.numServs = 1;
    c.iter = 1;
    c.server_name = "Default";
    c.host = "127.0.0.1";
    c.port = "8080";
    c.root = "web_root";
    c.index = "index.html";
    c.body_size = 30000;
    c.errors[404] = "/error/404.tml";

	locationConfig def;
	initLocationConfig(def);
	def.location_name = "/";
	def.location_root = "./";
	def.getOn = true;
	def.postOn = true;
	def.deleteOn = true;
	c.locations.push_back(def);

	locationConfig cgi_bin;
	initLocationConfig(cgi_bin);
	def.location_name = "/cgi_bin";
	def.location_root = "./";
	/* def.index = calculator.py; */
	def.getOn = true;
	def.postOn = true;
	def.deleteOn = true;
	c.locations.push_back(cgi_bin);
}

Config::Config(std::string fileName)
{
	initConfigStruct(c);
	Read(fileName);
}

Config::Config(const Config &other)
{
	initConfigStruct(c);
	c.numServs = other.c.numServs;
    c.iter = other.c.iter;
    c.server_name = other.c.server_name;
    c.host = other.c.host;
    c.port = other.c.port;
    c.root = other.c.root;	
    c.init_root = other.c.init_root;	
    c.activeDirectory = other.c.activeDirectory;
    c.user = other.c.user;	
    c.index = other.c.index;
    c.body_size = other.c.body_size;
    c.get_allowed = other.c.get_allowed;
    c.post_allowed = other.c.post_allowed;
    c.delete_allowed = other.c.delete_allowed;
    c.autoindex = other.c.autoindex;
	c.locations = other.c.locations;
    c.errors = other.c.errors;
}

Config& Config::operator=(const Config& other)
{
	if (this != &other)
	{
		initConfigStruct(c);
		c.numServs = other.c.numServs;
		c.iter = other.c.iter;
		c.server_name = other.c.server_name;
		c.host = other.c.host;
		c.port = other.c.port;
		c.root = other.c.root;
		c.init_root = other.c.init_root;
		c.activeDirectory = other.c.activeDirectory;
		c.user = other.c.user;
		c.index = other.c.index;
		c.body_size = other.c.body_size;
		c.get_allowed = other.c.get_allowed;
		c.post_allowed = other.c.post_allowed;
		c.delete_allowed = other.c.delete_allowed;
		c.autoindex = other.c.autoindex;
		c.locations = other.c.locations;
		c.errors = other.c.errors;
	}
	return (*this);
}

Config::~Config(){}

void Config::Read(std::string fileName)
{
	std::vector<std::string> fileContent;
	
	std::ifstream fileStream(fileName.c_str());
	if (!fileStream)
		std::runtime_error("Cannot open conf file");

	std::string line;
	while (std::getline(fileStream, line))
		fileContent.push_back(line);
}

const Config* Config::getServerConf() const
{
	Config* other = new Config(*this);
	return (other);
}

void Config::initConfigStruct(configStruct &st)
{
	st.numServs = 0;
    st.iter = 0;
    st.body_size = 0;
    st.get_allowed = false;
    st.post_allowed = false;
    st.delete_allowed = false;
    st.autoindex = false;
}

void Config::initLocationConfig(locationConfig &st)
{
	st.autoindex = false;
	st.getOn = false;	
	st.postOn = false;	
	st.deleteOn = false;
}
