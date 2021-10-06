NAME = a.out # rename
SRCS = serv.cpp
HEADER = serv.hpp
 
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