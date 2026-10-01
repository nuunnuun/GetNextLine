NAME = gnl_demo
CC = cc
CFLAGS = -Wall -Wextra -Werror
BUFFER_SIZE ?= 1024
CPPFLAGS = -DBUFFER_SIZE=$(BUFFER_SIZE)
SRC = main.c get_next_line.c get_next_line_utils.c

.PHONY: all clean fclean re test norm
all: $(NAME)

$(NAME): $(SRC) get_next_line.h Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SRC) -o $(NAME)

clean:
	rm -f *.o *.pch

fclean: clean
	rm -f $(NAME)

re: fclean
	$(MAKE) all

test:
	python3 tests/run_tests.py

norm:
	norminette get_next_line.c get_next_line_utils.c get_next_line.h main.c
