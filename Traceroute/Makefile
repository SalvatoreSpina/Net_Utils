NAME		= ft_traceroute

CFLAGS		= -Wall -Wextra -Werror -O3 -I./includes

SRCS		=   srcs/main.c \
				srcs/parser.c \
				srcs/network.c \
				srcs/traceroute.c \
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

bonus_debug:
	sudo ./$(NAME) google.com -d

bonus_first_ttl:
	sudo ./$(NAME) google.com -f 5

bonus_max_ttl:
	sudo ./$(NAME) google.com -m 1

bonus_nqueries:
	sudo ./$(NAME) google.com -q 1

bonus_icmpecho:
	sudo ./$(NAME) google.com -I

PHONY: all clean fclean re test bonus_debug bonus_first_ttl bonus_icmpecho bonus_max_ttl bonus_nqueries