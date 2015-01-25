
CXX      = g++
CXXFLAGS = -g -Wall -Winvalid-offsetof -DDEBUG_ALL
INCLUDES =
LDFLAGS  = -lpthread

all: multihash

OBJS = digest_common.o file_io.o main.o \
	   md4.o md5.o ed2k_file_hash.o \
	   sha1.o sha224.o sha256.o sha384.o sha512.o

LIBS = md4/md4_dgst.o md5/md5_dgst.o sha/sha1dgst.o sha/sha256.o sha/sha512.o

md4/md4_dgst.o :
	cd md4 && $(MAKE) $(MFLAGS)

md5/md5_dgst.o :
	cd md5 && $(MAKE) $(MFLAGS)

sha/sha1dgst.o sha/sha256.o sha/sha512.o :
	cd sha && $(MAKE) $(MFLAGS)

multihash: $(OBJS) $(LIBS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

.PHONY: clean

clean:
	rm -f multihash $(OBJS) $(LIBS)


