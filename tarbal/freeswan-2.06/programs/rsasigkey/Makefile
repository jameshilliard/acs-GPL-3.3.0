# Makefile for miscelaneous programs
# Copyright (C) 2002  Michael Richardson	<mcr@freeswan.org>
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
# RCSID $Id: Makefile,v 1.3 2003/12/11 20:05:54 mcr Exp $

FREESWANSRCDIR=../..
include ${FREESWANSRCDIR}/Makefile.inc

PROGRAM=rsasigkey
LIBS=${FREESWANLIB} -lgmp

CFLAGS+= -DIPSEC_KEY_MINIMUM=${IPSEC_KEY_MINIMUM}
CFLAGS+= -DIPSEC_KEY_MAXIMUM=${IPSEC_KEY_MAXIMUM}


include ../Makefile.program

#
# $Log: Makefile,v $
# Revision 1.3  2003/12/11 20:05:54  mcr
# 	rsasigkey random key length generation.
#
# Revision 1.2  2002/06/02 21:51:41  mcr
# 	changed TOPDIR->FREESWANSRCDIR in all Makefiles.
# 	(note that linux/net/ipsec/Makefile uses TOPDIR because this is the
# 	kernel sense.)
#
# Revision 1.1  2002/04/24 07:55:32  mcr
# 	#include patches and Makefiles for post-reorg compilation.
#
#
#

