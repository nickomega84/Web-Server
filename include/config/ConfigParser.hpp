/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkrasimi <nkrasimi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:57:19 by nkrasimi          #+#    #+#             */
/*   Updated: 2025/05/28 18:02:05 by nkrasimi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

class ConfigParser {
    private:
        int			numServs;						// Cantidad total de servidores (uso interno)
        std::string server_name;					// Nombre del servidor (directiva server_name)
        std::string host;							// IP o hostname donde escucha, puede estar vacío para que escuche a cualquier dirección IP
        std::string	port;							// Puerto donde escucha
        std::string root;							// Ruta raíz general del servidor
        std::string init_root;						// Copia de la raíz original (antes de ser modificada)
        std::string activeDirectory;				// Directorio activo en uso (puede cambiar en ejecución)
        std::string user;							// Usuario del proceso o sesión del servidor
        std::string index;							// Archivo por defecto a servir si no se especifica uno
        size_t		body_size;						// Tamaño máximo del cuerpo de la petición (en bytes)
        bool		get_allowed;					// GET permitido a nivel global
        bool		post_allowed;					// POST permitido a nivel global
        bool		delete_allowed;					// DELETE permitido a nivel global
        bool		autoindex;			
    public:
        void parsingMethod() {
            std::cout << "Parsing is done" << std::endl;
        }
};