#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream> // para std::ostringstream
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <csignal>
#include <ctime>
#include <dirent.h>

#define UPLOAD_DIR "./uploads"
#define PORT		8080
#define MAX_CONN	1000
#define MAX_EVENTS	64
#define BUFFER_SIZE	1024
