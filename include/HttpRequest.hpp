/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dangonz3 <dangonz3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 09:07:04 by dbonilla          #+#    #+#             */
/*   Updated: 2025/05/12 16:48:18 by dangonz3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef     HTTPREQUEST_HPP

#define     HTTPREQUEST_HPP

#include    <string>
#include    <map>
#include    <iostream>

class HttpRequest
{
private:
    std::string     method;
    std::string     path;
    std::string     version;
    std::map        <std::string,   std::string>    headers;
    std::string     body;
public:
    HttpRequest();
    HttpRequest(const   HttpRequest &other);
    HttpRequest &operator=(const    HttpRequest &rhs);
    ~HttpRequest();

    
};

#endif
