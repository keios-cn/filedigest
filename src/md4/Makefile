
#CXX      = g++
#CXXFLAGS = -g -Wall -Winvalid-offsetof
CC     = gcc
#CFLAGS = -g -Wall
CFLAGS = -O2 -Wall
INCLUDES =
LDFLAGS  =

OBJS = md4_dgst.o

all: $(OBJS)

md4_dgst.o : md4_dgst.c md32_common.h  md4.h md4_locl.h
	$(CC) $(CFLAGS) -c md4_dgst.c -o md4_dgst.o

.PHONY: clean

clean:
	rm -f $(OBJS)


