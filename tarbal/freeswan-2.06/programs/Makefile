# Makefile for the KLIPS interface utilities
# Copyright (C) 1998, 1999  Henry Spencer.
# Copyright (C) 1999, 2000, 2001  Richard Guy Briggs
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
# RCSID $Id: Makefile,v 1.16 2003/11/07 02:58:07 mcr Exp $

FREESWANSRCDIR=..
include ${FREESWANSRCDIR}/Makefile.inc

SUBDIRS=spi eroute spigrp tncfg klipsdebug pf_key proc pluto 
SUBDIRS+=_confread _copyright _include _keycensor _plutoload _plutorun
SUBDIRS+=_realsetup _secretcensor _startklips _updown
SUBDIRS+=auto barf verify ipsec look manual newhostkey ranbits 
SUBDIRS+=rsasigkey send-pr setup showdefaults showhostkey calcgoo mailkey
SUBDIRS+=ikeping

ifeq ($(USE_LWRES),true)
SUBDIRS+=lwdnsq
endif

ifeq ($(USE_IPSECPOLICY),true)
SUBDIRS+=showpolicy
endif

def:
	@echo "Please read doc/intro.html or INSTALL before running make"
	@false

# programs

cleanall distclean mostlyclean realclean install programs checkprograms check clean spotless install_file_list:
	@for d in $(SUBDIRS) ; \
	do \
		(cd $$d && $(MAKE) FREESWANSRCDIR=$(FREESWANSRCDIR)/.. $@ ) || exit 1;\
	done; \

