CC = c++

CFLAGS = -std=c++98 -Wall -Werror -Wextra -fsanitize=address
CFLAGS += -D __ERROR__


UNAME_S = $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	CFLAGS += -D __LINUX__ 
	NAME = ./bin/linux/ircserv
endif

ifeq ($(UNAME_S), Darwin)
	CFLAGS += -D __DARWIN__
	NAME = ./bin/macOs/ircserv	
endif

ifeq (1, 1)
	CFLAGS += -O3
endif

ifeq (0, 1)
	IFLAGS += -fsanitize=thread
endif

ifeq (1, 1)
	IFLAGS += -fsanitize=address
endif

OBJ_DIR = obj/

OBJ = main.o \
	  socket/Socket.o \
	  events/Events.o \
	  client/Client.o \
	  server/Server.o \
	  server/join_replies.o \
	  server/Loops.o \
	  channel/Channel.o \
	  server/Commands.o \
	  server/Commands2.o \


PREFIXE = $(addprefix  $(OBJ_DIR),$(OBJ))

all : mkdir_obj $(NAME)

$(NAME) : $(PREFIXE)
	$(CC) -o $@ $^ $(IFLAGS)

$(OBJ_DIR)%.o : ./sources/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

mkdir_obj :
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)channel/
	@mkdir -p $(OBJ_DIR)client/
	@mkdir -p $(OBJ_DIR)server/
	@mkdir -p $(OBJ_DIR)socket/
	@mkdir -p $(OBJ_DIR)events/
	@mkdir -p bin/
	@mkdir -p bin/linux/
	@mkdir -p bin/macOs/

.PHONY : all mkdir_obj clean fclean re rr r

clean :
	@rm -f $(OBJ_DIR)*.o
	@rm -f $(OBJ_DIR)channel/*.o
	@rm -f $(OBJ_DIR)client/*.o
	@rm -f $(OBJ_DIR)server/*.o
	@rm -f $(OBJ_DIR)socket/*.o
	@rm -f $(OBJ_DIR)commands/*.o
	@rm -f $(OBJ_DIR)events/*.o
	@rm -rf bin/*

fclean : clean
	@rm -f $(NAME)

re : fclean all

rr : re r

r : all
	$(NAME) 6667

# install:
# 	apt-get update
# 	apt-get install -y build-essential autoconf \
# 		automake libtoo libkqueue-dev cmake &> /dev/null
# 	git clone https://github.com/mheily/libkqueue.git /libkqueue
# 	cd /libkqueue
# 	cmake3 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib <path to source>
# 	make
# 	cpack3 -G RPM
# 	mkdir -p /usr/include/kqueue/sys/
# 	cp -f /libkqueue/include/sys/event.h /usr/include/kqueue/sys/event.h
# 	rm -rf /libkqueue
