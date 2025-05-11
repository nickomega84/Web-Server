/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbonilla <dbonilla@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 09:06:54 by dbonilla          #+#    #+#             */
/*   Updated: 2025/05/11 15:37:43 by dbonilla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http/HttpRequest.hpp"

HttpRequest::HttpRequest()
{
    std::cout << "Constructor by default" << std::endl;
}

HttpRequest::HttpRequest(const HttpRequest &other)
{
    *this = other;
    std::cout << "Construtor by default" << std::endl;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &rhs)
{
    
    if (this != &rhs)
    {

    }
    return(*this);
}

HttpRequest::~HttpRequest()
{
    std::cout << "Destructor by default" << std::endl;
}
