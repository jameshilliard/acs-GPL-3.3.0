/* Copyright (C) 1998-2002, 2003, 2004, 2005 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1998.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <resolv.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/nameser.h>
#include <not-cancel.h>

#include "nscd-client.h"
#include "nscd_proto.h"

int __nss_not_use_nscd_hosts;

static int nscd_gethst_r (const char *key, size_t keylen, request_type type,
			  struct hostent *resultbuf, char *buffer,
			  size_t buflen, struct hostent **result,
			  int *h_errnop) internal_function;


int
__nscd_gethostbyname_r (const char *name, struct hostent *resultbuf,
			char *buffer, size_t buflen, struct hostent **result,
			int *h_errnop)
{
  request_type reqtype;

  reqtype = (_res.options & RES_USE_INET6) ? GETHOSTBYNAMEv6 : GETHOSTBYNAME;

  return nscd_gethst_r (name, strlen (name) + 1, reqtype, resultbuf,
			buffer, buflen, result, h_errnop);
}


int
__nscd_gethostbyname2_r (const char *name, int af, struct hostent *resultbuf,
			 char *buffer, size_t buflen, struct hostent **result,
			 int *h_errnop)
{
  request_type reqtype;

  reqtype = af == AF_INET6 ? GETHOSTBYNAMEv6 : GETHOSTBYNAME;

  return nscd_gethst_r (name, strlen (name) + 1, reqtype, resultbuf,
			buffer, buflen, result, h_errnop);
}


int
__nscd_gethostbyaddr_r (const void *addr, socklen_t len, int type,
			struct hostent *resultbuf, char *buffer, size_t buflen,
			struct hostent **result, int *h_errnop)
{
  request_type reqtype;

  if (!((len == INADDRSZ && type == AF_INET)
	|| (len == IN6ADDRSZ && type == AF_INET6)))
    /* LEN and TYPE do not match.  */
    return -1;

  reqtype = type == AF_INET6 ? GETHOSTBYADDRv6 : GETHOSTBYADDR;

  return nscd_gethst_r (addr, len, reqtype, resultbuf, buffer, buflen, result,
			h_errnop);
}


libc_locked_map_ptr (, __hst_map_handle);
/* Note that we only free the structure if necessary.  The memory
   mapping is not removed since it is not visible to the malloc
   handling.  */
libc_freeres_fn (hst_map_free)
{
  if (__hst_map_handle.mapped != NO_MAPPING)
    {
      void *p = __hst_map_handle.mapped;
      __hst_map_handle.mapped = NO_MAPPING;
      free (p);
    }
}


static int
internal_function
nscd_gethst_r (const char *key, size_t keylen, request_type type,
	       struct hostent *resultbuf, char *buffer, size_t buflen,
	       struct hostent **result, int *h_errnop)
{
  int gc_cycle;
  int nretries = 0;

  /* If the mapping is available, try to search there instead of
     communicating with the nscd.  */
  struct mapped_database *mapped;
  mapped = __nscd_get_map_ref (GETFDHST, "hosts", &__hst_map_handle,
			       &gc_cycle);

 retry:;
  const hst_response_header *hst_resp = NULL;
  const char *h_name = NULL;
  const uint32_t *aliases_len = NULL;
  const char *addr_list = NULL;
  size_t addr_list_len = 0;
  int retval = -1;
  const char *recend = (const char *) ~UINTMAX_C (0);
  int sock = -1;
  if (mapped != NO_MAPPING)
    {
      const struct datahead *found = __nscd_cache_search (type, key, keylen,
							  mapped);
      if (found != NULL)
	{
	  hst_resp = &found->data[0].hstdata;
	  h_name = (char *) (hst_resp + 1);
	  aliases_len = (uint32_t *) (h_name + hst_resp->h_name_len);
	  addr_list = ((char *) aliases_len
		       + hst_resp->h_aliases_cnt * sizeof (uint32_t));
	  addr_list_len = hst_resp->h_addr_list_cnt * INADDRSZ;

#ifndef _STRING_ARCH_unaligned
	  /* The aliases_len array in the mapped database might very
	     well be unaligned.  We will access it word-wise so on
	     platforms which do not tolerate unaligned accesses we
	     need to make an aligned copy.  */
	  if (((uintptr_t) aliases_len & (__alignof__ (*aliases_len) - 1))
	      != 0)
	    {
	      uint32_t *tmp = alloca (hst_resp->h_aliases_cnt
				      * sizeof (uint32_t));
	      aliases_len = memcpy (tmp, aliases_len,
				    hst_resp->h_aliases_cnt
				    * sizeof (uint32_t));
	    }
#endif
	  if (type != GETHOSTBYADDR && type != GETHOSTBYNAME)
	    {
	      if (hst_resp->h_length == INADDRSZ)
		addr_list += addr_list_len;
	      addr_list_len = hst_resp->h_addr_list_cnt * IN6ADDRSZ;
	    }
	  recend = (const char *) found->data + found->recsize;
	  if (__builtin_expect ((const char *) addr_list + addr_list_len
				> recend, 0))
	    goto out_close;
	}
    }

  hst_response_header hst_resp_mem;
  if (hst_resp == NULL)
    {
      sock = __nscd_open_socket (key, keylen, type, &hst_resp_mem,
				 sizeof (hst_resp_mem));
      if (sock == -1)
	{
	  __nss_not_use_nscd_hosts = 1;
	  goto out;;
	}

      hst_resp = &hst_resp_mem;
    }

  /* No value found so far.  */
  *result = NULL;

  if (__builtin_expect (hst_resp->found == -1, 0))
    {
      /* The daemon does not cache this database.  */
      __nss_not_use_nscd_hosts = 1;
      goto out_close;
    }

  if (hst_resp->found == 1)
    {
      struct iovec vec[4];
      char *cp = buffer;
      uintptr_t align1;
      uintptr_t align2;
      size_t total_len;
      ssize_t cnt;
      char *ignore;
      int n;

      /* A first check whether the buffer is sufficiently large is possible.  */
      /* Now allocate the buffer the array for the group members.  We must
	 align the pointer and the base of the h_addr_list pointers.  */
      align1 = ((__alignof__ (char *) - (cp - ((char *) 0)))
		& (__alignof__ (char *) - 1));
      align2 = ((__alignof__ (char *) - ((cp + align1 + hst_resp->h_name_len)
					 - ((char *) 0)))
		& (__alignof__ (char *) - 1));
      if (buflen < (align1 + hst_resp->h_name_len + align2
		    + ((hst_resp->h_aliases_cnt + hst_resp->h_addr_list_cnt
			+ 2)
		       * sizeof (char *))
		    + hst_resp->h_addr_list_cnt * (type == AF_INET
						   ? INADDRSZ : IN6ADDRSZ)))
	{
	no_room:
	  *h_errnop = NETDB_INTERNAL;
	  __set_errno (ERANGE);
	  retval = ERANGE;
	  goto out_close;
	}
      cp += align1;

      /* Prepare the result as far as we can.  */
      resultbuf->h_aliases = (char **) cp;
      cp += (hst_resp->h_aliases_cnt + 1) * sizeof (char *);
      resultbuf->h_addr_list = (char **) cp;
      cp += (hst_resp->h_addr_list_cnt + 1) * sizeof (char *);

      resultbuf->h_name = cp;
      cp += hst_resp->h_name_len + align2;

      if (type == GETHOSTBYADDR || type == GETHOSTBYNAME)
	{
	  resultbuf->h_addrtype = AF_INET;
	  resultbuf->h_length = INADDRSZ;
	}
      else
	{
	  resultbuf->h_addrtype = AF_INET6;
	  resultbuf->h_length = IN6ADDRSZ;
	}
      for (cnt = 0; cnt < hst_resp->h_addr_list_cnt; ++cnt)
	{
	  resultbuf->h_addr_list[cnt] = cp;
	  cp += resultbuf->h_length;
	}
      resultbuf->h_addr_list[cnt] = NULL;

      if (h_name == NULL)
	{
	  vec[0].iov_base = resultbuf->h_name;
	  vec[0].iov_len = hst_resp->h_name_len;
	  total_len = hst_resp->h_name_len;
	  n = 1;

	  if (hst_resp->h_aliases_cnt > 0)
	    {
	      aliases_len = alloca (hst_resp->h_aliases_cnt
				    * sizeof (uint32_t));
	      vec[n].iov_base = (void *) aliases_len;
	      vec[n].iov_len = hst_resp->h_aliases_cnt * sizeof (uint32_t);

	      total_len += hst_resp->h_aliases_cnt * sizeof (uint32_t);
	      ++n;
	    }

	  if (type == GETHOSTBYADDR || type == GETHOSTBYNAME)
	    {
	      vec[n].iov_base = resultbuf->h_addr_list[0];
	      vec[n].iov_len = hst_resp->h_addr_list_cnt * INADDRSZ;

	      total_len += hst_resp->h_addr_list_cnt * INADDRSZ;

	      ++n;
	    }
	  else
	    {
	      if (hst_resp->h_length == INADDRSZ)
		{
		  ignore = alloca (hst_resp->h_addr_list_cnt * INADDRSZ);
		  vec[n].iov_base = ignore;
		  vec[n].iov_len = hst_resp->h_addr_list_cnt * INADDRSZ;

		  total_len += hst_resp->h_addr_list_cnt * INADDRSZ;

		  ++n;
		}

	      vec[n].iov_base = resultbuf->h_addr_list[0];
	      vec[n].iov_len = hst_resp->h_addr_list_cnt * IN6ADDRSZ;

	      total_len += hst_resp->h_addr_list_cnt * IN6ADDRSZ;

	      ++n;
	    }

	  if ((size_t) __readvall (sock, vec, n) != total_len)
	    goto out_close;
	}
      else
	{
	  memcpy (resultbuf->h_name, h_name, hst_resp->h_name_len);
	  memcpy (resultbuf->h_addr_list[0], addr_list, addr_list_len);
	}

      /*  Now we also can read the aliases.  */
      total_len = 0;
      for (cnt = 0; cnt < hst_resp->h_aliases_cnt; ++cnt)
	{
	  resultbuf->h_aliases[cnt] = cp;
	  cp += aliases_len[cnt];
	  total_len += aliases_len[cnt];
	}
      resultbuf->h_aliases[cnt] = NULL;

      if (__builtin_expect ((const char *) addr_list + addr_list_len
			    + total_len > recend, 0))
	goto out_close;
      /* See whether this would exceed the buffer capacity.  */
      if (__builtin_expect (cp > buffer + buflen, 0))
	goto no_room;

      /* And finally read the aliases.  */
      if (addr_list == NULL)
	{
	  if ((size_t) __readall (sock, resultbuf->h_aliases[0], total_len)
	      == total_len)
	    {
	      retval = 0;
	      *result = resultbuf;
	    }
	}
      else
	{
	  memcpy (resultbuf->h_aliases[0],
		  (const char *) addr_list + addr_list_len, total_len);

	  /* Try to detect corrupt databases.  */
	  if (resultbuf->h_name[hst_resp->h_name_len - 1] != '\0'
	      || ({for (cnt = 0; cnt < hst_resp->h_aliases_cnt; ++cnt)
		     if (resultbuf->h_aliases[cnt][aliases_len[cnt] - 1]
			 != '\0')
		       break;
		   cnt < hst_resp->h_aliases_cnt; }))
	    /* We cannot use the database.  */
	    goto out_close;

	  retval = 0;
	  *result = resultbuf;
	}
    }
  else
    {
      /* Store the error number.  */
      *h_errnop = hst_resp->error;

      /* The `errno' to some value != ERANGE.  */
      __set_errno (ENOENT);
      /* Even though we have not found anything, the result is zero.  */
      retval = 0;
    }

 out_close:
  if (sock != -1)
    close_not_cancel_no_status (sock);
 out:
  if (__nscd_drop_map_ref (mapped, &gc_cycle) != 0 && retval != -1)
    {
      /* When we come here this means there has been a GC cycle while we
	 were looking for the data.  This means the data might have been
	 inconsistent.  Retry if possible.  */
      if ((gc_cycle & 1) != 0 || ++nretries == 5)
	{
	  /* nscd is just running gc now.  Disable using the mapping.  */
	  __nscd_unmap (mapped);
	  mapped = NO_MAPPING;
	}

      goto retry;
    }

  return retval;
}
