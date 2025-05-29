/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkrasimi <nkrasimi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:57:19 by nkrasimi          #+#    #+#             */
/*   Updated: 2025/05/29 13:34:51 by nkrasimi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <vector>

class ConfigParser {
    private:
    	int	port;									// Puerto donde escucha
        std::string server_name;					// Nombre del servidor (directiva server_name)
        std::string host;							// IP o hostname donde escucha, puede estar vacío para que escuche a cualquier dirección IP
        std::string root;							// Ruta raíz general del servidor
        std::string activeDirectory;				// Directorio activo en uso (puede cambiar en ejecución)
        std::string user;							// Usuario del proceso o sesión del servidor
        std::string index;							// Archivo por defecto a servir si no se especifica uno
        size_t		body_size;						// Tamaño máximo del cuerpo de la petición (en bytes)
        bool		get_allowed;					// GET permitido a nivel global
        bool		post_allowed;					// POST permitido a nivel global
        bool		delete_allowed;					// DELETE permitido a nivel global
        bool		autoindex;			
    public:
		bool get_GetAllowed() const;
		bool get_PostAllowed() const;
		bool get_DeleteAllowed() const;
        bool get_Autoindex() const;

		bool set_GetAllowed(bool &get_allowed);
		bool set_PostAllowed(bool &post_allowed);
		bool set_DeleteAllowed(bool &delete_allowed);
        bool set_Autoindex(bool &autoindex);

        void parsingMethod() {
            std::cout << "Parsing is done" << std::endl;
        }
};