#!/bin/sh
TZ=GMT export TZ

dig @192.1.4.254 east.uml.freeswan.org. key
ping -c 2 -n 192.1.2.254

ipsec spi --clear
ipsec eroute --clear

#ipsec klipsdebug --set pfkey
ipsec klipsdebug --set tunnel-xmit
#ipsec klipsdebug --set verbose

ipsec eroute --add --eraf inet --src 0.0.0.0/0 --dst 0.0.0.0/0 --said %trap

ipsec tncfg --attach --virtual ipsec0 --physical eth1
ifconfig ipsec0 inet 192.1.2.23 netmask 0xffffff00 broadcast 192.1.2.255 up

ipsec look

# magic route command
route add -net 0.0.0.0   netmask 128.0.0.0 gw 192.1.2.254 dev ipsec0
route add -net 128.0.0.0 netmask 128.0.0.0 gw 192.1.2.254 dev ipsec0

# monitor upbound ACQUIRE messages
ipsec pf_key --daemon /var/run/pf_key.pid >/tmp/pfkey.txt

# do DNS requests
dig @192.1.4.254 east.uml.freeswan.org. key

# do DNS request with TCP
dig +tcp @192.1.4.254 east.uml.freeswan.org. key

# check it out
ipsec eroute



