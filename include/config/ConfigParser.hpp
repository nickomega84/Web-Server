/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkrasimi <nkrasimi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:57:19 by nkrasimi          #+#    #+#             */
/*   Updated: 2025/05/31 02:22:45 by nkrasimi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

        static ConfigParser& getInst();
        bool load(std::string const &file);
          
        std::string getGlobal(std::string const &key) const;
        int getGlobalInt(std::string const &key) const;

        std::string getLocation(std::string const &location, std::string const &key) const;

        void    setGlobal(std::string const &key, std::string const &value);
        void    setLocation(std::string const &location, std::string const &key, std::string const &value);

        void print() const;
};