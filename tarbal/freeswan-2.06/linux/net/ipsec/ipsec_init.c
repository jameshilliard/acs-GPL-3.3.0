/*
 * @(#) Initialization code.
 * Copyright (C) 1996, 1997  John Ioannidis.
 * Copyright (C) 1998, 1999, 2000, 2001, 2002  Richard Guy Briggs <rgb@freeswan.org>
 *                                 2001  Michael Richardson <mcr@freeswan.org>
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
 * /proc system code was split out into ipsec_proc.c after rev. 1.70.
 *
 */

char ipsec_init_c_version[] = "RCSID $Id: ipsec_init.c,v 1.97 2004/02/24 17:17:04 mcr Exp $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/skbuff.h>
#include <net/protocol.h>

#include "freeswan/ipsec_param.h"


#ifdef MALLOC_SLAB
# include <linux/slab.h> /* kmalloc() */
#else /* MALLOC_SLAB */
# include <linux/malloc.h> /* kmalloc() */
#endif /* MALLOC_SLAB */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> /* mark_bh */

#include <linux/netdevice.h>   /* struct device, and other headers */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/ip.h>          /* struct iphdr */
#include <linux/in.h>          /* struct sockaddr_in */
#include <linux/skbuff.h>
#include <linux/random.h>       /* get_random_bytes() */
#include <freeswan.h>

#ifdef SPINLOCK
# ifdef SPINLOCK_23
#  include <linux/spinlock.h> /* *lock* */
# else /* 23_SPINLOCK */
#  include <asm/spinlock.h> /* *lock* */
# endif /* 23_SPINLOCK */
#endif /* SPINLOCK */

#ifdef NET_21
# include <asm/uaccess.h>
# include <linux/in6.h>
#endif /* NET_21 */

#include <asm/checksum.h>
#include <net/ip.h>

#ifdef CONFIG_PROC_FS
# include <linux/proc_fs.h>
#endif /* CONFIG_PROC_FS */

#ifdef NETLINK_SOCK
# include <linux/netlink.h>
#else
# include <net/netlink.h>
#endif

#include "freeswan/radij.h"

#include "freeswan/ipsec_life.h"
#include "freeswan/ipsec_stats.h"
#include "freeswan/ipsec_sa.h"

#include "freeswan/ipsec_encap.h"
#include "freeswan/ipsec_radij.h"
#include "freeswan/ipsec_xform.h"
#include "freeswan/ipsec_tunnel.h"

#include "freeswan/ipsec_rcv.h"
#include "freeswan/ipsec_esp.h"

#ifdef CONFIG_KLIPS_IPCOMP
#include "freeswan/ipsec_ipcomp.h"
#endif /* CONFIG_KLIPS_IPCOMP */

#include "freeswan/ipsec_proto.h"

#include <pfkeyv2.h>
#include <pfkey.h>

#if !defined(CONFIG_KLIPS_ESP) 
#error "kernel configuration must include ESP"
#endif

/*
 * seems to be present in 2.4.10 (Linus), but also in some RH and other
 * distro kernels of a lower number.
 */
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

#ifdef CONFIG_KLIPS_DEBUG
int debug_eroute = 0;
int debug_spi = 0;
int debug_netlink = 0;
#endif /* CONFIG_KLIPS_DEBUG */

struct prng ipsec_prng;

extern int ipsec_device_event(struct notifier_block *dnot, unsigned long event, void *ptr);
/*
 * the following structure is required so that we receive
 * event notifications when network devices are enabled and
 * disabled (ifconfig up and down).
 */
static struct notifier_block ipsec_dev_notifier={
	ipsec_device_event,
	NULL,
	0
};

#ifdef CONFIG_SYSCTL
extern int ipsec_sysctl_register(void);
extern void ipsec_sysctl_unregister(void);
#endif

static inline int
freeswan_inet_add_protocol(struct inet_protocol *prot, unsigned protocol)
{
#ifdef NETDEV_25
	return inet_add_protocol(prot, protocol);
#else
	inet_add_protocol(prot);
	return 0;
#endif
}

static inline int
freeswan_inet_del_protocol(struct inet_protocol *prot, unsigned protocol)
{
#ifdef NETDEV_25
	return inet_del_protocol(prot, protocol);
#else
	inet_del_protocol(prot);
	return 0;
#endif
}

/* void */
int
ipsec_klips_init(void)
{
	static int inited=0;
	int error = 0;
	extern int des_check_key;
	unsigned char seed[256];

	if(inited) return 0;
	inited++;
	
	/* turn off checking of keys */
	des_check_key=0;

	KLIPS_PRINT(1, "klips_info:ipsec_init: "
		    "KLIPS startup, FreeS/WAN IPSec version: %s\n",
		    ipsec_version_code());

	error |= ipsec_proc_init();

#ifdef SPINLOCK
	ipsec_sadb.sadb_lock = SPIN_LOCK_UNLOCKED;
#else /* SPINLOCK */
	ipsec_sadb.sadb_lock = 0;
#endif /* SPINLOCK */

#ifndef SPINLOCK
	tdb_lock.lock = 0;
	eroute_lock.lock = 0;
#endif /* !SPINLOCK */

	error |= ipsec_sadb_init();
	error |= ipsec_radijinit();

	error |= pfkey_init();

	error |= register_netdevice_notifier(&ipsec_dev_notifier);

#ifdef CONFIG_KLIPS_ESP
	freeswan_inet_add_protocol(&esp_protocol, IPPROTO_ESP);
#endif /* CONFIG_KLIPS_ESP */

/* we never actually link IPCOMP to the stack */
#ifdef IPCOMP_USED_ALONE
#ifdef CONFIG_KLIPS_IPCOMP
 	freeswan_inet_add_protocol(&comp_protocol, IPPROTO_COMP);
#endif /* CONFIG_KLIPS_IPCOMP */
#endif

	error |= ipsec_tunnel_init_devices();


#ifdef CONFIG_SYSCTL
        error |= ipsec_sysctl_register();
#endif                                                                          

	get_random_bytes((void *)seed, sizeof(seed));
	prng_init(&ipsec_prng, seed, sizeof(seed));

	return error;
}	


/* void */
int
ipsec_cleanup(void)
{
	int error = 0;

#ifdef CONFIG_SYSCTL
        ipsec_sysctl_unregister();
#endif                                                                          
	KLIPS_PRINT(debug_netlink, /* debug_tunnel & DB_TN_INIT, */
		    "klips_debug:ipsec_cleanup: "
		    "calling ipsec_tunnel_cleanup_devices.\n");
	error |= ipsec_tunnel_cleanup_devices();

	KLIPS_PRINT(debug_netlink, "called ipsec_tunnel_cleanup_devices");

/* we never actually link IPCOMP to the stack */
#ifdef IPCOMP_USED_ALONE
#ifdef CONFIG_KLIPS_IPCOMP
 	if (freeswan_inet_del_protocol(&comp_protocol, IPPROTO_COMP) < 0)
		printk(KERN_INFO "klips_debug:ipsec_cleanup: "
		       "comp close: can't remove protocol\n");
#endif /* CONFIG_KLIPS_IPCOMP */
#endif /* IPCOMP_USED_ALONE */

#ifdef CONFIG_KLIPS_ESP
 	if (freeswan_inet_del_protocol(&esp_protocol, IPPROTO_ESP) < 0)
		printk(KERN_INFO "klips_debug:ipsec_cleanup: "
		       "esp close: can't remove protocol\n");
#endif /* CONFIG_KLIPS_ESP */

	error |= unregister_netdevice_notifier(&ipsec_dev_notifier);

	KLIPS_PRINT(debug_netlink, /* debug_tunnel & DB_TN_INIT, */
		    "klips_debug:ipsec_cleanup: "
		    "calling ipsec_sadb_cleanup.\n");
	error |= ipsec_sadb_cleanup(0);
	error |= ipsec_sadb_free();

	KLIPS_PRINT(debug_netlink, /* debug_tunnel & DB_TN_INIT, */
		    "klips_debug:ipsec_cleanup: "
		    "calling ipsec_radijcleanup.\n");
	error |= ipsec_radijcleanup();
	
	KLIPS_PRINT(debug_pfkey, /* debug_tunnel & DB_TN_INIT, */
		    "klips_debug:ipsec_cleanup: "
		    "calling pfkey_cleanup.\n");
	error |= pfkey_cleanup();

	ipsec_proc_cleanup();

	prng_final(&ipsec_prng);

	return error;
}

static int
init_ipsec(void)
{
	int error = 0;

	error |= ipsec_klips_init();

	return error;
}

static void
exit_ipsec(void)
{
	int error = 0;

	KLIPS_PRINT(debug_netlink, /* debug_tunnel & DB_TN_INIT, */
		    "klips_debug:cleanup_module: "
		    "calling ipsec_cleanup.\n");

	error |= ipsec_cleanup();

	KLIPS_PRINT(1, "klips_info:cleanup_module: "
		    "ipsec module unloaded.\n");
}

module_init(init_ipsec)
module_exit(exit_ipsec)

/*
 * $Log: ipsec_init.c,v $
 * Revision 1.97  2004/02/24 17:17:04  mcr
 * 	s/CONFIG_IPSEC/CONFIG_KLIPS/ as 26sec uses "CONFIG_IPSEC" to
 * 	turn it on/off as well.
 *
 * Revision 1.96  2004/02/22 06:50:42  mcr
 * 	kernel 2.6 port - merged with 2.4 code.
 *
 * Revision 1.95.6.1  2004/02/20 14:10:18  mcr
 * 	moved code to net/ipsec/ to make 2.6 happy.
 *
 * Revision 1.95  2003/12/15 20:38:11  mcr
 * 	make sure that IPIP is always on.
 *
 * Revision 1.94  2003/12/13 04:09:21  mcr
 * 	AH transform removed.
 *
 * Revision 1.93  2003/12/06 21:21:38  mcr
 * 	split up receive path into per-transform files, for
 * 	easier later removal.
 *
 * Revision 1.92  2003/12/04 19:05:54  mcr
 * 	cleaned up "sa_id" structure to use "ip_said" only.
 *
 * Revision 1.91  2003/11/07 02:58:06  mcr
 * 	backout of port-selector and X.509 patches
 *
 * Revision 1.89  2003/07/31 22:47:16  mcr
 * 	preliminary (untested by FS-team) 2.5 patches.
 *
 * Revision 1.88  2003/06/22 20:05:36  mcr
 * 	clarified why IPCOMP was not being registered, and put a new
 * 	#ifdef in rather than #if 0.
 *
 * Revision 1.87  2002/09/20 15:40:51  rgb
 * Added a lock to the global ipsec_sadb struct for future use.
 * Split ipsec_sadb_cleanup from new funciton ipsec_sadb_free to avoid problem
 * of freeing newly created structures when clearing the reftable upon startup
 * to start from a known state.
 *
 * Revision 1.86  2002/08/15 18:39:15  rgb
 * Move ipsec_prng outside debug code.
 *
 * Revision 1.85  2002/05/14 02:35:29  rgb
 * Change reference to tdb to ipsa.
 *
 * Revision 1.84  2002/04/24 07:55:32  mcr
 * 	#include patches and Makefiles for post-reorg compilation.
 *
 * Revision 1.83  2002/04/24 07:36:28  mcr
 * Moved from ./klips/net/ipsec/ipsec_init.c,v
 *
 * Revision 1.82  2002/04/20 00:12:25  rgb
 * Added esp IV CBC attack fix, disabled.
 *
 * Revision 1.81  2002/04/09 16:13:32  mcr
 * 	switch license to straight GPL.
 *
 * Revision 1.80  2002/03/24 07:34:08  rgb
 * Sanity check for at least one of AH or ESP configured.
 *
 * Revision 1.79  2002/02/05 22:55:15  mcr
 * 	added MODULE_LICENSE declaration.
 * 	This macro does not appear in all kernel versions (see comment).
 *
 * Revision 1.78  2002/01/29 17:17:55  mcr
 * 	moved include of ipsec_param.h to after include of linux/kernel.h
 * 	otherwise, it seems that some option that is set in ipsec_param.h
 * 	screws up something subtle in the include path to kernel.h, and
 * 	it complains on the snprintf() prototype.
 *
 * Revision 1.77  2002/01/29 04:00:51  mcr
 * 	more excise of kversions.h header.
 *
 * Revision 1.76  2002/01/29 02:13:17  mcr
 * 	introduction of ipsec_kversion.h means that include of
 * 	ipsec_param.h must preceed any decisions about what files to
 * 	include to deal with differences in kernel source.
 *
 * Revision 1.75  2001/11/26 09:23:48  rgb
 * Merge MCR's ipsec_sa, eroute, proc and struct lifetime changes.
 *
 * Revision 1.74  2001/11/22 05:44:11  henry
 * new version stuff
 *
 * Revision 1.71.2.2  2001/10/22 20:51:00  mcr
 * 	explicitely set des_check_key.
 *
 * Revision 1.71.2.1  2001/09/25 02:19:39  mcr
 * 	/proc manipulation code moved to new ipsec_proc.c
 *
 * Revision 1.73  2001/11/06 19:47:17  rgb
 * Changed lifetime_packets to uint32 from uint64.
 *
 * Revision 1.72  2001/10/18 04:45:19  rgb
 * 2.4.9 kernel deprecates linux/malloc.h in favour of linux/slab.h,
 * lib/freeswan.h version macros moved to lib/kversions.h.
 * Other compiler directive cleanups.
 *
 * Revision 1.71  2001/09/20 15:32:45  rgb
 * Minor pfkey lifetime fixes.
 *
 * Revision 1.70  2001/07/06 19:51:21  rgb
 * Added inbound policy checking code for IPIP SAs.
 *
 * Revision 1.69  2001/06/14 19:33:26  rgb
 * Silence startup message for console, but allow it to be logged.
 * Update copyright date.
 *
 * Revision 1.68  2001/05/29 05:14:36  rgb
 * Added PMTU to /proc/net/ipsec_tncfg output.  See 'man 5 ipsec_tncfg'.
 *
 * Revision 1.67  2001/05/04 16:34:52  rgb
 * Rremove erroneous checking of return codes for proc_net_* in 2.4.
 *
 * Revision 1.66  2001/05/03 19:40:34  rgb
 * Check error return codes in startup and shutdown.
 *
 * Revision 1.65  2001/02/28 05:03:27  rgb
 * Clean up and rationalise startup messages.
 *
 * Revision 1.64  2001/02/27 22:24:53  rgb
 * Re-formatting debug output (line-splitting, joining, 1arg/line).
 * Check for satoa() return codes.
 *
 * Revision 1.63  2000/11/29 20:14:06  rgb
 * Add src= to the output of /proc/net/ipsec_spi and delete dst from IPIP.
 *
 * Revision 1.62  2000/11/06 04:31:24  rgb
 * Ditched spin_lock_irqsave in favour of spin_lock_bh.
 * Fixed longlong for pre-2.4 kernels (Svenning).
 * Add Svenning's adaptive content compression.
 * Disabled registration of ipcomp handler.
 *
 * Revision 1.61  2000/10/11 13:37:54  rgb
 * #ifdef out debug print that causes proc/net/ipsec_version to oops.
 *
 * Revision 1.60  2000/09/20 03:59:01  rgb
 * Change static info functions to DEBUG_NO_STATIC to reveal function names
 * in oopsen.
 *
 * Revision 1.59  2000/09/16 01:06:26  rgb
 * Added cast of var to silence compiler warning about long fed to int
 * format.
 *
 * Revision 1.58  2000/09/15 11:37:01  rgb
 * Merge in heavily modified Svenning Soerensen's <svenning@post5.tele.dk>
 * IPCOMP zlib deflate code.
 *
 * Revision 1.57  2000/09/12 03:21:50  rgb
 * Moved radij_c_version printing to ipsec_version_get_info().
 * Reformatted ipsec_version_get_info().
 * Added sysctl_{,un}register() calls.
 *
 * Revision 1.56  2000/09/08 19:16:50  rgb
 * Change references from DEBUG_IPSEC to CONFIG_IPSEC_DEBUG.
 * Removed all references to CONFIG_IPSEC_PFKEYv2.
 *
 * Revision 1.55  2000/08/30 05:19:03  rgb
 * Cleaned up no longer used spi_next, netlink register/unregister, other
 * minor cleanup.
 * Removed cruft replaced by TDB_XFORM_NAME.
 * Removed all the rest of the references to tdb_spi, tdb_proto, tdb_dst.
 * Moved debug version strings to printk when /proc/net/ipsec_version is
 * called.
 *
 * Revision 1.54  2000/08/20 18:31:05  rgb
 * Changed cosmetic alignment in spi_info.
 * Changed addtime and usetime to use actual value which is relative
 * anyways, as intended. (Momchil)
 *
 * Revision 1.53  2000/08/18 17:37:03  rgb
 * Added an (int) cast to shut up the compiler...
 *
 * Revision 1.52  2000/08/01 14:51:50  rgb
 * Removed _all_ remaining traces of DES.
 *
 * Revision 1.51  2000/07/25 20:41:22  rgb
 * Removed duplicate parameter in spi_getinfo.
 *
 * Revision 1.50  2000/07/17 03:21:45  rgb
 * Removed /proc/net/ipsec_spinew.
 *
 * Revision 1.49  2000/06/28 05:46:51  rgb
 * Renamed ivlen to iv_bits for consistency.
 * Changed output of add and use times to be relative to now.
 *
 * Revision 1.48  2000/05/11 18:26:10  rgb
 * Commented out calls to netlink_attach/detach to avoid activating netlink
 * in the kenrel config.
 *
 * Revision 1.47  2000/05/10 22:35:26  rgb
 * Comment out most of the startup version information.
 *
 * Revision 1.46  2000/03/22 16:15:36  rgb
 * Fixed renaming of dev_get (MB).
 *
 * Revision 1.45  2000/03/16 06:40:48  rgb
 * Hardcode PF_KEYv2 support.
 *
 * Revision 1.44  2000/01/22 23:19:20  rgb
 * Simplified code to use existing macro TDB_XFORM_NAME().
 *
 * Revision 1.43  2000/01/21 06:14:04  rgb
 * Print individual stats only if non-zero.
 * Removed 'bits' from each keylength for brevity.
 * Shortened lifetimes legend for brevity.
 * Changed wording from 'last_used' to the clearer 'idle'.
 *
 * Revision 1.42  1999/12/31 14:57:19  rgb
 * MB fix for new dummy-less proc_get_info in 2.3.35.
 *
 * Revision 1.41  1999/11/23 23:04:03  rgb
 * Use provided macro ADDRTOA_BUF instead of hardcoded value.
 * Sort out pfkey and freeswan headers, putting them in a library path.
 *
 * Revision 1.40  1999/11/18 18:47:01  rgb
 * Added dynamic proc registration for 2.3.25+.
 * Changed all device registrations for static linking to
 * dynamic to reduce the number and size of patches.
 * Changed all protocol registrations for static linking to
 * dynamic to reduce the number and size of patches.
 *
 * Revision 1.39  1999/11/18 04:12:07  rgb
 * Replaced all kernel version macros to shorter, readable form.
 * Added Marc Boucher's 2.3.25 proc patches.
 * Converted all PROC_FS entries to dynamic to reduce kernel patching.
 * Added CONFIG_PROC_FS compiler directives in case it is shut off.
 *
 * Revision 1.38  1999/11/17 15:53:38  rgb
 * Changed all occurrences of #include "../../../lib/freeswan.h"
 * to #include <freeswan.h> which works due to -Ilibfreeswan in the
 * klips/net/ipsec/Makefile.
 *
 * Revision 1.37  1999/10/16 04:23:06  rgb
 * Add stats for replaywin_errs, replaywin_max_sequence_difference,
 * authentication errors, encryption size errors, encryption padding
 * errors, and time since last packet.
 *
 * Revision 1.36  1999/10/16 00:30:47  rgb
 * Added SA lifetime counting.
 *
 * Revision 1.35  1999/10/15 22:14:00  rgb
 * Clean out cruft.
 *
 * Revision 1.34  1999/10/03 18:46:28  rgb
 * Spinlock fixes for 2.0.xx and 2.3.xx.
 *
 * Revision 1.33  1999/10/01 17:08:10  rgb
 * Disable spinlock init.
 *
 * Revision 1.32  1999/10/01 16:22:24  rgb
 * Switch from assignment init. to functional init. of spinlocks.
 *
 * Revision 1.31  1999/10/01 15:44:52  rgb
 * Move spinlock header include to 2.1> scope.
 *
 * Revision 1.30  1999/10/01 00:00:16  rgb
 * Added eroute structure locking.
 * Added tdb structure locking.
 * Minor formatting changes.
 * Add call to initialize tdb hash table.
 *
 * Revision 1.29  1999/09/23 20:22:40  rgb
 * Enable, tidy and fix network notifier code.
 *
 * Revision 1.28  1999/09/18 11:39:56  rgb
 * Start to add (disabled) netdevice notifier code.
 *
 * Revision 1.27  1999/08/28 08:24:47  rgb
 * Add compiler directives to compile cleanly without debugging.
 *
 * Revision 1.26  1999/08/06 16:03:22  rgb
 * Correct error messages on failure to unload /proc entries.
 *
 * Local variables:
 * c-file-style: "linux"
 * End:
 *
 */
