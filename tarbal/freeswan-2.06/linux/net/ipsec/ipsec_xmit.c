/*
 * IPSEC Transmit code.
 * Copyright (C) 1996, 1997  John Ioannidis.
 * Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003  Richard Guy Briggs.
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
 */

char ipsec_xmit_c_version[] = "RCSID $Id: ipsec_xmit.c,v 1.14 2004/02/24 17:17:04 mcr Exp $";

#define __NO_VERSION__
#include <linux/module.h>
#include <linux/config.h>	/* for CONFIG_IP_FORWARD */
#include <linux/version.h>
#include <linux/kernel.h> /* printk() */

#include "freeswan/ipsec_param.h"

#ifdef MALLOC_SLAB
# include <linux/slab.h> /* kmalloc() */
#else /* MALLOC_SLAB */
# include <linux/malloc.h> /* kmalloc() */
#endif /* MALLOC_SLAB */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> /* mark_bh */

#include <linux/netdevice.h>   /* struct device, struct net_device_stats, dev_queue_xmit() and other headers */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/ip.h>          /* struct iphdr */
#include <linux/tcp.h>         /* struct tcphdr */
#include <linux/udp.h>         /* struct udphdr */
#include <linux/skbuff.h>
#include <freeswan.h>
#ifdef NET_21
# define MSS_HACK_		/* experimental */
# include <asm/uaccess.h>
# include <linux/in6.h>
# include <net/dst.h>
# define proto_priv cb
#endif /* NET_21 */
#include <asm/checksum.h>
#include <net/icmp.h>		/* icmp_send() */
#include <net/ip.h>
#ifdef NETDEV_23
# include <linux/netfilter_ipv4.h>
#endif /* NETDEV_23 */

#include <linux/if_arp.h>
#ifdef MSS_HACK
# include <net/tcp.h>		/* TCP options */
#endif	/* MSS_HACK */

#include "freeswan/radij.h"
#include "freeswan/ipsec_life.h"
#include "freeswan/ipsec_xform.h"
#include "freeswan/ipsec_eroute.h"
#include "freeswan/ipsec_encap.h"
#include "freeswan/ipsec_radij.h"
#include "freeswan/ipsec_xmit.h"
#include "freeswan/ipsec_sa.h"
#include "freeswan/ipsec_tunnel.h"
#include "freeswan/ipsec_ipe4.h"
#include "freeswan/ipsec_auth.h"

#include "freeswan/ipsec_esp.h"
#include "freeswan/ipsec_ipip.h"

#ifdef CONFIG_KLIPS_IPCOMP
#include "freeswan/ipsec_ipcomp.h"
#endif /* CONFIG_KLIPS_IPCOMP */

#include <pfkeyv2.h>
#include <pfkey.h>

#include "freeswan/ipsec_proto.h"


#ifdef CONFIG_KLIPS_DEBUG
int sysctl_ipsec_debug_verbose = 0;
#endif /* CONFIG_KLIPS_DEBUG */

int ipsec_xmit_trap_count = 0;
int ipsec_xmit_trap_sendcount = 0;

int sysctl_ipsec_icmp = 0;
int sysctl_ipsec_tos = 0;

#ifdef CONFIG_KLIPS_DEBUG_
DEBUG_NO_STATIC void
dmp(char *s, caddr_t bb, int len)
{
	int i;
	unsigned char *b = bb;
  
	if (debug_tunnel) {
		printk(KERN_INFO "klips_debug:ipsec_tunnel_:dmp: "
		       "at %s, len=%d:",
		       s,
		       len);
		for (i=0; i < len; i++) {
			if(!(i%16)){
				printk("\nklips_debug:  ");
			}
			printk(" %02x", *b++);
		}
		printk("\n");
	}
}
#else /* CONFIG_KLIPS_DEBUG */
#define dmp(_x, _y, _z) 
#endif /* CONFIG_KLIPS_DEBUG */

#ifndef SKB_COPY_EXPAND
/*
 *	This is mostly skbuff.c:skb_copy().
 */
struct sk_buff *
skb_copy_expand(struct sk_buff *skb, int headroom, int tailroom, int priority)
{
	struct sk_buff *n;
	unsigned long offset;

	/*
	 *	Do sanity checking
	 */
	if((headroom < 0) || (tailroom < 0) || ((headroom+tailroom) < 0)) {
		printk(KERN_WARNING
		       "klips_error:skb_copy_expand: "
		       "Illegal negative head,tailroom %d,%d\n",
		       headroom,
		       tailroom);
		return NULL;
	}
	/*
	 *	Allocate the copy buffer
	 */
	 
#ifndef NET_21
	IS_SKB(skb);
#endif /* !NET_21 */


	n=alloc_skb(skb->end - skb->head + headroom + tailroom, priority);

	KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
		    "klips_debug:skb_copy_expand: "
		    "allocating %d bytes, head=0p%p data=0p%p tail=0p%p end=0p%p end-head=%d tail-data=%d\n",
		    skb->end - skb->head + headroom + tailroom,
		    skb->head,
		    skb->data,
		    skb->tail,
		    skb->end,
		    skb->end - skb->head,
		    skb->tail - skb->data);

	if(n==NULL)
		return NULL;

	/*
	 *	Shift between the two data areas in bytes
	 */
	 
	/* Set the data pointer */
	skb_reserve(n,skb->data-skb->head+headroom);
	/* Set the tail pointer and length */
	if(skb_tailroom(n) < skb->len) {
		printk(KERN_WARNING "klips_error:skb_copy_expand: "
		       "tried to skb_put %ld, %d available.  This should never happen, please report.\n",
		       (unsigned long int)skb->len,
		       skb_tailroom(n));
		ipsec_kfree_skb(n);
		return NULL;
	}
	skb_put(n,skb->len);

	offset=n->head + headroom - skb->head;

	/* Copy the bytes */
	memcpy(n->head + headroom, skb->head,skb->end-skb->head);
#ifdef NET_21
	n->csum=skb->csum;
	n->priority=skb->priority;
	n->dst=dst_clone(skb->dst);
	if(skb->nh.raw)
		n->nh.raw=skb->nh.raw+offset;
#ifndef NETDEV_23
	n->is_clone=0;
#endif /* NETDEV_23 */
	atomic_set(&n->users, 1);
	n->destructor = NULL;
	n->security=skb->security;
#else /* NET_21 */
	n->link3=NULL;
	n->when=skb->when;
	if(skb->ip_hdr)
	        n->ip_hdr=(struct iphdr *)(((char *)skb->ip_hdr)+offset);
	n->saddr=skb->saddr;
	n->daddr=skb->daddr;
	n->raddr=skb->raddr;
	n->seq=skb->seq;
	n->end_seq=skb->end_seq;
	n->ack_seq=skb->ack_seq;
	n->acked=skb->acked;
	n->free=1;
	n->arp=skb->arp;
	n->tries=0;
	n->lock=0;
	n->users=0;
#endif /* NET_21 */
	n->protocol=skb->protocol;
	n->list=NULL;
	n->sk=NULL;
	n->dev=skb->dev;
	if(skb->h.raw)
		n->h.raw=skb->h.raw+offset;
	if(skb->mac.raw) 
		n->mac.raw=skb->mac.raw+offset;
	memcpy(n->proto_priv, skb->proto_priv, sizeof(skb->proto_priv));
#ifndef NETDEV_23
	n->used=skb->used;
#endif /* !NETDEV_23 */
	n->pkt_type=skb->pkt_type;
	n->stamp=skb->stamp;
	
#ifndef NET_21
	IS_SKB(n);
#endif /* !NET_21 */
	return n;
}
#endif /* !SKB_COPY_EXPAND */

#ifdef CONFIG_KLIPS_DEBUG
void
ipsec_print_ip(struct iphdr *ip)
{
	char buf[ADDRTOA_BUF];

	printk(KERN_INFO "klips_debug:   IP:");
	printk(" ihl:%d", ip->ihl << 2);
	printk(" ver:%d", ip->version);
	printk(" tos:%d", ip->tos);
	printk(" tlen:%d", ntohs(ip->tot_len));
	printk(" id:%d", ntohs(ip->id));
	printk(" %s%s%sfrag_off:%d",
               ip->frag_off & __constant_htons(IP_CE) ? "CE " : "",
               ip->frag_off & __constant_htons(IP_DF) ? "DF " : "",
               ip->frag_off & __constant_htons(IP_MF) ? "MF " : "",
               (ntohs(ip->frag_off) & IP_OFFSET) << 3);
	printk(" ttl:%d", ip->ttl);
	printk(" proto:%d", ip->protocol);
	if(ip->protocol == IPPROTO_UDP)
		printk(" (UDP)");
	if(ip->protocol == IPPROTO_TCP)
		printk(" (TCP)");
	if(ip->protocol == IPPROTO_ICMP)
		printk(" (ICMP)");
	printk(" chk:%d", ntohs(ip->check));
	addrtoa(*((struct in_addr*)(&ip->saddr)), 0, buf, sizeof(buf));
	printk(" saddr:%s", buf);
	if(ip->protocol == IPPROTO_UDP)
		printk(":%d",
		       ntohs(((struct udphdr*)((caddr_t)ip + (ip->ihl << 2)))->source));
	if(ip->protocol == IPPROTO_TCP)
		printk(":%d",
		       ntohs(((struct tcphdr*)((caddr_t)ip + (ip->ihl << 2)))->source));
	addrtoa(*((struct in_addr*)(&ip->daddr)), 0, buf, sizeof(buf));
	printk(" daddr:%s", buf);
	if(ip->protocol == IPPROTO_UDP)
		printk(":%d",
		       ntohs(((struct udphdr*)((caddr_t)ip + (ip->ihl << 2)))->dest));
	if(ip->protocol == IPPROTO_TCP)
		printk(":%d",
		       ntohs(((struct tcphdr*)((caddr_t)ip + (ip->ihl << 2)))->dest));
	if(ip->protocol == IPPROTO_ICMP)
		printk(" type:code=%d:%d",
		       ((struct icmphdr*)((caddr_t)ip + (ip->ihl << 2)))->type,
		       ((struct icmphdr*)((caddr_t)ip + (ip->ihl << 2)))->code);
	printk("\n");

	if(sysctl_ipsec_debug_verbose) {
		__u8 *c;
		int i;
		
		c = ((__u8*)ip) + ip->ihl*4;
		for(i = 0; i < ntohs(ip->tot_len) - ip->ihl*4; i++ /*, c++*/) {
			if(!(i % 16)) {
				printk(KERN_INFO
				       "klips_debug:   @%03x:",
				       i);
			}
			printk(" %02x", /***/c[i]);
			if(!((i + 1) % 16)) {
				printk("\n");
			}
		}
		if(i % 16) {
			printk("\n");
		}
	}
}
#endif /* CONFIG_KLIPS_DEBUG */

#ifdef MSS_HACK
/*
 * Issues:
 *  1) Fragments arriving in the tunnel should probably be rejected.
 *  2) How does this affect syncookies, mss_cache, dst cache ?
 *  3) Path MTU discovery handling needs to be reviewed.  For example,
 *     if we receive an ICMP 'packet too big' message from an intermediate 
 *     router specifying it's next hop MTU, our stack may process this and
 *     adjust the MSS without taking our AH/ESP overheads into account.
 */

 
/*
 * Recaclulate checksum using differences between changed datum, 
 * borrowed from netfilter.
 */
DEBUG_NO_STATIC u_int16_t 
ipsec_fast_csum(u_int32_t oldvalinv, u_int32_t newval, u_int16_t oldcheck)
{
	u_int32_t diffs[] = { oldvalinv, newval };
	return csum_fold(csum_partial((char *)diffs, sizeof(diffs),
	oldcheck^0xFFFF));
}

/*
 * Determine effective MSS.
 *
 * Note that we assume that there is always an MSS option for our own
 * SYN segments, which is mentioned in tcp_syn_build_options(), kernel 2.2.x.
 * This could change, and we should probably parse TCP options instead.
 *
 */
DEBUG_NO_STATIC u_int8_t
ipsec_adjust_mss(struct sk_buff *skb, struct tcphdr *tcph, u_int16_t mtu)
{
	u_int16_t oldmss, newmss;
	u_int32_t *mssp;
	struct sock *sk = skb->sk;
	
	newmss = tcp_sync_mss(sk, mtu);
	printk(KERN_INFO "klips: setting mss to %u\n", newmss);
	mssp = (u_int32_t *)tcph + sizeof(struct tcphdr) / sizeof(u_int32_t);
	oldmss = ntohl(*mssp) & 0x0000FFFF;
	*mssp = htonl((TCPOPT_MSS << 24) | (TCPOLEN_MSS << 16) | newmss);
	tcph->check = ipsec_fast_csum(htons(~oldmss), 
	                              htons(newmss), tcph->check);
	return 1;
}
#endif	/* MSS_HACK */
                                                        
/*
 * Sanity checks
 */
enum ipsec_xmit_value
ipsec_xmit_sanity_check_dev(struct ipsec_xmit_state *ixs)
{

	if (ixs->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_error:ipsec_xmit_sanity_check_dev: "
			    "No device associated with skb!\n" );
		return IPSEC_XMIT_NODEV;
	}

	ixs->prv = ixs->dev->priv;
	if (ixs->prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_error:ipsec_xmit_sanity_check_dev: "
			    "Device has no private structure!\n" );
		return 	IPSEC_XMIT_NOPRIVDEV;
	}

	ixs->physdev = ixs->prv->dev;
	if (ixs->physdev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_error:ipsec_xmit_sanity_check_dev: "
			    "Device is not attached to physical device!\n" );
		return IPSEC_XMIT_NOPHYSDEV;
	}

	ixs->physmtu = ixs->physdev->mtu;

	ixs->stats = (struct net_device_stats *) &(ixs->prv->mystats);

	return IPSEC_XMIT_OK;
}

enum ipsec_xmit_value
ipsec_xmit_sanity_check_skb(struct ipsec_xmit_state *ixs)
{
	/*
	 *	Return if there is nothing to do.  (Does this ever happen?) XXX
	 */
	if (ixs->skb == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_error:ipsec_xmit_sanity_check_skb: "
			    "Nothing to do!\n" );
		return IPSEC_XMIT_NOSKB;
	}
#ifdef NET_21
	/* if skb was cloned (most likely due to a packet sniffer such as
	   tcpdump being momentarily attached to the interface), make
	   a copy of our own to modify */
	if(skb_cloned(ixs->skb)) {
		if
#ifdef SKB_COW_NEW
	       (skb_cow(ixs->skb, skb_headroom(ixs->skb)) != 0)
#else /* SKB_COW_NEW */
	       ((ixs->skb = skb_cow(ixs->skb, skb_headroom(ixs->skb))) == NULL)
#endif /* SKB_COW_NEW */
		{
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_error:ipsec_xmit_sanity_check_skb: "
				    "skb_cow failed to allocate buffer, dropping.\n" );
			ixs->stats->tx_dropped++;
			return IPSEC_XMIT_ERRSKBALLOC;
		}
	}
#endif /* NET_21 */

#ifdef NET_21
	ixs->iph = ixs->skb->nh.iph;
#else /* NET_21 */
	ixs->iph = ixs->skb->ip_hdr;
#endif /* NET_21 */

	/* sanity check for IP version as we can't handle IPv6 right now */
	if (ixs->iph->version != 4) {
		KLIPS_PRINT(debug_tunnel,
			    "klips_debug:ipsec_xmit_sanity_check_skb: "
			    "found IP Version %d but cannot process other IP versions than v4.\n",
			    ixs->iph->version); /* XXX */
		ixs->stats->tx_dropped++;
		return IPSEC_XMIT_NOIPV6;
	}
	
#if IPSEC_DISALLOW_IPOPTIONS
	if ((ixs->iph->ihl << 2) != sizeof (struct iphdr)) {
		KLIPS_PRINT(debug_tunnel,
			    "klips_debug:ipsec_xmit_sanity_check_skb: "
			    "cannot process IP header options yet.  May be mal-formed packet.\n"); /* XXX */
		ixs->stats->tx_dropped++;
		return IPSEC_XMIT_NOIPOPTIONS;
	}
#endif /* IPSEC_DISALLOW_IPOPTIONS */
	
#ifndef NET_21
	if (ixs->iph->ttl <= 0) {
		/* Tell the sender its packet died... */
		ICMP_SEND(ixs->skb, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, 0, ixs->physdev);

		KLIPS_PRINT(debug_tunnel, "klips_debug:ipsec_xmit_sanity_check_skb: "
			    "TTL=0, too many hops!\n");
		ixs->stats->tx_dropped++;
		return IPSEC_XMIT_TTLEXPIRED;
	}
#endif /* !NET_21 */
	
	return IPSEC_XMIT_OK;
}

enum ipsec_xmit_value
ipsec_xmit_encap_once(struct ipsec_xmit_state *ixs,
		      struct xform_functions *protofuncs)
{
	int headroom = 0, tailroom = 0, ilen = 0, len = 0;
	unsigned char *dat;
#if 0
	__u8 hash[AH_AMAX];
	union {
#ifdef CONFIG_KLIPS_AUTH_HMAC_MD5
		MD5_CTX md5;
#endif /* CONFIG_KLIPS_AUTH_HMAC_MD5 */
#ifdef CONFIG_KLIPS_AUTH_HMAC_SHA1
		SHA1_CTX sha1;
#endif /* CONFIG_KLIPS_AUTH_HMAC_SHA1 */
	} tctx;
	int padlen = 0, i;
	unsigned char *idat, *pad;
	__u32 iv[2];
	struct esphdr *espp;
#endif
	
	ixs->iphlen = ixs->iph->ihl << 2;
	ixs->pyldsz = ntohs(ixs->iph->tot_len) - ixs->iphlen;
	ixs->sa_len = satot(&ixs->ipsp->ips_said, 0, ixs->sa_txt, SATOT_BUF);
	KLIPS_PRINT(debug_tunnel & DB_TN_OXFS,
		    "klips_debug:ipsec_xmit_encap_once: "
		    "calling output for <%s%s%s>, SA:%s\n", 
		    IPS_XFORM_NAME(ixs->ipsp),
		    ixs->sa_len ? ixs->sa_txt : " (error)");
	
	headroom += protofuncs->xmit_headroom;

	switch(ixs->ipsp->ips_authalg) {
#ifdef CONFIG_KLIPS_AUTH_HMAC_MD5
	case AH_MD5:
	  ixs->authlen = AHHMAC_HASHLEN;
	  break;
#endif /* CONFIG_KLIPS_AUTH_HMAC_MD5 */
#ifdef CONFIG_KLIPS_AUTH_HMAC_SHA1
	case AH_SHA:
	  ixs->authlen = AHHMAC_HASHLEN;
	  break;
#endif /* CONFIG_KLIPS_AUTH_HMAC_SHA1 */
	case AH_NONE:
	  break;
	default:
	  ixs->stats->tx_errors++;
	  return IPSEC_XMIT_ESP_BADALG;
	}		

	if(protofuncs->xmit_needtailroom) {
	  tailroom += ((8 - ((ixs->pyldsz + 2 * sizeof(unsigned char)) % 8)) % 8) + 2;
	  tailroom += ixs->authlen;
	}

	/*
	case IPPROTO_IPIP:
		ixs->iphlen = sizeof(struct iphdr);
	*/

	KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
		    "klips_debug:ipsec_xmit_encap_once: "
		    "pushing %d bytes, putting %d, proto %d.\n", 
		    headroom, tailroom, ixs->ipsp->ips_said.proto);

	if(skb_headroom(ixs->skb) < headroom) {
		printk(KERN_WARNING
		       "klips_error:ipsec_xmit_encap_once: "
		       "tried to skb_push headroom=%d, %d available.  This should never happen, please report.\n",
		       headroom, skb_headroom(ixs->skb));
		ixs->stats->tx_errors++;
		return IPSEC_XMIT_ESP_PUSHPULLERR;
	}
	dat = skb_push(ixs->skb, headroom);
	ilen = ixs->skb->len - tailroom;
	if(skb_tailroom(ixs->skb) < tailroom) {
		printk(KERN_WARNING
		       "klips_error:ipsec_xmit_encap_once: "
		       "tried to skb_put %d, %d available.  This should never happen, please report.\n",
		       tailroom, skb_tailroom(ixs->skb));
	ixs->stats->tx_errors++;
		return IPSEC_XMIT_ESP_PUSHPULLERR;
	}

	skb_put(ixs->skb, tailroom);
	KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
		    "klips_debug:ipsec_xmit_encap_once: "
		    "head,tailroom: %d,%d before xform.\n",
		    skb_headroom(ixs->skb), skb_tailroom(ixs->skb));
	len = ixs->skb->len;

	if(len > 0xfff0) {
		printk(KERN_WARNING "klips_error:ipsec_xmit_encap_once: "
		       "tot_len (%d) > 65520.  This should never happen, please report.\n",
		       len);
		ixs->stats->tx_errors++;
		return IPSEC_XMIT_BADLEN;
	}
	memmove((void *)dat, (void *)(dat + headroom), ixs->iphlen);
	ixs->iph = (struct iphdr *)dat;
	ixs->iph->tot_len = htons(ixs->skb->len);
	ixs->tailroom = tailroom;

	if(protofuncs->xmit_setup) {
	  enum ipsec_xmit_value retval =
	    (*protofuncs->xmit_setup)(ixs);
	  if(retval < 0) {
	    return retval;
	  }
	}

#ifdef NET_21
	ixs->skb->nh.raw = ixs->skb->data;
#else /* NET_21 */
	ixs->skb->ip_hdr = ixs->skb->h.iph = (struct iphdr *) ixs->skb->data;
#endif /* NET_21 */
	ixs->iph->check = 0;
	ixs->iph->check = ip_fast_csum((unsigned char *)ixs->iph, ixs->iph->ihl);
			
	KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
		    "klips_debug:ipsec_xmit_encap_once: "
		    "after <%s%s%s>, SA:%s:\n",
		    IPS_XFORM_NAME(ixs->ipsp),
		    ixs->sa_len ? ixs->sa_txt : " (error)");
	KLIPS_IP_PRINT(debug_tunnel & DB_TN_XMIT, ixs->iph);
 			
	ixs->ipsp->ips_life.ipl_bytes.ipl_count += len;
	ixs->ipsp->ips_life.ipl_bytes.ipl_last = len;

	if(!ixs->ipsp->ips_life.ipl_usetime.ipl_count) {
		ixs->ipsp->ips_life.ipl_usetime.ipl_count = jiffies / HZ;
	}
	ixs->ipsp->ips_life.ipl_usetime.ipl_last = jiffies / HZ;
	ixs->ipsp->ips_life.ipl_packets.ipl_count++; 

	ixs->ipsp = ixs->ipsp->ips_onext;
			
	return IPSEC_XMIT_OK;
}

/*
 * A TRAP eroute is installed and we want to replace it with a HOLD
 * eroute.
 */
static int create_hold_eroute(struct eroute *origtrap,
			      struct sk_buff * skb, struct iphdr * iph,
			      uint32_t eroute_pid)
{
	struct eroute hold_eroute;
	ip_said hold_said;
	struct sk_buff *first, *last;
	int error;

	first = last = NULL;
	memset((caddr_t)&hold_eroute, 0, sizeof(hold_eroute));
	memset((caddr_t)&hold_said, 0, sizeof(hold_said));
	
	hold_said.proto = IPPROTO_INT;
	hold_said.spi = htonl(SPI_HOLD);
	hold_said.dst.u.v4.sin_addr.s_addr = INADDR_ANY;
	
	hold_eroute.er_eaddr.sen_len = sizeof(struct sockaddr_encap);
	hold_eroute.er_emask.sen_len = sizeof(struct sockaddr_encap);
	hold_eroute.er_eaddr.sen_family = AF_ENCAP;
	hold_eroute.er_emask.sen_family = AF_ENCAP;
	hold_eroute.er_eaddr.sen_type = SENT_IP4;
	hold_eroute.er_emask.sen_type = 255;
	
	hold_eroute.er_eaddr.sen_ip_src.s_addr = iph->saddr;
	hold_eroute.er_eaddr.sen_ip_dst.s_addr = iph->daddr;
	hold_eroute.er_emask.sen_ip_src.s_addr = INADDR_BROADCAST;
	hold_eroute.er_emask.sen_ip_dst.s_addr = INADDR_BROADCAST;
	hold_eroute.er_pid = eroute_pid;
	hold_eroute.er_count = 0;
	hold_eroute.er_lasttime = jiffies/HZ;

#ifdef CONFIG_KLIPS_DEBUG
	if (debug_pfkey) {
		char buf1[64], buf2[64];
		subnettoa(hold_eroute.er_eaddr.sen_ip_src,
			  hold_eroute.er_emask.sen_ip_src, 0, buf1, sizeof(buf1));
		subnettoa(hold_eroute.er_eaddr.sen_ip_dst,
			  hold_eroute.er_emask.sen_ip_dst, 0, buf2, sizeof(buf2));
		KLIPS_PRINT(debug_pfkey,
			    "klips_debug:ipsec_tunnel_start_xmit: "
			    "calling breakeroute and makeroute for %s->%s HOLD eroute.\n",
			    buf1, 
			    buf2);
	}
#endif /* CONFIG_KLIPS_DEBUG */

	if (ipsec_breakroute(&(hold_eroute.er_eaddr), &(hold_eroute.er_emask),
			     &first, &last)) {
		KLIPS_PRINT(debug_pfkey,
			    "klips_debug:ipsec_tunnel_start_xmit: "
			    "HOLD breakeroute found nothing.\n");
	} else {
		KLIPS_PRINT(debug_pfkey,
			    "klips_debug:ipsec_tunnel_start_xmit: "
			    "HOLD breakroute deleted %u.%u.%u.%u -> %u.%u.%u.%u\n",
			    NIPQUAD(hold_eroute.er_eaddr.sen_ip_src),
			    NIPQUAD(hold_eroute.er_eaddr.sen_ip_dst));
	}
	if (first != NULL)
		kfree_skb(first);
	if (last != NULL)
		kfree_skb(last);

	error = ipsec_makeroute(&(hold_eroute.er_eaddr),
				&(hold_eroute.er_emask),
				hold_said, eroute_pid, skb, NULL, NULL);
	if (error) {
		KLIPS_PRINT(debug_pfkey,
			    "klips_debug:ipsec_tunnel_start_xmit: "
			    "HOLD makeroute returned %d, failed.\n", error);
	} else {
		KLIPS_PRINT(debug_pfkey,
			    "klips_debug:ipsec_tunnel_start_xmit: "
			    "HOLD makeroute call successful.\n");
	}
	return (error == 0);
}

enum ipsec_xmit_value
ipsec_xmit_encap_internal(struct ipsec_xmit_state *ixs)
{
	switch (ntohl(ixs->outgoing_said.spi)) {
	case SPI_DROP:
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "shunt SA of DROP or no eroute: dropping.\n");
		ixs->stats->tx_dropped++;
		break;
		
	case SPI_REJECT:
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "shunt SA of REJECT: notifying and dropping.\n");
		ICMP_SEND(ixs->skb,
			  ICMP_DEST_UNREACH,
			  ICMP_PKT_FILTERED,
			  0,
			  ixs->physdev);
		ixs->stats->tx_dropped++;
		break;
		
	case SPI_PASS:
#ifdef NET_21
		ixs->pass = 1;
#endif /* NET_21 */
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "PASS: calling dev_queue_xmit\n");
		return IPSEC_XMIT_PASS;
		
	case SPI_HOLD:
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "shunt SA of HOLD: this does not make sense here, dropping.\n");
		ixs->stats->tx_dropped++;
		break;
		
	case SPI_PASSTRAP:			
	case SPI_TRAP:
	case SPI_TRAPSUBNET:
	{
		struct sockaddr_in src, dst;
		
		/* Signal all listening KMds with a PF_KEY ACQUIRE */
		
		src.sin_family = AF_INET;
		dst.sin_family = AF_INET;
		src.sin_addr.s_addr = ixs->iph->saddr;
		dst.sin_addr.s_addr = ixs->iph->daddr;
		
		ixs->ips.ips_transport_protocol = 0;
		src.sin_port = 0;
		dst.sin_port = 0;
		{
			int i;
			for(i = 0;
			    i < sizeof(struct sockaddr_in)
				    - offsetof(struct sockaddr_in, sin_zero);
			    i++) {
				src.sin_zero[i] = 0;
				dst.sin_zero[i] = 0;
			}
		}
		
		ixs->ips.ips_addr_s = (struct sockaddr*)(&src);
		ixs->ips.ips_addr_d = (struct sockaddr*)(&dst);

		if(KLIPS_PRINTIF(debug_tunnel & DB_TN_XMIT))
		{
			char bufsrc[ADDRTOA_BUF], bufdst[ADDRTOA_BUF];
			struct sockaddr_in *s, *d;
			char *bs, *bd;

			s = (struct sockaddr_in*)(ixs->ips.ips_addr_s);
			d = (struct sockaddr_in*)(ixs->ips.ips_addr_d);

			bs = (addrtoa(s->sin_addr, 0,
				      bufsrc, sizeof(bufsrc)) <= ADDRTOA_BUF)
				? bufsrc : "BAD_ADDR";

			bd = (addrtoa(d->sin_addr, 0,
				      bufdst, sizeof(bufdst)) <= ADDRTOA_BUF)
				? bufdst : "BAD_ADDR";

			KLIPS_WRITE("klips_debug:ipsec_xmit_encap_bundle: "
				    "SADB_ACQUIRE sent due to %d with "
				    "src=%s:%d, dst=%s:%d, proto=%d.\n",
				    ntohl(ixs->outgoing_said.spi),
				    bs,
				    ntohs(s->sin_port),
				    bd, 
				    ntohs(d->sin_port),
				    ixs->ips.ips_said.proto);
		}
		
		/* increment count of total traps needed */
		ipsec_xmit_trap_count++;
		
		if (pfkey_acquire(&ixs->ips) == 0) {
			
			/* note that we succeeded */
			ipsec_xmit_trap_sendcount++;
			
			if (ixs->outgoing_said.spi==htonl(SPI_TRAPSUBNET))
			{
				/*
				 * The spinlock is to prevent any other
				 * process from accessing or deleting
				 * the eroute while we are using and
				 * updating it.
				 */
				spin_lock(&eroute_lock);
				ixs->eroute = ipsec_findroute(&ixs->matcher);
				if(ixs->eroute) {
					ixs->eroute->er_said.spi = htonl(SPI_HOLD);
					ixs->eroute->er_first = ixs->skb;
					ixs->skb = NULL;
				}
				spin_unlock(&eroute_lock);
			}
			else if (create_hold_eroute(ixs->eroute,
						    ixs->skb,
						    ixs->iph,
						    ixs->eroute_pid))
			{
				if(ixs->outgoing_said.spi == htonl(SPI_PASSTRAP))
				{
					ixs->skb = skb_clone(ixs->skb, GFP_ATOMIC);
				}
				else
				{
					ixs->skb = NULL;
				}
			} 
		}
		/* whether or not the above succeeded, we continue */
		
		/* become a %pass if we were told to! */
		if(ixs->outgoing_said.spi == htonl(SPI_PASSTRAP)) {
			ixs->pass = 1;
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_debug:ipsec_xmit_encap_bundle: "
				    "PASSTRAP: letting packet out after %%trap\n");
			return IPSEC_XMIT_PASS;
		}
		
		ixs->stats->tx_dropped++;
	}
	default:
		/* XXX what do we do with an unknown shunt spi? */
		break;
	} /* end of switch */
	
	return IPSEC_XMIT_STOLEN;
}

enum ipsec_xmit_value
ipsec_xmit_encap_bundle(struct ipsec_xmit_state *ixs)
{
	enum ipsec_xmit_value bundle_stat = IPSEC_XMIT_OK;
 
	ixs->newdst = ixs->orgdst = ixs->iph->daddr;
	ixs->newsrc = ixs->orgsrc = ixs->iph->saddr;
	ixs->orgedst = ixs->outgoing_said.dst.u.v4.sin_addr.s_addr;
	ixs->iphlen = ixs->iph->ihl << 2;
	ixs->pyldsz = ntohs(ixs->iph->tot_len) - ixs->iphlen;
	ixs->max_headroom = ixs->max_tailroom = 0;
		
	if (ixs->outgoing_said.proto == IPPROTO_INT) {
		return ipsec_xmit_encap_internal(ixs);
	}
		
	/*
	  The spinlock is to prevent any other process from
	  accessing or deleting the ipsec_sa hash table or any of the
	  ipsec_sa s while we are using and updating them.
		  
	  This is not optimal, but was relatively straightforward
	  at the time.  A better way to do it has been planned for
	  more than a year, to lock the hash table and put reference
	  counts on each ipsec_sa instead.  This is not likely to happen
	  in KLIPS1 unless a volunteer contributes it, but will be
	  designed into KLIPS2.
	*/
	spin_lock(&tdb_lock);

	ixs->ipsp = ipsec_sa_getbyid(&ixs->outgoing_said);
	ixs->sa_len = satot(&ixs->outgoing_said, 0, ixs->sa_txt, sizeof(ixs->sa_txt));

	if (ixs->ipsp == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "no ipsec_sa for SA%s: outgoing packet with no SA, dropped.\n",
			    ixs->sa_len ? ixs->sa_txt : " (error)");
		ixs->stats->tx_dropped++;
		bundle_stat = IPSEC_XMIT_SAIDNOTFOUND;
		goto cleanup;
	}
		
	ipsec_sa_put(ixs->ipsp); /* incomplete */

	KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
		    "klips_debug:ipsec_xmit_encap_bundle: "
		    "found ipsec_sa -- SA:<%s%s%s> %s\n",
		    IPS_XFORM_NAME(ixs->ipsp),
		    ixs->sa_len ? ixs->sa_txt : " (error)");
		
	/*
	 * How much headroom do we need to be able to apply
	 * all the grouped transforms?
	 */
	ixs->ipsq = ixs->ipsp;	/* save the head of the ipsec_sa chain */
	while (ixs->ipsp) {
		ixs->sa_len = satot(&ixs->ipsp->ips_said, 0, ixs->sa_txt, sizeof(ixs->sa_txt));
		if(ixs->sa_len == 0) {
			strcpy(ixs->sa_txt, "(error)");
		}

		/* If it is in larval state, drop the packet, we cannot process yet. */
		if(ixs->ipsp->ips_state == SADB_SASTATE_LARVAL) {
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_debug:ipsec_xmit_encap_bundle: "
				    "ipsec_sa in larval state for SA:<%s%s%s> %s, cannot be used yet, dropping packet.\n",
				    IPS_XFORM_NAME(ixs->ipsp),
				    ixs->sa_len ? ixs->sa_txt : " (error)");
			ixs->stats->tx_errors++;
			bundle_stat = IPSEC_XMIT_SAIDNOTLIVE;
			goto cleanup;
		}

		if(ixs->ipsp->ips_state == SADB_SASTATE_DEAD) {
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_debug:ipsec_xmit_encap_bundle: "
				    "ipsec_sa in dead state for SA:<%s%s%s> %s, can no longer be used, dropping packet.\n",
				    IPS_XFORM_NAME(ixs->ipsp),
				    ixs->sa_len ? ixs->sa_txt : " (error)");
			ixs->stats->tx_errors++;
			bundle_stat = IPSEC_XMIT_SAIDNOTLIVE;
			goto cleanup;
		}

		/* If the replay window counter == -1, expire SA, it will roll */
		if(ixs->ipsp->ips_replaywin && ixs->ipsp->ips_replaywin_lastseq == -1) {
			pfkey_expire(ixs->ipsp, 1);
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_debug:ipsec_xmit_encap_bundle: "
				    "replay window counter rolled for SA:<%s%s%s> %s, packet dropped, expiring SA.\n",
				    IPS_XFORM_NAME(ixs->ipsp),
				    ixs->sa_len ? ixs->sa_txt : " (error)");
			ipsec_sa_delchain(ixs->ipsp);
			ixs->stats->tx_errors++;
			bundle_stat = IPSEC_XMIT_REPLAYROLLED;
			goto cleanup;
		}

		/*
		 * if this is the first time we are using this SA, mark start time,
		 * and offset hard/soft counters by "now" for later checking.
		 */
#if 0
		if(ixs->ipsp->ips_life.ipl_usetime.count == 0) {
			ixs->ipsp->ips_life.ipl_usetime.count = jiffies;
			ixs->ipsp->ips_life.ipl_usetime.hard += jiffies;
			ixs->ipsp->ips_life.ipl_usetime.soft += jiffies;
		}
#endif
			  

		if(ipsec_lifetime_check(&ixs->ipsp->ips_life.ipl_bytes, "bytes", ixs->sa_txt, 
					ipsec_life_countbased, ipsec_outgoing, ixs->ipsp) == ipsec_life_harddied ||
		   ipsec_lifetime_check(&ixs->ipsp->ips_life.ipl_addtime, "addtime",ixs->sa_txt,
					ipsec_life_timebased,  ipsec_outgoing, ixs->ipsp) == ipsec_life_harddied ||
		   ipsec_lifetime_check(&ixs->ipsp->ips_life.ipl_usetime, "usetime",ixs->sa_txt,
					ipsec_life_timebased,  ipsec_outgoing, ixs->ipsp) == ipsec_life_harddied ||
		   ipsec_lifetime_check(&ixs->ipsp->ips_life.ipl_packets, "packets",ixs->sa_txt,
					ipsec_life_countbased, ipsec_outgoing, ixs->ipsp) == ipsec_life_harddied) {
				
			ipsec_sa_delchain(ixs->ipsp);
			ixs->stats->tx_errors++;
			bundle_stat = IPSEC_XMIT_LIFETIMEFAILED;
			goto cleanup;
		}
			

		ixs->headroom = ixs->tailroom = 0;
		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "calling room for <%s%s%s>, SA:%s\n", 
			    IPS_XFORM_NAME(ixs->ipsp),
			    ixs->sa_len ? ixs->sa_txt : " (error)");
		switch(ixs->ipsp->ips_said.proto) {
#ifdef CONFIG_KLIPS_ESP
		case IPPROTO_ESP:
			switch(ixs->ipsp->ips_encalg) {
#ifdef CONFIG_KLIPS_ENC_3DES
			case ESP_3DES:
				ixs->headroom += sizeof(struct esphdr);
				break;
#endif /* CONFIG_KLIPS_ENC_3DES */
			default:
				ixs->stats->tx_errors++;
				bundle_stat = IPSEC_XMIT_ESP_BADALG;
				goto cleanup;
			}
			switch(ixs->ipsp->ips_authalg) {
#ifdef CONFIG_KLIPS_AUTH_HMAC_MD5
			case AH_MD5:
				ixs->tailroom += AHHMAC_HASHLEN;
				break;
#endif /* CONFIG_KLIPS_AUTH_HMAC_MD5 */
#ifdef CONFIG_KLIPS_AUTH_HMAC_SHA1
			case AH_SHA:
				ixs->tailroom += AHHMAC_HASHLEN;
				break;
#endif /* CONFIG_KLIPS_AUTH_HMAC_SHA1 */
			case AH_NONE:
				break;
			default:
				ixs->stats->tx_errors++;
				bundle_stat = IPSEC_XMIT_AH_BADALG;
				goto cleanup;
			}			
			ixs->tailroom += ((8 - ((ixs->pyldsz + 2 * sizeof(unsigned char)) % 8)) % 8) + 2;
			break;
#endif /* !CONFIG_KLIPS_ESP */
#ifdef CONFIG_KLIPS_IPIP
		case IPPROTO_IPIP:
			ixs->headroom += sizeof(struct iphdr);
			break;
#endif /* !CONFIG_KLIPS_IPIP */
		case IPPROTO_COMP:
#ifdef CONFIG_KLIPS_IPCOMP
			/*
			  We can't predict how much the packet will
			  shrink without doing the actual compression.
			  We could do it here, if we were the first
			  encapsulation in the chain.  That might save
			  us a skb_copy_expand, since we might fit
			  into the existing skb then.  However, this
			  would be a bit unclean (and this hack has
			  bit us once), so we better not do it. After
			  all, the skb_copy_expand is cheap in
			  comparison to the actual compression.
			  At least we know the packet will not grow.
			*/
			break;
#endif /* CONFIG_KLIPS_IPCOMP */
		default:
			ixs->stats->tx_errors++;
			bundle_stat = IPSEC_XMIT_BADPROTO;
			goto cleanup;
		}
		ixs->ipsp = ixs->ipsp->ips_onext;
		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "Required head,tailroom: %d,%d\n", 
			    ixs->headroom, ixs->tailroom);
		ixs->max_headroom += ixs->headroom;
		ixs->max_tailroom += ixs->tailroom;
		ixs->pyldsz += (ixs->headroom + ixs->tailroom);
	}
	ixs->ipsp = ixs->ipsq;	/* restore the head of the ipsec_sa chain */
		
	KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
		    "klips_debug:ipsec_xmit_encap_bundle: "
		    "existing head,tailroom: %d,%d before applying xforms with head,tailroom: %d,%d .\n",
		    skb_headroom(ixs->skb), skb_tailroom(ixs->skb),
		    ixs->max_headroom, ixs->max_tailroom);
		
	ixs->tot_headroom += ixs->max_headroom;
	ixs->tot_tailroom += ixs->max_tailroom;
		
	ixs->mtudiff = ixs->prv->mtu + ixs->tot_headroom + ixs->tot_tailroom - ixs->physmtu;

	KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
		    "klips_debug:ipsec_xmit_encap_bundle: "
		    "mtu:%d physmtu:%d tothr:%d tottr:%d mtudiff:%d ippkttotlen:%d\n",
		    ixs->prv->mtu, ixs->physmtu,
		    ixs->tot_headroom, ixs->tot_tailroom, ixs->mtudiff, ntohs(ixs->iph->tot_len));
	if(ixs->mtudiff > 0) {
		int newmtu = ixs->physmtu - (ixs->tot_headroom + ((ixs->tot_tailroom + 2) & ~7) + 5);

		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_info:ipsec_xmit_encap_bundle: "
			    "dev %s mtu of %d decreased by %d to %d\n",
			    ixs->dev->name,
			    ixs->prv->mtu,
			    ixs->prv->mtu - newmtu,
			    newmtu);
		ixs->prv->mtu = newmtu;
#ifdef NET_21
#if 0
		ixs->skb->dst->pmtu = ixs->prv->mtu; /* RGB */
#endif /* 0 */
#else /* NET_21 */
#if 0
		ixs->dev->mtu = ixs->prv->mtu; /* RGB */
#endif /* 0 */
#endif /* NET_21 */
	}

	/* 
	   If the sender is doing PMTU discovery, and the
	   packet doesn't fit within ixs->prv->mtu, notify him
	   (unless it was an ICMP packet, or it was not the
	   zero-offset packet) and send it anyways.

	   Note: buggy firewall configuration may prevent the
	   ICMP packet from getting back.
	*/
	if(sysctl_ipsec_icmp
	   && ixs->prv->mtu < ntohs(ixs->iph->tot_len)
	   && (ixs->iph->frag_off & __constant_htons(IP_DF)) ) {
		int notify = ixs->iph->protocol != IPPROTO_ICMP
			&& (ixs->iph->frag_off & __constant_htons(IP_OFFSET)) == 0;
			
#ifdef IPSEC_obey_DF
		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "fragmentation needed and DF set; %sdropping packet\n",
			    notify ? "sending ICMP and " : "");
		if (notify)
			ICMP_SEND(ixs->skb,
				  ICMP_DEST_UNREACH,
				  ICMP_FRAG_NEEDED,
				  ixs->prv->mtu,
				  ixs->physdev);
		ixs->stats->tx_errors++;
		bundle_stat = IPSEC_XMIT_CANNOTFRAG;
		goto cleanup;
#else /* IPSEC_obey_DF */
		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "fragmentation needed and DF set; %spassing packet\n",
			    notify ? "sending ICMP and " : "");
		if (notify)
			ICMP_SEND(ixs->skb,
				  ICMP_DEST_UNREACH,
				  ICMP_FRAG_NEEDED,
				  ixs->prv->mtu,
				  ixs->physdev);
#endif /* IPSEC_obey_DF */
	}
		
#ifdef MSS_HACK
	/*
	 * If this is a transport mode TCP packet with
	 * SYN set, determine an effective MSS based on 
	 * AH/ESP overheads determined above.
	 */
	if (ixs->iph->protocol == IPPROTO_TCP 
	    && ixs->outgoing_said.proto != IPPROTO_IPIP) {
		struct tcphdr *tcph = ixs->skb->h.th;
		if (tcph->syn && !tcph->ack) {
			if(!ipsec_adjust_mss(ixs->skb, tcph, ixs->prv->mtu)) {
				printk(KERN_WARNING
				       "klips_warning:ipsec_xmit_encap_bundle: "
				       "ipsec_adjust_mss() failed\n");
				ixs->stats->tx_errors++;
				bundle_stat = IPSEC_XMIT_MSSERR;
				goto cleanup;
			}
		}
	}
#endif /* MSS_HACK */

	if(!ixs->hard_header_stripped) {
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "allocating %d bytes for hardheader.\n",
			    ixs->hard_header_len);
		if((ixs->saved_header = kmalloc(ixs->hard_header_len, GFP_ATOMIC)) == NULL) {
			printk(KERN_WARNING "klips_debug:ipsec_xmit_encap_bundle: "
			       "Failed, tried to allocate %d bytes for temp hard_header.\n", 
			       ixs->hard_header_len);
			ixs->stats->tx_errors++;
			bundle_stat = IPSEC_XMIT_ERRMEMALLOC;
			goto cleanup;
		}
		{
			int i;
			for (i = 0; i < ixs->hard_header_len; i++) {
				ixs->saved_header[i] = ixs->skb->data[i];
			}
		}
		if(ixs->skb->len < ixs->hard_header_len) {
			printk(KERN_WARNING "klips_error:ipsec_xmit_encap_bundle: "
			       "tried to skb_pull hhlen=%d, %d available.  This should never happen, please report.\n",
			       ixs->hard_header_len, (int)(ixs->skb->len));
			ixs->stats->tx_errors++;
			bundle_stat = IPSEC_XMIT_ESP_PUSHPULLERR;
			goto cleanup;
		}
		skb_pull(ixs->skb, ixs->hard_header_len);
		ixs->hard_header_stripped = 1;
			
/*			ixs->iph = (struct iphdr *) (ixs->skb->data); */
		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "head,tailroom: %d,%d after hard_header stripped.\n",
			    skb_headroom(ixs->skb), skb_tailroom(ixs->skb));
		KLIPS_IP_PRINT(debug_tunnel & DB_TN_CROUT, ixs->iph);
	} else {
		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "hard header already stripped.\n");
	}
		
	ixs->ll_headroom = (ixs->hard_header_len + 15) & ~15;

	if ((skb_headroom(ixs->skb) >= ixs->max_headroom + 2 * ixs->ll_headroom) && 
	    (skb_tailroom(ixs->skb) >= ixs->max_tailroom)
#ifndef NET_21
	    && ixs->skb->free
#endif /* !NET_21 */
		) {
		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "data fits in existing skb\n");
	} else {
		struct sk_buff* tskb;

		if(!ixs->oskb) {
			ixs->oskb = ixs->skb;
		}

		tskb = skb_copy_expand(ixs->skb,
				       /* The need for 2 * link layer length here remains unexplained...RGB */
				       ixs->max_headroom + 2 * ixs->ll_headroom,
				       ixs->max_tailroom,
				       GFP_ATOMIC);
#ifdef NET_21
		if(tskb && ixs->skb->sk) {
			skb_set_owner_w(tskb, ixs->skb->sk);
		}
#endif /* NET_21 */
		if(ixs->skb != ixs->oskb) {
			ipsec_kfree_skb(ixs->skb);
		}
		ixs->skb = tskb;
		if (!ixs->skb) {
			printk(KERN_WARNING
			       "klips_debug:ipsec_xmit_encap_bundle: "
			       "Failed, tried to allocate %d head and %d tailroom\n", 
			       ixs->max_headroom, ixs->max_tailroom);
			ixs->stats->tx_errors++;
			bundle_stat = IPSEC_XMIT_ERRSKBALLOC;
			goto cleanup;
		}
		KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
			    "klips_debug:ipsec_xmit_encap_bundle: "
			    "head,tailroom: %d,%d after allocation\n",
			    skb_headroom(ixs->skb), skb_tailroom(ixs->skb));
	}
		
	/*
	 * Apply grouped transforms to packet
	 */
	while (ixs->ipsp) {
		enum ipsec_xmit_value encap_stat = IPSEC_XMIT_OK;
		struct xform_functions *protofuncs;

		switch(ixs->ipsp->ips_said.proto) {
#ifdef CONFIG_KLIPS_ESP
		case IPPROTO_ESP:
		  protofuncs = esp_xform_funcs;
		  break;
		break;
#endif /* !CONFIG_KLIPS_ESP */
#ifdef CONFIG_KLIPS_IPIP
		case IPPROTO_IPIP:
		  protofuncs = ipip_xform_funcs;
		  break;
#endif /* !CONFIG_KLIPS_IPIP */
#ifdef CONFIG_KLIPS_IPCOMP
		case IPPROTO_COMP:
		  protofuncs = ipcomp_xform_funcs;
		  break;
#endif /* CONFIG_KLIPS_IPCOMP */
		default:
		  ixs->stats->tx_errors++;
		  bundle_stat = IPSEC_XMIT_BADPROTO;
		  goto cleanup; 
		}

		encap_stat = ipsec_xmit_encap_once(ixs, protofuncs);
		if(encap_stat != IPSEC_XMIT_OK) {
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_debug:ipsec_xmit_encap_bundle: encap_once failed: %d\n",
				    encap_stat);
				
			bundle_stat = encap_stat;
			goto cleanup;
		}
	}
	/* end encapsulation loop here XXX */
 cleanup:
	spin_unlock(&tdb_lock);
	return bundle_stat;
}

/*
 * $Log: ipsec_xmit.c,v $
 * Revision 1.14  2004/02/24 17:17:04  mcr
 * 	s/CONFIG_IPSEC/CONFIG_KLIPS/ as 26sec uses "CONFIG_IPSEC" to
 * 	turn it on/off as well.
 *
 * Revision 1.13  2004/02/16 04:41:19  mcr
 * 	re-factored IPPROTO_INT code into ipsec_xmit_encap_internal()
 * 	fixed code - if we are going to send and %trap (therefore
 * 	save), then we should copy it. Note, we are going to send the
 * 	packet *TWICE*. A second time through the possible tunnel.
 * 	maybe this is wrong.
 * 	more debugging for port-53 examination - pick which which
 * 	source of port information is non-zero.
 *
 * Revision 1.12  2004/02/14 21:59:52  mcr
 * 	TRAPPASS code - port-53 code.
 *
 * Revision 1.11  2004/02/14 21:13:25  mcr
 * 	added TRAPPASS handling code.
 *
 * Revision 1.10  2003/12/13 04:09:21  mcr
 * 	AH transform removed.
 *
 * Revision 1.9  2003/12/11 20:15:04  mcr
 * 	refactored the xmit code, to move all encapsulation
 * 	code into protocol functions. Note that all functions
 * 	are essentially done by a single function, which is probably
 * 	wrong.
 * 	the rcv_functions structures are renamed xform_functions.
 *
 * Revision 1.8  2003/12/06 21:21:38  mcr
 * 	split up receive path into per-transform files, for
 * 	easier later removal.
 *
 * Revision 1.7  2003/12/04 19:05:55  mcr
 * 	cleaned up "sa_id" structure to use "ip_said" only.
 *
 * Revision 1.6  2003/11/07 02:58:07  mcr
 * 	backout of port-selector and X.509 patches
 *
 * Revision 1.4  2003/06/20 02:28:10  mcr
 * 	misstype of variable name, not detected by module build.
 *
 * Revision 1.3  2003/06/20 01:42:21  mcr
 * 	added counters to measure how many ACQUIREs we send to pluto,
 * 	and how many are successfully sent.
 *
 * Revision 1.2  2003/04/03 17:38:35  rgb
 * Centralised ipsec_kfree_skb and ipsec_dev_{get,put}.
 * Normalised coding style.
 * Simplified logic and reduced duplication of code.
 *
 * Revision 1.1  2003/02/12 19:31:23  rgb
 * Refactored from ipsec_tunnel.c
 *
 */
