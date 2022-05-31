NAME		= program

SRCS		= srcs/main.cpp srcs/Webserv.cpp

INC			= -I includes

OBJS		= ${SRCS:.cpp=.o}

DEPS		= ${SRCS:.cpp=.d}

RM			= rm -f

CFLAGS		= -Wall -Wextra -Werror -std=c++98

COMPILER	= c++

${NAME}: ${OBJS}
	${COMPILER} ${CFLAGS} ${OBJS} -o ${NAME}

%.o: %.cpp
	$(COMPILER) $(CFLAGS) $(INC) -c $< -o $@

all: ${NAME}

clean:
	${RM} ${OBJS}
	${RM} ${DEPS}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re
