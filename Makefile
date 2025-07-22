# **************************************************************************** #
#                                CONFIGURACIÓN                                #
# **************************************************************************** #

NAME        := webserv
CXX         := c++

# Flags comunes
CXXFLAGS_BASE   := -Wall -Wextra -Werror -std=c++98 -MMD -MP -Iinclude
DEBUG_FLAGS     := -g
RELEASE_FLAGS   := -O3 -DNDEBUG

# Selección de flags según target
# Por defecto, "all" apunta a "release"
CXXFLAGS_debug    := $(CXXFLAGS_BASE) $(DEBUG_FLAGS)
CXXFLAGS_release  := $(CXXFLAGS_BASE) $(RELEASE_FLAGS)

# Directorios
SRC_DIR     := src
OBJ_DIR     := obj
INC_DIR     := include

# Colores
DEF_COLOR   = \033[0;39m
GRAY        = \033[0;90m
RED         = \033[0;31m
GREEN       = \033[0;92m
YELLOW      = \033[0;93m
BLUE        = \033[0;94m
MAGENTA     = \033[0;95m
CYAN        = \033[0;96m
WHITE       = \033[0;97m
CLEAR       = \033[0m

# Fuentes y objetos
SRCS        := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS        := $(OBJS:.o=.d)

# Targets principales
.PHONY: all debug release clean fclean re deleteme
all: release

# Modo debug: no define NDEBUG, incluye mensajes de #ifndef NDEBUG
debug: CXXFLAGS := $(CXXFLAGS_debug)
debug: $(NAME)
	@echo "$(CYAN)[🐞 Debug build]$(CLEAR) $(NAME)"

# Modo release: define NDEBUG, desactiva mensajes de debug
release: CXXFLAGS := $(CXXFLAGS_release)
release: $(NAME)
	@echo "$(GREEN)[✅ Release build]$(CLEAR) $(NAME)"

# Reglas de compilación
$(NAME): $(OBJS)
	@echo "$(CYAN)[🔧 Linking]$(CLEAR) $(NAME)"
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(BLUE)[Compiling]$(CLEAR) $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpieza
clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)[🧹 Cleaned object files]$(CLEAR)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)[🧹 Cleaned executable]$(CLEAR)"

re: fclean all

deleteme:
	@echo "Creating temporal file: www/DELETEME.txt"
	@touch "www/DELETEME.txt"

-include $(DEPS)
