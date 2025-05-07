# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nkrasimi <nkrasimi@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/28 17:21:48 by nkrasimi          #+#    #+#              #
#    Updated: 2025/04/28 17:22:19 by nkrasimi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
SRC = cgi.cpp
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Wno-unused-function
all: $(NAME)
OBJS = $(SRC:.cpp=.o)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
