CC	= gcc
CFLAGS	= -O2 -Wall -fPIC -I../capi20 -L../capi20
LDFLAGS	= -shared -L../capi20

PLUGINDIR=${DESTDIR}/etc/ppp/plugins
PEERDIR=${DESTDIR}/etc/ppp/peers/isdn
PEERS= arcor otelo talkline avm avm-ml
INSTALL=./install-sh -c

ALL = capiplugin.so userpass.so

all:	$(ALL)

capiplugin.so: capiplugin.o capiconn.o
	$(CC) -o $@ $(LDFLAGS) capiplugin.o capiconn.o -lcapi20dyn

userpass.so: userpass.c
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) -nostdlib userpass.c

clean:
	$(RM) *.so *.o comperr

install: $(ALL)
	./mkinstalldirs $(PLUGINDIR)
	@for i in $(ALL); do \
		echo $(INSTALL) $$i $(PLUGINDIR); \
		$(INSTALL) $$i $(PLUGINDIR); \
	done
	./mkinstalldirs $(PEERDIR)
	@for i in $(PEERS); do \
		echo $(INSTALL) peers/$$i $(PEERDIR); \
		$(INSTALL) peers/$$i $(PEERDIR); \
	done

config:
	@echo nothing to configure
