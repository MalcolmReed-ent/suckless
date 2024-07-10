# dmenu - dynamic menu
# See LICENSE file for copyright and license details.

include config.mk

SRC = drw.c dmenu.c stest.c util.c
OBJ = $(SRC:.c=.o)

all: options dmenu stest

options:
	@echo dmenu build options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

.c.o:
	$(CC) -c $(CFLAGS) $<

config.h:
	@echo "Using existing config.h file"

$(OBJ): arg.h config.h config.mk drw.h

dmenu: dmenu.o drw.o util.o
	$(CC) -o $@ dmenu.o drw.o util.o $(LDFLAGS)

stest: stest.o
	$(CC) -o $@ stest.o $(LDFLAGS)

clean:
	rm -f dmenu stest $(OBJ) dmenu-$(VERSION).tar.gz

dist: clean
	mkdir -p dmenu-$(VERSION)
	cp LICENSE Makefile README arg.h config.def.h config.mk dmenu.1\
		drw.h util.h dmenu_path dmenu_run scripts/dmenu_search scripts/dmenu_dfm scripts/dmenu_youtube scripts/dmenu_youtube_channels stest.1 $(SRC)\
		dmenu-$(VERSION)
	tar -cf dmenu-$(VERSION).tar dmenu-$(VERSION)
	gzip dmenu-$(VERSION).tar
	rm -rf dmenu-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dmenu dmenu_path dmenu_run scripts/dmenu_dfm scripts/dmenu_search scripts/dmenu_youtube scripts/dmenu_youtube_channels stest $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dmenu
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dmenu_path
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dmenu_run
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dmenu_dfm
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dmenu_search
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dmenu_youtube
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dmenu_youtube_channels
	chmod 755 $(DESTDIR)$(PREFIX)/bin/stest
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < dmenu.1 > $(DESTDIR)$(MANPREFIX)/man1/dmenu.1
	sed "s/VERSION/$(VERSION)/g" < stest.1 > $(DESTDIR)$(MANPREFIX)/man1/stest.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/dmenu.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/stest.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/dmenu
	rm -f $(DESTDIR)$(PREFIX)/bin/dmenu_path
	rm -f $(DESTDIR)$(PREFIX)/bin/dmenu_run
	rm -f ${DESTDIR}${PREFIX}/bin/dmenu_dfm
	rm -f $(DESTDIR)$(PREFIX)/bin/dmenu_search
	rm -f $(DESTDIR)$(PREFIX)/bin/dmenu_youtube
	rm -f $(DESTDIR)$(PREFIX)/bin/dmenu_youtube_channels
	rm -f $(DESTDIR)$(PREFIX)/bin/stest
	rm -f $(DESTDIR)$(MANPREFIX)/man1/dmenu.1
	rm -f $(DESTDIR)$(MANPREFIX)/man1/stest.1

.PHONY: all options clean dist install uninstall
