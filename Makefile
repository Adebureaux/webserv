NAME		= webserv

SRCS		=	srcs/Cluster.cpp\
				srcs/Server.cpp\
				srcs/Client.cpp\
				srcs/Request.cpp\
				srcs/Response.cpp\
				srcs/Parser.cpp\
				srcs/Autoindex.cpp

INCS		= -I includes
OBJS		:= $(SRCS:.cpp=.o)
DEPS 		:= $(OBJS:.o=.d)
RM			= rm -f
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g3\
# -fsanitize=address
COMPILER	= c++

$(NAME): $(OBJS)
	$(COMPILER) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(COMPILER) $(CXXFLAGS) -MMD -MP $(INCS) -c $< -o $@

all: $(NAME)

clean:
	$(RM) $(OBJS) $(DEPS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

test: $(NAME)
		./$(NAME)

-include $(DEPS)

.PHONY: all clean fclean re -include test
