/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbonilla <dbonilla@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:46:12 by dbonilla          #+#    #+#             */
/*   Updated: 2025/06/20 17:51:29 by dbonilla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATICFILEHANDLER_HPP
# define STATICFILEHANDLER_HPP

# include "handler/IRequestHandler.hpp"


class StaticFileHandler : public IRequestHandler
{
    private:
	std::string _rootPath;
	Response	doGET(Response& res, std::string uri);
	Response	doPOST(const Request& req, Response& res);
	int			createPOSTfile(const Request& req, std::string& relative_path);
	std::string	createPOSTbody(std::string full_path);
	Response	doDELETE(Response res, std::string uri);
	std::string	get_date();
    
    public:
	StaticFileHandler(const std::string &root);
	virtual ~StaticFileHandler();
    
	Response handleRequest(const Request &request);
};

#endif

// #ifndef     STATICFILEHANDLER_HPP
// # define    STATICFILEHANDLER_HPP

// # include "handler/IRequestHandler.hpp"
// #include "../../include/response/IResponseBuilder.hpp"



// class StaticFileHandler : public IRequestHandler
// {
//   private:
//     std::string _rootPath;
//     IResponseBuilder* _builder;

//     Response	doGET(Response& res, std::string uri);
//     Response	doPOST(const Request& req, Response& res);
//     int			createPOSTfile(const Request& req, std::string& relative_path);
//     std::string	createPOSTbody(std::string full_path);
//     Response	doDELETE(Response res, std::string uri);
//     std::string	get_date();
    

//   public:
//     explicit StaticFileHandler(const std::string &root);
    

//     virtual ~StaticFileHandler();
//     // UploadHandlerFactory(const std::string& root, IResponseBuilder* rb)
//     //     : _root(root), _builder(rb) {}

//     IRequestHandler* createHandler() const;
//     Response handleRequest(const Request &request);
// };

// #endif
