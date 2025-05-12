/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dangonz3 <dangonz3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 09:06:54 by dbonilla          #+#    #+#             */
/*   Updated: 2025/05/12 16:47:53 by dangonz3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

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
