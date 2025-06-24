#include "../../include/server/ClientBuffer.hpp"

ClientBuffer::ClientBuffer(): nmb_read(0), client_fd(-1), loop_active(false), lenght(0)
{}

ClientBuffer::ClientBuffer(const ClientBuffer& other)
{
	n = other.n;
	loop_active = other.flag;
}

ClientBuffer& ClientBuffer::operator=(const ClientBuffer& other)
{
	if (this != other)
	{
		n = other.n;
		loop_active = other.flag;
	}
	return (*other);
}

~ClientBuffer::ClientBuffer()
{}

void ClientBuffer::read_all(int client_fd, std::string buffer, ssize_t n)
{
	std::string new_buffer;
	if (n > 0)
	{
    	n = recv(client_fd, new_buffer.c_str(), sizeof(new_buffer.c_str()), 0);
		buffer += new_buffer;
		read_all(buffer, n);
	}
	persistent_buffer += buffer;
}

std::string ClientBuffer::get_buffer() const {return (persistent_buffer);}

void ClientBuffer::setNmbRead(ssize_t n) {nmb_read = n;}

ssize_t ClientBuffer::getNmbRead() const {return (n);}

void ClientBuffer::setClientFd(int fd) {client_fd = fd;}

int ClientBuffer::getClientFd() const {return (client_fd);}

void ClientBuffer::looping(bool bol)
{
	if (bol)
		loop_active = true;
	else
		loop_active = false;
}

bool ClientBuffer::looping() const {return (bool);}

int ClientBuffer::setBodyLenght(std::string contentLenght)
{
	int len;

	std::stringstream ss(contentLenght);
	ss >> lenght;
	if (ss.fail())
		return (1);
	bodyLenght = len;
	return (0);
}

ssize_t ClientBuffer::getBodyLenght() {return (bodyLenght);}

void ClientBuffer::setHeaderEnd(ssize_t pos) {headerEnd = pos;}

ssize_t ClientBuffer::getHeaderEnd() const {return (headerEnd);}
