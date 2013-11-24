WARNINGS= -pedantic -Wall
LIBS    = -pthread
CPPFLAGS= -O2 -DDEBUG #-fsave-memoized -fmemoize-lookups
CC=gcc
CPP=c++
OBJS = src/main.o \
	src/server.o \
	src/user.o \
	src/xmlparser.o \
	src/message.o \
	src/room.o \
	src/httpconnection.o \
	src/authorizehttpconnection.o \
	src/stringqueue.o \
	src/roommember.o \
	src/player.o \
	src/Game.o

BIN = xmlsocketd
main : $(OBJS)
	$(CPP) $(CPPFLAGS) $(WARNINGS) $(OBJS) -o $(BIN) $(LIBS)

clean :
	rm -f *.o xmlsocketd src/*.o
