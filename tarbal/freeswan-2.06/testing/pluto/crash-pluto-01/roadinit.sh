#!/bin/sh

rndc stop
named
sleep 2

/testing/pluto/bin/look-for-key road.uml.freeswan.org AQNxbOBmD

netstat -rn

route add -net default gw 192.1.3.254
ipsec setup start

/testing/pluto/oe-road-01/policy-wait.sh

echo done


