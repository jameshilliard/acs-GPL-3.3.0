/*

    Program: asciigrab - ASCII output of live video
    Copyright (C) 1999  A. Schiffler

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    To contact the author try these:
    Andreas Schiffler, 
    ZKM, Lorezstr. 19, 76135 Karlsruhe, Germany
    schiffler@zkm.de

*/

/*
 * Note: Requires XWindows running at 16bit depth 565 weight.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bgrab.h"
#include "xutil.h"

#define WIDTH	96
#define HEIGHT	64
#define DEPTH 	2

static int reverse_ascii;

/* Gray2ASCII convertion table */

char *intens[16] = {
    "   ...,,,vvuuwww",
    "   ...,,,vvuuwww",
    "  ..,:,,,vvvuuww",
    " ..,:::,;;vvvuww",
    "'',::::o;;;;nnww",
    "''::::ooo;;;nnmw",
    "'''::ooooo;yynmm",
    "''''oooooOOyyymm",
    "^^YYYoooOOOOyyyy",
    "^^^YYYoOOOO%%yyy",
    "^^^^YYYOOO%%%%yy",
    "\"^^^YYYYO%%%$$$y",
    "\"\"^PPPRRR%%$$$$#",
    "\"\"\"PPPPRRR$$$$##",
    "\"\"\"\"PPPPRRR$$###",
    "\"\"\"\"\"PPPPRRR####",
   };

int main (int argc, char *argv[])
{
 struct fgdevice fg;
 struct xwinbuffer xwin;
 unsigned short *curpos, *curpos2;
 int i,r,g,b,x,y;
 char aline[1024], *curchar;
 KeySym key;
 int button;
 int mousex,mousey;
 int in_loop=1;
 int contrast, brightness;
 
 fprintf (stderr,"asciigrab\n\n");
  
 if (setup_xwinbuffer (&xwin, argv[0], WIDTH, HEIGHT, DEPTH)!=0) exit(-1);
   
 if (fg_open_device (&fg, argv[1])!=0) exit(-1); 	

 if (fg_print_info (&fg)!=0) exit(-1);

 if (fg_set_channel (&fg,1)!=0) exit(-1);
 
 if (fg_start_grab_image(&fg, WIDTH, HEIGHT, FORMAT_RGB565)!=0) exit(-1);

 while (in_loop) { 

  /* Update framebuffer with new image */
  if (fg_get_next_image(&fg)!=NULL) {

   /* Copy image to screenbuffer */
   memcpy((char *)xwin.shmimage->data,(char *)fg.current_image,fg.image_size);
   
   /* Draw it */
   sync_xwinbuffer(&xwin);

   if (keypoll_xwinbuffer (&xwin, &key, &button, &mousex, &mousey)!=0) {
     /* Mousebutton */
     if (button) {
     }
     /* Keyboard */
     switch (key) {
      case XK_q:
       in_loop=0;
       break; 
      case XK_c:
       contrast=fg_get_setting(&fg,SETTING_CONTRAST)-64;
       fg_set_setting(&fg,SETTING_CONTRAST,contrast);
       break; 
      case XK_v:
       contrast=fg_get_setting(&fg,SETTING_CONTRAST)+64;
       fg_set_setting(&fg,SETTING_CONTRAST,contrast);
       break; 
      case XK_b:
       brightness=fg_get_setting(&fg,SETTING_BRIGHTNESS)-64;
       fg_set_setting(&fg,SETTING_BRIGHTNESS,brightness);
       break; 
      case XK_n:
       brightness=fg_get_setting(&fg,SETTING_BRIGHTNESS)+64;
       fg_set_setting(&fg,SETTING_BRIGHTNESS,brightness);
       break; 
       }
   }

   /* Generate ASCII output */
  
   /* Clear screen */
   printf ("\033[H");

   /* Determine 4bit luminance from 565bit RGB in place */
   curpos=(unsigned short *)fg.current_image;
   if (reverse_ascii) {
    for (i=0; i<fg.image_pixels; i++) {
     r=(*curpos >> 11);
     g=((*curpos >> 5) & 63);
     b=(*curpos & 31);
     *curpos=15-(r*2450+g*2404+b*934)/16384;          
     curpos++;
    }
   } else {
    for (i=0; i<fg.image_pixels; i++) {
     r=(*curpos >> 11);
     g=((*curpos >> 5) & 63);
     b=(*curpos & 31);
     *curpos=(r*2450+g*2404+b*934)/16384;          
     curpos++;
    }
   }

   /* Output ASCII matrix from line pairs */
   curpos=(unsigned short *)fg.current_image;
   curpos2=curpos;
   curpos2+=fg.width;
   aline[fg.width]=0;
   for (y=0; y<(fg.height/2); y++) {
    curchar=aline;
    for (x=0; x<fg.width; x++) {
     *curchar=intens[*curpos][*curpos2];
     curchar++;
     curpos++;
     curpos2++;
    }
    curpos += fg.width;
    curpos2 += fg.width;
    printf ("%s\n",aline);
   } 

  }
  
 }           

 if (close_xwinbuffer(&xwin)!=0) exit(-1);
 
 if (fg_stop_grab_image(&fg)!=0) exit(-1); 	

 if (fg_close_device (&fg)!=0) exit(-1); 	
 
 exit(0);
}
