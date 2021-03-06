/* Test mpz_[cft]div_[qr]_2exp.

Copyright 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


/* If the remainder is in the correct range and q*d+r is correct, then q
   must have rounded correctly.  */

void
check_one (mpz_srcptr a, unsigned long d)
{
  mpz_t  q, r, p, d2exp;
  int    inplace;

  mpz_init (d2exp);
  mpz_init (q);
  mpz_init (r);
  mpz_init (p);

  mpz_set_ui (d2exp, 1L);
  mpz_mul_2exp (d2exp, d2exp, d);

#define INPLACE(fun,dst,src,d)  \
  if (inplace)                  \
    {                           \
      mpz_set (dst, src);       \
      fun (dst, dst, d);        \
    }                           \
  else                          \
    fun (dst, src, d);
  
  for (inplace = 0; inplace <= 1; inplace++)
    {
      INPLACE (mpz_fdiv_q_2exp, q, a, d);
      INPLACE (mpz_fdiv_r_2exp, r, a, d);

      mpz_mul_2exp (p, q, d);
      mpz_add (p, p, r);
      if (mpz_sgn (r) < 0 || mpz_cmp (r, d2exp) >= 0)
        {
          printf ("mpz_fdiv_r_2exp result out of range\n");
          goto error;
        }
      if (mpz_cmp (p, a) != 0)
        {
          printf ("mpz_fdiv_[qr]_2exp doesn't multiply back\n");
          goto error;
        }


      INPLACE (mpz_cdiv_q_2exp, q, a, d);
      INPLACE (mpz_cdiv_r_2exp, r, a, d);

      mpz_mul_2exp (p, q, d);
      mpz_add (p, p, r);
      if (mpz_sgn (r) > 0 || mpz_cmpabs (r, d2exp) >= 0)
        {
          printf ("mpz_cdiv_r_2exp result out of range\n");
          goto error;
        }
      if (mpz_cmp (p, a) != 0)
        {
          printf ("mpz_cdiv_[qr]_2exp doesn't multiply back\n");
          goto error;
        }


      INPLACE (mpz_tdiv_q_2exp, q, a, d);
      INPLACE (mpz_tdiv_r_2exp, r, a, d);

      mpz_mul_2exp (p, q, d);
      mpz_add (p, p, r);
      if (mpz_sgn (r) != 0 && mpz_sgn (r) != mpz_sgn (a))
        {
          printf ("mpz_tdiv_r_2exp result wrong sign\n");
          goto error;
        }
      if (mpz_cmpabs (r, d2exp) >= 0)
        {
          printf ("mpz_tdiv_r_2exp result out of range\n");
          goto error;
        }
      if (mpz_cmp (p, a) != 0)
        {
          printf ("mpz_tdiv_[qr]_2exp doesn't multiply back\n");
          goto error;
        }
    }

  mpz_clear (d2exp);
  mpz_clear (q);
  mpz_clear (r);
  mpz_clear (p);
  return;


 error:
  mpz_trace ("a", a);
  printf    ("d=%lu\n", d);
  mpz_trace ("q", q);
  mpz_trace ("r", r);
  mpz_trace ("p", p);

  mp_trace_base = -16;
  mpz_trace ("a", a);
  printf    ("d=0x%lX\n", d);
  mpz_trace ("q", q);
  mpz_trace ("r", r);
  mpz_trace ("p", p);

  abort ();
}


void
check_all (mpz_ptr a, unsigned long d)
{
  check_one (a, d);
  mpz_neg (a, a);
  check_one (a, d);
}


void
check_various (void)
{
  unsigned long  i, d;
  mpz_t          a;

  mpz_init (a);

  /* a==0, and various d */
  mpz_set_ui (a, 0L);
  for (d = 0; d < 2*BITS_PER_MP_LIMB+4; d++)
    check_one (a, d);

  /* a==2^i, and various d */
  for (i = 0; i < 5*BITS_PER_MP_LIMB; i++)
    {
      d = (i < BITS_PER_MP_LIMB+3 ? 0 : i-(BITS_PER_MP_LIMB+3));
      for ( ; d < i+BITS_PER_MP_LIMB+3; d++)
        {
          mpz_set_ui (a, 1L);
          mpz_mul_2exp (a, a, i);
          check_all (a, d);
        }
    }

  mpz_clear (a);
}


void
check_random (int argc, char *argv[])
{
  gmp_randstate_ptr  rands = RANDS;
  int            reps = 5000;
  mpz_t          a;
  unsigned long  d;
  int            i;

  if (argc == 2)
    reps = atoi (argv[1]);

  mpz_init (a);

  for (i = 0; i < reps; i++)
    {
      /* exponentially within 2 to 257 bits */
      mpz_erandomb (a, rands, urandom () % 8 + 2);

      d = urandom () % 256;

      check_all (a, d);
    }

  mpz_clear (a);
}


int
main (int argc, char *argv[])
{
  tests_start ();

  check_various ();
  check_random (argc, argv);

  tests_end ();
  exit (0);
}
