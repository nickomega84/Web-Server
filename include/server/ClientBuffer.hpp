#pragma once
#include "../include/server/Server.hpp"
#include "../../include/core/Request.hpp"

class ClientBuffer
{
	private:
		std::string persistent_buffer;
		ssize_t nmb_read;
		int client_fd;
		bool loop_active;
		ssize_t bodyLenght;
		ssize_t headerEnd;

	public:
		ClientBuffer();
		ClientBuffer(const ClientBuffer& other);
		ClientBuffer& operator=(const ClientBuffer& other);
		~ClientBuffer();

		void read_all(std::string buffer, ssize_t n, int client_fd);
		std::string get_buffer() const;

		void setNmbRead(ssize_t n);
 		ssize_t getNmbRead() const;

		void setClientFd(int fd);
		int getClientFd() const;

		void set_loop(bool bol);
 		bool looping() const;

		int setBodyLenght(std::string contentLenght);
		ssize_t getBodyLenght();

		void setHeaderEnd(ssize_t pos);
		ssize_t getHeaderEnd() const;
};
