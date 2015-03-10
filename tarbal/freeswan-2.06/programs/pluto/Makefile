# Pluto Makefile
# Copyright (C) 1997 Angelos D. Keromytis.
# Copyright (C) 1998-2001  D. Hugh Redelmeier
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# RCSID $Id: Makefile,v 1.141 2004/01/17 03:52:29 dhr Exp $


# relative path to top directory of FreeS/WAN source
# Note: referenced in ${FREESWANSRCDIR}/Makefile.inc
FREESWANSRCDIR=../..

include ${FREESWANSRCDIR}/Makefile.inc

FMANDIR=$(MANTREE)/man5
PMANDIR=$(MANTREE)/man8

# -O on Linux makes gcc coredump when compiling sha1.c
# -Wundef is nice but RHL5.2 compiler doesn't support it
CFLAGS = -g -Wall -W -Wmissing-prototypes -Wpointer-arith -Wbad-function-cast \
	-Wcast-qual -Wmissing-declarations -Wwrite-strings \
	-Wstrict-prototypes # -Wundef

# where to find klips headers and FreeS/WAN headers
# and 2.6 kernel's <rtnetlink.h> and <xfrm.h>
HDRDIRS = -I$(KLIPSINC) -I${FREESWANSRCDIR}/programs/pluto/linux26

# On non-LINUX systems, these one of these may be needed (see endian.h)
# BYTE_ORDER = -DBIG_ENDIAN=4321 -DLITTLE_ENDIAN=1234 -DBYTE_ORDER=BIG_ENDIAN
# BYTE_ORDER = -DBIG_ENDIAN=4321 -DLITTLE_ENDIAN=1234 -DBYTE_ORDER=LITTLE_ENDIAN

# -DKLIPS enables interface to Kernel LINUX IPsec code
# -DDEBUG enables debugging code, allowing for debugging output
#    (note that output must also be selected at runtime, so it is
#    reasonable to always define this)
# -DPLUTO_SENDS_VENDORID enables pluto to send out a VendorID payload.
#    this can be used by remote nodes to work around faults (bugs),
#    but is most useful to humans who are debugging things.
# -DGCC_LINT uses gcc-specific declarations to improve compile-time
#    diagnostics.
# -DLEAK_DETECTIVE enables crude code to find memory allocation leaks.
# -DOLD_RESOLVER.  At some point, the resolver interface changed.
#    This macro enables Pluto support for the old interface.
#    It is automatically defined, based on the value of the <resolver.h>
#    macro __RES.  We don't know the correct threshold, so you may
#    find that you must manually define this.  If so, please inform
#    us so that we can refine the threshold.

# The following are best left undefined -- each can be overridden at runtime
# if need be.
# -DPORT=n sets the default UDP port for IKE messages (otherwise 500)
# -DSHARED_SECRETS_FILE=string overrides /etc/ipsec.secrets as the
#    default name of the file containing secrets used to authenticate other
#    IKE daemons.  In the Makefile, two levels of quoting are needed:
#    -DSHARED_SECRETS_FILE='"/etc/ipsec.secrets"'
# -DDEFAULT_CTLBASE=string overrides /var/run/pluto as default directory
#    and basename for pluto's lockfile (.pid) and control socket (.ctl).
#    Double quoting may be needed.

ifeq ($(USE_LWRES),true)
LWRESDEF=-DUSE_LWRES
USE_ADNS=false
BINNAMEADNSIFNEEDE=
else
USE_ADNS=true
BINNAMEADNSIFNEEDED=$(BINNAMEADNS)
endif

ifeq ($(USE_IPSECPOLICY),true)
IPSECPOLICY_FILES=rcv_info.c
IPSECPOLICY_DEFINES=-DIPSECPOLICY
IPSECPOLICY_LIBS=$(POLICYLIB)
IPSECPOLICY_OBJS=rcv_info.o
endif

ifeq ($(USE_VENDORID),true)
VENDORID=-DPLUTO_SENDS_VENDORID
endif

ifeq ($(USE_KEYRR),true)
KEYRR_DEFINES=-DUSE_KEYRR
endif

ifeq ($(USE_KERNEL26),true)
KERNEL26_DEFS=-DKERNEL26_SUPPORT -DKERNEL26_HAS_KAME_DUPLICATES 
KERNEL26_SRCS=kernel_netlink.c kernel_netlink.h
KERNEL26_OBJS=kernel_netlink.o
endif

DEFINES = $(EXTRA_DEFINES) \
	$(IPSECPOLICY_DEFINES) ${VENDORID} \
	$(KEYRR_DEFINES) \
	$(BYTE_ORDER) \
	$(LWRESDEF) \
	$(KERNEL26_DEFS) \
	-DPLUTO \
	-DKLIPS \
	-DDEBUG \
	-DGCC_LINT \
	# -DLEAK_DETECTIVE

CPPFLAGS = $(HDRDIRS) $(DEFINES) \
	-DSHARED_SECRETS_FILE=\"${FINALCONFDIR}/ipsec.secrets\" \
	-DPOLICYGROUPSDIR=\"${FINALCONFDDIR}/policies\" \
	-DPERPEERLOGDIR=\"${FINALLOGDIR}/pluto/peer\"

ALLFLAGS = $(CPPFLAGS) $(CFLAGS)

# libefence is a free memory allocation debugger
# Solaris 2 needs -lsocket -lnsl
LIBSPLUTO = $(OBJSGCRYPT) $(LIBDESLITE) $(FREESWANLIB) $(IPSECPOLICY_LIBS)
LIBSPLUTO+= -lgmp -lresolv # -lefence

ifneq ($(LD_LIBRARY_PATH),)
LDFLAGS=-L$(LD_LIBRARY_PATH)
endif

LIBSADNS = $(FREESWANLIB)
LIBSADNS += -lresolv # -lefence

# Solaris needs -lsocket -lnsl
LIBSWHACK = ${FREESWANLIB}

BINNAMEPLUTO = pluto
BINNAMEWHACK = whack
BINNAMEADNS = _pluto_adns

RM = /bin/rm
RMFLAGS = -f

.SUFFIXES:
.SUFFIXES: .c .o

# files for a (source) distribution

DISTMISC = CHANGES PLUTO-CONVENTIONS TODO ipsec.secrets Makefile routing.txt \
	 pluto.8 ipsec.secrets.5 .cvsignore

DISTGCRYPT = \
	gcryptfix.c gcryptfix.h \
	dsa.c dsa.h \
	elgamal.c elgamal.h \
	primegen.c \
	smallprime.c

DISTSRC = \
	connections.c connections.h \
	foodgroups.c foodgroups.h \
	constants.c constants.h \
	cookie.c cookie.h \
	crypto.h crypto.c \
	defs.h defs.c \
	demux.c demux.h \
	dnskey.c dnskey.h \
	endian.h \
	id.c id.h \
	ipsec_doi.c ipsec_doi.h \
	kernel.c kernel.h \
	kernel_netlink.c kernel_netlink.h \
	kernel_pfkey.c kernel_pfkey.h \
	kernel_noklips.c kernel_noklips.h \
	rcv_whack.c rcv_whack.h \
	$(IPSECPOLICY_FILES) \
	log.c log.h \
	plutomain.c \
	md5.c md5.h \
	packet.c packet.h \
	lex.c lex.h \
	keys.c keys.h \
	rnd.c rnd.h \
	server.c server.h \
	sha1.c sha1.h \
	spdb.c spdb.h \
	state.c state.h \
	timer.c timer.h \
	adns.c adns.h \
	whack.c whack.h whacklib.c

DIST = $(DISTMISC) $(DISTSRC)


# start of support for DSS/DSA.  Not currently used.
# OBJSGCRYPT =  gcryptfix.o dsa.o elgamal.o primegen.o smallprime.o
OBJSGCRYPT =

OBJSLIBPLUTO = whacklib.o

OBJSPLUTO = connections.o constants.o cookie.o crypto.o defs.o foodgroups.o log.o \
	state.o plutomain.o server.o timer.o id.o ipsec_doi.o kernel.o \
	$(KERNEL26_OBJS) kernel_pfkey.o kernel_noklips.o \
	rcv_whack.o ${IPSECPOLICY_OBJS} demux.o packet.o lex.o keys.o dnskey.o\
	rnd.o spdb.o sha1.o md5.o  ${OBJSLIBPLUTO}

OBJSADNS = adns.o

OBJSWHACK = whack.o

all: $(BINNAMEPLUTO) $(BINNAMEADNSIFNEEDED) $(BINNAMEWHACK)
programs: $(BINNAMEPLUTO) $(BINNAMEADNSIFNEEDED) $(BINNAMEWHACK)


install: all
	mkdir -p ${LIBEXECDIR} ${LIBDIR}
	$(INSTALL) $(INSTBINFLAGS) $(BINNAMEPLUTO) $(BINNAMEWHACK) $(LIBEXECDIR)
	if $(USE_ADNS) ; then $(INSTALL) $(INSTBINFLAGS) $(BINNAMEADNS)                  $(LIBDIR) ; fi
	$(INSTALL) $(INSTMANFLAGS) pluto.8 $(PMANDIR)/ipsec_pluto.8
	sh ${FREESWANSRCDIR}/packaging/utils/manlink pluto.8 | \
		while read from to ; \
		do \
			ln -s -f ipsec_$$from $(PMANDIR)/$$to; \
		done
	$(INSTALL) $(INSTMANFLAGS) ipsec.secrets.5 $(FMANDIR)
	sh ${FREESWANSRCDIR}/packaging/utils/manlink ipsec.secrets.5 | \
		while read from to ; \
		do \
		 ln -s -f $$from $(FMANDIR)/$$to; \
		done

install_file_list:
	@echo $(LIBEXECDIR)/$(BINNAMEPLUTO)
	@if $(USE_ADNS) ; then echo $(LIBDIR)/$(BINNAMEADNS) ; fi
	@echo $(LIBEXECDIR)/$(BINNAMEWHACK)
	@echo $(PMANDIR)/ipsec_pluto.8
	@sh ${FREESWANSRCDIR}/packaging/utils/manlink pluto.8 | \
		while read from to; \
		do\
			 echo $(PMANDIR)/$$to; \
		done
	@echo $(FMANDIR)/ipsec.secrets.5
	@sh ${FREESWANSRCDIR}/packaging/utils/manlink ipsec.secrets.5 | \
		while read from to; \
		do \
			echo $(FMANDIR)/$$to; \
		done

$(BINNAMEPLUTO): $(OBJSPLUTO)
	$(CC) -o $(BINNAMEPLUTO) $(LDFLAGS) $(OBJSPLUTO) $(LIBSPLUTO)

$(BINNAMEADNS): $(OBJSADNS)
	$(CC) -o $(BINNAMEADNS) $(OBJSADNS) $(LIBSADNS)

$(BINNAMEWHACK): $(OBJSWHACK)
	$(CC) -o $(BINNAMEWHACK) $(OBJSWHACK) $(LIBSWHACK)

distlist:
	@echo $(DIST)

# Exuberant Ctags doesn't work if LC_ALL is set to something other than C

CTAGSFLAGS = -N --format=1 # fishy options required for Exuberant Ctags

tags:	$(DISTSRC)
	LC_ALL=C ctags $(CTAGSFLAGS) $(DISTSRC) $(LIBFREESWANDIR)/*.[ch]

TAGS:	$(DISTSRC)
	LC_ALL=C etags $(ETAGSFLAGS) $(DISTSRC) $(LIBFREESWANDIR)/*.[ch]

cleanall: clean

distclean: clean

mostlyclean: clean

realclean: clean

clean:
	$(RM) $(RMFLAGS) *.core core *~ a.out ktrace.out \
		$(OBJSPLUTO) $(BINNAMEPLUTO) \
		$(OBJSWHACK) $(BINNAMEWHACK) \
		$(OBJSADNS) $(BINNAMEADNS)

check:
	echo no checks in lib right now.

checkprograms:

.c.o:
	$(CC) $(COPTS) $(ALLFLAGS) -c $<

# Gather dependencies caused by explicit #includes within .c files
#
# Each .c is assumed to compile into a .o with the corresponding name.
# Only dependencies on based on "" includes are considered, not <>.
# Dependencies caused by includes within headers are not noticed.
# Unlike dependencies generated by the compiler, these include dependencies
# suppressed by conditional compilation (good, we think).
# This code can be tricked by embeding #include in comments or
# vice-versa, but we're among friends.

gatherdeps:
	@ls $(DISTSRC) | grep '\.c' | sed -e 's/\(.*\)\.c$$/\1.o: \1.c/'
	@echo
	@ls $(DISTSRC) | grep '\.c' | xargs grep '^#[ 	]*include[ 	]*"' | \
		sed -e 's/\.c:#[ 	]*include[ 	]*"/.o: /' -e 's/".*//'

# Dependencies generated by "make gatherdeps":
adns.o: adns.c
connections.o: connections.c
constants.o: constants.c
cookie.o: cookie.c
crypto.o: crypto.c
defs.o: defs.c
demux.o: demux.c
dnskey.o: dnskey.c
foodgroups.o: foodgroups.c
id.o: id.c
ipsec_doi.o: ipsec_doi.c
kernel.o: kernel.c
kernel_netlink.o: kernel_netlink.c
kernel_noklips.o: kernel_noklips.c
kernel_pfkey.o: kernel_pfkey.c
keys.o: keys.c
lex.o: lex.c
log.o: log.c
md5.o: md5.c
packet.o: packet.c
plutomain.o: plutomain.c
rcv_whack.o: rcv_whack.c
rnd.o: rnd.c
server.o: server.c
sha1.o: sha1.c
spdb.o: spdb.c
state.o: state.c
timer.o: timer.c
whack.o: whack.c
whacklib.o: whacklib.c

adns.o: constants.h
adns.o: adns.h
connections.o: kameipsec.h
connections.o: constants.h
connections.o: defs.h
connections.o: id.h
connections.o: connections.h
connections.o: foodgroups.h
connections.o: packet.h
connections.o: demux.h
connections.o: state.h
connections.o: timer.h
connections.o: ipsec_doi.h
connections.o: server.h
connections.o: kernel.h
connections.o: log.h
connections.o: keys.h
connections.o: adns.h
connections.o: dnskey.h
connections.o: whack.h
connections.o: diag.h
constants.o: constants.h
constants.o: defs.h
constants.o: log.h
constants.o: packet.h
cookie.o: constants.h
cookie.o: defs.h
cookie.o: sha1.h
cookie.o: rnd.h
cookie.o: cookie.h
crypto.o: constants.h
crypto.o: defs.h
crypto.o: state.h
crypto.o: log.h
crypto.o: md5.h
crypto.o: sha1.h
crypto.o: crypto.h
defs.o: constants.h
defs.o: defs.h
defs.o: log.h
defs.o: whack.h
demux.o: constants.h
demux.o: defs.h
demux.o: cookie.h
demux.o: id.h
demux.o: connections.h
demux.o: state.h
demux.o: packet.h
demux.o: md5.h
demux.o: sha1.h
demux.o: crypto.h
demux.o: log.h
demux.o: demux.h
demux.o: ipsec_doi.h
demux.o: timer.h
demux.o: whack.h
demux.o: server.h
dnskey.o: constants.h
dnskey.o: adns.h
dnskey.o: defs.h
dnskey.o: id.h
dnskey.o: log.h
dnskey.o: connections.h
dnskey.o: keys.h
dnskey.o: dnskey.h
dnskey.o: packet.h
dnskey.o: timer.h
dnskey.o: diag.h
foodgroups.o: constants.h
foodgroups.o: defs.h
foodgroups.o: id.h
foodgroups.o: connections.h
foodgroups.o: foodgroups.h
foodgroups.o: kernel.h
foodgroups.o: lex.h
foodgroups.o: log.h
foodgroups.o: whack.h
id.o: constants.h
id.o: defs.h
id.o: id.h
id.o: log.h
id.o: connections.h
id.o: packet.h
id.o: whack.h
ipsec_doi.o: constants.h
ipsec_doi.o: defs.h
ipsec_doi.o: state.h
ipsec_doi.o: id.h
ipsec_doi.o: connections.h
ipsec_doi.o: keys.h
ipsec_doi.o: packet.h
ipsec_doi.o: demux.h
ipsec_doi.o: adns.h
ipsec_doi.o: dnskey.h
ipsec_doi.o: kernel.h
ipsec_doi.o: log.h
ipsec_doi.o: cookie.h
ipsec_doi.o: server.h
ipsec_doi.o: spdb.h
ipsec_doi.o: timer.h
ipsec_doi.o: rnd.h
ipsec_doi.o: ipsec_doi.h
ipsec_doi.o: whack.h
ipsec_doi.o: sha1.h
ipsec_doi.o: md5.h
ipsec_doi.o: crypto.h
kernel.o: kameipsec.h
kernel.o: constants.h
kernel.o: defs.h
kernel.o: rnd.h
kernel.o: id.h
kernel.o: connections.h
kernel.o: state.h
kernel.o: timer.h
kernel.o: kernel.h
kernel.o: kernel_netlink.h
kernel.o: kernel_pfkey.h
kernel.o: kernel_noklips.h
kernel.o: log.h
kernel.o: server.h
kernel.o: whack.h
kernel_netlink.o: kameipsec.h
kernel_netlink.o: constants.h
kernel_netlink.o: defs.h
kernel_netlink.o: id.h
kernel_netlink.o: connections.h
kernel_netlink.o: kernel.h
kernel_netlink.o: kernel_netlink.h
kernel_netlink.o: kernel_pfkey.h
kernel_netlink.o: log.h
kernel_netlink.o: whack.h
kernel_noklips.o: constants.h
kernel_noklips.o: defs.h
kernel_noklips.o: id.h
kernel_noklips.o: connections.h
kernel_noklips.o: kernel.h
kernel_noklips.o: kernel_noklips.h
kernel_noklips.o: log.h
kernel_noklips.o: whack.h
kernel_pfkey.o: constants.h
kernel_pfkey.o: defs.h
kernel_pfkey.o: id.h
kernel_pfkey.o: connections.h
kernel_pfkey.o: kernel.h
kernel_pfkey.o: kernel_pfkey.h
kernel_pfkey.o: log.h
kernel_pfkey.o: whack.h
keys.o: constants.h
keys.o: defs.h
keys.o: id.h
keys.o: connections.h
keys.o: state.h
keys.o: lex.h
keys.o: keys.h
keys.o: adns.h
keys.o: dnskey.h
keys.o: log.h
keys.o: whack.h
keys.o: timer.h
keys.o: diag.h
lex.o: constants.h
lex.o: defs.h
lex.o: log.h
lex.o: whack.h
lex.o: lex.h
log.o: constants.h
log.o: defs.h
log.o: log.h
log.o: server.h
log.o: state.h
log.o: id.h
log.o: connections.h
log.o: kernel.h
log.o: whack.h
log.o: timer.h
log.o: diag.h
md5.o: md5.h
md5.o: endian.h
packet.o: constants.h
packet.o: defs.h
packet.o: log.h
packet.o: diag.h
packet.o: packet.h
packet.o: whack.h
plutomain.o: constants.h
plutomain.o: defs.h
plutomain.o: id.h
plutomain.o: connections.h
plutomain.o: foodgroups.h
plutomain.o: packet.h
plutomain.o: demux.h
plutomain.o: server.h
plutomain.o: kernel.h
plutomain.o: log.h
plutomain.o: keys.h
plutomain.o: adns.h
plutomain.o: dnskey.h
plutomain.o: rnd.h
plutomain.o: state.h
plutomain.o: ipsec_doi.h
plutomain.o: sha1.h
plutomain.o: md5.h
plutomain.o: crypto.h
rcv_whack.o: constants.h
rcv_whack.o: defs.h
rcv_whack.o: id.h
rcv_whack.o: connections.h
rcv_whack.o: foodgroups.h
rcv_whack.o: whack.h
rcv_whack.o: packet.h
rcv_whack.o: demux.h
rcv_whack.o: state.h
rcv_whack.o: ipsec_doi.h
rcv_whack.o: kernel.h
rcv_whack.o: rcv_whack.h
rcv_whack.o: log.h
rcv_whack.o: diag.h
rcv_whack.o: keys.h
rcv_whack.o: adns.h
rcv_whack.o: dnskey.h
rcv_whack.o: server.h
rnd.o: sha1.h
rnd.o: constants.h
rnd.o: defs.h
rnd.o: rnd.h
rnd.o: log.h
rnd.o: timer.h
server.o: constants.h
server.o: defs.h
server.o: state.h
server.o: id.h
server.o: connections.h
server.o: kernel.h
server.o: log.h
server.o: diag.h
server.o: server.h
server.o: timer.h
server.o: packet.h
server.o: demux.h
server.o: rcv_whack.h
server.o: rcv_info.h
server.o: keys.h
server.o: adns.h
server.o: dnskey.h
server.o: whack.h
server.o: kameipsec.h
sha1.o: sha1.h
sha1.o: endian.h
spdb.o: constants.h
spdb.o: defs.h
spdb.o: id.h
spdb.o: connections.h
spdb.o: state.h
spdb.o: packet.h
spdb.o: keys.h
spdb.o: kernel.h
spdb.o: log.h
spdb.o: diag.h
spdb.o: spdb.h
spdb.o: whack.h
spdb.o: sha1.h
spdb.o: md5.h
spdb.o: crypto.h
state.o: constants.h
state.o: defs.h
state.o: id.h
state.o: connections.h
state.o: state.h
state.o: kernel.h
state.o: log.h
state.o: packet.h
state.o: keys.h
state.o: rnd.h
state.o: timer.h
state.o: whack.h
state.o: demux.h
state.o: ipsec_doi.h
state.o: sha1.h
state.o: md5.h
state.o: crypto.h
state.o: diag.h
timer.o: constants.h
timer.o: defs.h
timer.o: id.h
timer.o: connections.h
timer.o: state.h
timer.o: packet.h
timer.o: demux.h
timer.o: ipsec_doi.h
timer.o: kernel.h
timer.o: server.h
timer.o: log.h
timer.o: rnd.h
timer.o: timer.h
timer.o: whack.h
whack.o: constants.h
whack.o: defs.h
whack.o: whack.h
whacklib.o: constants.h
whacklib.o: defs.h
whacklib.o: whack.h
whacklib.o: diag.h
