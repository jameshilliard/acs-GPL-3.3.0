#
# $Id: Makefile,v 1.5 2004/09/24 13:13:21 kukuk Exp $
#
# This Makefile controls a build process of $(TITLE) module and
# application for Linux-PAM. You should not modify this Makefile
# (unless you know what you are doing!).
#
#

include ../../Make.Rules

TITLE=pam_tally

#
## Additional rules for making (and moving) the application added.
## Assuming that all modules' applications are called $TITLE
#

LIBSRC = $(TITLE).c
LIBOBJ = $(TITLE).o
LIBOBJD = $(addprefix dynamic/,$(LIBOBJ))
LIBOBJS = $(addprefix static/,$(LIBOBJ))

APPSRC = $(TITLE)_app.c
APPOBJ = $(TITLE)_app.o
APPOBJD = $(addprefix dynamic/,$(APPOBJ))
APPOBJS = $(addprefix static/,$(APPOBJ))
    
dynamic/%.o : %.c
	$(CC) $(CFLAGS) $(DYNAMIC) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@

static/%.o : %.c
	$(CC) $(CFLAGS) $(STATIC) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@


ifdef DYNAMIC
LIBSHARED = $(TITLE).so
endif

ifdef STATIC
LIBSTATIC = lib$(TITLE).o
endif

APPLICATION = $(TITLE)
APPMODE = 755

LINK_PAMMODUTILS = -L../pammodutil -lpammodutil -L../../libpam -lpam
INCLUDE_PAMMODUTILS = -I../pammodutil/include

LDFLAGS += $(LINK_PAMMODUTILS)
CFLAGS += $(INCLUDE_PAMMODUTILS)

####################### don't edit below #######################

all: dirs $(LIBSHARED) $(LIBSTATIC) register $(APPLICATION)

dirs:
ifdef DYNAMIC
	$(MKDIR) ./dynamic
endif
ifdef STATIC
	$(MKDIR) ./static
endif

register:
ifdef STATIC
	( cd .. ; ./register_static $(TITLE) $(TITLE)/$(LIBSTATIC) )
endif

ifdef DYNAMIC
$(LIBOBJD): $(LIBSRC)

$(LIBSHARED):   $(LIBOBJD)
	$(LD_D) -o $@ $(LIBOBJD) $(LDFLAGS)

$(APPLICATION): $(APPOBJD) $(TITLE).c
	$(CC) $(CFLAGS) -o $@ $(APPOBJD) $(LDFLAGS) $(LOADLIBES)
    
endif

ifdef STATIC
$(LIBOBJS): $(LIBSRC)

$(LIBSTATIC): $(LIBOBJS)
	$(LD) -r -o $@ $(LIBOBJS)

$(APPLICATION): $(APPOBJS) $(TITLE).c
	$(CC) $(CFLAGS) -o $@ $(APPOBJS) $(LOADLIBES)
endif

install: all
	$(MKDIR) $(FAKEROOT)$(SECUREDIR)
ifdef DYNAMIC
	$(INSTALL) -m $(SHLIBMODE) $(LIBSHARED) $(FAKEROOT)$(SECUREDIR)
endif
	$(MKDIR) $(FAKEROOT)$(SUPLEMENTED)
	$(INSTALL) -m $(APPMODE) $(APPLICATION) $(FAKEROOT)$(SUPLEMENTED)

remove:
	rm -f $(FAKEROOT)$(SECUREDIR)/$(TITLE).so
	rm -f $(FAKEROOT)$(SUPLEMENTED)/$(TITLE)

clean:
	rm -f $(LIBOBJD) $(LIBOBJS) $(APPOBJD) $(APPOBJS) core *~
	rm -f *.a *.o *.so *.bak dynamic/* static/* $(APPLICATION)
	rm -rf dynamic static

.c.o:   
	$(CC) $(CFLAGS) -c $<
