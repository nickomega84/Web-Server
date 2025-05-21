/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IServer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbonilla <dbonilla@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 20:37:08 by dbonilla          #+#    #+#             */
/*   Updated: 2025/05/11 20:37:09 by dbonilla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISERVER_HPP
#define ISERVER_HPP

class IServer {
public:
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual ~IServer() {}
};

#endif