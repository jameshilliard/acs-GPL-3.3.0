#
# $Id: Makefile,v 1.4 2004/09/24 13:13:22 kukuk Exp $
#
#

include ../../Make.Rules

LIBNAME=libpammodutil

# ---------------------------------------------

dummy: all

# ---------------------------------------------

CFLAGS += $(PIC) $(STATIC) $(MOREFLAGS) \
  -DLIBPAM_VERSION_MAJOR=$(MAJOR_REL) \
  -DLIBPAM_VERSION_MINOR=$(MINOR_REL)

# all the object files we care about
LIBOBJECTS = modutil_cleanup.o modutil_getpwnam.o modutil_getpwuid.o \
	modutil_getspnam.o modutil_getgrnam.o modutil_getgrgid.o \
	modutil_ingroup.o modutil_getlogin.o modutil_ioloop.o

# static library name
LIBSTATIC = $(LIBNAME).a

SLIBOBJECTS = $(addprefix static/,$(LIBOBJECTS) $(STATICOBJ))

# ---------------------------------------------
## rules

all: dirs $(LIBSTATIC) ../../Make.Rules

dirs:
	$(MKDIR) static

static/%.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@

$(LIBSTATIC): $(SLIBOBJECTS)
	ar cr $@ $(SLIBOBJECTS)
	$(RANLIB) $@

install:
	@echo "at this time, we're not installing $(LIBSTATIC)"

remove:
	@echo "at this time, there is nothing to remove"

clean:
	rm -f a.out core *~ static/*.o
	rm -f *.a *.o
	if [ -d dynamic ]; then rmdir dynamic ; fi
	if [ -d static ]; then rmdir static ; fi
