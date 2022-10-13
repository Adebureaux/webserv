# *** COLORS - NAME - CC - FLAGS ****************************************************** #

NAME					=	webserv

CC						= 	clang++

CFLAGS					= 	-Wall -Wextra -Werror -std=c++98 -g3 -I includes/

RM						= rm -rf

# *** PROJECT HEADER ********************************************************* #

HDIR					=	$(addprefix -I, $(HEAD))

HEAD					=	$(addprefix $(HEADER_DIR), $(HEADER))

HEADER_DIR				=	\
							./includes/

HEADER							=	\
									Autoindex.hpp	\
									Client.hpp		\
									Cluster.hpp		\
									Conf.hpp		\
									Errors_html.hpp	\
									Parser.hpp		\
									Request.hpp		\
									Response.hpp	\
									Shared.hpp		\
									Utility.hpp		\
									Multipart.hpp		\

# *** SRCS ******************************************************************* #

SRCS_DIR					=	./srcs/

SRCS_LIST				=	\
							Cluster.cpp		\
							Client.cpp		\
							Request.cpp		\
							Response.cpp	\
							Parser.cpp		\
							Utility.cpp		\
							Conf.cpp		\
							Autoindex.cpp	\
							Multipart.cpp	\

SRCS					=	$(addprefix $(SRCS_DIR), $(SRCS_LIST))

# *** OBJS ******************************************************************* #

OBJS_DIR				=	./objs/


OBJS_LIST				=	$(patsubst %.cpp, %.o, $(SRCS_LIST))

OBJS					=	$(addprefix $(OBJS_DIR), $(OBJS_LIST))

# *** DEPS ******************************************************************* #

DEPS					=	$(OBJS:.o=.d)

yellow = /bin/echo -e "\x1b[33m\x1b[1m➜ $1\x1b[0m"
green = /bin/echo -e "\x1b[32m\x1b[1m➜ $1\x1b[0m"

all: build $(OBJS) $(SRCS) $(NAME)
	@ $(call green, "Done building")

build:
	@ $(call yellow, "Building ...")

test: $(NAME)
	./$(NAME)

$(NAME): $(OBJS) $(SRCS)
	@ $(CC) $(CFLAGS) $(HDIR) $(OBJS) -o $@

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $(HDIR) -MMD -MP -c -o $@ $<

-include $(DEPS)

clean:
	@ $(call yellow, "Cleaning ...")
	@ $(RM) $(OBJS_DIR)
	@ $(call green, "Done cleaning")

fclean: clean
	@ $(RM) $(NAME)

re: fclean all

re_test: fclean test

.PHONY: all clean fclean re re_test test
