/*
 * @(#) routines to makes kernel 2.4 compatible with 2.6 usage.

 * Copyright (C) 2004 Michael Richardson <mcr@sandelman.ottawa.on.ca>
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * RCSID $Id: ipsec_kern24.h,v 1.1 2004/02/22 06:49:15 mcr Exp $
 */

#ifndef _IPSEC_KERN24_H

#ifndef NET_26
#define sk_receive_queue  receive_queue
#define sk_destruct       destruct
#define sk_reuse          reuse
#define sk_zapped         zapped
#define sk_family         family
#define sk_protocol       protocol
#define sk_protinfo       protinfo
#define sk_sleep          sleep
#define sk_state_change   state_change
#define sk_shutdown       shutdown
#define sk_err            err
#define sk_stamp          stamp
#define sk_socket         socket
#define sk_sndbuf         sndbuf
#define sock_flag(sk, flag)  sk->dead
#define sk_for_each(sk, node, plist) for(sk=*plist; sk!=NULL; sk = sk->next)
#endif

#define _IPSEC_KERN24_H 1

#endif /* _IPSEC_KERN24_H */

