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
	if (uri.find(".py") != std::string::npos) 
		return (1);
	else if (uri.find(".sh") != std::string::npos) 
		return (2);
	return (0);
}

int CGIHandler::identifyMethod(Request &req)
{
	std::string method = req.getMethod();
	if (method.find("GET") != std::string::npos)
		return (2);
	if (method.find("POST") != std::string::npos)
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
	if (indx == 1 || indx == 2)
		return (handleError(501), true);
	else if (indx == 3)
		handleGET(req, res, PYTHON_INTERPRETER);
	else if (indx == 4)
		handleGET(req, res, SH_INTERPRETER);
	else if (indx == 5)
		handlePOST(req, res, PYTHON_INTERPRETER);
	else if (indx == 6)
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
	else if(pos_name >= pos_query)
		return (*success = false, "");
	return (uri.substr(pos_name, pos_query)); //si omotimos el segundo argumento de substr se usa std::string::npos por defecto (el final dels string)
}

std::string CGIHandler::getQueryString(const std::string &uri)
{
	std::string::size_type pos_query = uri.find("?");
	if (pos_query == std::string::npos)
		return ("");
	return (uri.substr(pos_query)); //si omotimos el segundo argumento de substr se usa std::string::npos por defecto (el final dels string)
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

char ** CGIHandler::enviromentGET(std::string path, std::string queryString)
{
	std::string request_method = "REQUEST_METHOD=GET";
	std::string path_info = "PATH_INFO=" + path;
	std::string query_string = "QUERY_STRING=" + queryString;
	std::string script_name = "SCRIPT_NAME=" + path;
	std::string server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
	std::string content_lenght = "CONTENT_LENGTH=0"; //GET no tiene cuerpo, LENGHT se refiere al tamaño del cuerpo del request

	static char* envp[7]; //STATIC: Duración de vida estática: no se destruye al salir de la función; persiste durante toda la ejecución del programa. Memoria compartida: la misma variable es reutilizada cada vez que se llama la función. No se crea una copia nueva en cada llamada.
	envp[0] = const_cast<char *>(request_method.c_str());
	envp[1] = const_cast<char *>(path_info.c_str());
	envp[2] = const_cast<char *>(query_string.c_str());
	envp[3] = const_cast<char *>(script_name.c_str());
	envp[4] = const_cast<char *>(server_protocol.c_str());
	envp[5] = const_cast<char *>(content_lenght.c_str());
	envp[6] = NULL;
	return(envp);
}

int CGIHandler::checkHandler(Request &req, std::map<std::string, std::string> &m)
{
	bool success = true;
	m["dir"] = "./www" + getDir(req.getURI(), &success);
	m["name"] = getName(req.getURI(), &success);
	m["name_without_slash"] = m["name"].substr(1);
	m["path"] = m["dir"] + m["name"];
	m["queryString"] = getQueryString(req.getURI());
	if (!success)
		return (handleError(404), 1);
	if (!checkLocation(m["dir"], m["name_without_slash"]))
		return (handleError(404), 1);
	if (!checkExePermission(m["path"]))
		return (handleError(502), 1);
		//CONFIG! comprueba si el directorio tiene permisos de acuerdo al archivo de configuración (404 si no tiene);	return (0);
	return (0);
}

int CGIHandler::handleGET(Request &req, Response &res, std::string interpreter) //ejemplo del header del request: GET /cgi-bin/hello.cgi?name=Juan HTTP/1.1
{
	std::map<std::string, std::string> m;
	if (checkHandler(req, m))
		return (1);

	char *argv[] = {(char *)m["interpreter"].c_str(), (char *)m["name_without_slash"].c_str(), NULL};
	char **envp = enviromentGET(m["path"], m["queryString"]);

	int pipefd[2];
	if (pipe(pipefd) < 0)
		return (handleError(500), 1);

	pid_t pid = fork(); //fork() devuelve el PID del proceso hijo al proceso padre, y 0 al proceso hijo. No devuelve el PID del proceso actual.
	if (pid < 0)
		return (handleError(500), 1);
	if (!pid)
	{
		close (pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) < 0)
			return (kill(getpid(), SIGTERM), 1);
		close (pipefd[1]);

		if (chdir(m["dir"].c_str()) < 0)
			return (kill(getpid(), SIGTERM), 1);
		execve((char *)interpreter.c_str(), argv, envp);
		return (kill(getpid(), SIGTERM), 1);
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
			return (handleError(500), 1);
		if (WIFSIGNALED(status))
			return (handleError(500), 1);

		res.setStatus(200, "OK");
		res.setHeader("Content-Type", "text/html");
		res.setBody(output);
	}
	return (0);
}

char ** CGIHandler::enviromentPOST(std::string path, std::string queryString, Request &req)
{
	std::stringstream body_lenght;
	body_lenght << req.getBody().length();

	std::string request_method = "REQUEST_METHOD=POST";
	std::string path_info = "PATH_INFO=" + path;
	std::string query_string = "QUERY_STRING=" + queryString;
	std::string script_name = "SCRIPT_NAME=" + path;
	std::string server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
	std::string content_type = "CONTENT_TYPE=" + req.getHeader("Content-Type");
	std::string content_lenght = "CONTENT_LENGTH=" + body_lenght.str(); //GET no tiene cuerpo, LENGHT se refiere al tamaño del cuerpo del request

	static char* envp[8]; //STATIC: Duración de vida estática: no se destruye al salir de la función; persiste durante toda la ejecución del programa. Memoria compartida: la misma variable es reutilizada cada vez que se llama la función. No se crea una copia nueva en cada llamada.
	envp[0] = const_cast<char *>(request_method.c_str());
	envp[1] = const_cast<char *>(path_info.c_str());
	envp[2] = const_cast<char *>(query_string.c_str());
	envp[3] = const_cast<char *>(script_name.c_str());
	envp[4] = const_cast<char *>(server_protocol.c_str());
	envp[5] = const_cast<char *>(content_type.c_str());
	envp[6] = const_cast<char *>(content_lenght.c_str());
	envp[7] = NULL;
	return(envp);
}

int CGIHandler::handlePOST(Request &req, Response &res, std::string interpreter)
{
	std::map<std::string, std::string> m;
	if (checkHandler(req, m))
		return (1);

	char *argv[] = {(char *)interpreter.c_str(), (char *)m["path"].c_str(), NULL};
	char **envp = enviromentPOST(m["path"], m["queryString"], req);
	
	int pipeInput[2];
	if (pipe(pipeInput) < 0)
		return (handleError(500), 1);
	int pipeOutput[2];
	if (pipe(pipeOutput) < 0)
		return (handleError(500), 1);

	pid_t pid = fork();
	if (pid < 0)
		return (handleError(500), 1);
	if (!pid)
	{
		close (pipeInput[1]);
		if (dup2(pipeInput[0], STDIN_FILENO) < 0)
			return (kill(getpid(), SIGTERM), 1);
		close (pipeInput[0]);
		close (pipeOutput[0]);
		if (dup2(pipeOutput[1], STDOUT_FILENO) < 0)
			return (kill(getpid(), SIGTERM), 1);
		close (pipeOutput[1]);

		if (chdir(m["dir"].c_str()) < 0)
			return (kill(getpid(), SIGTERM), 1);
		execve((char *)interpreter.c_str(), argv, envp);
		return (kill(getpid(), SIGTERM), 1);
	}
	else
	{
		char buffer[BUFFER_SIZE];
		std::string output;
		ssize_t count;
		std::string body = req.getBody();

		close(pipeInput[0]);
		if (0 == write(pipeInput[1], body.c_str(), body.size()))
			return (handleError(500), 1);
		close(pipeInput[1]);
		close(pipeOutput[1]);
		while ((count = read(pipeOutput[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, count);
		close(pipeOutput[0]);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			return (handleError(500), 1);
		if (WIFSIGNALED(status))
			return (handleError(500), 1);

		res.setStatus(200, "OK");
		res.setHeader("Content-Type", "text/html");
		res.setBody(output);
	}
	return (0);
}
