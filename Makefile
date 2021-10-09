NAME = a.out # rename
SRCS =  User.cpp main.cpp Channel.cpp IRCServer.cpp Message.cpp

HEADER = Channel.hpp IRCServer.hpp Message.hpp User.hpp

OBJS = $(SRCS:.cpp=.o)

CLANG = clang++ -g -std=c++98 #-Wall -Werror -Wextra -fsanitize=address

all: $(NAME)

$(NAME): $(OBJS) $(HEADER)
	$(CLANG) $(OBJS) -o $(NAME)

$(OBJS): %.o : %.cpp
	$(CLANG) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
