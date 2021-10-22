NAME		= bot
OBJS_DIR	= ./objs/
CXX			= clang++
CXX_FLAGS	= -g -Wall -Wextra -Werror

SRCS		= main.cpp	 \
			  Client.cpp \
			  ../server/srcs/User.cpp \
			  ../server/srcs/Message.cpp \
			  ../server/srcs/utils.cpp

OBJS		= $(notdir $(SRCS:.cpp=.o))
OBJS_PATH	= $(addprefix $(OBJS_DIR), $(OBJS))

$(OBJS_DIR)%.o : %.cpp Client.hpp ../server/includes/User.hpp
	@mkdir -p $(OBJS_DIR)
	@echo "\033[1;31m- Done :\033[0m $<"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJS_DIR)%.o : ../server/srcs/%.cpp Client.hpp ../server/includes/User.hpp
	@mkdir -p $(OBJS_DIR)
	@echo "\033[1;31m- Done :\033[0m $<"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

$(NAME): $(OBJS_PATH)
	@$(CXX) -o $(NAME) $(OBJS_PATH)
	@echo "🔥🔥🔥 \033[1;31;43mIRC-BOT IS COMPLETED\033[0m 🔥🔥🔥"

all: $(NAME)

clean:
	rm -Rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY : all clean fclean re
