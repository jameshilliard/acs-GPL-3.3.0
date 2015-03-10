#ifndef _CONFIG_ALL_H_
/*
 * Copyright (C) 2002              Michael Richardson <mcr@freeswan.org>
 * 
 * This kernel module is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/lgpl.txt>.
 * 
 * This kernel module is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * RCSID $Id: config-all.h,v 1.6 2004/02/24 17:17:25 mcr Exp $
 */
#define	_CONFIG_ALL_H_	/* seen it, no need to see it again */

#define CONFIG_KLIPS 1

#ifndef CONFIG_KLIPS_AH
#define CONFIG_KLIPS_AH 0
#endif

#ifndef CONFIG_KLIPS_DEBUG 
#define CONFIG_KLIPS_DEBUG 1
#endif

#ifndef CONFIG_KLIPS_ESP
#define CONFIG_KLIPS_ESP 1
#endif

#ifndef CONFIG_KLIPS_IPCOMP
#define CONFIG_KLIPS_IPCOMP 1
#endif

#ifndef CONFIG_KLIPS_IPIP
#define CONFIG_KLIPS_IPIP 1
#endif

#ifndef CONFIG_KLIPS_AUTH_HMAC_MD5
#define CONFIG_KLIPS_AUTH_HMAC_MD5 1
#endif

#ifndef CONFIG_KLIPS_AUTH_HMAC_SHA1
#define CONFIG_KLIPS_AUTH_HMAC_SHA1 1
#endif 

#ifndef CONFIG_KLIPS_DYNDEV
#define CONFIG_KLIPS_DYNDEV 1
#endif

#ifndef CONFIG_KLIPS_ENC_3DES
#define CONFIG_KLIPS_ENC_3DES 1
#endif

#ifndef CONFIG_KLIPS_REGRESS
#define CONFIG_KLIPS_REGRESS 0
#endif


#endif /* _CONFIG_ALL_H */
