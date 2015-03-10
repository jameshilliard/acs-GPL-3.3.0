/*

    Subroutine: Utility routines to get XWindows access through shared memory.
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


#include "xutil.h"

int setup_xwinbuffer(struct xwinbuffer *xwin,char *prgname, int width, int height, int depth) {
  int screen;
  XWindowAttributes win_attributes;
  XVisualInfo desired_visual_template;
  XVisualInfo *visual_info_list; 
  int no_visual_matched;
  XGCValues gc_values;
  int localdepth;

  /* Store parameters */
  xwin->width=width;
  xwin->height=height;
  xwin->depth=depth;
  
  /* Open window */
  xwin->display_ = XOpenDisplay(NULL);
  xwin->wtwin_ = XCreateSimpleWindow(xwin->display_, DefaultRootWindow(xwin->display_),
			       0, 0, xwin->width, xwin->height, xwin->depth,
			       BlackPixel(xwin->display_, DefaultScreen(xwin->display_)),
			       BlackPixel(xwin->display_, DefaultScreen(xwin->display_)));  
  XMapWindow(xwin->display_, xwin->wtwin_);
  XSelectInput(xwin->display_, xwin->wtwin_, KeyPressMask | KeyReleaseMask | ButtonPressMask);
  gc_values.graphics_exposures = False;
  xwin->gc_ = XCreateGC(xwin->display_, xwin->wtwin_, GCGraphicsExposures, &gc_values);  

  /* Create shared memory segment for direct framebuffer access */
  screen = DefaultScreen(xwin->display_);
  XGetWindowAttributes(xwin->display_, xwin->wtwin_, &win_attributes);
  localdepth=win_attributes.depth;
  xwin->shminfo = (XShmSegmentInfo*) malloc (sizeof(XShmSegmentInfo));
  xwin->shmimage = XShmCreateImage(xwin->display_, DefaultVisual(xwin->display_, screen),
			     localdepth, ZPixmap, NULL, xwin->shminfo,
			     xwin->width,xwin->height);
  if ((xwin->shminfo->shmid=shmget(getpid(),
			  xwin->shmimage->bytes_per_line * xwin->shmimage->height,
			  IPC_CREAT | IPC_EXCL | 0777))==-1) {
   perror("shmget()");
   return(-1);
  } 			  
  xwin->shminfo->shmaddr = (char *)shmat(xwin->shminfo->shmid, (void *) 0, 0);
  xwin->shmimage->data = xwin->shminfo->shmaddr;
  XShmAttach(xwin->display_, xwin->shminfo);

  /* Set color mapping */
  desired_visual_template.screen = screen;
  desired_visual_template.class = TrueColor;
  visual_info_list = XGetVisualInfo(xwin->display_, 
				    VisualScreenMask | VisualClassMask,
				    &desired_visual_template,
				    &no_visual_matched);
  if (no_visual_matched == 0) {
   fprintf(stderr, "No visual matched\n");
   return(-1);
  }
  
  return(0);  
}

int sync_xwinbuffer(struct xwinbuffer *xwin)
{
 /* Draw screen onto display */
 XShmPutImage(xwin->display_, xwin->wtwin_, xwin->gc_, xwin->shmimage,
 	        0, 0, 0, 0, xwin->width, xwin->height, False);
 XSync(xwin->display_, 0); 
 return(0);
}

int keypoll_xwinbuffer (struct xwinbuffer *xwin, KeySym *key, int *button, int *mousex, int *mousey)
{
  XEvent event; 
 
  /* Check keyboard/mouse input */
  if (XPending(xwin->display_)>0) {
   *button=0;
   *key=0;
   XNextEvent(xwin->display_, &event);
   switch (event.type) {
    case ButtonPress:
     *mousex=event.xbutton.x;
     *mousey=event.xbutton.y;
     *button=1;
     break;
    case KeyPress:
     *key=XLookupKeysym((void *)&event,0);
     break;
    case MappingNotify:
     XRefreshKeyboardMapping((void  *)&event);
     break;
   }
   return(1);
  } else { 
   return(0);
  } 
}  

int close_xwinbuffer (struct xwinbuffer *xwin)
{
 /* Mark Shm as destroyed */
 shmctl(xwin->shminfo->shmid,IPC_RMID,NULL);

 /* Detatch shared memory segment */
 shmdt ((char *)xwin->shminfo->shmaddr);
 
 return(0);
}
