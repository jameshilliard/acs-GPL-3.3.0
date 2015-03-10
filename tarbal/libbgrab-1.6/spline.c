/*

    Subroutine: Simple 2D Spline interpolator 

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
Usage notes: 

Create 2 temprary arrays with the x and y coordinates to be interpolated. 
Call create_spline with the number of points and the x and y arrays. The
routine will return a xyspline structure with all variables properly 
allocated. The x and y arrays are not required anymore. To interpolate,
call spline_value with the xyspline structure, position to interpolate 
(range between 0.0 and 1.0) and a pointer to an array for two floats. When
done using the spline values, call delete_spline to clean up.

*/
     

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "spline.h"
			
/*
 *  Calculate the coefficients for spline
 */
 
struct xyspline create_spline(int numpoints, float *x, float *y)
{
  int index;
  float *l,*mu,*z;
  struct xyspline the_spline;
  
  /* Store input data */
  the_spline.spline_x=(float *)malloc(numpoints*sizeof(float));
  the_spline.spline_y=(float *)malloc(numpoints*sizeof(float));
  for (index=0; index<numpoints; index++) {
   the_spline.spline_x[index]=x[index];
   the_spline.spline_y[index]=y[index];
  }
  numpoints--;
  the_spline.snumpoints=numpoints;
  /* Allocate structure variables */
  the_spline.spline_xb=(float *)malloc(numpoints*sizeof(float));
  the_spline.spline_xc=(float *)malloc(numpoints*sizeof(float));
  the_spline.spline_xd=(float *)malloc(numpoints*sizeof(float));
  the_spline.spline_yb=(float *)malloc(numpoints*sizeof(float));
  the_spline.spline_yc=(float *)malloc(numpoints*sizeof(float));
  the_spline.spline_yd=(float *)malloc(numpoints*sizeof(float));
  /* Allocate temporary variables */
  l=(float *)malloc(numpoints*sizeof(float));
  mu=(float *)malloc(numpoints*sizeof(float));
  z=(float *)malloc(numpoints*sizeof(float));
 
  /* calculate x coefficients */
  l[ 0 ] = mu[ 0 ] = z[ 0 ] = 0.0;
  for ( index = 1; index < numpoints; index++ ) 
    {
      l[ index ] = 4.0-mu[index-1];
      mu[ index ] = 1.0/l[index];
      z[ index ] = (3.0*(the_spline.spline_x[index+1]-2.0*the_spline.spline_x[index]+the_spline.spline_x[index-1])
      		   -z[index-1])/l[index];
   }
  the_spline.spline_xc[ numpoints ] = 0.0;
  for ( index = (numpoints - 1); index >= 0; index-- )
    the_spline.spline_xc[ index ] = z[index]-mu[index]*the_spline.spline_xc[index+1];
  /* calculate b and d */
  for ( index = (numpoints - 1); index >= 0; index-- ) 
    {
      the_spline.spline_xb[index] = (the_spline.spline_x[index+1]-the_spline.spline_x[index])
   		        -(the_spline.spline_xc[index+1]+2.0*the_spline.spline_xc[index])/3.0;
      the_spline.spline_xd[index] = (the_spline.spline_xc[index+1]-the_spline.spline_xc[index])/3.0;	
    }

  /* calculate y coefficients */
  l[ 0 ] = mu[ 0 ] = z[ 0 ] = 0.0;
  for ( index = 1; index < numpoints; index++ ) 
    {
      l[ index ] = 4.0-mu[index-1];
      mu[ index ] = 1.0/l[index];
      z[ index ] = (3.0*(the_spline.spline_y[index+1]-2.0*the_spline.spline_y[index]+the_spline.spline_y[index-1])
      		   -z[index-1])/l[index];
   }
  the_spline.spline_yc[ numpoints ] = 0.0;
  for ( index = (numpoints - 1); index >= 0; index-- )
    the_spline.spline_yc[ index ] = z[index]-mu[index]*the_spline.spline_yc[index+1];
  /* calculate b and d */
  for ( index = (numpoints - 1); index >= 0; index-- ) 
    {
      the_spline.spline_yb[index] = (the_spline.spline_y[index+1]-the_spline.spline_y[index])
   		        -(the_spline.spline_yc[index+1]+2.0*the_spline.spline_yc[index])/3.0;
      the_spline.spline_yd[index] = (the_spline.spline_yc[index+1]-the_spline.spline_yc[index])/3.0;	
    }

 free(l);
 free(mu);
 free(z);
     
 return(the_spline);
}

/*
 *  calculate spline interpolator at position 'xpt' (0 .. 1)
 */

void spline_value (struct xyspline the_spline, float xpt, float *result) 
{
  int loc;
  float cx;
  
  xpt *= (float)the_spline.snumpoints; 
  loc=(int)xpt;
  cx=xpt-(float)loc;
  
  result[0]=(((the_spline.spline_xd[loc]*cx+the_spline.spline_xc[loc])*cx+the_spline.spline_xb[loc])*cx+the_spline.spline_x[loc]);
  result[1]=(((the_spline.spline_yd[loc]*cx+the_spline.spline_yc[loc])*cx+the_spline.spline_yb[loc])*cx+the_spline.spline_y[loc]);
}

/*
 * Clean up memory used by spline 
 */
 
void delete_spline(struct xyspline the_spline) 
{
 free(the_spline.spline_x);
 free(the_spline.spline_y);
 free(the_spline.spline_xb);
 free(the_spline.spline_xc);
 free(the_spline.spline_xd);
 free(the_spline.spline_yb);
 free(the_spline.spline_yc);
 free(the_spline.spline_yd);
}