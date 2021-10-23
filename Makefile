NAME		=	ircserv
CXX			=	clang++
CXX_FLAGS	=	-g -std=c++98 -MMD -Wall -Wextra -Werror
OBJS_DIR	=	./objs/
SERVER_DIR	=	./server/srcs/
CLIENT_DIR	=	./client/
BOT_DIR		=	./bot/

SRCS		=	IRCServer.cpp	\
				cmds.cpp		\
				Channel.cpp		\
				User.cpp		\
				Message.cpp		\
				utils.cpp		\
				main.cpp

OBJS		= $(notdir $(SRCS:.cpp=.o))
OBJS_PATH	= $(addprefix $(OBJS_DIR), $(OBJS))

$(OBJS_DIR)%.o : $(SERVER_DIR)%.cpp ./server/includes/IRCServer.hpp
	@mkdir -p $(OBJS_DIR)
	@echo "\033[1;31m- Done :\033[0m $<"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

$(NAME): $(OBJS_PATH)
	@$(CXX) $(CXX_FLAGS) -o $(NAME) $(OBJS_PATH)
	@echo "🔥🔥🔥 \033[1;33;41mIRC-SERVER IS COMPLETED\033[0m 🔥🔥🔥"

all: $(NAME)

bonus:
	@$(MAKE) -C $(CLIENT_DIR)
	@$(MAKE) -C $(BOT_DIR)

clean:
	rm -Rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)
	@$(MAKE) -C $(CLIENT_DIR) fclean
	@$(MAKE) -C $(BOT_DIR) fclean

re: fclean all

.PHONY : all bonus clean fclean re
