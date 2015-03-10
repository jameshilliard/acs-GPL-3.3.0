#!/bin/sh

TEST_TYPE=klipstest

TESTNAME=east-dns-01
TESTHOST=east

EXITONEMPTY=--exitonempty

#PRIV_INPUT=../inputs/01-sunrise-sunset-ping.pcap

REF_PUB_OUTPUT=spi1-output.txt
REF_PUB_FILTER=../fixups/no-arp-pcap.pl

REF_CONSOLE_OUTPUT=spi1-console.txt
REF_CONSOLE_FIXUPS="kern-list-fixups.sed nocr.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS cutout.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS klips-spi-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS klips-spi-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS klips-debug-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS ipsec-look-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS host-dig-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS host-ping-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS pfkey-sanitize.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS east-prompt-splitline.pl"

TCPDUMPFLAGS="-n"

INIT_SCRIPT=spi1.sh

FINAL_SCRIPT=pfkeyhalt.sh

ADDITIONAL_HOSTS="nic"


