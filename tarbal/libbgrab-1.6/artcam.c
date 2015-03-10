/*

    Program: artcam - modify colors on the fly (glide output)
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
 * NOTE 2.: Does not require X-Windows 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "bgrab.h"

#include <glide.h>
#include <linutil.h>

#define GRABWIDTH	512
#define GRABHEIGHT	512
#define DEPTH 		2
#define NUMTEXTURES	4

int use_interpolation=1;

unsigned short convtable[1024*64];

/* Define color conversion table - input/output color space is RGB565 */

void initConversionTable()
{
 int i;
 unsigned short r,g,b;
 unsigned short value;
 unsigned short dummy;

 for (i=0; i<(64*1024); i++) {
  /* Get RGB values */
  value=(unsigned short)i;
  r=(value >> 11);
  g=((value >> 5) & 63);
  b=(value & 31);
  /* Super simple color conversion function - change at will */
  dummy=r;
  r=31-b;
  b=31-dummy;
  g=63-g;
  /* Store new color value */
  value=(r << 11) | (g << 5) | b;
  convtable[i]=value;
 }
}

int main (int argc, char *argv[])
{
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
 float scale=1.0;
 unsigned short *curpos;
 fprintf (stderr,"artcam\n\n");

 /* Setup conversion table */
 initConversionTable();

 /* Setup  framegrabber */
 if (fg_open_device (&fg, "/dev/video")!=0) exit(-1); 	
 if (fg_print_info (&fg)!=0) exit(-1);
 if (fg_set_channel (&fg,1)!=0) exit(-1);
 if (fg_start_grab_image(&fg, GRABWIDTH, GRABHEIGHT, FORMAT_RGB565)!=0) exit(-1);

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
 while ( !kbhit() ) { 

  /* Update framebuffer with new image */
  if (fg_get_next_image(&fg)!=NULL) {

   /* Color conversion from table */
   curpos=(unsigned short *)fg.current_image;
   for (i=0; i<fg.image_pixels; i++) {
    *curpos=convtable[*curpos];
    curpos++;
   }
    
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

    grDrawTriangle( &vtxA, &vtxD, &vtxC );
    grDrawTriangle( &vtxA, &vtxB, &vtxD );
   }
   
   grBufferSwap( 1 );
   grSstIdle();
   
  }
  
 }           

 grGlideShutdown();

 if (fg_stop_grab_image(&fg)!=0) exit(-1); 	
 if (fg_close_device (&fg)!=0) exit(-1); 	
 
 exit(0);
}
