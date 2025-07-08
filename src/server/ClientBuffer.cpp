#include "../../include/server/ClientBuffer.hpp"

ClientBuffer::ClientBuffer(): nmb_read(0), client_fd(-1), chunked(false), \
chunkedEnd(false), contentLenght(0), headerEnd(-1), finishedReading(false)
{}

ClientBuffer::ClientBuffer(const ClientBuffer& other)
{
	*this = other;
}

ClientBuffer& ClientBuffer::operator=(const ClientBuffer& other)
{
	if (this != &other)
	{
		nmb_read = other.nmb_read;
		client_fd = other.client_fd;
		chunked = other.chunked;
		chunkedEnd = other.chunkedEnd;
		contentLenght = other.contentLenght;
		headerEnd = other.headerEnd;
		finishedReading = other.finishedReading;
	}
	return (*this);
}

ClientBuffer::~ClientBuffer()
{}

void ClientBuffer::add_buffer(std::string buffer)
{
	persistent_buffer.append(buffer);
}

std::string& ClientBuffer::get_buffer() {return (persistent_buffer);}

void ClientBuffer::setNmbRead(ssize_t n) {nmb_read = n;}

ssize_t ClientBuffer::getNmbRead() const {return (nmb_read);}

void ClientBuffer::setClientFd(int fd) {client_fd = fd;}

int ClientBuffer::getClientFd() const {return (client_fd);}

void ClientBuffer::setChunked(bool bol) {chunked = bol;}

bool ClientBuffer::getChunked() const {return (chunked);}

void ClientBuffer::setChunkedEnd(bool bol) {chunkedEnd = bol;}

bool ClientBuffer::getChunkedEnd() const {return (chunkedEnd);}

int ClientBuffer::setContentLenght(std::string newContentLenght)
{
	size_t len;
	std::stringstream ss(newContentLenght);
	ss >> len;
	if (ss.fail())
		return (1);
	contentLenght = len;
	return (0);
}

ssize_t ClientBuffer::getContentLenght() const {return (contentLenght);}

void ClientBuffer::setHeaderEnd(ssize_t pos) {headerEnd = pos;}

ssize_t ClientBuffer::getHeaderEnd() const {return (headerEnd);}

void ClientBuffer::setFinishedReading(bool bol) {finishedReading = bol;}

bool ClientBuffer::getFinishedReading() const {return (finishedReading);}

void ClientBuffer::setRequest(Request& req) {_req = req;}

Request& ClientBuffer::getRequest() {return (_req);}

void ClientBuffer::reset()
{
	persistent_buffer.clear();
	nmb_read = 0;
	client_fd = -1;
	chunked = false;
	chunkedEnd = false;
	contentLenght = 0;
	headerEnd = -1;
	finishedReading = false;
}
