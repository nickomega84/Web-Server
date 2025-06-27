/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandlerFactory.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbonilla <dbonilla@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 05:36:17 by dbonilla          #+#    #+#             */
/*   Updated: 2025/06/26 06:00:49 by dbonilla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/factory/CGIHandlerFactory.hpp"
#include "../../include/cgi/CGIHandler.hpp"

CGIHandlerFactory::CGIHandlerFactory(const std::string& cgiBinPath, IResponseBuilder* builder)
    : _cgiBinPath(cgiBinPath), _builder(builder) 
{
    std::cout << "[DEBUG] CGIHandlerFactory initialized with cgiRoot: " << _cgiBinPath << std::endl;
}
CGIHandlerFactory::CGIHandlerFactory(const std::string& cgiBinPath)
    : _cgiBinPath(cgiBinPath) 
{
    std::cout << "[DEBUG] CGIHandlerFactory initialized with cgiRoot: " << _cgiBinPath << std::endl;
}
CGIHandlerFactory::~CGIHandlerFactory() {}

IRequestHandler* CGIHandlerFactory::createHandler() const
{
    return new CGIHandler(_builder, _cgiBinPath);
}


