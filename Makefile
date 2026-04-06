# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/28 21:11:29 by maaugust          #+#    #+#              #
#    Updated: 2026/04/06 16:49:29 by maaugust         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================ PROJECT FILE NAMES ============================ #
NAME           = minishell

# ============================== COMPILER FLAGS ============================== #
CC             = cc
CFLAGS         = -Wall -Wextra -Werror -MMD -MP
INCLUDES       = -Iincludes -Ilibft/includes
RM             = rm -rf

# ================================== COLORS ================================== #
GREEN          := \033[32m
RED            := \033[31m
YELLOW         := \033[33m
CYAN           := \033[36m
RESET          := \033[0m
BOLD           := \033[1m

# ================================== LIBFT =================================== #
LIBFT_PATH     = ./libft
LIBFT_LIB      = $(LIBFT_PATH)/libft.a

# =============================== SOURCE FILES =============================== #
SRC_PATH       = ./srcs
SRC            = $(shell find $(SRC_PATH) -name '*.c')

# =============================== OBJECT FILES =============================== #
OBJ_PATH       = ./objs
OBJ            = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

# ============================ COMPILATION RULES ============================= #
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@printf "$(CYAN)Compiling:$(RESET) $(YELLOW)$<$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# =============================== BUILD TARGETS ============================== #
all: $(NAME)

bonus: all

$(NAME): $(LIBFT_LIB) $(OBJ)
	@printf "$(GREEN)✔ Minishell objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(OBJ) $(LIBFT_LIB) -lreadline -o $(NAME)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(NAME)$(RESET)\n"

$(LIBFT_LIB):
	@printf "$(CYAN)→ Building Libft...$(RESET)\n"
	@$(MAKE) -C $(LIBFT_PATH) >/dev/null \
	  || { printf "$(RED)✖ Libft build failed!$(RESET)\n"; exit 1; }
	@printf "$(GREEN)✔ Libft built.$(RESET)\n"
	
# =============================== CLEAN TARGETS ============================== #
clean:
	@$(RM) $(OBJ_PATH)
	@rmdir -p --ignore-fail-on-non-empty $(OBJ_PATH) 2>/dev/null || true
	@$(MAKE) -C $(LIBFT_PATH) clean >/dev/null
	@printf "$(YELLOW)• Cleaned object files.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@$(MAKE) -C $(LIBFT_PATH) fclean >/dev/null
	@printf "$(RED)• Full clean complete.$(RESET)\n"

VALGRIND_SUPP = $(CURDIR)/.supps/readline.supp

valgrind: all
	valgrind --suppressions=./.supps/readline.supp --track-fds=yes \
		--leak-check=full --show-leak-kinds=all  -s ./minishell

# ============================== REBUILD TARGETS ============================= #
re: fclean all

.PHONY: all bonus clean fclean re valgrind

# =============================== DEPENDENCIES =============================== #
-include $(OBJ:.o=.d)
