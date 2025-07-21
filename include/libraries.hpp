#pragma once 

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define UPLOAD_DIR "./uploads"
#define PORT		8081
#define MAX_EVENTS	64
#define BUFFER_SIZE	1024
#define MAX_DEFAULT_REQUEST_BODY_SIZE "1000000"

#define COOKIES false
#define COOKIE_FILE0 "cookie_file0.png"
#define COOKIE_FILE1 "cookie_file1.png"	
#define COOKIE_FILE2 "cookie_file2.png"

#define CGI_WAIT_TIME 5 //secs

#define PYTHON_INTERPRETER "/usr/bin/python3"
#define SH_INTERPRETER "/usr/bin/sh"
