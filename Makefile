# **************************************************************************** #
#                                  VARIABLES                                  #
# **************************************************************************** #

NAME        := webserv
CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98 -MMD -MP -Iinclude

SRC_DIR     := sources
OBJ_DIR     := obj
INC_DIR     := include

# Archivos y carpetas críticas para validar
REQUIRED_DIRS := $(SRC_DIR) $(INC_DIR)/core $(INC_DIR)/http $(INC_DIR)/utils
REQUIRED_FILES := $(SRC_DIR)/main.cpp $(INC_DIR)/core/Server.hpp

# Encuentra todos los .cpp en src/ y subdirectorios
SRCS        := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS        := $(OBJS:.o=.d)

# **************************************************************************** #
#                               VALIDATION                                    #
# **************************************************************************** #

# check_structure:
# 	@for dir in $(REQUIRED_DIRS); do \
# 		if [ ! -d $$dir ]; then \
# 			echo "❌ Missing required directory: $$dir"; \
# 			exit 1; \
# 		fi \
# 	done
# 	@for file in $(REQUIRED_FILES); do \
# 		if [ ! -f $$file ]; then \
# 			echo "❌ Missing required file: $$file"; \
# 			exit 1; \
# 		fi \
# 	done
# @echo "✅ Estructura verificada."

# **************************************************************************** #
#                                   RULES                                     #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJS)
	@echo "🔧 Linking $(NAME)..."
	@$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "🧹 Cleaned object files."

fclean: clean
	@rm -f $(NAME)
	@echo "🧹 Cleaned executable."

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re check_structure
