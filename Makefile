CC=gcc

CFLAGS  = -Os -s -Wall
CFLAGS += -DTEST
CFLAGS += -Wno-error=unused-result

DESTLIB = /usr/lib/

DLIB = libnvram.so
APP = nvram

BINS = $(DLIB) 

all: $(BINS)

$(DLIB) : nvram.o nvram_lock.o
	$(CC) $(CFLAGS) -Os -s -shared -Wl,-soname,$(DLIB) -o $@ $^ 

nvram.o : nvram.c
	$(CC) $(CFLAGS) -Os -s -Wall -fPIC -g -c $^ -o $@

nvram_lock.o: nvram_lock.c
	$(CC) $(CFLAGS) -Os -s -g -Wall -fPIC -c $^ -o $@

install :
	cp -f $(DLIB) $(DESTLIB)

clean:
	rm -rf *~ *.o *.so nvram $(BINS)

uninstall:
	-rm -rf $(DESTLIB)/$(DLIB)


