#!/bin/sh

TEST_TYPE=klipstest
TESTNAME=east-reject-01
TESTHOST=east
EXITONEMPTY=--exitonempty
PRIVINPUT=../inputs/01-sunrise-sunset-ping.pcap

REF_PUB_OUTPUT=spi1-output.txt

REF_PRIV_OUTPUT=icmp-output.txt
REF_PRIV_FILTER=../../klips/fixups/one-to-eight-icmp.pl

REFCONSOLEOUTPUT=spi1-console.txt
REF_CONSOLE_FIXUPS="kern-list-fixups.sed nocr.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS klips-spi-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS ipsec-look-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS east-prompt-splitline.pl"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS klips-debug-sanitize.sed"
TCPDUMPFLAGS="-n"
INIT_SCRIPT=spi1.sh



