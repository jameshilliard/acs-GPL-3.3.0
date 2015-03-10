/*
 *  Definitions for the SCOOP interface found on various Sharp PDAs
 *
 *  Copyright (c) 2004 Richard Purdie
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 */

#define SCOOP_MCR  0x00
#define SCOOP_CDR  0x04
#define SCOOP_CSR  0x08
#define SCOOP_CPR  0x0C
#define SCOOP_CCR  0x10
#define SCOOP_IRR  0x14
#define SCOOP_IRM  0x14
#define SCOOP_IMR  0x18
#define SCOOP_ISR  0x1C
#define SCOOP_GPCR 0x20
#define SCOOP_GPWR 0x24
#define SCOOP_GPRR 0x28

#define SCOOP_GPCR_PA22	( 1 << 12 )
#define SCOOP_GPCR_PA21	( 1 << 11 )
#define SCOOP_GPCR_PA20	( 1 << 10 )
#define SCOOP_GPCR_PA19	( 1 << 9 )
#define SCOOP_GPCR_PA18	( 1 << 8 )
#define SCOOP_GPCR_PA17	( 1 << 7 )
#define SCOOP_GPCR_PA16	( 1 << 6 )
#define SCOOP_GPCR_PA15	( 1 << 5 )
#define SCOOP_GPCR_PA14	( 1 << 4 )
#define SCOOP_GPCR_PA13	( 1 << 3 )
#define SCOOP_GPCR_PA12	( 1 << 2 )
#define SCOOP_GPCR_PA11	( 1 << 1 )

struct scoop_config {
	unsigned short io_out;
	unsigned short io_dir;
};

void reset_scoop(void);
unsigned short set_scoop_gpio(unsigned short bit);
unsigned short reset_scoop_gpio(unsigned short bit);
unsigned short read_scoop_reg(unsigned short reg);
void write_scoop_reg(unsigned short reg, unsigned short data);
