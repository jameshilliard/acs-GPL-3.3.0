/*

    Program: zoomgrab - zoomed output of live video using GLIDE
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
 * NOTE 1.: Requires glide libraries and installed Voodoo card.
 * NOTE 2.: Edit zoomdef.txt.
 * NOTE 3.: Does not require X-Windows 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "bgrab.h"

#include <glide.h>
#include <linutil.h>

#include "spline.h"

#ifdef USEAUDIO
 #include "linux/soundcard.h"
#endif 

#define GRABWIDTH	512
#define GRABHEIGHT	512
#define DEPTH 		2
#define NUMTEXTURES	4
#define MAXPOINTS	1024
#define MAXLINE		128

int use_interpolation=0;

struct xyspline spline;

int main (int argc, char *argv[])
{
 FILE *infile;
 int i,y;
 struct fgdevice fg;
 GrHwConfiguration glidehw;
 GrVertex vtxA, vtxB, vtxC, vtxD;
 FxU32 textureSize, startAddress;
 GrTexInfo info[NUMTEXTURES];
 char subimage[NUMTEXTURES][GRABWIDTH*GRABHEIGHT*DEPTH/4];
 char *sourceptr[NUMTEXTURES];
 char *targetptr[NUMTEXTURES];
 char glideversion[80];
 float scale=0.0;
 double curtime,reftime,maxtime,diftime;
 int numpoints=0;
 float xp[MAXPOINTS];
 float yp[MAXPOINTS];
 char aline[MAXLINE];
 float value,result[2];
 float volume;
#ifdef USEAUDIO
 int mixfd;
 int vol;
#endif
         
 fprintf (stderr,"zoomgrab\n\n");

 /* Setup zoom spline */
 infile=fopen("zoomdef.txt","r");
 maxtime=0.0;
 fgets(aline,MAXLINE,infile);
 while ( (!feof(infile)) && (numpoints<MAXPOINTS)) {
  sscanf(aline,"%f %f",&xp[numpoints],&yp[numpoints]);
  numpoints++;
  fgets(aline,MAXLINE,infile);
 }
 fclose(infile); 
 spline=(struct xyspline)create_spline(numpoints,xp,yp);
 maxtime=(double)numpoints;
 
 /* Setup  framegrabber */
 if (fg_open_device (&fg, "/dev/video")!=0) exit(-1); 	
 if (fg_print_info (&fg)!=0) exit(-1);
 if (fg_set_channel (&fg,1)!=0) exit(-1);
 if (fg_start_grab_image(&fg, GRABWIDTH, GRABHEIGHT, FORMAT_RGB565)!=0) exit(-1);

#ifdef USEAUDIO
 /* Soundcard initialization */
 if ((mixfd=open("/dev/mixer",O_RDWR,0))==-1) {
  perror("open() mixer");
  exit(-1);
 }
#endif
 
 /* Glide initialization */
 grGlideGetVersion( glideversion );
 fprintf(stderr,"Glide - Version %s\n", glideversion );
 grGlideInit();

 /* Voodoo-Card initialization */
 assert(grSstQueryHardware(&glidehw));
 grSstSelect( 0 );
 
 /* Define textures */
 for (i=0; i<NUMTEXTURES; i++) {
  info[i].smallLod = GR_LOD_256;
  info[i].largeLod = GR_LOD_256;
  info[i].aspectRatio = GR_ASPECT_1x1;
  info[i].format = GR_TEXFMT_RGB_565;
  info[i].data = (FxU16 *)subimage[i];
 } 
 textureSize=grTexTextureMemRequired(GR_MIPMAPLEVELMASK_BOTH, &info[0]);
 startAddress = grTexMinAddress(GR_TMU0);

 /* Open screen */
 assert(grSstWinOpen(0, 
  GR_RESOLUTION_640x480, 
  GR_REFRESH_60Hz, 
  GR_COLORFORMAT_RGBA, 
  GR_ORIGIN_UPPER_LEFT,
  2,1));

  /* Program texture- and color-combine units */
  grTexCombine ( GR_TMU0, 
                  GR_COMBINE_FUNCTION_LOCAL, 
                  GR_COMBINE_FACTOR_NONE,
                  GR_COMBINE_FUNCTION_LOCAL, 
                  GR_COMBINE_FACTOR_NONE, 
                  FXFALSE, 
                  FXFALSE); 
  grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                   GR_COMBINE_FACTOR_ONE,
                   GR_COMBINE_LOCAL_NONE,
                   GR_COMBINE_OTHER_TEXTURE,
                   FXFALSE );
 
 if (use_interpolation) {
  /* Enable Bilinear Filtering + Mipmapping */
  grTexFilterMode( GR_TMU0,
                     GR_TEXTUREFILTER_BILINEAR,
                     GR_TEXTUREFILTER_BILINEAR );
  grTexMipMapMode( GR_TMU0,
                     GR_MIPMAP_NEAREST,
                     FXFALSE );
 }

 /* Rendering loop */
 reftime=timeGet();
 
 while ( !kbhit() ) { 

  /* Scale calculation */
  curtime=timeGet();
  diftime = curtime-reftime;
  if (diftime>maxtime) {
   reftime=curtime;
   curtime=0.0;
  }
  diftime *= (1.0/maxtime); 
  value=diftime;
  spline_value(spline,value,result);
  scale=result[0];
  volume=result[1];
     
  /* Clip scale */
  if (scale<0.0) scale=0.0;
  if (scale>250.0) scale=250.0;

  /* Clip volume */
  if (volume<0.0) volume=0.0;
  if (volume>100.0) volume=100.0;

#ifdef USEAUDIO
  /* Set volume */
  vol=(volume + (volume*256));
  if (ioctl(mixfd, SOUND_MIXER_WRITE_VOLUME, &vol) == -1) {
   perror ("ioctl(SOUND_MIXER_WRITE)");
  }
#endif  
  
  /* Update framebuffer with new image */
  if (fg_get_next_image(&fg)!=NULL) {
  
   /* Split image up into subimages */
   for (i=0; i<NUMTEXTURES; i++) {
    sourceptr[i]=fg.current_image;
    sourceptr[i] += DEPTH*( (i % 2)*(GRABWIDTH/2) + (i/2)*GRABWIDTH*(GRABHEIGHT/2) );
    targetptr[i]=subimage[i];
   }
   for (y=0; y<(GRABHEIGHT/2); y++) {
    for (i=0; i<NUMTEXTURES; i++) {
     memcpy(targetptr[i],sourceptr[i],(GRABWIDTH/2*DEPTH));
     targetptr[i] += (GRABWIDTH/2*DEPTH);
     sourceptr[i] += (GRABWIDTH*DEPTH);
    }
   }
    
   grBufferClear( 0, 0, GR_ZDEPTHVALUE_FARTHEST );

   for (i=0; i<NUMTEXTURES; i++) {
    grTexDownloadMipMap(GR_TMU0, startAddress+i*textureSize, GR_MIPMAPLEVELMASK_BOTH, &info[i]);
    grTexSource(GR_TMU0, startAddress+i*textureSize, GR_MIPMAPLEVELMASK_BOTH, &info[i]); 
       
    /*---- 
      A-B
      |\|
      C-D
     -----*/

    vtxA.oow = 1.0f;
    vtxB = vtxC = vtxD = vtxA;

    if (scale<=1.0) {

     /* Shrink image */
    
     switch (i) {
      case 0:
       vtxA.x = vtxC.x = (640/2)-scale*(640/2);
       vtxB.x = vtxD.x = (640/2);	
       vtxA.y = vtxB.y = (480/2)-scale*(480/2);
       vtxC.y = vtxD.y = (480/2);
       break;
      case 1:
       vtxA.x = vtxC.x = (640/2);
       vtxB.x = vtxD.x = (640/2)+scale*(640/2);	
       vtxA.y = vtxB.y = (480/2)-scale*(480/2);
       vtxC.y = vtxD.y = (480/2);
       break;
      case 2:
       vtxA.x = vtxC.x = (640/2)-scale*(640/2);
       vtxB.x = vtxD.x = (640/2);	
       vtxA.y = vtxB.y = (480/2);
       vtxC.y = vtxD.y = (480/2)+scale*(480/2);
       break;
      case 3:
       vtxA.x = vtxC.x = (640/2);
       vtxB.x = vtxD.x = (640/2)+scale*(640/2);	
       vtxA.y = vtxB.y = (480/2);
       vtxC.y = vtxD.y = (480/2)+scale*(480/2);
       break;
     }  
     vtxA.tmuvtx[0].sow = vtxC.tmuvtx[0].sow = 0.0f;
     vtxB.tmuvtx[0].sow = vtxD.tmuvtx[0].sow = 255.0f;
     vtxA.tmuvtx[0].tow = vtxB.tmuvtx[0].tow = 0.0f;
     vtxC.tmuvtx[0].tow = vtxD.tmuvtx[0].tow = 255.0f;

    } else {
     
     /* Enlarge image */
     
     switch (i) {
      case 0:
       vtxA.x = vtxC.x = (640/2)-(640/2);
       vtxB.x = vtxD.x = (640/2);	
       vtxA.y = vtxB.y = (480/2)-(480/2);
       vtxC.y = vtxD.y = (480/2);
       break;
      case 1:
       vtxA.x = vtxC.x = (640/2);
       vtxB.x = vtxD.x = (640/2)+(640/2);	
       vtxA.y = vtxB.y = (480/2)-(480/2);
       vtxC.y = vtxD.y = (480/2);
       break;
      case 2:
       vtxA.x = vtxC.x = (640/2)-(640/2);
       vtxB.x = vtxD.x = (640/2);	
       vtxA.y = vtxB.y = (480/2);
       vtxC.y = vtxD.y = (480/2)+(480/2);
       break;
      case 3:
       vtxA.x = vtxC.x = (640/2);
       vtxB.x = vtxD.x = (640/2)+(640/2);	
       vtxA.y = vtxB.y = (480/2);
       vtxC.y = vtxD.y = (480/2)+(480/2);
       break;
     }  
  
     switch (i) {
      case 0:
       vtxA.tmuvtx[0].sow = vtxC.tmuvtx[0].sow = 255.0-255.0/scale;
       vtxB.tmuvtx[0].sow = vtxD.tmuvtx[0].sow = 255.0;
       vtxA.tmuvtx[0].tow = vtxB.tmuvtx[0].tow = 255.0-255.0/scale;
       vtxC.tmuvtx[0].tow = vtxD.tmuvtx[0].tow = 255.0;
       break;
      case 1:
       vtxA.tmuvtx[0].sow = vtxC.tmuvtx[0].sow = 0.0;
       vtxB.tmuvtx[0].sow = vtxD.tmuvtx[0].sow = 255.0/scale;
       vtxA.tmuvtx[0].tow = vtxB.tmuvtx[0].tow = 255.0-255.0/scale;
       vtxC.tmuvtx[0].tow = vtxD.tmuvtx[0].tow = 255.0;
       break;
      case 2:
       vtxA.tmuvtx[0].sow = vtxC.tmuvtx[0].sow = 255.0-255.0/scale;
       vtxB.tmuvtx[0].sow = vtxD.tmuvtx[0].sow = 255.0;
       vtxA.tmuvtx[0].tow = vtxB.tmuvtx[0].tow = 0.0;
       vtxC.tmuvtx[0].tow = vtxD.tmuvtx[0].tow = 255.0/scale;
       break;
      case 3:
       vtxA.tmuvtx[0].sow = vtxC.tmuvtx[0].sow = 0.0;
       vtxB.tmuvtx[0].sow = vtxD.tmuvtx[0].sow = 255.0/scale;
       vtxA.tmuvtx[0].tow = vtxB.tmuvtx[0].tow = 0.0;
       vtxC.tmuvtx[0].tow = vtxD.tmuvtx[0].tow = 255.0/scale;
       break;
     }   
    }
   
    grDrawTriangle( &vtxA, &vtxD, &vtxC );
    grDrawTriangle( &vtxA, &vtxB, &vtxD );
   }
   
   grBufferSwap( 1 );
   grSstIdle();
   
  }
  
 }           

 grGlideShutdown();

#ifdef USEAUDIO
 close(mixfd);
#endif
  
 if (fg_stop_grab_image(&fg)!=0) exit(-1); 	
 if (fg_close_device (&fg)!=0) exit(-1); 	
 
 exit(0);
}
