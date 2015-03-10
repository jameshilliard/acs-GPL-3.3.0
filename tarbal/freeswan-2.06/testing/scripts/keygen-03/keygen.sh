: ==== start ====
ipsec rsasigkey --help

ipsec rsasigkey --verbose --random=/testing/scripts/keygen-03/devrandom.bin 2192
ipsec rsasigkey --verbose --random=/dev/zero 2192
: ==== end ====