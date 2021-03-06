#
# doc/Makefile.am --- automake input file for gawk
#
# Copyright (C) 2000, 2001, 2002, 2004 the Free Software Foundation, Inc.
#
# This file is part of GAWK, the GNU implementation of the
# AWK Programming Language.
#
# GAWK is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# GAWK is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
#

## process this file with automake to produce Makefile.in

info_TEXINFOS = gawk.texi gawkinet.texi

man_MANS = gawk.1 igawk.1

EXTRA_DIST = ChangeLog README.card ad.block setter.outline \
	awkcard.in awkforai.txt texinfo.tex cardfonts \
	macros colors no.colors $(man_MANS) \
	lflashlight.eps rflashlight.eps \
	statist.jpg statist.eps

MAKEINFO = @MAKEINFO@ --no-split --force

TROFF = groff -t -Tps -U
SEDME = sed -e "s/^level0 restore/level0 restore flashme 100 72 moveto (Copyright `date '+%m-%d-%y %T'`, FSF, Inc. (all)) show/" \
		-e "s/^\/level0 save def/\/level0 save def 30 -48 translate/"

CARDSRC = $(srcdir)/macros $(srcdir)/cardfonts $(srcdir)/colors awkcard.tr
CARDSRC_N = $(srcdir)/macros $(srcdir)/cardfonts $(srcdir)/no.colors awkcard.tr
CARDFILES= $(CARDSRC) ad.block awkcard.in setter.outline

# Use this if your troff can correctly handle macros from 'colors' file
AWKCARD = awkcard.ps

# Uncomment the following definition of AWKCARD if your troff can produce
# Postscript but still has troubles with macros from 'colors'.  As this
# is not groff you will have to change TROFF macro as well.  Do not forget
# to ensure that awkcard.tr is processed by tbl.
#AWKCARD = awkcard.nc

# The following is patterned after the main Makefile.am.  The point is to
# make pgawk.1 a link to gawk.1 in the installed man directory.

# We want hard links for install-data-hook, below
LN= ln

# Link gawk.1 to pgawk.1
install-data-hook:
	(cd $(DESTDIR)$(man1dir); \
	$(LN) gawk.1 pgawk.1 2>/dev/null ; \
	exit 0)

# Undo the above when uninstalling
uninstall-hook:
	cd $(DESTDIR)$(man1dir); rm -f pgawk.1 ; exit 0

postscript: gawk.ps gawkinet.ps gawk.1.ps igawk.1.ps $(AWKCARD)

gawk.ps: gawk.dvi
	dvips -o gawk.ps gawk.dvi

gawkinet.ps: gawkinet.dvi
	dvips -o gawkinet.ps gawkinet.dvi

gawk.1.ps: gawk.1
	-groff -man $(srcdir)/gawk.1 > gawk.1.ps

igawk.1.ps: igawk.1
	-groff -man $(srcdir)/igawk.1 > igawk.1.ps

awkcard.tr: awkcard.in
	sed 's:SRCDIR:$(srcdir):' < $(srcdir)/awkcard.in > awkcard.tr

awkcard.ps: $(CARDFILES)
	$(TROFF) $(CARDSRC) | $(SEDME) | cat $(srcdir)/setter.outline - > awkcard.ps

awkcard.nc: $(CARDFILES)
	$(TROFF) $(CARDSRC_N) | $(SEDME) | cat $(srcdir)/setter.outline - > awkcard.ps && touch awkcard.nc

html: gawk.html gawkinet.html

gawk.html: gawk.texi
	$(MAKEINFO) --html $<

gawkinet.html: gawkinet.texi
	$(MAKEINFO) --html $<

clean:
	rm -f *.ps *~ awkcard.nc awkcard.tr *.html
