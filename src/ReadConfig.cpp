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
	std::vector<std::string> server_name;
	std::string error_page_404;
	int client_max_body_size;
	std::vector<std::string> index_files;
	std::string root;

	std::vector<LocationConfig> locations;
};

ReadConfig::ConfReader()
{
	config.listen_port = 8080;
	config.host = "127.0.0.1";
	config.server_name.push_back("example.com");
	config.server_name.push_back("www.example.com");
	config.error_page_404 = "/home/nkrasimi/Desktop/webServer/error/404.html";
	config.client_max_body_size = 1024;
	config.root = "/home/nkrasimi/Desktop/webServer/";
	config.index_files.push_back("index.html");
	config.index_files.push_back("index.htm");

	LocationConfig loc1;
	loc1.path = 
};


ReadConfig::ConfReader(std:string fileName)
{
	Read(fileName);
}

ReadConfig::ConfReader(ConfReader &other)
{
	config.listen_port = other.listen_port;
	config.host = other.host;
	config.server_name = other.server_name;
	config.error_page_404 = other.error_page_404;
	config.client_max_body_size = other.client_max_body_size;
	config.root = other.root;
	config.index_files = other.config.index_files;

	//...
}

ReadConfig::ConfReader& operator=(ConfReader& other)
{
	if (this != &other)
	{
		config.listen_port = other.listen_port;
		config.host = other.host;
		config.server_name = other.server_name;
		config.error_page_404 = other.error_page_404;
		config.client_max_body_size = other.client_max_body_size;
		config.root = other.root;
		config.index_files = other.config.index_files;
	
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

