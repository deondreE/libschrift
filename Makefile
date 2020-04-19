# See LICENSE file for copyright and license details.

.POSIX:

include config.mk

.PHONY: all clean install uninstall

all: libschrift.a sftdemo

libschrift.a: schrift.o
	$(AR) rc $@ $^$>
	$(RANLIB) $@

sftdemo: sftdemo.o libschrift.a
	$(LD) $(LDFLAGS) $< -o $@ -L$(X11LIB) -L. -lX11 -lXrender -lschrift

sftdemo.o: sftdemo.c schrift.h arg.h
	$(CC) -c $(CFLAGS) $< -o $@ $(CPPFLAGS) -I$(X11INC)

schrift.o: schrift.h

clean:
	rm -f *.o
	rm -f libschrift.a
	rm -f sftdemo

install: libschrift.a schrift.h schrift.3
	# libschrift.a
	mkdir -p "$(DESTDIR)$(PREFIX)/lib"
	cp -f libschrift.a "$(DESTDIR)$(PREFIX)/lib"
	chmod 644 "$(DESTDIR)$(PREFIX)/lib/libschrift.a"
	# schrift.h
	mkdir -p "$(DESTDIR)$(PREFIX)/include"
	cp -f schrift.h "$(DESTDIR)$(PREFIX)/include"
	chmod 644 "$(DESTDIR)$(PREFIX)/include/schrift.h"
	#schrift.3
	mkdir -p "$(DESTDIR)$(MANPREFIX)/man3"
	cp schrift.3 "$(DESTDIR)$(MANPREFIX)/man3"
	chmod 644 "$(DESTDIR)$(MANPREFIX)/man3/schrift.3"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/lib/libschrift.a"
	rm -f "$(DESTDIR)$(PREFIX)/include/schrift.h"
	rm -f "$(DESTDIR)$(MANPREFIX)/man3/schrift.3"

