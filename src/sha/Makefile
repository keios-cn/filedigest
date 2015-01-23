
#CXX      = g++
#CXXFLAGS = -g -Wall -Winvalid-offsetof
CC     = gcc
CFLAGS = -g -Wall
INCLUDES =
LDFLAGS  =

OBJS = sha1dgst.o

all: $(OBJS)

sha1dgst.o : sha1dgst.c md32_common.h  sha.h  sha_locl.h
	$(CC) $(CFLAGS) -c sha1dgst.c -o sha1dgst.o

.PHONY: clean

clean:
	rm -f $(OBJS)


