#!/bin/sh

TEST_PURPOSE=regress
TEST_PROB_REPORT=0
TEST_TYPE=ctltest

TESTNAME=keygen-02
TESTHOST=east

REF_CONSOLE_OUTPUT=console.txt
REF_CONSOLE_FIXUPS="script-only.sed nocr.sed"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS east-prompt-splitline.pl"
REF_CONSOLE_FIXUPS="$REF_CONSOLE_FIXUPS datekey-sanitize.sed"

INIT_SCRIPT=keygen.sh







