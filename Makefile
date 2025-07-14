# **************************************************************************** #
#                                CONFIGURACIÓN                                #
# **************************************************************************** #

NAME        := webserv
CXX         := c++
CXXFLAGS    := -g -Wall -Wextra -Werror -std=c++98 -MMD -MP -Iinclude

SRC_DIR     := src
OBJ_DIR     := obj
INC_DIR     := include

# **************************************************************************** #
#                                COLORES                                       #
# **************************************************************************** #

DEF_COLOR 	= \033[0;39m
GRAY 		= \033[0;90m
RED 		= \033[0;31m
GREEN 		= \033[0;92m
YELLOW 		= \033[0;93m
BLUE 		= \033[0;94m
MAGENTA 	= \033[0;95m
CYAN 		= \033[0;96m
WHITE 		= \033[0;97m
CLEAR 		= \033[0m

# **************************************************************************** #
#                                ARCHIVOS                                      #
# **************************************************************************** #

SRCS        := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS        := $(OBJS:.o=.d)

# **************************************************************************** #
#                                  REGLAS                                       #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(CYAN)[🔧 Linking]$(CLEAR) $(NAME)"
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)[✅ Build complete]$(CLEAR)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(BLUE)[Compiling]$(CLEAR) $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)[🧹 Cleaned uploads]$(CLEAR)"
	@rm -rf $(UPLOADS)
	@echo "$(YELLOW)[🧹 Cleaned object files]$(CLEAR)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)[🧹 Cleaned executable]$(CLEAR)"

re: fclean all

deleteme:
	@echo "Creating temporal file: www/DELETEME.txt"
	@touch "www/DELETEME.txt"

-include $(DEPS)

.PHONY: all clean fclean re
