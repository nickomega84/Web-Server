#pragma once
#include "../../include/core/Request.hpp"

class ClientBuffer
{
	private:
		std::string persistent_buffer;
		ssize_t nmb_read;
		int client_fd;
		bool chunked;
		bool chunkedEnd;
		ssize_t bodyLenght;
		ssize_t headerEnd;
		bool finishedReading;

	public:
		ClientBuffer();
		ClientBuffer(const ClientBuffer& other);
		ClientBuffer& operator=(const ClientBuffer& other);
		~ClientBuffer();

		void add_buffer(std::string buffer);
		/* void read_all(int client_fd, std::string buffer, ssize_t n); */
		std::string& get_buffer();

		void setNmbRead(ssize_t n);
 		ssize_t getNmbRead() const;

		void setClientFd(int fd);
		int getClientFd() const;

		int setBodyLenght(std::string contentLenght);
		ssize_t getBodyLenght() const;

		void setChunked(bool bol);
		bool getChunked() const;

		void setChunkedEnd(bool bol);
		bool getChunkedEnd() const;

		void setHeaderEnd(ssize_t pos);
		ssize_t getHeaderEnd() const;

		void setFinishedReading(bool bol);
		bool getFinishedReading() const;

		void reset();
};
