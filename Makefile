CC=gcc

CFLAGS  = -Os -s -Wall
CFLAGS += -DTEST
CFLAGS += -Wno-error=unused-result

DESTLIB = /usr/lib/

BINS = libscnvram.so 

all: $(BINS)

libscnvram.so : nvram.o nvram_lock.o
	$(CC) $(CFLAGS) -Os -s -shared -Wl,-soname,libscnvram.so -o libscnvram.so $^ 

nvram.o : nvram.c
	$(CC) $(CFLAGS) -Os -s -Wall -fPIC -g -c $^ -o $@

nvram_lock.o: nvram_lock.c
	$(CC) $(CFLAGS) -Os -s -g -Wall -fPIC -c $^ -o $@

install :
	cp -f libscnvram.so $(DESTLIB)

clean:
	rm -rf *~ *.o *.so nvram $(BINS)

uninstall:
	-rm -rf $(DESTLIB)/libscnvram.so


