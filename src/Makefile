
CXX      = g++
CXXFLAGS = -g -Wall -Winvalid-offsetof -DDEBUG_ALL
INCLUDES =
LDFLAGS  = -lpthread

all: multihash

OBJS = digest_common.o file_io.o main.o \
	   md4.o ed2k_file_hash.o

LIBS = md4/md4_dgst.o sha/sha1dgst.o

md4/md4_dgst.o :
	cd md4 && $(MAKE) $(MFLAGS)

sha/sha1dgst.o :
	cd sha && $(MAKE) $(MFLAGS)

multihash: $(OBJS) $(LIBS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

.PHONY: clean

clean:
	rm -f multihash $(OBJS) $(LIBS)


