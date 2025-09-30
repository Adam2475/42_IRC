NAME = ircserv

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = $(wildcard src/*.cpp)

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning object files..."
	@rm -f $(OBJS)

fclean: clean
	@echo "Cleaning executable..."
	@rm -f $(NAME)

re: fclean all

.PHONY: all re clean fclean