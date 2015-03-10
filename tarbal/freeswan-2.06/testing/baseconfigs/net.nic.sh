#
# $Id: net.nic.sh,v 1.3 2003/04/10 16:45:15 mcr Exp $
#
if [ -n "$UML_public_CTL" ]
then
    net_eth0="eth0=daemon,10:00:00:de:ad:ba,unix,$UML_public_CTL,$UML_public_DATA";
else
    net_eth0="eth0=mcast,10:00:00:de:ad:ba,239.192.1.2,31200";
fi

if [ -n "$UML_northpublic_CTL" ]
then
    net_eth1="eth1=daemon,10:00:00:32:64:ba,unix,$UML_northpublic_CTL,$UML_northpublic_DATA";
else
    net_eth1="eth1=mcast,10:00:00:32:64:ba,239.192.3.2,31210";
fi

net="$net_eth0 $net_eth1"




