
#CXX      = g++
#CXXFLAGS = -g -Wall -Winvalid-offsetof
CC     = gcc
#CFLAGS = -g -Wall
CFLAGS = -O2 -Wall
INCLUDES = -I../md4/
LDFLAGS  =

OBJS = md5_dgst.o

all: $(OBJS)

md5_dgst.o : md5_dgst.c md5.h md5_locl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c md5_dgst.c -o md5_dgst.o

.PHONY: clean

clean:
	rm -f $(OBJS)


