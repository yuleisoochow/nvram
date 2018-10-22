CC=gcc

CFLAGS  = -Os -s -Wall -Werror
CFLAGS += -DTEST
CFLAGS += -Wno-error=unused-result
CFLAGS += -Wno-error=unused-function

DESTLIB = /usr/lib/

DLIB = libnvram.so
APP = nvram

BINS = $(DLIB) 

all: $(BINS)

$(DLIB) : nvram.o nvram_lock.o
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(DLIB) -o $@ $^ 

nvram.o : nvram.c
	$(CC) $(CFLAGS) -fPIC -g -c $^ -o $@

nvram_lock.o: nvram_lock.c
	$(CC) $(CFLAGS) -fPIC -g -c $^ -o $@

install :
	cp -f $(DLIB) $(DESTLIB)

clean:
	rm -rf *~ *.o *.so nvram $(BINS)

uninstall:
	-rm -rf $(DESTLIB)/$(DLIB)


