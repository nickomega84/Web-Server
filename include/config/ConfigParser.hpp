/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbonilla <dbonilla@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:57:19 by nkrasimi          #+#    #+#             */
/*   Updated: 2025/06/09 20:45:18 by dbonilla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>

class ConfigParser {
    private:
        std::string filename;
    	std::map<std::string, std::string> globalConfig;
        std::map<std::string, std::map<std::string, std::string> > locations;
    public:
        ConfigParser();
        ~ConfigParser();
        ConfigParser(ConfigParser const &src);
        ConfigParser &operator=(ConfigParser const &src);

        bool    load(std::string const &file);
        void    setGlobal(std::string const &key, std::string const &value);
        void    setLocation(std::string const &location, std::string const &key, std::string const &value);
        
        std::string getGlobal(std::string const &key) const;
        std::string getLocation(std::string const &location, std::string const &key) const;
        int         getGlobalInt(std::string const &key) const;
        static ConfigParser& getInst();


        void print() const;
};

#endif