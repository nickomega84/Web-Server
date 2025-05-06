#include "ReadConfig.hpp"

struct LocationConfig 
{
	std::string path;
	std::map<std::string, std::string> directives;
};

struct ServerConfig 
{
	int listen_port;
	std::string host;
	int client_max_body_size;
	std::string root;
	std::vector<std::string> server_name; //opcional //puede haber más de uno
	std::map<int, std::string> error_pages;
	std::vector<std::string> index_files; //puede haber más de uno

	std::vector<LocationConfig> locations;
};

ReadConfig::ConfReader()
{
	conf.listen_port = 8080;
	conf.host = "127.0.0.1";
	conf.client_max_body_size = 30000;
	conf.root = "/site_root";
	conf.error_pages.insert(std::pair<int, std::string>(404, "/error/404.html"));



};


ReadConfig::ConfReader(std:string fileName)
{
	Read(fileName);
}

ReadConfig::ConfReader(ConfReader &other)
{
	conf.listen_port = other.listen_port;
	conf.host = other.host;
	conf.server_name = other.server_name;
	conf.error_page_404 = other.error_page_404;
	conf.client_max_body_size = other.client_max_body_size;
	conf.root = other.root;
	conf.index_files = other.conf.index_files;

	//...
}

ReadConfig::ConfReader& operator=(ConfReader& other)
{
	if (this != &other)
	{
		conf.listen_port = other.listen_port;
		conf.host = other.host;
		conf.server_name = other.server_name;
		conf.error_page_404 = other.error_page_404;
		conf.client_max_body_size = other.client_max_body_size;
		conf.root = other.root;
		conf.index_files = other.conf.index_files;
	
		//...
	
	}
	return (*this);
}

~ReadConfig::ConfReader(){}

void ReadConfig::Read(std::string fileName)
{
	std::ifstream fileStream(fileName.c_str());
	if (!fileStream)
		std::runtime_error("Cannot open conf file");

	std::vector<std::string> fileContent;
	std::string line;
	while (std::getline(fileStream, line))
		fileContent.push_back(line);
	//Parse(fileContent);
}

void ReadConfig::Parse(std::vector<std::string> fileContent)
{

}

ReadConfig* ReadConfig::getServerConfig()
{
	ReadConfig* other = new ReadConfig(*this);
	return (other);
}

