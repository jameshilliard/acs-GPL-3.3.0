#!/bin/sh

# this script is used by "road" UMLs that want to have per-test
# configuration files, and will be hitting sunrise-oe to test with.

mkdir -p /tmp/$TESTNAME
cp /testing/pluto/$TESTNAME/road.conf /tmp/$TESTNAME/ipsec.conf
cp /etc/ipsec.secrets                    /tmp/$TESTNAME

mkdir -p /tmp/$TESTNAME/ipsec.d/policies
cp /etc/ipsec.d/policies/* /tmp/$TESTNAME/ipsec.d/policies

: make sure that target is in policy private!
echo 192.0.2.2/32	>>/tmp/$TESTNAME/ipsec.d/policies/private

IPSEC_CONFS=/tmp/$TESTNAME export IPSEC_CONFS
