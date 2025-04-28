NAME = webserv
CC = c++
CCFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c++98 -I$(INCLUDES)

SRC_DIR = src
OBJ_DIR = obj
INCLUDES = inc

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)
	@echo "$(COLOR_GREEN)------------ PROCESS FINISHED ------------$(COLOR_RESET)"

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@
	@echo "$(COLOR_GREEN)------------ MESSAGE: $@ COMPILED ------------$(COLOR_RESET)"

client:
	gcc $(SRC_DIR)/*.c -o "client" && ./client
	@echo "$(COLOR_GREEN)------------ MESSAGE: CLIENT READY ------------$(COLOR_RESET)"

clean:
	@rm -fr $(OBJ_DIR)
	@echo "$(COLOR_GREEN)------------ MESSAGE: CLEANING COMPLETED ------------$(COLOR_RESET)"

fclean: clean
	@rm -fr client
	@rm -fr $(NAME)
	@echo "$(COLOR_GREEN)------------ MESSAGE: FCLEANING COMPLETED ------------$(COLOR_RESET)"

re: fclean all

.PHONY: all clean fclean re

COLOR_GREEN = \033[0;32m
COLOR_RESET = \033[0m
