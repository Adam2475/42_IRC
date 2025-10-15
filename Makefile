NAME = ircserv
CC = c++

# -Wall -Wextra -Werror
CFLAGS = -std=c++98

SRCS = ./main.cpp ./Server.cpp ./User.cpp ./Channel.cpp ./header.cpp ./Commands.cpp
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