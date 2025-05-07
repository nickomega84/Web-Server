#include "Conf.hpp"

Conf::Conf() //solo para pruebas
{
    ::bzero(&c, sizeof(c));
	c.numServs = 1;
    c.iter = 1;
    c.server_name = "Default";
    c.host = "127.0.0.1";
    c.port = "8080";
    c.root = ;
    c.init_root = ;
    c.activeDirectory = ;
    c.user = ;
    c.index = ;
    c.body_size = ;
    c.get_allowed = ;
    c.post_allowed = ;
    c.delete_allowed = ;
    c.autoindex = ;
	c.locations = ;
    c.errors = ;
}

Conf::Conf(std::string fileName)
{
	::bzero(&c, sizeof(c));
	Read(fileName);
}

Conf::Conf(const Conf &other)
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

Conf& Conf::operator=(const Conf& other)
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

Conf::~Conf(){}

void Conf::Read(std::string fileName)
{
	std::vector<std::string> fileContent;
	
	std::ifstream fileStream(fileName.c_str());
	if (!fileStream)
		std::runtime_error("Cannot open conf file");

	std::string line;
	while (std::getline(fileStream, line))
		fileContent.push_back(line);

}

const Conf* Conf::getServerConf() const
{
	Conf* other = new Conf(*this);
	return (other);
}

