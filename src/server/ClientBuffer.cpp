#include "../../include/server/ClientBuffer.hpp"

ClientBuffer::ClientBuffer(): nmb_read(0), client_fd(-1), loop_active(false), bodyLenght(0)
{}

ClientBuffer::ClientBuffer(const ClientBuffer& other)
{
	nmb_read = other.nmb_read;
	loop_active = other.loop_active;
}

ClientBuffer& ClientBuffer::operator=(const ClientBuffer& other)
{
	if (this != &other)
	{
		nmb_read = other.nmb_read;
		loop_active = other.loop_active;
	}
	return (*this);
}

ClientBuffer::~ClientBuffer()
{}

void ClientBuffer::read_all(int client_fd, std::string buffer, ssize_t n)
{
	while (n > 0)
	{
		char new_buffer[BUFFER_SIZE];
    	n = recv(client_fd, new_buffer, BUFFER_SIZE - 1, 0);
		new_buffer[n] = '\0';
		buffer = buffer + new_buffer;
		std::cout << "[DEBUG] read_all n = "<< n << std::endl;
	}
	persistent_buffer = persistent_buffer + buffer;

	std::cout << "[DEBUG MARTES] persistent_buffer.length = "<< persistent_buffer.length() << std::endl;
}

std::string ClientBuffer::get_buffer() const {return (persistent_buffer);}

void ClientBuffer::setNmbRead(ssize_t n) {nmb_read = n;}

ssize_t ClientBuffer::getNmbRead() const {return (nmb_read);}

void ClientBuffer::setClientFd(int fd) {client_fd = fd;}

int ClientBuffer::getClientFd() const {return (client_fd);}

void ClientBuffer::set_loop(bool bol)
{
	if (bol)
		loop_active = true;
	else
		loop_active = false;
}

bool ClientBuffer::get_loop() const {return (loop_active);}

int ClientBuffer::setBodyLenght(std::string contentLenght)
{
	int len;

	std::stringstream ss(contentLenght);
	ss >> len;
	if (ss.fail())
		return (1);
	bodyLenght = len;
	return (0);
}

ssize_t ClientBuffer::getBodyLenght() {return (bodyLenght);}

void ClientBuffer::setHeaderEnd(ssize_t pos) {headerEnd = pos;}

ssize_t ClientBuffer::getHeaderEnd() const {return (headerEnd);}
