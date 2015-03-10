/* Shared speed subroutines.  */

/*
Copyright (C) 1999, 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> /* for qsort */
#include <string.h>
#include <unistd.h>
#if 0
#include <sys/ioctl.h>
#endif

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#include "speed.h"

/* Change this to "#define TRACE(x) x" to get traces. */
#define TRACE(x)


typedef int (*qsort_function_t) _PROTO ((const void *, const void *));


int   speed_option_addrs = 0;


void
pentium_wbinvd(void)
{
#if 0
  {
    static int  fd = -2;

    if (fd == -2)
      {
        fd = open ("/dev/wbinvd", O_RDWR);
        if (fd == -1)
          perror ("open /dev/wbinvd");
      }

    if (fd != -1)
      ioctl (fd, 0, 0);
  }
#endif

#if 0
#define WBINVDSIZE  1024*1024*2
  {
    static char  *p = NULL;
    int   i, sum;

    if (p == NULL)
      p = malloc (WBINVDSIZE);

#if 0
    for (i = 0; i < WBINVDSIZE; i++)
      p[i] = i & 0xFF;
#endif

    sum = 0;
    for (i = 0; i < WBINVDSIZE; i++)
      sum += p[i];

    mpn_cache_fill_dummy (sum);
  }
#endif
}

static int
double_cmp_ptr (const double *p, const double *q)
{
  if (*p > *q)  return 1;
  if (*p < *q)  return -1;
  return 0;
}


/* Measure the speed of a given routine.

   The routine is run with enough repetitions to make it take at least
   speed_precision * speed_unittime.  This aims to minimize the effects of a
   limited accuracy time base and the overhead of the measuring itself.

   Measurements are made looking for 4 results within TOLERANCE of each
   other (or 3 for routines taking longer than 2 seconds).  This aims to get
   an accurate reading even if some runs are bloated by interrupts or task
   switches or whatever.

   The given (*fun)() is expected to run its function "s->reps" many times
   and return the total elapsed time measured using speed_starttime() and
   speed_endtime().  If the function doesn't support the given s->size or
   s->r, -1.0 should be returned.  See the various base routines below.  */

double
speed_measure (double (*fun) _PROTO ((struct speed_params *s)),
               struct speed_params *s)
{
#define TOLERANCE    1.005  /* 0.5% */

  struct speed_params  s_dummy;
  int     i, j, e;
  double  t[30];
  double  t_unsorted[30];

  /* Use dummy parameters if caller doesn't provide any.  Only a few special
     "fun"s will cope with this, speed_noop() is one.  */
  if (s == NULL)
    {
      memset (&s_dummy, '\0', sizeof (s_dummy));
      s = &s_dummy;
    }
  
  s->reps = 1;
  s->time_divisor = 1.0;
  for (i = 0; i < numberof (t); i++)
    {
      for (;;)
        {
          s->src_num = 0;
          s->dst_num = 0;

          t[i] = (*fun) (s);
          t_unsorted[i] = t[i];

          TRACE (printf("size=%ld reps=%u r=%d attempt=%d  %.9f\n", 
                        s->size, s->reps, s->r, i, t[i]));

          if (t[i] == -1.0)
            return -1.0;

          if (t[i] >= speed_unittime * speed_precision)
            break;

          /* go to a value of reps to make t[i] >= precision */
          s->reps = (unsigned) ceil (1.1 * s->reps
                                     * speed_unittime * speed_precision
                                     / MAX (t[i], speed_unittime));
        }
      t[i] /= s->reps;

      if (speed_precision == 0)
        return t[i];

      /* require 3 values within TOLERANCE when >= 2 secs, 4 when below */
      if (t[0] >= 2.0)
        e = 3;
      else
        e = 4;

      /* Look for e many t[]'s within TOLERANCE of each other to consider a
         valid measurement.  Return smallest among them.  */
      if (i >= e)
        {
          qsort (t, i+1, sizeof(t[0]), (qsort_function_t) double_cmp_ptr);
          for (j = e-1; j < i; j++)
            if (t[j] <= t[j-e+1] * TOLERANCE)
              return t[j-e+1] / s->time_divisor;
        }
    } 

  fprintf (stderr, "speed_measure() could not get %d results within %.1f%%\n",
           e, (TOLERANCE-1.0)*100.0);
  fprintf (stderr, "  %.12f is about 0.5%%\n", t[0]*(TOLERANCE-1.0));
  for (i = 0; i < numberof (t); i++)
    fprintf (stderr, "  %.09f\n", t_unsorted[i]);

  return -1.0;
}


/* Read all of ptr,size to get it into the CPU memory cache.
  
   A call to mpn_cache_fill_dummy() is used to make sure the compiler
   doesn't optimize away the whole loop.  Using "volatile mp_limb_t sum"
   would work too, but the function call means we don't rely on every
   compiler actually implementing volatile properly.

   mpn_cache_fill_dummy() is in a separate source file to stop gcc thinking
   it can inline it.  */

void
mpn_cache_fill (mp_srcptr ptr, mp_size_t size)
{
  mp_limb_t  sum = 0;
  mp_size_t  i;

  for (i = 0; i < size; i++)
    sum += ptr[i];

  mpn_cache_fill_dummy(sum);
}


void
mpn_cache_fill_write (mp_ptr ptr, mp_size_t size)
{
  mpn_cache_fill (ptr, size);

#if 0
  mpn_random (ptr, size);
#endif

#if 0
  mp_size_t  i;

  for (i = 0; i < size; i++)
    ptr[i] = i;
#endif
}


void
speed_operand_src (struct speed_params *s, mp_ptr ptr, mp_size_t size)
{
  if (s->src_num >= numberof (s->src))
    {
      fprintf (stderr, "speed_operand_src: no room left in s->src[]\n");
      abort ();
    }
  s->src[s->src_num].ptr = ptr;
  s->src[s->src_num].size = size;
  s->src_num++;
}


void
speed_operand_dst (struct speed_params *s, mp_ptr ptr, mp_size_t size)
{
  if (s->dst_num >= numberof (s->dst))
    {
      fprintf (stderr, "speed_operand_dst: no room left in s->dst[]\n");
      abort ();
    }
  s->dst[s->dst_num].ptr = ptr;
  s->dst[s->dst_num].size = size;
  s->dst_num++;
}


void
speed_cache_fill (struct speed_params *s)
{
  static struct speed_params  prev;
  int  i;

  /* FIXME: need a better way to get the format string for a pointer */

  if (speed_option_addrs)
    {
      int  different;

      different = (s->dst_num != prev.dst_num || s->src_num != prev.src_num);
      for (i = 0; i < s->dst_num; i++)
        different |= (s->dst[i].ptr != prev.dst[i].ptr);
      for (i = 0; i < s->src_num; i++)
        different |= (s->src[i].ptr != prev.src[i].ptr);

      if (different) 
        {
          if (s->dst_num != 0)
            {
              printf ("dst");
              for (i = 0; i < s->dst_num; i++)
                printf (" %08lX", (unsigned long) s->dst[i].ptr);
              printf (" ");
            }

          if (s->src_num != 0)
            {
              printf ("src");
              for (i = 0; i < s->src_num; i++)
                printf (" %08lX", (unsigned long) s->src[i].ptr);
              printf (" ");
            }
          printf ("  (cf sp approx %08lX)\n", (unsigned long) &different);

        }

      memcpy (&prev, s, sizeof(prev));
    }

  switch (s->cache) {
  case 0:
    for (i = 0; i < s->dst_num; i++)
      mpn_cache_fill_write (s->dst[i].ptr, s->dst[i].size);
    for (i = 0; i < s->src_num; i++)
      mpn_cache_fill (s->src[i].ptr, s->src[i].size);
    break;
  case 1:
    pentium_wbinvd();
    break;
  }
}


/* Return p advanced to the next multiple of "align" bytes.  "align" must be
   a power of 2.  Care is taken not to assume sizeof(int)==sizeof(pointer).
   Using "unsigned long" avoids a warning on hpux.  */
void *
align_pointer (void *p, size_t align)
{
  unsigned long  d;
  d = ((unsigned long) p) & (align-1);
  d = (d != 0 ? align-d : 0);
  return (void *) (((char *) p) + d);
}

/* Note that memory allocated with this function can never be freed, because
   the start address of the block allocated is discarded. */
void *
_mp_allocate_func_aligned (size_t bytes, size_t align)
{
  return align_pointer ((*_mp_allocate_func) (bytes + align-1), align);
}


void *
_mp_allocate_or_reallocate (void *ptr, size_t oldsize, size_t newsize)
{
  if (ptr == NULL)
    return (*_mp_allocate_func) (newsize);
  else
    return (*_mp_reallocate_func) (ptr, oldsize, newsize);
}


/* Adjust ptr to align to CACHE_LINE_SIZE bytes plus "align" limbs.  ptr
   needs to have room for up to CACHE_LINE_SIZE-4 extra bytes.  */

mp_ptr
speed_tmp_alloc_adjust (void *ptr, mp_size_t align)
{
  /*
  printf("%p %ld -> %p %X %X\n", ptr, align,
         (mp_ptr) ptr 
         + ((align - ((mp_size_t) ptr >> 2)) &
            SPEED_TMP_ALLOC_ADJUST_MASK),
         ((mp_size_t) ptr >> 2) & SPEED_TMP_ALLOC_ADJUST_MASK,
         SPEED_TMP_ALLOC_ADJUST_MASK);
  */

  return (mp_ptr) ptr 
    + ((align - ((mp_size_t) ptr >> 2)) & SPEED_TMP_ALLOC_ADJUST_MASK);
}


void
mpz_set_n (mpz_ptr z, mp_srcptr p, mp_size_t size)
{
  ASSERT (size >= 0);
  MPN_NORMALIZE (p, size);
  MPZ_REALLOC (z, size);
  MPN_COPY (PTR(z), p, size);
  SIZ(z) = size;
}


/* Miscellanous options accepted by tune and speed programs under -o. */

void
speed_option_set (const char *s)
{
  if (strcmp (s, "addrs") == 0)  speed_option_addrs = 1;
  else 
    {
      printf ("Unrecognised -o option: %s\n", s);
      exit (1);
    }
}


/* The following are basic speed running routines for various gmp functions.
   Many are very similar and use speed.h macros.

   Each routine allocates it's own destination space for the result of the
   function, because only it can know what the function needs.

   speed_starttime() and speed_endtime() are put tight around the code to be
   measured.  Any setups are done outside the timed portion.

   Each routine is responsible for its own cache priming.
   speed_cache_fill() is a good way to do this, see examples in speed.h.
   One cache priming possibility, for CPUs with write-allocate cache, and
   functions that don't take too long, is to do one dummy call before timing
   so as to cache everything that gets used.  But speed_measure() runs a
   routine at least twice and will take the smaller time, so this might not
   be necessary.

   Data alignment will be important, for source, destination and temporary
   workspace.  A routine can align its destination and workspace.  Programs
   using the routines will ensure s->xp and s->yp are aligned.  Aligning
   onto a CACHE_LINE_SIZE boundary is suggested.  s->align_wp and
   s->align_wp2 should be respected where it makes sense to do so.
   SPEED_TMP_ALLOC_LIMBS is a good way to do this.

   A loop of the following form can be expected to turn into good assembler
   code on most CPUs, thereby minimizing overhead in the measurement.  It
   can always be assumed s->reps >= 1.

          i = s->reps
          do
            foo();
          while (--i != 0);

   Additional parameters might be added to "struct speed_params" in the
   future.  Routines should ignore anything they don't use.

   s->size can be used creatively, and s->xp and s->yp can be ignored.  For
   example, speed_mpz_fac_ui() uses s->size as n for the factorial.  s->r is
   just a user-supplied parameter.  speed_mpn_lshift() uses it as a shift,
   speed_mpn_mul_1() uses it as a multiplier.  */


/* MPN_COPY etc can be macros, so the _CALL forms are necessary */
double
speed_MPN_COPY (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_COPY_CALL (MPN_COPY (wp, s->xp, s->size));
}
double
speed_MPN_COPY_INCR (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_COPY_CALL (MPN_COPY_INCR (wp, s->xp, s->size));
}
double
speed_MPN_COPY_DECR (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_COPY_CALL (MPN_COPY_DECR (wp, s->xp, s->size));
}
double
speed_memcpy (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_COPY_CALL
    (memcpy (wp, s->xp, s->size * BYTES_PER_MP_LIMB));
}


double
speed_mpn_addmul_1 (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_UNARY_1 (mpn_addmul_1);
}
double
speed_mpn_submul_1 (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_UNARY_1 (mpn_submul_1);
}


double
speed_mpn_mul_1 (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_UNARY_1 (mpn_mul_1);
}


double
speed_mpn_lshift (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_UNARY_1 (mpn_lshift);
}
double
speed_mpn_rshift (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_UNARY_1 (mpn_rshift);
}


/* The carry-in variants (if available) are good for measuring because they
   won't skip a division if high<divisor.  Alternately, use -1 as a divisor
   with the plain _1 forms. */
double
speed_mpn_divrem_1 (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_DIVREM_1 (mpn_divrem_1);
}
double
speed_mpn_divrem_1f (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_DIVREM_1F (mpn_divrem_1);
}
#if HAVE_NATIVE_mpn_divrem_1c
double
speed_mpn_divrem_1c (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_DIVREM_1C (mpn_divrem_1c);
}
double
speed_mpn_divrem_1cf (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_DIVREM_1CF (mpn_divrem_1c);
}
#endif

double
speed_mpn_divrem_2 (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_DIVREM_2 (mpn_divrem_2);
}

double
speed_mpn_mod_1 (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_MOD_1 (mpn_mod_1);
}
#if HAVE_NATIVE_mpn_mod_1c
double
speed_mpn_mod_1c (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_MOD_1C (mpn_mod_1c);
}
#endif

double
speed_mpn_divexact_by3 (struct speed_params *s)
{
  /* mpn_divexact_by3 is a macro, so the _CALL form is necessary */
  SPEED_ROUTINE_MPN_COPY_CALL(mpn_divexact_by3 (wp, s->xp, s->size));
}


double
speed_mpn_bz_divrem_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BZ_DIVREM_N (mpn_bz_divrem_n);
}
double
speed_mpn_bz_divrem_sb (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BZ_DIVREM_SB (mpn_sb_divrem_mn);
}
double
speed_mpn_bz_tdiv_qr (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BZ_TDIV_QR (mpn_tdiv_qr);
}


double
speed_mpn_popcount (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_POPCOUNT (mpn_popcount);
}
double
speed_mpn_hamdist (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_HAMDIST (mpn_hamdist);
}


double
speed_mpn_add_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N (mpn_add_n);
}
double
speed_mpn_sub_n (struct speed_params *s)
{
SPEED_ROUTINE_MPN_BINARY_N (mpn_sub_n);
}
double
speed_mpn_add_n_self (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N_SELF (mpn_add_n);
}
double
speed_mpn_add_n_inplace (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N_INPLACE (mpn_add_n);
}


/* mpn_and_n etc can be macros and so have to be handled with
   SPEED_ROUTINE_MPN_BINARY_N_CALL forms */
double
speed_mpn_and_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N_CALL (mpn_and_n (wp, s->xp, s->yp, s->size));
}
double
speed_mpn_andn_n (struct speed_params *s)
{
SPEED_ROUTINE_MPN_BINARY_N_CALL (mpn_andn_n (wp, s->xp, s->yp, s->size));
}
double
speed_mpn_nand_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N_CALL (mpn_nand_n (wp, s->xp, s->yp, s->size));
}
double
speed_mpn_ior_n (struct speed_params *s)
{
SPEED_ROUTINE_MPN_BINARY_N_CALL (mpn_ior_n (wp, s->xp, s->yp, s->size));
}
double
speed_mpn_iorn_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N_CALL (mpn_iorn_n (wp, s->xp, s->yp, s->size));
}
double
speed_mpn_nior_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N_CALL (mpn_nior_n (wp, s->xp, s->yp, s->size));
}
double
speed_mpn_xor_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N_CALL (mpn_xor_n (wp, s->xp, s->yp, s->size));
}
double
speed_mpn_xnor_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_BINARY_N_CALL (mpn_xnor_n (wp, s->xp, s->yp, s->size));
}


double
speed_mpn_mul_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_MUL_N (mpn_mul_n);
}
double
speed_mpn_sqr_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_SQR (mpn_sqr_n);
}
double
speed_mpn_mul_n_sqr (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_SQR_CALL (mpn_mul_n (wp, s->xp, s->xp, s->size));
}

double
speed_mpn_mul_basecase (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_MUL_BASECASE(mpn_mul_basecase);
}
double
speed_mpn_sqr_basecase (struct speed_params *s)
{
  /* FIXME: size restrictions on some versions of sqr_basecase */
  SPEED_ROUTINE_MPN_SQR (mpn_sqr_basecase);
}

double
speed_mpn_kara_mul_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_KARA_MUL_N (mpn_kara_mul_n);
}
double
speed_mpn_kara_sqr_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_KARA_SQR_N (mpn_kara_sqr_n);
}

double
speed_mpn_toom3_mul_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_TOOM3_MUL_N (mpn_toom3_mul_n);
}
double
speed_mpn_toom3_sqr_n (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_TOOM3_SQR_N (mpn_toom3_sqr_n);
}

double
speed_mpn_mul_fft_full (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_MUL_N_CALL
    (mpn_mul_fft_full (wp, s->xp, s->size, s->yp, s->size));
}
double
speed_mpn_mul_fft_full_sqr (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_SQR_CALL
    (mpn_mul_fft_full (wp, s->xp, s->size, s->xp, s->size));
}


/* These are mod 2^N+1 multiplies and squares.  If s->r is supplied it's
   used as k, otherwise the best k for the size is used.  If s->size isn't a
   multiple of 2^k it's rounded up to make the effective operation size.  */

#define SPEED_ROUTINE_MPN_MUL_FFT_CALL(call, sqr)       \
  {                                                     \
    mp_ptr     wp;                                      \
    mp_size_t  pl;                                      \
    int        k;                                       \
    unsigned   i;                                       \
    double     t;                                       \
    TMP_DECL (marker);                                  \
                                                        \
    SPEED_RESTRICT_COND (s->size >= 1);                 \
                                                        \
    if (s->r != 0)                                      \
      k = s->r;                                         \
    else                                                \
      k = mpn_fft_best_k (s->size, sqr);                \
                                                        \
    TMP_MARK (marker);                                  \
    pl = mpn_fft_next_size (s->size, k);                \
    wp = SPEED_TMP_ALLOC_LIMBS (pl+1, s->align_wp);     \
                                                        \
    speed_operand_src (s, s->xp, s->size);              \
    if (!sqr)                                           \
      speed_operand_src (s, s->yp, s->size);            \
    speed_operand_dst (s, wp, pl+1);                    \
    speed_cache_fill (s);                               \
                                                        \
    speed_starttime ();                                 \
    i = s->reps;                                        \
    do                                                  \
      call;                                             \
    while (--i != 0);                                   \
    t = speed_endtime ();                               \
                                                        \
    TMP_FREE (marker);                                  \
    return t;                                           \
  }  

double
speed_mpn_mul_fft (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_MUL_FFT_CALL
    (mpn_mul_fft (wp, pl, s->xp, s->size, s->yp, s->size, k), 0);
}

double
speed_mpn_mul_fft_sqr (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_MUL_FFT_CALL
    (mpn_mul_fft (wp, pl, s->xp, s->size, s->xp, s->size, k), 1);
}


double
speed_mpn_gcd (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_GCD (mpn_gcd);
}
double
speed_mpn_gcdext (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_GCDEXT (mpn_gcdext);
}
double
speed_mpn_gcd_1 (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_GCD_1 (mpn_gcd_1);
}


double
speed_mpn_jacobi_base (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_JACBASE (mpn_jacobi_base);
}


double
speed_mpz_fac_ui (struct speed_params *s)
{
  SPEED_ROUTINE_MPZ_UI (mpz_fac_ui);
}
double
speed_mpz_fib_ui (struct speed_params *s)
{
  SPEED_ROUTINE_MPZ_UI (mpz_fib_ui);
}


double
speed_mpz_powm (struct speed_params *s)
{
  SPEED_ROUTINE_MPZ_POWM (mpz_powm);
}


double
speed_modlimb_invert (struct speed_params *s)
{
  SPEED_ROUTINE_MODLIMB_INVERT (modlimb_invert);
}


double
speed_noop (struct speed_params *s)
{
  unsigned  i;
  
  speed_starttime ();
  i = s->reps;
  do 
    noop ();
  while (--i != 0);
  return speed_endtime ();
}  

double
speed_noop_wxs (struct speed_params *s)
{
  mp_ptr   wp;
  unsigned i;
  double   t;
  TMP_DECL (marker);

  TMP_MARK (marker);
  wp = TMP_ALLOC_LIMBS (1);

  speed_starttime ();
  i = s->reps;
  do
    noop_wxs (wp, s->xp, s->size);
  while (--i != 0);
  t = speed_endtime ();

  TMP_FREE (marker);
  return t;
}  

double
speed_noop_wxys (struct speed_params *s)
{
  mp_ptr   wp;
  unsigned i;
  double   t;
  TMP_DECL (marker);

  TMP_MARK (marker);
  wp = TMP_ALLOC_LIMBS (1);

  speed_starttime ();
  i = s->reps;
  do
    noop_wxys (wp, s->xp, s->yp, s->size);
  while (--i != 0);
  t = speed_endtime ();

  TMP_FREE (marker);
  return t;
}  


#define SPEED_ROUTINE_ALLOC_FREE(variables, calls)      \
  {                                                     \
    unsigned  i;                                        \
    variables;                                          \
                                                        \
    speed_starttime ();                                 \
    i = s->reps;                                        \
    do                                                  \
      {                                                 \
        calls;                                          \
      }                                                 \
    while (--i != 0);                                   \
    return speed_endtime ();                            \
  }


/* Compare these to see how much malloc/free costs and then how much
   _mp_default_allocate/free and mpz_init/clear add.  mpz_init/clear or
   mpq_init/clear will be doing a 1 limb allocate, so use that as the size
   when including them in comparisons.  */

double
speed_malloc_free (struct speed_params *s)
{
  size_t  bytes = s->size * BYTES_PER_MP_LIMB;
  SPEED_ROUTINE_ALLOC_FREE (void *p,
                            p = malloc (bytes);
                            free (p));
}  

double
speed_malloc_realloc_free (struct speed_params *s)
{
  size_t  bytes = s->size * BYTES_PER_MP_LIMB;
  SPEED_ROUTINE_ALLOC_FREE (void *p,
                            p = malloc (BYTES_PER_MP_LIMB);
                            p = realloc (p, bytes);
                            free (p));
}  

double
speed_mp_allocate_free (struct speed_params *s)
{
  size_t  bytes = s->size * BYTES_PER_MP_LIMB;
  SPEED_ROUTINE_ALLOC_FREE (void *p,
                            p = (*_mp_allocate_func) (bytes);
                            (*_mp_free_func) (p, bytes));
}  

double
speed_mp_allocate_reallocate_free (struct speed_params *s)
{
  size_t  bytes = s->size * BYTES_PER_MP_LIMB;
  SPEED_ROUTINE_ALLOC_FREE
    (void *p,
     p = (*_mp_allocate_func) (BYTES_PER_MP_LIMB);
     p = (*_mp_reallocate_func) (p, bytes, BYTES_PER_MP_LIMB);
     (*_mp_free_func) (p, bytes));
}  

double
speed_mpz_init_clear (struct speed_params *s)
{
  SPEED_ROUTINE_ALLOC_FREE (mpz_t z,
                            mpz_init (z);
                            mpz_clear (z));
}  

double
speed_mpz_init_realloc_clear (struct speed_params *s)
{
  SPEED_ROUTINE_ALLOC_FREE (mpz_t z,
                            mpz_init (z);
                            _mpz_realloc (z, s->size);
                            mpz_clear (z));
}  

double
speed_mpq_init_clear (struct speed_params *s)
{
  SPEED_ROUTINE_ALLOC_FREE (mpq_t q,
                            mpq_init (q);
                            mpq_clear (q));
}  

double
speed_mpf_init_clear (struct speed_params *s)
{
  SPEED_ROUTINE_ALLOC_FREE (mpf_t f,
                            mpf_init (f);
                            mpf_clear (f));
}  


/* Compare this to mpn_add_n to see how much overhead mpz_add adds.  Note
   that repeatedly calling mpz_add with the same data gives branch predition
   in it an advantage.  */

double
speed_mpz_add (struct speed_params *s)
{
  mpz_t     w, x, y;
  unsigned  i;
  double    t;

  mpz_init (w);
  mpz_init (x);
  mpz_init (y);

  mpz_set_n (x, s->xp, s->size);
  mpz_set_n (y, s->yp, s->size);
  mpz_add (w, x, y);

  speed_starttime ();
  i = s->reps;
  do 
    {
      mpz_add (w, x, y);
    }
  while (--i != 0);
  t = speed_endtime ();

  mpz_clear (w);
  mpz_clear (x);
  mpz_clear (y);
  return t;
}  


/* If r==0, calculate (size,size/2),
   otherwise calculate (size,r). */

double
speed_mpz_bin_uiui (struct speed_params *s)
{
  mpz_t          w;
  unsigned long  k;
  unsigned  i;
  double    t;

  mpz_init (w);
  if (s->r != 0)
    k = s->r;
  else
    k = s->size/2;

  speed_starttime ();
  i = s->reps;
  do 
    {
      mpz_bin_uiui (w, s->size, k);
    }
  while (--i != 0);
  t = speed_endtime ();

  mpz_clear (w);
  return t;
}  


/* The multiplies are successively dependent so the latency is measured, not
   the issue rate.  There's only 10 per loop so the code doesn't get too big
   since umul_ppmm is several instructions on some cpus.

   Putting the arguments as "h,l,l,h" gets slightly better code from gcc
   2.95.2 on x86, it puts only one mov between each mul, not two.  That mov
   though will probably show up as a bogus extra cycle though.

   The measuring function macros are into three parts to avoid overflowing
   preprocessor expansion space if umul_ppmm is big.

   Limitations:

   Don't blindly use this to set UMUL_TIME in gmp-mparam.h, check the code
   generated first, especially on CPUs with low latency multipliers.

   The default umul_ppmm doing h*l will be getting increasing numbers of
   high zero bits in the calculation.  CPUs with data-dependent multipliers
   will want to use umul_ppmm.1 to get some randomization into the
   calculation.  The extra xors and fetches will be a slowdown of course.  */

#define SPEED_MACRO_UMUL_PPMM_A \
  {                             \
    mp_limb_t  h, l;            \
    unsigned   i;               \
    double     t;               \
                                \
    s->time_divisor = 10;       \
                                \
    h = s->xp[0];               \
    l = s->yp[0];               \
                                \
    switch (s->r) {             \
    case 1:                     \
      speed_starttime ();       \
      i = s->reps;              \
      do                        \
        {

#define SPEED_MACRO_UMUL_PPMM_B \
        }                       \
      while (--i != 0);         \
      t = speed_endtime ();     \
      break;                    \
                                \
    default:                    \
      speed_starttime ();       \
      i = s->reps;              \
      do                        \
        {

#define SPEED_MACRO_UMUL_PPMM_C                                         \
        }                                                               \
      while (--i != 0);                                                 \
      t = speed_endtime ();                                             \
      break;                                                            \
    }                                                                   \
                                                                        \
    /* stop the compiler optimizing away the whole calculation! */      \
    noop_1 (h);                                                         \
    noop_1 (l);                                                         \
                                                                        \
    return t;                                                           \
  }


double
speed_umul_ppmm (struct speed_params *s)
{
  SPEED_MACRO_UMUL_PPMM_A;
  {
    umul_ppmm (h, l, l, h);  h ^= s->xp_block[0]; l ^= s->yp_block[0];
     umul_ppmm (h, l, l, h); h ^= s->xp_block[1]; l ^= s->yp_block[1];
     umul_ppmm (h, l, l, h); h ^= s->xp_block[2]; l ^= s->yp_block[2];
    umul_ppmm (h, l, l, h);  h ^= s->xp_block[3]; l ^= s->yp_block[3];
     umul_ppmm (h, l, l, h); h ^= s->xp_block[4]; l ^= s->yp_block[4];
     umul_ppmm (h, l, l, h); h ^= s->xp_block[5]; l ^= s->yp_block[5];
    umul_ppmm (h, l, l, h);  h ^= s->xp_block[6]; l ^= s->yp_block[6];
     umul_ppmm (h, l, l, h); h ^= s->xp_block[7]; l ^= s->yp_block[7];
     umul_ppmm (h, l, l, h); h ^= s->xp_block[8]; l ^= s->yp_block[8];
    umul_ppmm (h, l, l, h);  h ^= s->xp_block[9]; l ^= s->yp_block[9];
  }
  SPEED_MACRO_UMUL_PPMM_B;
  {
    umul_ppmm (h, l, l, h);
     umul_ppmm (h, l, l, h);
     umul_ppmm (h, l, l, h);
    umul_ppmm (h, l, l, h);
     umul_ppmm (h, l, l, h);
     umul_ppmm (h, l, l, h);
    umul_ppmm (h, l, l, h);
     umul_ppmm (h, l, l, h);
     umul_ppmm (h, l, l, h);
    umul_ppmm (h, l, l, h);
  }
  SPEED_MACRO_UMUL_PPMM_C;
}


#if HAVE_NATIVE_mpn_umul_ppmm

#if defined (__hppa) && W_TYPE_SIZE == 64
#define CALL_MPN_UMUL_PPMM  (h = __MPN (umul_ppmm) (h, l, &l))
#else
#define CALL_MPN_UMUL_PPMM  (h = __MPN (umul_ppmm) (&l, h, l))
#endif

double
speed_mpn_umul_ppmm (struct speed_params *s)
{
  SPEED_MACRO_UMUL_PPMM_A;
  {
    CALL_MPN_UMUL_PPMM;  h ^= s->xp_block[0]; l ^= s->yp_block[0];
     CALL_MPN_UMUL_PPMM; h ^= s->xp_block[1]; l ^= s->yp_block[1];
     CALL_MPN_UMUL_PPMM; h ^= s->xp_block[2]; l ^= s->yp_block[2];
    CALL_MPN_UMUL_PPMM;  h ^= s->xp_block[3]; l ^= s->yp_block[3];
     CALL_MPN_UMUL_PPMM; h ^= s->xp_block[4]; l ^= s->yp_block[4];
     CALL_MPN_UMUL_PPMM; h ^= s->xp_block[5]; l ^= s->yp_block[5];
    CALL_MPN_UMUL_PPMM;  h ^= s->xp_block[6]; l ^= s->yp_block[6];
     CALL_MPN_UMUL_PPMM; h ^= s->xp_block[7]; l ^= s->yp_block[7];
     CALL_MPN_UMUL_PPMM; h ^= s->xp_block[8]; l ^= s->yp_block[8];
    CALL_MPN_UMUL_PPMM;  h ^= s->xp_block[9]; l ^= s->yp_block[9];
  }
  SPEED_MACRO_UMUL_PPMM_B;
  {
    CALL_MPN_UMUL_PPMM;
     CALL_MPN_UMUL_PPMM;
     CALL_MPN_UMUL_PPMM;
    CALL_MPN_UMUL_PPMM;
     CALL_MPN_UMUL_PPMM;
     CALL_MPN_UMUL_PPMM;
    CALL_MPN_UMUL_PPMM;
     CALL_MPN_UMUL_PPMM;
     CALL_MPN_UMUL_PPMM;
    CALL_MPN_UMUL_PPMM;
  }
  SPEED_MACRO_UMUL_PPMM_C;
}
#endif


/* The divisions are successively dependent so latency is measured, not
   issue rate.  There's only 10 per loop so the code doesn't get too big,
   especially for udiv_qrnnd_preinv and preinv2norm, which are several
   instructions each.

   Note that it's only the division which is measured here, there's no data
   fetching and no shifting if the divisor gets normalized.

   In speed_udiv_qrnnd with gcc 2.95.2 on x86 the parameters "q,r,r,q,d"
   generate x86 div instructions with nothing in between.

   The measuring function macros are in two parts to avoid overflowing
   preprocessor expansion space if udiv_qrnnd etc are big.

   Limitations:

   Don't blindly use this to set UDIV_TIME in gmp-mparam.h, check the code
   generated first.

   CPUs with data-dependent divisions may want more attention paid to the
   randomness of the data used.  Probably the measurement wanted is over
   uniformly distributed numbers, but what's here might not be giving that.  */

#define SPEED_ROUTINE_UDIV_QRNND_A(normalize)           \
  {                                                     \
    double     t;                                       \
    unsigned   i;                                       \
    mp_limb_t  q, r, d;                                 \
    mp_limb_t  dinv;                                    \
                                                        \
    s->time_divisor = 10;                               \
                                                        \
    /* divisor from "r" parameter, or a default */      \
    d = s->r;                                           \
    if (d == 0)                                         \
      d = 0x12345678;                                   \
                                                        \
    if (normalize)                                      \
      {                                                 \
        unsigned  norm;                                 \
        count_leading_zeros (norm, d);                  \
        d <<= norm;                                     \
        invert_limb (dinv, d);                          \
      }                                                 \
                                                        \
    q = s->xp[0];                                       \
    r = s->yp[0] % d;                                   \
                                                        \
    speed_starttime ();                                 \
    i = s->reps;                                        \
    do                                                  \
      {

#define SPEED_ROUTINE_UDIV_QRNND_B                                      \
      }                                                                 \
    while (--i != 0);                                                   \
    t = speed_endtime ();                                               \
                                                                        \
    /* stop the compiler optimizing away the whole calculation! */      \
    noop_1 (q);                                                         \
    noop_1 (r);                                                         \
                                                                        \
    return t;                                                           \
  }

double
speed_udiv_qrnnd (struct speed_params *s)
{
  SPEED_ROUTINE_UDIV_QRNND_A (UDIV_NEEDS_NORMALIZATION);
  {
    udiv_qrnnd (q, r, r, q, d);
     udiv_qrnnd (q, r, r, q, d);
     udiv_qrnnd (q, r, r, q, d);
    udiv_qrnnd (q, r, r, q, d);
     udiv_qrnnd (q, r, r, q, d);
     udiv_qrnnd (q, r, r, q, d);
    udiv_qrnnd (q, r, r, q, d);
     udiv_qrnnd (q, r, r, q, d);
     udiv_qrnnd (q, r, r, q, d);
    udiv_qrnnd (q, r, r, q, d);
  }
  SPEED_ROUTINE_UDIV_QRNND_B;
}

double
speed_udiv_qrnnd_preinv (struct speed_params *s)
{
  SPEED_ROUTINE_UDIV_QRNND_A (1);
  {
    udiv_qrnnd_preinv (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv (q, r, r, q, d, dinv);
    udiv_qrnnd_preinv (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv (q, r, r, q, d, dinv);
    udiv_qrnnd_preinv (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv (q, r, r, q, d, dinv);
    udiv_qrnnd_preinv (q, r, r, q, d, dinv);
  }
  SPEED_ROUTINE_UDIV_QRNND_B;
}  

double
speed_udiv_qrnnd_preinv2norm (struct speed_params *s)
{
  SPEED_ROUTINE_UDIV_QRNND_A (1);
  {
    udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
    udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
    udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
     udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
    udiv_qrnnd_preinv2norm (q, r, r, q, d, dinv);
  }
  SPEED_ROUTINE_UDIV_QRNND_B;
}

#if HAVE_NATIVE_mpn_udiv_qrnnd

#if defined (__hppa) && W_TYPE_SIZE == 64
#define CALL_MPN_UDIV_QRNND  (q = __MPN (udiv_qrnnd) (r, q, d, &r))
#else
#define CALL_MPN_UDIV_QRNND  (q = __MPN (udiv_qrnnd) (&r, r, q, d))
#endif

double
speed_mpn_udiv_qrnnd (struct speed_params *s)
{

  SPEED_ROUTINE_UDIV_QRNND_A (1);
  {
    CALL_MPN_UDIV_QRNND;
     CALL_MPN_UDIV_QRNND;
     CALL_MPN_UDIV_QRNND;
    CALL_MPN_UDIV_QRNND;
     CALL_MPN_UDIV_QRNND;
     CALL_MPN_UDIV_QRNND;
    CALL_MPN_UDIV_QRNND;
     CALL_MPN_UDIV_QRNND;
     CALL_MPN_UDIV_QRNND;
    CALL_MPN_UDIV_QRNND;
  }
  SPEED_ROUTINE_UDIV_QRNND_B;
}
#endif