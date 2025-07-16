#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../include/libraries.hpp"
#include "../../include/config/ConfigParser.hpp"
#include "../include/core/Request.hpp"
#include "../include/server/Cookies.hpp"

class Request 
{
    private:
        std::string		_method;
        std::string		_uri;
        std::string		_version;
        std::map<std::string, std::string>	_headers;
        std::string		_body;
        std::string		_path;
        std::string		_queryString;
        bool			_keepAlive;
		std::string		_physicalPath;
        std::string		_originalUri;
        std::string		_basePath;
		ConfigParser*	_cfg;
		size_t			_serverIndex;
		Cookies			_cookie;

    public:
        Request();
        Request(const Request& other);
        Request& operator=(const Request& other);
        ~Request();
        
        const std::string& getPath() const;
        const std::string& getQueryString() const;
        bool               isKeepAlive() const;

        bool parse(const std::string& raw);
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getMethod() const;
        const std::string& getURI() const;
        const std::string& getVersion() const;
        const std::string& getHeader(const std::string& key) const;
        const std::string& getBody() const;
		void setPhysicalPath(const std::string &p);
		const std::string& getPhysicalPath() const ;
        void setPath(const std::string& path);
        const std::string& getOriginalUri() const { return _originalUri; }
        void setBasePath(const std::string& basePath) { _basePath = basePath; }
        const std::string& getBasePath() const { return _basePath; }
		void setCfg(ConfigParser &parser);
		ConfigParser* getCfg() const;
		void setServerIndex(size_t serverIndex);
		size_t getServerIndex() const;
		void setCookie(Cookies &cookie);
		const Cookies& getCookie() const;
};

#endif

