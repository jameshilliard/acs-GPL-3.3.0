/*
 * count number of 0-bits and 1-bits in a buffer, returning difference.
 * Copyright (C) 2003  Michael Richardson <mcr@freeswan.org>
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/lgpl.txt>.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * RCSID $Id: parityofbuffer.c,v 1.2 2004/01/17 02:43:22 dhr Exp $
 */

#include <sys/types.h>

#include "freeswan.h"

static signed char byteparity[256];
static int inited = 0;

static void
initbyteparity(void)
{
    unsigned int b;

    for(b=0; b<256; b++) {
	unsigned int ones  = 0;
	unsigned int bit;

	for(bit = 0x80; bit != 0; bit >>= 1)
	    if(bit & b)
		ones++;

	byteparity[b] = ones - (8-ones);
    }

    inited = 1;
}
		
int parityofbuffer(const unsigned char *buf, size_t len)
{
    signed int sum;

    if(!inited) {
      initbyteparity();
    }

    sum = 0;
    while(len > 0) {
	sum += byteparity[*buf];
	buf++;
	len--;
    }

    return sum;
}
	

#ifdef PARITYOFBUFFER_MAIN
#include <unistd.h>

static void regress(void);

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s -r\n", argv[0]);
		exit(2);
	}

	if (strcmp(argv[1], "-r") == 0) {
		regress();
		fprintf(stderr, "regress() returned?!?\n");
		exit(1);
	}
	exit(0);
}

unsigned char onebuf[]={0x01,0x01,0x01,0x01,0x01,0x00};
unsigned char twobuf[]={0xff,0xff,0xff,0xff,0xff,0x00};

static struct rtab {
	char *input;
	int   output;			/* NULL means error expected */
} rtab[] = {
  {onebuf, -30},
  {twobuf, 40},
  {"the nice frog jump over a red log",	-26},
  {"mnBtkrZVGelXnsdtGAygtrWe/J8=",	  4},
  {NULL,				-1}
};

static void regress(void)
{
	struct rtab *r;
	int  count, status, sum;

	status = 0;
	count  = 1;

	for (r = rtab; r->input != NULL; r++) {
	  sum = parityofbuffer(r->input, strlen(r->input));

	  if(sum != r->output) {
	    printf("case %d failed; computed %d vs expected %d\n",
		   count, sum, r->output);
	    status = 1;
	  }
	  count++;
	}
	exit(status);
}

#endif /* PARITYOFBUFFER_MAIN */
