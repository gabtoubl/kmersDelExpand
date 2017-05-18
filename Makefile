NAME=		kmersDel
NAME2=		kmersExpand
SRCS=		del.cpp        \
		count.cpp      \
		options.cpp
SRCS2=		expand.cpp     \
		options.cpp
OBJS=		$(SRCS:.cpp=.o)
OBJS2=		$(SRCS2:.cpp=.o)
CC=		g++
CXXFLAGS=	-W -Wall -Wextra -ansi -pedantic -std=c++11 -O3 -I.
LDFLAGS=	-lpthread
RM=		rm -vf

all:		$(NAME) $(NAME2)

$(NAME):	$(OBJS)
		$(CC) -o $(NAME) $(OBJS) $(LDFLAGS)

$(NAME2):	$(OBJS2)
		$(CC) -o $(NAME2) $(OBJS2) $(LDFLAGS)

clean:
		@$(RM) $(OBJS) $(OBJS2) $(NAME) $(NAME2)

re:		clean all
