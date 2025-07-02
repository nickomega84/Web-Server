/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dangonz3 <dangonz3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:57:19 by nkrasimi          #+#    #+#             */
/*   Updated: 2025/06/23 15:03:08 by dangonz3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <cstdlib>

class ConfigParser {
    private:
        std::string filename;
    	std::map<std::string, std::string> globalConfig;
        std::map<std::string, std::map<std::string, std::string> > locations;
        std::vector<std::map<std::string, std::string> > _serversConfig;     
        std::vector<std::map<std::string, std::map<std::string, std::string> > > _serversLocations;
        size_t _currentServer;
    
	public:
        ConfigParser();
        ~ConfigParser();
        ConfigParser(ConfigParser const &src);
        ConfigParser &operator=(ConfigParser const &src);

        bool    load(std::string const &file);
        size_t  serverCount() const;
        void    setGlobal(std::string const &key, std::string const &value);
        void    setLocation(std::string const &location, std::string const &key, std::string const &value);
        bool    setServer(size_t index);

        std::string getGlobal(std::string const &key) const;
        std::string getLocation(std::string const &location, std::string const &key) const;
        int         getGlobalInt(std::string const &key) const;
        static ConfigParser& getInst();
        std::vector<int> getPorts(size_t serverIndex) const;

        void print() const;
};

#endif
