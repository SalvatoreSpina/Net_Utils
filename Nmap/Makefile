NAME		= nmap

CFLAGS		= -Wall -Wextra -Werror -O3 -I./includes

SRCS		=	srcs/main.c

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
