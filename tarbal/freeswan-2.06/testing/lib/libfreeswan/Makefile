# FreeS/WAN testing makefile
# Copyright (C) 2002 Michael Richardson <mcr@freeswan.org>
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
# RCSID $Id: Makefile,v 1.2 2003/11/05 04:57:30 dhr Exp $

FREESWANSRCDIR=../../..
include $(FREESWANSRCDIR)/Makefile.inc

SUBDIRS=
UTILS=${FREESWANSRCDIR}/testing/utils

def:
	@echo "Please read doc/intro.html or INSTALL before running make"
	@false

check:
	REGRESSRESULTS=$${REGRESSRESULTS-../results} ; source ${UTILS}/functions.sh; source TESTLIST

install install_file_list checkprograms programs:
	@true

clean:
	cat TESTLIST | while read testtype name status; \
	do\
		rm -rf `basename $$name .c`;\
	done

cleanall: clean

distclean: clean

mostlyclean: clean

realclean: clean

#
# $Log: Makefile,v $
# Revision 1.2  2003/11/05 04:57:30  dhr
#
# set up a default REGRESSRESULTS for libfreeswan tests (like other kinds of tests)
#
# Revision 1.1  2002/06/08 17:41:22  mcr
# 	library test routines.
#
# 



