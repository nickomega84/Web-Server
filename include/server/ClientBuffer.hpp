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
		ssize_t contentLenght;
		ssize_t headerEnd;
		bool finishedReading;
		Request _req;

	public:
		ClientBuffer();
		ClientBuffer(const ClientBuffer& other);
		ClientBuffer& operator=(const ClientBuffer& other);
		~ClientBuffer();

		void add_buffer(std::string buffer);
		std::string& get_buffer();

		void setNmbRead(ssize_t n);
 		ssize_t getNmbRead() const;

		void setClientFd(int fd);
		int getClientFd() const;

		int setContentLenght(std::string contentLenght);
		ssize_t getContentLenght() const;

		void setChunked(bool bol);
		bool getChunked() const;

		void setChunkedEnd(bool bol);
		bool getChunkedEnd() const;

		void setHeaderEnd(ssize_t pos);
		ssize_t getHeaderEnd() const;

		void setFinishedReading(bool bol);
		bool getFinishedReading() const;

		void setRequest(Request& req);
		Request& getRequest();

		void reset();
};
