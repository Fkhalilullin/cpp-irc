NAME		= client
OBJS_DIR	= ./objs/
CXX			= clang++
CXX_FLAGS	= -MMD #-Wall -Wextra -Werror -fsanitize=address

SRCS		= main.cpp	 \
			  Client.cpp

OBJS		= $(notdir $(SRCS:.cpp=.o))
OBJS_PATH	= $(addprefix $(OBJS_DIR), $(OBJS))

$(OBJS_DIR)%.o : %.cpp Client.hpp
	@mkdir -p $(OBJS_DIR)
	@echo "\033[1;31m- Done :\033[0m $<"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

$(NAME): $(OBJS_PATH)
	@$(CXX) -o $(NAME) $(OBJS_PATH)
	@echo "🔥🔥🔥 \033[1;31;43mCLIENT IS COMPLETED\033[0m 🔥🔥🔥"

all: $(NAME)

clean:
	rm -Rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY : all clean fclean re
