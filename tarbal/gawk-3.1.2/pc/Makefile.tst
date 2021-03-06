# Makefile for GNU Awk test suite.
#
# Copyright (C) 1988-2003 the Free Software Foundation, Inc.
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

# ============================================================================
# MS-DOS & OS/2 Notes: READ THEM!
# ============================================================================

# As of version 2.91, efforts to make this makefile run in MS-DOS and OS/2
# have started in earnest.  The following steps need to be followed in order 
# to run this makefile:
#
# 1. The first thing that you will need to do is to convert all of the 
#    files ending in ".ok" in the test directory, all of the files ending 
#    in ".good" (or ".goo") in the test/reg directory, and mmap8k.in from
#    having a linefeed to having carriage return/linefeed at the end of each
#    line. There are various public domain UNIX to DOS converters and any 
#    should work.  Alternatively, you can use diff instead of cmp--most 
#    versions of diff don't care about how the lines end.
#
# 2. You will need an sh-compatible shell.  Please refer to the "README.pc"
#    file in the README_d directory for information about obtaining a copy.
#    You will also need various UNIX utilities.  At a minimum, you will 
#    need: rm, tr, cmp (or diff, see above), cat, wc, and sh.  
#    You should also have a UNIX-compatible date program.
#
# The makefile has only been tested with dmake 3.8 and DJGPP Make 3.74 or
# later.  After making all of these changes, typing "dmake bigtest extra"
# or "make bigtest extra" (with DJGPP Make) should run successfully.

# The Bash shell (compiled with djgpp) works very well with the
# MSC & djgpp-compiled gawks.  It is currently the recommended shell to use
# for testing, along with DJGPP make.  See README.pc for 
# more information on OS/2 and DOS shells.

# You will almost certainly need to change some of the values (MACROS) 
# defined on the next few lines.  

# This won't work unless you have "sh" and set SHELL equal to it (Make 3.74
# or later which comes with DJGPP will work with SHELL=/bin/sh if you have
# sh.exe anywhere on your PATH).
#SHELL = e:\bin\sh.exe
SHELL = /bin/sh

# Point to gawk
AWK = ../gawk.exe
# Also point to gawk but for DOS commands needing backslashes.  We need
# the forward slash version too or 'arrayparam' fails.
AWK2 = '..\gawk.exe'
AWKPROG = ../gawk.exe

# Set your cmp command here (you can use most versions of diff instead of cmp
# if you don't want to convert the .ok files to the DOS CR/LF format).
# This is also an issue for the "mmap8k" test.  If it fails, make sure that
# mmap8k.in has CR/LFs or that you've used diff.
#
# The following comment is for users of OSs which support long file names
# (such as Windows 95) for all versions of gawk (both 16 & 32-bit).
# If you use a shell which doesn't support long filenames, temporary files
# created by this makefile will be truncated by your shell.  "_argarra" is an
# example of this.  If $(CMP) is a DJGPP-compiled program, then it will fail
# because it looks for the long filename (eg. _argarray).  To fix this, you
# need to set LFN=n in your shell's environment.
# NOTE: Setting LFN in the makefile most probably won't help you because LFN
# needs to be an environment variable.
#CMP = cmp
# See the comment above for why you might want to set CMP to "env LFN=n diff"
#CMP = env LFN=n diff
CMP = diff
#CMP = diff -c
#CMP = gcmp

# Set your "cp" and "mkdir" commands here.  Note: DOS's copy must take forward
# slashes.
#CP = cp
#CP = : && command -c copy
CP  = command.com /c copy

#MKDIR = mkdir
#MKDIR = gmkdir
#MKDIR = : && command -c mkdir
MKDIR  = command.com /c mkdir

# Set your unix-style date function here
#DATE = date
DATE = gdate

# MS-DOS and OS/2 use ; as a PATH delimiter
PATH_SEPARATOR = ;

# ============================================================================
# You shouldn't need to modify anything below this line.
# ============================================================================

srcdir = .

# try to keep these sorted
BASIC_TESTS = addcomma anchgsub argarray arrayparm arrayref arrymem1 arynasty \
	arynocls arysubnm asgext awkpath back89 backgsub childin clobber \
	clsflnam compare compare2 concat1 convfmt datanonl defref delarprm \
	dynlj eofsplit fldchg fldchgnf fnamedat fnarray fnarydel fnaryscl \
	fnasgnm fnmisc fnparydl forsimp fsbs fsrs fstabplus funsemnl funsmnam \
	funstack getline getline2 getline3 getlnbuf getnr2tb getnr2tm gsubasgn gsubtest \
	gsubtst2 gsubtst3 gsubtst4 hsprint inputred intest intprec leaddig leadnl \
	litoct longsub longwrds math membug1 messages minusstr mmap8k \
	nasty nasty2 negexp nfldstr nfneg nfset nlfldsep nlinstr nlstrina \
	noeffect nofmtch noloop1 noloop2 nonl noparms nors nulrsend \
	numindex numsubstr octsub ofmt ofmtbig ofmtfidl ofmts onlynl \
	opasnidx opasnslf paramdup paramtyp parseme pcntplus prdupval prec \
	printf0 printf1 prmarscl prmreuse prt1eval prtoeval psx96sub rand rebt8b1 \
	rebt8b2 redfilnm regeq reindops reparse resplit rs rsnul1nl \
	rstest1 rstest2 rswhite sclforin sclifin splitargv splitarr splitdef \
	splitvar splitwht sprintfc strtod subslash substr swaplns synerr1 \
	tradanch tweakfld uninitialized uninit2 zeroe0 zeroflag


UNIX_TESTS = fflush getlnhd pid pipeio1 pipeio2 poundbang space strftlng

GAWK_EXT_TESTS = argtest asort asorti badargs clos1way fieldwdth fsfwfs \
	gensub gnuops2 gnureops icasefs icasers igncdym igncfs ignrcase lint \
	match1 manyfiles nondec posix procinfs regx8bit rebuf reint shadow \
	sort1 strftime


EXTRA_TESTS = regtest inftest

INET_TESTS = inetechu inetecht inetdayu inetdayt

# List of the tests which should be run with --lint option:
NEED_LINT = defref noeffect nofmtch shadow uninitialized uninit2

# List of the files that appear in manual tests or are for reserve testing:
GENTESTS_UNUSED = Makefile.in gtlnbufv.awk printfloat.awk

# message stuff is to make it a little easier to follow
check:	msg \
	basic-msg-start  basic           basic-msg-end \
	unix-msg-start   unix-tests      unix-msg-end \
	extend-msg-start gawk-extensions extend-msg-end \
	pass-fail

basic:	$(BASIC_TESTS)

unix-tests: $(UNIX_TESTS)

gawk-extensions: $(GAWK_EXT_TESTS)

#extra:	$(EXTRA_TESTS) inet
extra:	$(EXTRA_TESTS) inetmesg

inet:	inetmesg $(INET_TESTS)

msg::
	@echo ''
	@echo 'Any output from "cmp" is bad news, although some differences'
	@echo 'in floating point values are probably benign -- in particular,'
	@echo 'some systems may omit a leading zero and the floating point'
	@echo 'precision may lead to slightly different output in a few cases.'

basic-msg-start:
	@echo "======== Starting basic tests ========"

basic-msg-end:
	@echo "======== Done with basic tests ========"

unix-msg-start:
	@echo "======== Starting Unix tests ========"

unix-msg-end:
	@echo "======== Done with Unix tests ========"

extend-msg-start:
	@echo "======== Starting gawk extension tests ========"

extend-msg-end:
	@echo "======== Done with gawk extension tests ========"

# This test is a PITA because increasingly, /tmp is getting
# mounted noexec.  So, we'll test it locally.  Sigh.
#
# More PITA; some systems have medium short limits on #! paths,
# so this can still fail
poundbang::
	@echo $@
	@sed "s;/tmp/gawk;`pwd`/$(AWKPROG);" < $(srcdir)/poundbang.awk > ./_pbd.awk
	@chmod +x ./_pbd.awk
	@if ./_pbd.awk $(srcdir)/poundbang.awk > _`basename $@` ; \
	then : ; \
	else \
		sed "s;/tmp/gawk;../$(AWKPROG);" < $(srcdir)/poundbang.awk > ./_pbd.awk ; \
		chmod +x ./_pbd.awk ; \
		./_pbd.awk $(srcdir)/poundbang.awk > _`basename $@`;  \
	fi
	@-$(CMP) $(srcdir)/poundbang.awk _`basename $@` && rm -f _`basename $@` _pbd.awk

messages::
	@echo $@
	@$(AWK) -f $(srcdir)/messages.awk >out2 2>out3
	@-$(CMP) $(srcdir)/out1.ok out1 && $(CMP) $(srcdir)/out2.ok out2 && $(CMP) $(srcdir)/out3.ok out3 && rm -f out1 out2 out3

argarray::
	@echo $@
	@case $(srcdir) in \
	.)	: ;; \
	*)	cp $(srcdir)/argarray.in . ;; \
	esac
	@TEST=test echo just a test | $(AWK) -f $(srcdir)/argarray.awk ./argarray.in - >_$@
	@case $(srcdir) in \
	.)	: ;; \
	*)	rm -f ./argarray.in ;; \
	esac
	@-$(CMP) $(srcdir)/argarray.ok _$@ && rm -f _$@

regtest::
	@echo 'Some of the output from regtest is very system specific, do not'
	@echo 'be distressed if your output differs from that distributed.'
	@echo 'Manual inspection is called for.'
	AWK=`pwd`/$(AWK) $(srcdir)/regtest.sh

manyfiles::
	@echo manyfiles
	@rm -rf junk
	@mkdir junk
	@$(AWK) 'BEGIN { for (i = 1; i <= 300; i++) print i, i}' >_$@
	@$(AWK) -f $(srcdir)/manyfiles.awk _$@ _$@
	@wc -l junk/* | $(AWK) '$$1 != 2' | wc -l | sed 's/  *//g' > _$@
	@rm -rf junk ; $(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

compare::
	@echo $@
	@$(AWK) -f $(srcdir)/compare.awk 0 1 $(srcdir)/compare.in >_$@
	@-$(CMP) $(srcdir)/compare.ok _$@ && rm -f _$@

inftest::
	@echo $@
	@echo This test is very machine specific...
	@$(AWK) -f $(srcdir)/inftest.awk | sed 's/inf/Inf/g' >_$@
	@-$(CMP) $(srcdir)/inftest.ok _$@ && rm -f _$@

getline2::
	@echo $@
	@$(AWK) -f $(srcdir)/getline2.awk $(srcdir)/getline2.awk $(srcdir)/getline2.awk >_$@
	@-$(CMP) $(srcdir)/getline2.ok _$@ && rm -f _$@

awkpath::
	@echo $@
	@AWKPATH="$(srcdir)$(PATH_SEPARATOR)$(srcdir)/lib" $(AWK) -f awkpath.awk >_$@
	@-$(CMP) $(srcdir)/awkpath.ok _$@ && rm -f _$@

argtest::
	@echo $@
	@$(AWK) -f $(srcdir)/argtest.awk -x -y abc >_$@
	@-$(CMP) $(srcdir)/argtest.ok _$@ && rm -f _$@

badargs::
	@echo $@
	@-$(AWK) -f 2>&1 | grep -v patchlevel >_$@
	@-$(CMP) $(srcdir)/badargs.ok _$@ && rm -f _$@

nonl::
	@echo $@
	@-AWKPATH=$(srcdir) $(AWK) --lint -f nonl.awk /dev/null >_$@ 2>&1
	@-$(CMP) $(srcdir)/nonl.ok _$@ && rm -f _$@

strftime::
	@echo This test could fail on slow machines or on a second boundary,
	@echo so if it does, double check the actual results:
	@echo $@
	@LC_ALL=C; export LC_ALL; LANG=C; export LANG; \
	TZ=GMT0; export TZ; \
	date | $(AWK) -v OUTPUT=_$@ -f $(srcdir)/strftime.awk
	@-$(CMP) strftime.ok _$@ && rm -f _$@ strftime.ok || exit 0

litoct::
	@echo $@
	@echo ab | $(AWK) --traditional -f $(srcdir)/litoct.awk >_$@
	@-$(CMP) $(srcdir)/litoct.ok _$@ && rm -f _$@

fflush::
	@echo $@
	@$(srcdir)/fflush.sh >_$@
	@-$(CMP) $(srcdir)/fflush.ok _$@ && rm -f _$@

tweakfld::
	@echo $@
	@$(AWK) -f $(srcdir)/tweakfld.awk $(srcdir)/tweakfld.in >_$@
	@rm -f errors.cleanup
	@-$(CMP) $(srcdir)/tweakfld.ok _$@ && rm -f _$@

mmap8k::
	@echo $@
	@$(AWK) '{ print }' $(srcdir)/mmap8k.in >_$@
	@-$(CMP) $(srcdir)/mmap8k.in _$@ && rm -f _$@

tradanch::
	@echo $@
	@$(AWK) --traditional -f $(srcdir)/tradanch.awk $(srcdir)/tradanch.in >_$@
	@-$(CMP) $(srcdir)/tradanch.ok _$@ && rm -f _$@

# AIX /bin/sh exec's the last command in a list, therefore issue a ":"
# command so that pid.sh is fork'ed as a child before being exec'ed.
pid::
	@echo pid
	@AWKPATH=$(srcdir) AWK=$(AWKPROG) LC_ALL=C $(SHELL) $(srcdir)/pid.sh $$$$ > _`basename $@` ; :
	@echo 'Expect pid to fail in DOS.'
	@-$(CMP) $(srcdir)/pid.ok _`basename $@` && rm -f _`basename $@` _`basename $@`.in

strftlng::
	@echo $@
	@TZ=UTC; export TZ; $(AWK) -f $(srcdir)/strftlng.awk >_$@
	@if $(CMP) -s $(srcdir)/strftlng.ok _$@ ; then : ; else \
	TZ=UTC0; export TZ; $(AWK) -f $(srcdir)/strftlng.awk >_$@ ; \
	fi
	@-$(CMP) $(srcdir)/strftlng.ok _$@ && rm -f _$@

nors::
	@echo $@
	@echo A B C D E | tr -d '\12\15' | $(AWK) '{ print $$NF }' - $(srcdir)/nors.in > _$@
	@-$(CMP) $(srcdir)/nors.ok _$@ && rm -f _$@

reint::
	@echo $@
	@$(AWK) --re-interval -f $(srcdir)/reint.awk $(srcdir)/reint.in >_$@
	@-$(CMP) $(srcdir)/reint.ok _$@ && rm -f _$@

pipeio1::
	@echo $@
	@$(AWK) -f $(srcdir)/pipeio1.awk >_$@
	@rm -f test1 test2
	@-$(CMP) $(srcdir)/pipeio1.ok _$@ && rm -f _$@

pipeio2::
	@echo $@
	@$(AWK) -v SRCDIR=$(srcdir) -f $(srcdir)/pipeio2.awk >_$@
	@-$(CMP) $(srcdir)/pipeio2.ok _$@ && rm -f _$@

clobber::
	@echo $@
	@$(AWK) -f $(srcdir)/clobber.awk >_$@
	@-$(CMP) $(srcdir)/clobber.ok seq && $(CMP) $(srcdir)/clobber.ok _$@ && rm -f _$@
	@rm -f seq

arynocls::
	@echo $@
	@-AWKPATH=$(srcdir) $(AWK) -v INPUT=$(srcdir)/arynocls.in -f arynocls.awk >_$@
	@-$(CMP) $(srcdir)/arynocls.ok _$@ && rm -f _$@

getlnbuf::
	@echo $@
	@-AWKPATH=$(srcdir) $(AWK) -f getlnbuf.awk $(srcdir)/getlnbuf.in > _$@
	@-AWKPATH=$(srcdir) $(AWK) -f gtlnbufv.awk $(srcdir)/getlnbuf.in > _2$@
	@-$(CMP) $(srcdir)/getlnbuf.ok _$@ && $(CMP) $(srcdir)/getlnbuf.ok _2$@ && rm -f _$@ _2$@

inetmesg::
	@echo These tests only work if your system supports the services
	@echo "'discard'" at port 9 and "'daytimed'" at port 13. Check your
	@echo file /etc/services and do "'netstat -a'".
	@echo Expect inet tests to fail with MSC and DJGPP because "|&" is not supported.

inetechu::
	@echo This test is for establishing UDP connections
	@$(AWK) 'BEGIN {print "" |& "/inet/udp/0/127.0.0.1/9"}'

inetecht::
	@echo This test is for establishing TCP connections
	@$(AWK) 'BEGIN {print "" |& "/inet/tcp/0/127.0.0.1/9"}'

inetdayu::
	@echo This test is for bidirectional UDP transmission
	@$(AWK) 'BEGIN { print "" |& "/inet/udp/0/127.0.0.1/13"; \
	"/inet/udp/0/127.0.0.1/13" |& getline; print $0}'

inetdayt::
	@echo This test is for bidirectional TCP transmission
	@$(AWK) 'BEGIN { print "" |& "/inet/tcp/0/127.0.0.1/13"; \
	"/inet/tcp/0/127.0.0.1/13" |& getline; print $0}'

redfilnm::
	@echo $@
	@$(AWK) -f $(srcdir)/redfilnm.awk srcdir=$(srcdir) $(srcdir)/redfilnm.in >_$@
	@-$(CMP) $(srcdir)/redfilnm.ok _$@ && rm -f _$@

leaddig::
	@echo $@
	@$(AWK) -v x=2E  -f $(srcdir)/leaddig.awk >_$@
	@-$(CMP) $(srcdir)/leaddig.ok _$@ && rm -f _$@

gsubtst3::
	@echo $@
	@$(AWK) --re-interval -f $(srcdir)/$@.awk $(srcdir)/$@.in >_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

space::
	@echo $@
	@echo 'Expect space to fail in DOS.'
	@$(AWK) -f ' ' $(srcdir)/space.awk >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

printf0::
	@echo $@
	@$(AWK) --posix -f $(srcdir)/$@.awk >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@
Gt-dummy:
# file Maketests, generated from Makefile.am by the Gentests program
addcomma:
	@echo addcomma
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

anchgsub:
	@echo anchgsub
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

arrayparm:
	@echo arrayparm
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

arrayref:
	@echo arrayref
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

arrymem1:
	@echo arrymem1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

arynasty:
	@echo arynasty
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

arysubnm:
	@echo arysubnm
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

asgext:
	@echo asgext
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

back89:
	@echo back89
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

backgsub:
	@echo backgsub
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

childin:
	@echo childin
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

clsflnam:
	@echo clsflnam
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

compare2:
	@echo compare2
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

concat1:
	@echo concat1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

convfmt:
	@echo convfmt
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

datanonl:
	@echo datanonl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

defref:
	@echo defref
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  --lint >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

delarprm:
	@echo delarprm
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

dynlj:
	@echo dynlj
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

eofsplit:
	@echo eofsplit
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fldchg:
	@echo fldchg
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fldchgnf:
	@echo fldchgnf
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fnamedat:
	@echo fnamedat
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fnarray:
	@echo fnarray
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fnarydel:
	@echo fnarydel
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fnaryscl:
	@echo fnaryscl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fnasgnm:
	@echo fnasgnm
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fnmisc:
	@echo fnmisc
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fnparydl:
	@echo fnparydl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

forsimp:
	@echo forsimp
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fsbs:
	@echo fsbs
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fsrs:
	@echo fsrs
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fstabplus:
	@echo fstabplus
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

funsemnl:
	@echo funsemnl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

funsmnam:
	@echo funsmnam
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

funstack:
	@echo funstack
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

getline:
	@echo getline
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

getline3:
	@echo getline3
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

getnr2tb:
	@echo getnr2tb
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

getnr2tm:
	@echo getnr2tm
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

gsubasgn:
	@echo gsubasgn
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

gsubtest:
	@echo gsubtest
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

gsubtst2:
	@echo gsubtst2
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

gsubtst4:
	@echo gsubtst4
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

hsprint:
	@echo hsprint
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

inputred:
	@echo inputred
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

intest:
	@echo intest
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

intprec:
	@echo intprec
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

leadnl:
	@echo leadnl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

longsub:
	@echo longsub
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

longwrds:
	@echo longwrds
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

math:
	@echo math
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

membug1:
	@echo membug1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

minusstr:
	@echo minusstr
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nasty:
	@echo nasty
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nasty2:
	@echo nasty2
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

negexp:
	@echo negexp
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nfldstr:
	@echo nfldstr
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nfneg:
	@echo nfneg
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nfset:
	@echo nfset
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nlfldsep:
	@echo nlfldsep
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nlinstr:
	@echo nlinstr
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nlstrina:
	@echo nlstrina
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

noeffect:
	@echo noeffect
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  --lint >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nofmtch:
	@echo nofmtch
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  --lint >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

noloop1:
	@echo noloop1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

noloop2:
	@echo noloop2
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

noparms:
	@echo noparms
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nulrsend:
	@echo nulrsend
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

numindex:
	@echo numindex
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

numsubstr:
	@echo numsubstr
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

octsub:
	@echo octsub
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

ofmt:
	@echo ofmt
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

ofmtbig:
	@echo ofmtbig
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

ofmtfidl:
	@echo ofmtfidl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

ofmts:
	@echo ofmts
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

onlynl:
	@echo onlynl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

opasnidx:
	@echo opasnidx
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

opasnslf:
	@echo opasnslf
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

paramdup:
	@echo paramdup
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

paramtyp:
	@echo paramtyp
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

parseme:
	@echo parseme
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

pcntplus:
	@echo pcntplus
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

prdupval:
	@echo prdupval
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

prec:
	@echo prec
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

printf1:
	@echo printf1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

prmarscl:
	@echo prmarscl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

prmreuse:
	@echo prmreuse
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

prt1eval:
	@echo prt1eval
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

prtoeval:
	@echo prtoeval
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

psx96sub:
	@echo psx96sub
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rand:
	@echo rand
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rebt8b1:
	@echo rebt8b1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rebt8b2:
	@echo rebt8b2
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

regeq:
	@echo regeq
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

reindops:
	@echo reindops
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

reparse:
	@echo reparse
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

resplit:
	@echo resplit
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rs:
	@echo rs
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rsnul1nl:
	@echo rsnul1nl
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rstest1:
	@echo rstest1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rstest2:
	@echo rstest2
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rswhite:
	@echo rswhite
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

sclforin:
	@echo sclforin
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

sclifin:
	@echo sclifin
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

splitargv:
	@echo splitargv
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

splitarr:
	@echo splitarr
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

splitdef:
	@echo splitdef
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

splitvar:
	@echo splitvar
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

splitwht:
	@echo splitwht
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

sprintfc:
	@echo sprintfc
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

strtod:
	@echo strtod
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

subslash:
	@echo subslash
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

substr:
	@echo substr
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

swaplns:
	@echo swaplns
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

synerr1:
	@echo synerr1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

uninitialized:
	@echo uninitialized
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  --lint >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

uninit2:
	@echo uninit2
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  --lint >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

zeroe0:
	@echo zeroe0
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

zeroflag:
	@echo zeroflag
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

getlnhd:
	@echo getlnhd
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

asort:
	@echo asort
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

asorti:
	@echo asorti
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

clos1way:
	@echo clos1way
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@
	@echo Expect clos1way to fail with MSC and DJGPP because "|&" is not supported.

fieldwdth:
	@echo fieldwdth
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

fsfwfs:
	@echo fsfwfs
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

gensub:
	@echo gensub
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

gnuops2:
	@echo gnuops2
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

gnureops:
	@echo gnureops
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

icasefs:
	@echo icasefs
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

icasers:
	@echo icasers
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

igncdym:
	@echo igncdym
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

igncfs:
	@echo igncfs
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

ignrcase:
	@echo ignrcase
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

lint:
	@echo lint
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

match1:
	@echo match1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

nondec:
	@echo nondec
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

posix:
	@echo posix
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

procinfs:
	@echo procinfs
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

regx8bit:
	@echo regx8bit
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

rebuf:
	@echo rebuf
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  < $(srcdir)/$@.in >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

shadow:
	@echo shadow
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  --lint >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

sort1:
	@echo sort1
	@AWKPATH=$(srcdir) $(AWK) -f $@.awk  >_$@ 2>&1 || echo EXIT CODE: $$? >>_$@
	@-$(CMP) $(srcdir)/$@.ok _$@ && rm -f _$@

# end of file Maketests

# Targets generated for other tests:

$(srcdir)/Maketests: $(srcdir)/Makefile.am $(srcdir)/Gentests
	$(AWK) -f $(srcdir)/Gentests "$<" *.awk *.in > $(srcdir)/Maketests

clean:
	rm -fr _* core junk out1 out2 out3 strftime.ok test1 test2 seq *~

# An attempt to print something that can be grepped for in build logs
pass-fail:
	@COUNT=`ls _* 2>/dev/null | wc -l` ; \
	if test $$COUNT = 0 ; \
	then	echo ALL TESTS PASSED ; \
	else	echo $$COUNT TESTS FAILED ; \
	fi

# This target for my convenience to look at all the results
diffout:
	for i in _* ; \
	do  \
		echo ============== $$i ============= ; \
		diff -c $${i#_}.ok  $$i ; \
	done | more

# This target is for testing with electric fence.
efence:
	for i in $$(ls _* | sed 's;_\(.*\);\1;') ; \
	do \
		bad=$$(wc -l < _$$i) \
		ok=$$(wc -l < $$i.ok) ; \
		if (( $$bad == $$ok + 2 )) ; \
		then \
			rm _$$i ; \
		fi ; \
	done
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
