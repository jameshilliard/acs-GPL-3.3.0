#!/usr/bin/perl

while(<>) {
	next if(/^arp who-has/);
	next if(/^arp reply/);

	print;
}


