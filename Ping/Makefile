NAME		= ping

CFLAGS		= -Wall -Wextra -Werror -O3 -I./includes

SRCS		=   srcs/global.c \
				srcs/main.c \
				srcs/parser.c \
				srcs/network.c \
				srcs/signals.c \
				srcs/libft.c \
				srcs/print_utils.c

OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

.c.o:
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

sanitized: CFLAGS += -fsanitize=address
sanitized: clean all

test:
	sudo ./$(NAME) google.com

bonus_quiet:
	sudo ./$(NAME) -q google.com

bonus_verbose:
	sudo ./$(NAME) -v -c 5 google.com

bonus_ttl:
	sudo ./$(NAME) -v -t 1 google.com

bonus_size:
	sudo ./$(NAME) -v -s 10 google.com

bonus_interval:
	sudo ./$(NAME) -v -i 5 google.com
