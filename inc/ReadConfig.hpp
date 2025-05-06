#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

struct LocationConfig 
{
	std::string path;
	std::map<std::string, std::string> directives;
};

struct ServerConfig 
{
	int listen_port;
	std::string host;
	std::string server_name;
	std::string error_page_404;
	int client_max_body_size;
	std::vector<std::string> index_files;
	std::string root;

	std::vector<LocationConfig> locations;
};

class ReadConfig
{
	private:
	ServerConfig config;

	public:
	ConfReader();
	ConfReader(std::string fileName);
	ConfReader(ConfReader &other);
	ConfReader& operator=(ConfReader& other);
	~ConfReader();

	void Read(std::ifstream fileStream);
	//void Parse(std::vector<std::string> fileContent)

	ReadConfig* getServerConfig();
}
