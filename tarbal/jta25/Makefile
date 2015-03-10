# This file is part of "The Java Telnet Application".
#
# (c) Matthias L. Jugel, Marcus Meißner 1996-2002. All Rights Reserved.
#
# Please visit http://javatelnet.org/ for updates and contact.
#
# --LICENSE NOTICE--
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# --LICENSE NOTICE--
#

JAVA	=	$(JAVA_DIR)/bin/java
JAR	=	$(JAVA_DIR)/bin/jar
JAVAC	=	$(JAVA_DIR)/bin/javac
JARSIGNER =	$(JAVA_DIR)/bin/jarsigner
KEYTOOL =	$(JAVA_DIR)/bin/keytool
JAVADOC =	$(JAVA_DIR)/bin/javadoc
JAVAH 	=	$(JAVA_DIR)/bin/javah
#DEBUG	=	-g -deprecation
DEBUG	=	-g
#JFLAGS	=	-classpath $(CLASSPATH):.
JFLAGS = -classpath $(CLASSPATH):jar/org.apache.crimson.jar:$(JDOM_JAR):$(GNUEXP_JAR):.

SRCDIR	=	de
PKGNAME	=	jta25
VERSION	=	`java -version 2>&1 | head -1 | \
		 sed 's/^java version //' | sed 's/"//g'`
DATE	:=	$(shell date +%Y%m%d-%T)

.SUFFIXES:	.java .class

# compile java files to class
.java.class:
	$(JAVAC) $(DEBUG) $(JFLAGS) $<

#
# major rules to create files
#
all: 	app jsch cont jar tools/mrelayd tools/relayd

run:	app
	$(JAVA) $(JFLAGS) de.mud.jta.Main

doc:	app
	@echo Creating source documentation ...
	@if [ ! -d doc ]; then mkdir doc; fi
	@-rm -r doc/source/*.html doc/source/de
	@$(JAVADOC) -d doc/source -version -author \
	  -sourcepath $(CLASSPATH):.:contrib \
	  `(find de -name \*.java -printf '%h\n'; \
	    (cd contrib; find * -name \*.java -printf '%h\n')) | \
	    sort | uniq | \
	    grep -v CVS | sed 's/\//./g'`; > /dev/null
	@echo Source documentation done.

tex:	app
	@echo Creating latex source documentation ...
	@$(JAVADOC) -docletpath ../tex -doclet TexDoclet \
	  -output $(PKGNAME).tex \
	  -docdir de/mud/jta `find de/mud -type d -print | \
	    grep -v CVS | grep -v '^de/mud$$' | sed 's/\//./g'`; > /dev/null
	@echo Source documentation done.

jsch:
	@rm -rf com/jcraft/jsch
	@cp -a jsch-0.1.14_mod/src/com/jcraft/jsch/ com/jcraft/jsch
	@find com -name \*.java | sed 's/java$$/class/' | xargs make
	@echo Done.

jar:	app build
	@echo Creating binary archive ...
	@if [ ! -d jar ]; then mkdir jar; fi
	@cp contrib/de/mud/jta/plugin/Jsch*class $(SRCDIR)/mud/jta/plugin
	@touch "Created-$(DATE)"
	@$(JAR) cvfm jar/$(PKGNAME).jar jta.manifest \
	  "Created-$(DATE)" readme \
	  license/copying \
	  de/mud/ssh/license.txt \
	  `find $(SRCDIR) -name \*.class` \
	  `find $(SRCDIR) -name \*.conf` \
	  `find com -name \*.class` > /dev/null
	@rm -f Created-*
	@echo Created jar/$(PKGNAME).jar

opt:
	@[ -f ../Jopt.jar ] && $(JAVA) -jar ../Jopt.jar jar/$(PKGNAME).jar
	@echo Created jar/$(PKGNAME)_o.jar

cont:
	@echo Compiling contributed software ...
	@(cd contrib; make)
	@cp contrib/de/mud/jta/plugin/MyUserInfo.class $(SRCDIR)/mud/jta/plugin
	@cp contrib/de/mud/jta/plugin/keysearch.class $(SRCDIR)/mud/jta/plugin

dist:	all opt revision changes
	@echo Creating distribution package ...
	@if [ "$(CVSROOT)" = "" ]; then echo "Missing CVSROOT!"; exit -1; fi
	@(cvs -Q -d $(CVSROOT) export -D now -d $(PKGNAME) jta && \
	  export CLASSPATH=$(CLASSPATH):`pwd`/jar/cryptix.jar && \
	  cp REVISION CHANGES $(PKGNAME)/ && \
	  cp -r doc/source $(PKGNAME)/doc/ && \
	  touch "$(PKGNAME)/Created-$(DATE)" && \
	  sed "s/<!-- DATE -->/$(DATE)/g" < $(PKGNAME)/index.html \
	                                  > $(PKGNAME)/index.new && \
	  mv $(PKGNAME)/index.new $(PKGNAME)/index.html && \
	  echo "s/<!-- DATE -->/$(DATE)/g" > /tmp/jta.sed && \
	  (cd $(PKGNAME); make cont; \
	   rm -r contrib/de/mud/bsx contrib/de/mud/jta/plugin/BSX*; \
	   $(JAR) cvMf ../jar/contrib.jar contrib) &&\
	  (cd jar; for i in *.jar; do \
	    echo 's/<!-- SIZE-'$$i' -->/'`\ls -l $$i | \
	          awk '{printf("%dk", $$5/1024);}' `'/g'; \
	  done) >> /tmp/jta.sed && \
	  sed -f /tmp/jta.sed < $(PKGNAME)/html/download.html \
	                      > $(PKGNAME)/html/download.new && \
	  rm -f /tmp/jta.sed && \
	  mv $(PKGNAME)/html/download.new $(PKGNAME)/html/download.html && \
	  bin/users.pl > $(PKGNAME)/html/users.html.new && \
	  mv $(PKGNAME)/html/users.html.new $(PKGNAME)/html/users.html && \
	  rm -r $(PKGNAME)/tools/* \
	        $(PKGNAME)/html/users.db \
	        $(PKGNAME)/bin/users.pl && \
          (cd $(PKGNAME); $(MAKE) clean) && \
	  $(JAR) cvMf jar/$(PKGNAME)-src.jar $(PKGNAME) \
	 ) > /dev/null 
	@rm -rf $(PKGNAME) 
	@echo Created jar/$(PKGNAME)-src.jar
	@$(JAR) cvMf jar/relay.jar \
	  tools/relayd tools/mrelayd tools/*.c tools/*.exe

build:
	@echo "package de.mud.jta; public class Build implements Version { public String getDate() { return \"$(DATE)\"; } }" > de/mud/jta/Build.java

changes:
	@rcs2log > CHANGES
	@echo Created CHANGES.

revision:
	@find de -name \*.java | xargs cat | grep @version | \
	  awk 'BEGIN{ \
	         printf("%-26.26s %2.2s.%-2.2s (%10s) %s\n", \
		        "File","R","M", "Date", "Last Accessed by:"); \
	       } \
	       { \
	         split($$5,rev,"."); \
	         printf("%-26.26s %2.2s.%-2.2s (%10s) %s\n", \
		   $$4,rev[1],rev[2],$$6,$$8); \
	       }' \
	  > REVISION
	  @echo Created REVISION.

small: app
	jar cvf jar/jta20-small.jar \
		license/readme \
		de/mud/jta/SmallApplet*class \
		de/mud/telnet/TelnetProtocolHandler.class \
		de/mud/terminal/VDU*class \
		de/mud/terminal/vt320*class \
		de/mud/terminal/SoftFont.class

# 
# application dependencies
#
app:
	@find $(SRCDIR) -name \*.java | sed 's/java$$/class/' | xargs make
	@echo Done.

clean:
	-find . -name \*.class -print | xargs rm > /dev/null 2>&1
	-find . -name \*~ -print | xargs rm > /dev/null 2>&1
	-rm -f tools/relayd tools/mrelayd
	cd jni && $(MAKE) clean

realclean: clean
	-rm -f jar/$(PKGNAME).jar $(PKGNAME).jar jar/$(PKGNAME)-src.jar
	cd jni && $(MAKE) realclean
