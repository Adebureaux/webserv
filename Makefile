NAME		= webserv

SRCS		=	srcs/main.cpp srcs/SimpleSocket.cpp\
				srcs/Request.cpp srcs/Response.cpp srcs/cgi.cpp srcs/parsing.cpp

INCS		= -I includes

OBJS		= ${SRCS:.cpp=.o}

RM			= rm -f

CFLAGS		= -Wall -Wextra -Werror -std=c++98 -g3

COMPILER	= c++

${NAME}: ${OBJS}
	${COMPILER} ${CFLAGS} ${OBJS} -o ${NAME}

%.o: %.cpp
	$(COMPILER) $(CFLAGS) $(INCS) -c $< -o $@

all: ${NAME}

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re
