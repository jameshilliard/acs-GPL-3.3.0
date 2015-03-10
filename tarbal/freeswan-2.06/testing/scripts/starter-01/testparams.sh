#!/bin/sh

TEST_PURPOSE=goal
TEST_PROB_REPORT=0
TEST_TYPE=ctltest

TESTNAME=starter-01
TESTHOST=east

REF_CONSOLE_OUTPUT=east-console.txt

REF_CONSOLE_FIXUPS="nocr.sed script-only.sed ipsec-setup-sanitize.sed pluto-log-sanitize.sed east-prompt-splitline.pl "

INIT_SCRIPT=start01.sh
