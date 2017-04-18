NAME=		kmersCount
SRCS=		main.cpp       \
		count.cpp      \
		options.cpp    \
		maps.cpp
INCLUDES=	kmersCount.hpp
OBJS=		$(SRCS:.cpp=.o)
CC=		g++
CXXFLAGS=	-W -Wall -Wextra -ansi -pedantic -std=c++11 -O3
LDFLAGS=	-lpthread
RM=		rm -vf

$(NAME):	$(OBJS) $(INCLUDES)
		$(CC) -o $(NAME) $(OBJS) $(LDFLAGS)

all:		$(NAME)

clean:
		@$(RM) $(OBJS)
		@$(RM) $(NAME)


re:		clean all
