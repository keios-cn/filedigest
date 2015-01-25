
#CXX      = g++
#CXXFLAGS = -g -Wall -Winvalid-offsetof
CC     = gcc
#CFLAGS = -g -Wall
CFLAGS = -O2 -Wall
INCLUDES = -I../md4/
LDFLAGS  =

OBJS = sha1dgst.o sha256.o sha512.o

all: $(OBJS)

sha1dgst.o : sha1dgst.c sha.h  sha_locl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c sha1dgst.c -o sha1dgst.o

sha256.o : sha256.c sha.h  sha_locl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c sha256.c -o sha256.o

sha512.o : sha512.c sha.h  sha_locl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c sha512.c -o sha512.o

.PHONY: clean

clean:
	rm -f $(OBJS)


