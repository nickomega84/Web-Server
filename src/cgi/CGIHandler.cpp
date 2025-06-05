#include "../../include/cgi/CGIHandler.hpp"

CGIHandler::CGIHandler(int *error): _error(error)
{
	*_error = 200;
}

CGIHandler::~CGIHandler()
{}

int CGIHandler::identifyType(Request &req)
{
	std::string uri = req.getURI();
	std::string::size_type query_pos = uri.find('?');
	std::string path_part = uri;
	if (query_pos != std::string::npos)
		path_part = uri.substr(0, query_pos);	
	if (path_part.length() > 3 && path_part.substr(path_part.length() - 3) == ".py")
		return (1);
	if (path_part.length() > 3 && path_part.substr(path_part.length() - 3) == ".sh")
		return (2);
	return (0);
}

int CGIHandler::identifyMethod(Request &req)
{
	std::string method = req.getMethod();
	if (method == "GET")
		return (2);
	if (method == "POST")
		return (4);
	return (0);
}

void CGIHandler::handleError(int error)
{
	*_error = error;
}

bool CGIHandler::identifyCGI(Request &req, Response &res)
{
	int indx = identifyType(req);
	if (indx == 0)
		return (false);
	indx += identifyMethod(req);
	if (indx == INVALID1 || indx == INVALID2)
		return (std::cerr << "[ERROR] CGI invalid method" << std::endl, handleError(501), true);
	else if (indx == GET_PY)
		handleGET(req, res, PYTHON_INTERPRETER);
	else if (indx == GET_SH)
		handleGET(req, res, SH_INTERPRETER);
	else if (indx == POST_PY)
		handlePOST(req, res, PYTHON_INTERPRETER);
	else if (indx == POST_SH)
		handlePOST(req, res, SH_INTERPRETER);
	return (true);
}

std::string CGIHandler::getDir(const std::string &uri, bool *success)
{
	std::string::size_type pos = uri.rfind("/");
	if (pos == std::string::npos)
		return (*success = false, "");
	return (uri.substr(0, pos));
}

std::string CGIHandler::getName(const std::string &uri, bool *success)
{
	std::string::size_type pos_name = uri.rfind("/");
	if (pos_name == std::string::npos)
		return (*success = false, "");
	std::string::size_type pos_query = uri.find("?");
	if (pos_query == std::string::npos)
		return (uri.substr(pos_name));
	else if (pos_name >= pos_query)
		return (*success = false, "");
	return (uri.substr(pos_name, pos_query - pos_name)); //si omotimos el segundo argumento de substr se usa std::string::npos por defecto (el final dels string)
}

std::string CGIHandler::getQueryString(const std::string &uri)
{
	std::string::size_type pos_query = uri.find("?");
	if (pos_query == std::string::npos)
		return ("");
	return (uri.substr(pos_query + 1)); //si omotimos el segundo argumento de substr se usa std::string::npos por defecto (el final dels string)
}

bool CGIHandler::checkLocation(std::string &directory, std::string &name) //comprueba si el archivo existe dentro del directorio
{
	DIR *dir;
	struct dirent *entry;
	std::vector<std::string> file_names;

	if ((dir = opendir(directory.c_str())) == NULL)	//DIR es una estructura opaca definida en <dirent.h>, utilizada por las funciones del sistema para representar un flujo de directorio abierto. Su contenido interno está oculto al usuario y gestionado por el sistema operativo.
		return (false);
	while ((entry = readdir(dir)) != NULL)
		file_names.push_back(entry->d_name);
	closedir(dir);
	if (find(file_names.begin(), file_names.end(), name) == file_names.end())
		return (false);
	return (true);
}

bool CGIHandler::checkExePermission(std::string path)
{
	if (!access(path.c_str(), X_OK))
		return (true);
	return (false);
}

int CGIHandler::checkHandler(Request &req, std::map<std::string, std::string> &m)
{
	bool success = true;
	m["dir"] = "./www" + getDir(req.getURI(), &success);
	m["name"] = getName(req.getURI(), &success);
	m["name_without_slash"] = m["name"].substr(1);
	m["path"] = m["dir"] + m["name"];
	m["queryString"] = getQueryString(req.getURI());

	std::cout << std::endl;
	for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); ++it)
		std::cout << it->first << " = " << it->second << std::endl;
	std::cout << std::endl;

	if (!success)
		return (std::cerr << "[ERROR] CGI couldn't getDir() or getName()" << std::endl, handleError(404), 1);
	if (!checkLocation(m["dir"], m["name_without_slash"]))
		return (std::cerr << "[ERROR] CGI couldn't checkLocation()" << std::endl, handleError(404), 1);
	if (!checkExePermission(m["path"]))
		return (std::cerr << "[ERROR] CGI couldn't checkExePermission()" << std::endl, handleError(500), 1);

	//CONFIG! comprueba si el directorio tiene permisos de acuerdo al archivo de configuración (404 si no tiene);	return (0);
	
	return (0);
}

std::vector<std::string> CGIHandler::enviromentGET(std::string path, std::string queryString)
{
	std::vector<std::string> envp;
	envp.push_back("REQUEST_METHOD=GET");
	envp.push_back("PATH_INFO=" + path);
	envp.push_back("QUERY_STRING=" + queryString);
	envp.push_back("SCRIPT_NAME=" + path);
	envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envp.push_back("CONTENT_LENGTH=0"); //GET no tiene cuerpo, LENGHT se refiere al tamaño del cuerpo del reques)t
	return(envp);
}

int CGIHandler::handleGET(Request &req, Response &res, std::string interpreter) //ejemplo del header del request: GET /cgi-bin/hello.cgi?name=Juan HTTP/1.1
{
	std::map<std::string, std::string> m;
	if (checkHandler(req, m))
		return (1);

	char *argv[] = {(char *)m["interpreter"].c_str(), (char *)m["name_without_slash"].c_str(), NULL};
	std::vector<std::string> en = enviromentGET(m["path"], m["queryString"]);
	char** envp = new char*[en.size() + 1];
	for (size_t i = 0; i < en.size(); ++i)
		envp[i] = const_cast<char*>(en[i].c_str());
	envp[en.size()] = NULL;

	int pipefd[2];
	if (pipe(pipefd) < 0)
		return (std::cerr << "[ERROR] CGI pipe() on handleGET" << std::endl, handleError(500), delete[] envp, 1);

	pid_t pid = fork(); //fork() devuelve el PID del proceso hijo al proceso padre, y 0 al proceso hijo. No devuelve el PID del proceso actual.
	if (pid < 0)
		return (std::cerr << "[ERROR] CGI pid on handleGET" << std::endl, handleError(500), delete[] envp, 1);
	if (!pid)
	{
		close (pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		close (pipefd[1]);

		if (chdir(m["dir"].c_str()) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		execve((char *)interpreter.c_str(), argv, envp);
		return (kill(getpid(), SIGTERM), delete[] envp, 1);
	}
	else
	{
		char buffer[BUFFER_SIZE];
		std::string output;
		ssize_t count;

		close(pipefd[1]);
		while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, count);
		close(pipefd[0]);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			return (std::cerr << "[ERROR] CGI on waitpid()" << std::endl, handleError(500), delete[] envp, 1);
		if (WIFSIGNALED(status))
			return (std::cerr << "[ERROR] CGI child killed SIGTERM" << std::endl, handleError(500), delete[] envp, 1);

		if (createResponse(output, res))
			return (std::cerr << "[ERROR] CGI couldn't create reponse" << std::endl, handleError(500), delete[] envp, 1);
	}
	return (delete[] envp, 0);
}

std::vector<std::string> CGIHandler::enviromentPOST(std::string path, std::string queryString, Request &req)
{
	std::vector<std::string> envp;
	std::stringstream body_lenght;
	body_lenght << req.getBody().length();

	envp.push_back("REQUEST_METHOD=POST");
	envp.push_back("PATH_INFO=" + path);
	envp.push_back("QUERY_STRING=" + queryString);
	envp.push_back("SCRIPT_NAME=" + path);
	envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envp.push_back("CONTENT_TYPE=" + req.getHeader("Content-Type"));
	envp.push_back("CONTENT_LENGTH=" + body_lenght.str());
	return(envp);
}

int CGIHandler::handlePOST(Request &req, Response &res, std::string interpreter)
{
	std::map<std::string, std::string> m;
	if (checkHandler(req, m))
		return (1);

	char *argv[] = {(char *)m["interpreter"].c_str(), (char *)m["name_without_slash"].c_str(), NULL};
	std::vector<std::string> en = enviromentPOST(m["path"], m["queryString"], req);
	char** envp = new char*[en.size() + 1];
	for (size_t i = 0; i < en.size(); ++i)
		envp[i] = const_cast<char*>(en[i].c_str());
	envp[en.size()] = NULL;
	
	int pipeInput[2];
	if (pipe(pipeInput) < 0)
		return (std::cerr << "[ERROR] CGI on pipeInput" << std::endl, handleError(500), delete[] envp, 1);
	int pipeOutput[2];
	if (pipe(pipeOutput) < 0)
		return (std::cerr << "[ERROR] CGI on pipeOutput" << std::endl, handleError(500), delete[] envp, 1);

	pid_t pid = fork();
	if (pid < 0)
		return (std::cerr << "[ERROR] CGI pid() on handlePOST" << std::endl, handleError(500), delete[] envp, 1);
	if (!pid)
	{
		close (pipeInput[1]);
		if (dup2(pipeInput[0], STDIN_FILENO) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		close (pipeInput[0]);
		close (pipeOutput[0]);
		if (dup2(pipeOutput[1], STDOUT_FILENO) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		close (pipeOutput[1]);

		if (chdir(m["dir"].c_str()) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		execve((char *)interpreter.c_str(), argv, envp);
		return (kill(getpid(), SIGTERM), delete[] envp, 1);
	}
	else
	{
		char buffer[BUFFER_SIZE];
		std::string output;
		ssize_t count;
		std::string body = req.getBody();

		close(pipeInput[0]);
		ssize_t bytes_written = write(pipeInput[1], body.c_str(), body.size());
		if (bytes_written == -1 || static_cast<size_t>(bytes_written) != body.size())
			return (std::cerr << "[ERROR] CGI write on handlePOST" << std::endl, handleError(500), delete[] envp, close(pipeInput[1]), close(pipeOutput[1]), close(pipeOutput[0]), 1);
		close(pipeInput[1]);
		close(pipeOutput[1]);
		while ((count = read(pipeOutput[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, count);
		close(pipeOutput[0]);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			return (std::cerr << "[ERROR] CGI waitpid()" << std::endl, handleError(500), delete[] envp, 1);
		if (WIFSIGNALED(status))
			return (std::cerr << "[ERROR] CGI child was killed SIGTERM" << std::endl, handleError(500), delete[] envp, 1);

		if (createResponse(output, res))
			return (std::cerr << "[ERROR] CGI couldn't createResponse()" << std::endl, handleError(500), delete[] envp, 1);
	}
	return (delete[] envp, 0);
}


int CGIHandler::createResponse(std::string output, Response &res)
{
	res.setStatus(200, "OK");
 
	std::string cgi_headers_str;
	std::string cgi_body_str;
	std::string::size_type header_end_pos = output.find("\r\n\r\n");
	if (header_end_pos == std::string::npos)
		header_end_pos = output.find("\n\n"); //algunos scripts no devuelven \r\n\r\n como dicta la convencción de HTTP, en su lugar devuelven \n\n 
	if (header_end_pos == std::string::npos)
		return (1);
	cgi_headers_str = output.substr(0, header_end_pos);
	cgi_body_str = output.substr(header_end_pos + 4); // +4 para saltar \r\n\r\n

	res.setBody(cgi_body_str);

	std::stringstream ss(cgi_headers_str);
	std::string line;
	while (std::getline(ss, line) && !line.empty()) 
	{
		std::string::size_type colon_pos = line.find(':');
		if (colon_pos != std::string::npos) 
		{
			std::string header_name = line.substr(0, colon_pos);
			std::string header_value = line.substr(colon_pos + 1);
			size_t first_char_pos = header_value.find_first_not_of(" \t"); // Quitar espacios en blanco al inicio del valor
			if (first_char_pos != std::string::npos)
				header_value = header_value.substr(first_char_pos);
			else
				header_value = ""; // Valor vacío si solo hay espacios
			res.setHeader(header_name, header_value);
		}
	}
	return (0);
}
