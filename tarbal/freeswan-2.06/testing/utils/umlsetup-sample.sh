#!/bin/sh

# This is the configuration file that helps setup for
# a kernel pool for UML compilation w/FreeSWAN.
#
# Copy this file to the top of your FreeSWAN source directory as
# umlsetup.sh, edit that copy, and populate the paths.


# space for everything:
# Just a shorthand for the following definitions.
# Can be eliminated if available space is fragmented.
UMLPREFIX=?/uml

# set this to someplace with at least 100Mb free.
POOLSPACE=$UMLPREFIX/umlbuild

# Set this to original kernel source.
# It will not be modified.
# Could be native build:
#KERNPOOL=/usr/src/linux
#
# or something you downloaded.
KERNPOOL=$UMLPREFIX/kernel/linux-2.4.19/linux

# For make check's kernel patch tests, virgin kernel sources are needed
# They will not be modified.
#KERNEL_LINUS2_0_SRC=
#KERNEL_LINUS2_2_SRC=
KERNEL_LINUS2_4_SRC=$KERNPOOL

# set this to the UML tar file, gotten from, e.g.
#     http://ftp.nl.linux.org/uml/uml-patch-2.4.18-53.bz2
#
UMLPATCH=$UMLPREFIX/download/uml-patch-2.4.19-47.bz2

#
# set this to "" (or unset) to get the automatic non-interactive patch for 
# the 2.4 kernels. 2.6 does not require it, set to "none"
#
#NONINTPATCH=none

# this symbol with be added to various file names to get the 2.6
# version. 2.0, 2.2 and 2.4 should use ""
#
#KERNVER=26



# set BASICROOT this to an unpacked copy of the root file system you
# want to use.
#
# a small-ish one is at:
#     http://www.sandelman.ottawa.on.ca/freeswan/uml/
#
# umlfreeroot-5.1.tar.gz  is 17Mb, unpacks to around 50Mb.
#
# umlfreesharemini.tar.gz is 3Mb, unpacks to around 8Mb.
# umlfreeshareall.tar.gz is 6Mb, unpacks to around 26Mb.
#
# I did
#   mkdir -p $UMLPREFIX/basic-root
#   cd $UMLPREFIX/basic-root
#   nftp -o - http://www.sandelman.ottawa.on.ca/freeswan/uml/umlfreeroot-12.0.tar.gz | tar xzvf -
#  (or ncftp, or whatever your favorite program is)
#
# There is an advantage to having this on the same partition as
# $POOLSPACE, as hard links can be used.
#
BASICROOT=$UMLPREFIX/basic-root/root-15.1

# the mini /usr/share has Canada zoneinfo and "en" locale only.
# the all one has everything from the original UML debian root.
# I run debian, so I can just use my native /usr/share!
SHAREDIR=/usr/share

# note that "nic" and "sec" are the same host in different configurations
REGULARHOSTS='sunrise sunset nic carrot beet sec'
FREESWANHOSTS='east west japan road'

# tell system location of special tcpdump, if any
#export TCPDUMP="/usr/local/sbin/tcpdump"

# comment this out to signify that you've customized this script
echo "please create a umlsetup.sh" ; exit 99
