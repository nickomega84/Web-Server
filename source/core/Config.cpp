#include "../include/core/Config.hpp"
/* Config::Config() //solo para pruebas //basado en default.conf
{
    ::bzero(&c, sizeof(c));
	c.numServs = 1;
    c.iter = 1;
    c.server_name = "Default";
    c.host = "127.0.0.1";
    c.port = "8080";
    c.root = "web_root";
    c.index = "index.html";
    c.body_size = 30000;
    c.errors[404] = "/error/404.html";

	LocationConfig def;
	::bzero(&def, sizeof(LocationConfig));
	def.location_name = "/";
	def.location_root = "./"
	def.getOn = TRUE;
	def.postOn = TRUE;
	def.deleteOn = TRUE;
	c.locations.insert(def);

	LocationConfig cgi-bin;
	::bzero(&cgi-bin, sizeof(LocationConfig));
	def.location_name = "/cgi-bin";
	def.location_root = "./"
	def.index = calculator.py;
	def.getOn = TRUE;
	def.postOn = TRUE;
	def.deleteOn = TRUE;
	c.locations.insert(def);
	c.locations.insert(cgi-bin);
} */

Config::Config() //multiple.conf
{
    ::bzero(&c, sizeof(c));
	c.numServs = 1;
    c.iter = 1;
    c.server_name = "Default";
    c.host = "127.0.0.1";
    c.port = "8080";
    c.root = "web_root";
    c.index = "index.html";
    c.body_size = 30000;
    c.errors[404] = "/error/404.html";

    LocationConfig def;
	::bzero(&def, sizeof(LocationConfig));
	def.location_name = "/";
	def.location_root = "./";
	def.getOn = 0;
	def.postOn = 0;
	def.deleteOn = 0;
	// c.locations.insert(def);

	// LocationConfig cgi-bin;
	// ::bzero(&cgi-bin, sizeof(LocationConfig));
	// def.location_name = "/cgi-bin";
	// def.location_root = "./";
	// def.index = calculator.py;
	// def.getOn = 0;
	// def.postOn = 0;
	// def.deleteOn = 0;
	// c.locations.insert(def);
	// c.locations.insert(cgi-bin);


}


Config::Config(std::string fileName)
{
	::bzero(&c, sizeof(c));
	Read(fileName);
}

Config::Config(const Config &other)
{
    ::bzero(&c, sizeof(c));
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
		::bzero(&c, sizeof(c));
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

