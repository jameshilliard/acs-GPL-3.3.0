#
# $Id: net.road.sh,v 1.2 2003/06/01 18:18:19 mcr Exp $
#
if [ -n "$UML_public_CTL" ]
then
    net_eth0="eth0=daemon,10:00:00:ab:cd:02,unix,$UML_public_CTL,$UML_public_DATA";
else
    net_eth0="eth0=mcast,10:00:00:96:96:A1,239.192.1.2,31200"
fi

net="$net_eth0"

