/*

    Program: testgrab - testprogram for libgrab.
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
 * NOTE: Requires XWindows running at 16bit depth 565 weight.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bgrab.h"
#include "xutil.h"

#define WIDTH	512
#define HEIGHT	384
#define DEPTH 	2

int main (int argc, char *argv[])
{
 struct fgdevice fg;
 struct xwinbuffer xwin;
 int not_first_time=0;
 int in_loop=1;
 KeySym key;
 int button;
 int mousex,mousey;
 int contrast;
 int brightness;
 
 fprintf (stderr,"testgrab - libbgrab testprogram: display 1000 frames.\n\n");
  
 if (setup_xwinbuffer (&xwin, argv[0], WIDTH, HEIGHT, DEPTH)!=0) exit(-1);
   
 if (argc>1) {
  if (fg_open_device (&fg, argv[1])!=0) exit(-1); 	
 } else {
  if (fg_open_device (&fg, "/dev/video")!=0) exit(-1); 	
 }
 
 if (fg_print_info (&fg)!=0) exit(-1);

 if (fg_set_channel (&fg,1)!=0) exit(-1);
 
 if (fg_start_grab_image(&fg, WIDTH, HEIGHT, FORMAT_RGB565)!=0) exit(-1);

 fg_set_fps_interval(&fg,10);
 
 /* View loop */
 while (in_loop) { 

  /* Update framebuffer with new image */
  if (fg_get_next_image(&fg)!=NULL) {

   /* Copy image to screenbuffer */
   memcpy((char *)xwin.shmimage->data,(char *)fg.current_image,fg.image_size);
   
   /* Draw it */
   sync_xwinbuffer(&xwin);
  }

  /* Poll keyboard */
  if (keypoll_xwinbuffer (&xwin, &key, &button, &mousex, &mousey)) {
   switch (key) {
    case XK_q:
     in_loop=0;
     break; 
    case XK_c:
     contrast=fg_get_setting(&fg,SETTING_CONTRAST)-64;
     fprintf (stderr,"Contrast set to %i\n",contrast);
     fg_set_setting(&fg,SETTING_CONTRAST,contrast);
     break; 
    case XK_v:
     contrast=fg_get_setting(&fg,SETTING_CONTRAST)+64;
     fprintf (stderr,"Contrast set to %i\n",contrast);
     fg_set_setting(&fg,SETTING_CONTRAST,contrast);
     break; 
    case XK_b:
     brightness=fg_get_setting(&fg,SETTING_BRIGHTNESS)-64;
     fprintf (stderr,"Brightness set to %i\n",brightness);
     fg_set_setting(&fg,SETTING_BRIGHTNESS,brightness);
     break; 
    case XK_n:
     brightness=fg_get_setting(&fg,SETTING_BRIGHTNESS)+64;
     fprintf (stderr,"Brightness set to %i\n",brightness);
     fg_set_setting(&fg,SETTING_BRIGHTNESS,brightness);
     break; 
   }
  }
  
  /* Print fps */
  if (fg.framecount==0) {
   if (not_first_time) {
    printf ("FPS=%7.2f\n",fg_get_fps(&fg));
   } else {
    not_first_time=1; 
   } 
  } 
 }           

 if (close_xwinbuffer(&xwin)!=0) exit(-1);
 
 if (fg_stop_grab_image(&fg)!=0) exit(-1); 	

 if (fg_close_device (&fg)!=0) exit(-1); 	
 
 exit(0);
}
