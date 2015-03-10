/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PATH = 258,
     PATHTYPE = 259,
     INCLUDE = 260,
     IDENTIFIER = 261,
     VENDORID = 262,
     LOGGING = 263,
     LOGLEV = 264,
     PADDING = 265,
     PAD_RANDOMIZE = 266,
     PAD_RANDOMIZELEN = 267,
     PAD_MAXLEN = 268,
     PAD_STRICT = 269,
     PAD_EXCLTAIL = 270,
     LISTEN = 271,
     X_ISAKMP = 272,
     X_ISAKMP_NATT = 273,
     X_ADMIN = 274,
     STRICT_ADDRESS = 275,
     ADMINSOCK = 276,
     DISABLED = 277,
     MODECFG = 278,
     CFG_NET4 = 279,
     CFG_MASK4 = 280,
     CFG_DNS4 = 281,
     CFG_NBNS4 = 282,
     CFG_AUTH_SOURCE = 283,
     CFG_SYSTEM = 284,
     CFG_RADIUS = 285,
     CFG_LOCAL = 286,
     CFG_NONE = 287,
     CFG_ACCOUNTING = 288,
     CFG_CONF_SOURCE = 289,
     CFG_MOTD = 290,
     CFG_POOL_SIZE = 291,
     CFG_AUTH_THROTTLE = 292,
     RETRY = 293,
     RETRY_COUNTER = 294,
     RETRY_INTERVAL = 295,
     RETRY_PERSEND = 296,
     RETRY_PHASE1 = 297,
     RETRY_PHASE2 = 298,
     NATT_KA = 299,
     ALGORITHM_CLASS = 300,
     ALGORITHMTYPE = 301,
     STRENGTHTYPE = 302,
     SAINFO = 303,
     FROM = 304,
     REMOTE = 305,
     ANONYMOUS = 306,
     INHERIT = 307,
     EXCHANGE_MODE = 308,
     EXCHANGETYPE = 309,
     DOI = 310,
     DOITYPE = 311,
     SITUATION = 312,
     SITUATIONTYPE = 313,
     CERTIFICATE_TYPE = 314,
     CERTTYPE = 315,
     PEERS_CERTFILE = 316,
     CA_TYPE = 317,
     VERIFY_CERT = 318,
     SEND_CERT = 319,
     SEND_CR = 320,
     IDENTIFIERTYPE = 321,
     MY_IDENTIFIER = 322,
     PEERS_IDENTIFIER = 323,
     VERIFY_IDENTIFIER = 324,
     DNSSEC = 325,
     CERT_X509 = 326,
     CERT_PLAINRSA = 327,
     NONCE_SIZE = 328,
     DH_GROUP = 329,
     KEEPALIVE = 330,
     PASSIVE = 331,
     INITIAL_CONTACT = 332,
     NAT_TRAVERSAL = 333,
     NAT_TRAVERSAL_LEVEL = 334,
     PROPOSAL_CHECK = 335,
     PROPOSAL_CHECK_LEVEL = 336,
     GENERATE_POLICY = 337,
     SUPPORT_PROXY = 338,
     PROPOSAL = 339,
     EXEC_PATH = 340,
     EXEC_COMMAND = 341,
     EXEC_SUCCESS = 342,
     EXEC_FAILURE = 343,
     GSSAPI_ID = 344,
     COMPLEX_BUNDLE = 345,
     DPD = 346,
     DPD_DELAY = 347,
     DPD_RETRY = 348,
     DPD_MAXFAIL = 349,
     PREFIX = 350,
     PORT = 351,
     PORTANY = 352,
     UL_PROTO = 353,
     ANY = 354,
     IKE_FRAG = 355,
     MODE_CFG = 356,
     PFS_GROUP = 357,
     LIFETIME = 358,
     LIFETYPE_TIME = 359,
     LIFETYPE_BYTE = 360,
     STRENGTH = 361,
     SCRIPT = 362,
     PHASE1_UP = 363,
     PHASE1_DOWN = 364,
     NUMBER = 365,
     SWITCH = 366,
     BOOLEAN = 367,
     HEXSTRING = 368,
     QUOTEDSTRING = 369,
     ADDRSTRING = 370,
     UNITTYPE_BYTE = 371,
     UNITTYPE_KBYTES = 372,
     UNITTYPE_MBYTES = 373,
     UNITTYPE_TBYTES = 374,
     UNITTYPE_SEC = 375,
     UNITTYPE_MIN = 376,
     UNITTYPE_HOUR = 377,
     EOS = 378,
     BOC = 379,
     EOC = 380,
     COMMA = 381
   };
#endif
#define PATH 258
#define PATHTYPE 259
#define INCLUDE 260
#define IDENTIFIER 261
#define VENDORID 262
#define LOGGING 263
#define LOGLEV 264
#define PADDING 265
#define PAD_RANDOMIZE 266
#define PAD_RANDOMIZELEN 267
#define PAD_MAXLEN 268
#define PAD_STRICT 269
#define PAD_EXCLTAIL 270
#define LISTEN 271
#define X_ISAKMP 272
#define X_ISAKMP_NATT 273
#define X_ADMIN 274
#define STRICT_ADDRESS 275
#define ADMINSOCK 276
#define DISABLED 277
#define MODECFG 278
#define CFG_NET4 279
#define CFG_MASK4 280
#define CFG_DNS4 281
#define CFG_NBNS4 282
#define CFG_AUTH_SOURCE 283
#define CFG_SYSTEM 284
#define CFG_RADIUS 285
#define CFG_LOCAL 286
#define CFG_NONE 287
#define CFG_ACCOUNTING 288
#define CFG_CONF_SOURCE 289
#define CFG_MOTD 290
#define CFG_POOL_SIZE 291
#define CFG_AUTH_THROTTLE 292
#define RETRY 293
#define RETRY_COUNTER 294
#define RETRY_INTERVAL 295
#define RETRY_PERSEND 296
#define RETRY_PHASE1 297
#define RETRY_PHASE2 298
#define NATT_KA 299
#define ALGORITHM_CLASS 300
#define ALGORITHMTYPE 301
#define STRENGTHTYPE 302
#define SAINFO 303
#define FROM 304
#define REMOTE 305
#define ANONYMOUS 306
#define INHERIT 307
#define EXCHANGE_MODE 308
#define EXCHANGETYPE 309
#define DOI 310
#define DOITYPE 311
#define SITUATION 312
#define SITUATIONTYPE 313
#define CERTIFICATE_TYPE 314
#define CERTTYPE 315
#define PEERS_CERTFILE 316
#define CA_TYPE 317
#define VERIFY_CERT 318
#define SEND_CERT 319
#define SEND_CR 320
#define IDENTIFIERTYPE 321
#define MY_IDENTIFIER 322
#define PEERS_IDENTIFIER 323
#define VERIFY_IDENTIFIER 324
#define DNSSEC 325
#define CERT_X509 326
#define CERT_PLAINRSA 327
#define NONCE_SIZE 328
#define DH_GROUP 329
#define KEEPALIVE 330
#define PASSIVE 331
#define INITIAL_CONTACT 332
#define NAT_TRAVERSAL 333
#define NAT_TRAVERSAL_LEVEL 334
#define PROPOSAL_CHECK 335
#define PROPOSAL_CHECK_LEVEL 336
#define GENERATE_POLICY 337
#define SUPPORT_PROXY 338
#define PROPOSAL 339
#define EXEC_PATH 340
#define EXEC_COMMAND 341
#define EXEC_SUCCESS 342
#define EXEC_FAILURE 343
#define GSSAPI_ID 344
#define COMPLEX_BUNDLE 345
#define DPD 346
#define DPD_DELAY 347
#define DPD_RETRY 348
#define DPD_MAXFAIL 349
#define PREFIX 350
#define PORT 351
#define PORTANY 352
#define UL_PROTO 353
#define ANY 354
#define IKE_FRAG 355
#define MODE_CFG 356
#define PFS_GROUP 357
#define LIFETIME 358
#define LIFETYPE_TIME 359
#define LIFETYPE_BYTE 360
#define STRENGTH 361
#define SCRIPT 362
#define PHASE1_UP 363
#define PHASE1_DOWN 364
#define NUMBER 365
#define SWITCH 366
#define BOOLEAN 367
#define HEXSTRING 368
#define QUOTEDSTRING 369
#define ADDRSTRING 370
#define UNITTYPE_BYTE 371
#define UNITTYPE_KBYTES 372
#define UNITTYPE_MBYTES 373
#define UNITTYPE_TBYTES 374
#define UNITTYPE_SEC 375
#define UNITTYPE_MIN 376
#define UNITTYPE_HOUR 377
#define EOS 378
#define BOC 379
#define EOC 380
#define COMMA 381




/* Copy the first part of user declarations.  */
#line 3 "./cfparse.y"

/*
 * Copyright (C) 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002 and 2003 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/socket.h>

#include <netinet/in.h>
#ifdef HAVE_NETINET6_IPSEC
#  include <netinet6/ipsec.h>
#else
#  include <netinet/ipsec.h>
#endif

#ifdef ENABLE_HYBRID
#include <arpa/inet.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <pwd.h>
#include <grp.h>

#include "var.h"
#include "misc.h"
#include "vmbuf.h"
#include "plog.h"
#include "sockmisc.h"
#include "str2val.h"
#include "genlist.h"
#include "debug.h"
#ifdef ENABLE_ADMINPORT
#include "admin.h"
#endif

#include "cfparse_proto.h"
#include "cftoken_proto.h"
#include "algorithm.h"
#include "localconf.h"
#include "policy.h"
#include "sainfo.h"
#include "oakley.h"
#include "pfkey.h"
#include "remoteconf.h"
#include "grabmyaddr.h"
#include "isakmp_var.h"
#include "handler.h"
#include "isakmp.h"
#ifdef ENABLE_HYBRID
#include "isakmp_xauth.h"
#include "isakmp_cfg.h"
#endif
#include "ipsec_doi.h"
#include "strnames.h"
#include "gcmalloc.h"
#ifdef HAVE_GSSAPI
#include "gssapi.h"
#endif
#include "vendorid.h"
#include "rsalist.h"

struct proposalspec {
	time_t lifetime;		/* for isakmp/ipsec */
	int lifebyte;			/* for isakmp/ipsec */
	struct secprotospec *spspec;	/* the head is always current spec. */
	struct proposalspec *next;	/* the tail is the most prefered. */
	struct proposalspec *prev;
};

struct secprotospec {
	int prop_no;
	int trns_no;
	int strength;		/* for isakmp/ipsec */
	int encklen;		/* for isakmp/ipsec */
	time_t lifetime;	/* for isakmp */
	int lifebyte;		/* for isakmp */
	int proto_id;		/* for ipsec (isakmp?) */
	int ipsec_level;	/* for ipsec */
	int encmode;		/* for ipsec */
	int vendorid;		/* for isakmp */
	char *gssid;
	struct sockaddr *remote;
	int algclass[MAXALGCLASS];

	struct secprotospec *next;	/* the tail is the most prefiered. */
	struct secprotospec *prev;
	struct proposalspec *back;
};

static int num2dhgroup[] = {
	0,
	OAKLEY_ATTR_GRP_DESC_MODP768,
	OAKLEY_ATTR_GRP_DESC_MODP1024,
	OAKLEY_ATTR_GRP_DESC_EC2N155,
	OAKLEY_ATTR_GRP_DESC_EC2N185,
	OAKLEY_ATTR_GRP_DESC_MODP1536,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	OAKLEY_ATTR_GRP_DESC_MODP2048,
	OAKLEY_ATTR_GRP_DESC_MODP3072,
	OAKLEY_ATTR_GRP_DESC_MODP4096,
	OAKLEY_ATTR_GRP_DESC_MODP6144,
	OAKLEY_ATTR_GRP_DESC_MODP8192
};

static struct remoteconf *cur_rmconf;
static int tmpalgtype[MAXALGCLASS];
static struct sainfo *cur_sainfo;
static int cur_algclass;

static struct proposalspec *newprspec __P((void));
static void insprspec __P((struct proposalspec *, struct proposalspec **));
static struct secprotospec *newspspec __P((void));
static void insspspec __P((struct secprotospec *, struct proposalspec **));
static void adminsock_conf __P((vchar_t *, vchar_t *, vchar_t *, int));

static int set_isakmp_proposal
	__P((struct remoteconf *, struct proposalspec *));
static void clean_tmpalgtype __P((void));
static int expand_isakmpspec __P((int, int, int *,
	int, int, time_t, int, int, int, char *, struct remoteconf *));
static int listen_addr __P((struct sockaddr *addr, int udp_encap));

void freeetypes (struct etypes **etypes);

#if 0
static int fix_lifebyte __P((u_long));
#endif


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 172 "./cfparse.y"
typedef union YYSTYPE {
	unsigned long num;
	vchar_t *val;
	struct remoteconf *rmconf;
	struct sockaddr *saddr;
	struct sainfoalg *alg;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 504 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 516 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   337

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  127
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  137
/* YYNRULES -- Number of rules. */
#define YYNRULES  251
/* YYNRULES -- Number of states. */
#define YYNSTATES  459

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   381

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    30,    36,    37,    42,
      46,    49,    50,    55,    56,    61,    65,    67,    69,    74,
      75,    78,    79,    84,    85,    90,    91,    96,    97,   102,
     103,   108,   113,   114,   117,   118,   123,   124,   129,   130,
     135,   136,   144,   145,   150,   151,   156,   157,   161,   164,
     165,   167,   172,   173,   176,   177,   182,   183,   188,   189,
     194,   195,   200,   201,   206,   207,   212,   213,   218,   219,
     224,   225,   230,   231,   236,   237,   242,   243,   248,   249,
     254,   259,   260,   263,   264,   269,   270,   276,   277,   282,
     283,   289,   290,   296,   297,   303,   304,   305,   314,   316,
     319,   325,   328,   329,   333,   334,   337,   338,   343,   344,
     351,   352,   359,   360,   365,   366,   371,   372,   378,   380,
     381,   386,   389,   390,   392,   393,   395,   397,   399,   401,
     403,   404,   406,   407,   414,   415,   420,   424,   427,   429,
     430,   433,   434,   439,   440,   445,   446,   451,   454,   455,
     460,   461,   467,   468,   474,   475,   481,   482,   487,   488,
     493,   494,   499,   500,   505,   506,   512,   513,   519,   520,
     525,   526,   531,   532,   537,   538,   543,   544,   549,   550,
     556,   557,   563,   564,   569,   570,   575,   576,   581,   582,
     587,   588,   593,   594,   599,   600,   605,   606,   611,   612,
     617,   618,   623,   624,   631,   632,   637,   638,   645,   646,
     652,   653,   656,   657,   663,   664,   669,   671,   673,   674,
     676,   678,   679,   682,   683,   688,   689,   696,   697,   704,
     705,   710,   711,   716,   717,   723,   725,   727,   729,   731,
     733,   735
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     128,     0,    -1,    -1,   128,   129,    -1,   130,    -1,   134,
      -1,   135,    -1,   139,    -1,   141,    -1,   149,    -1,   161,
      -1,   177,    -1,   186,    -1,   207,    -1,   132,    -1,    -1,
       3,     4,   114,   131,   123,    -1,    -1,    90,   111,   133,
     123,    -1,     5,   114,   123,    -1,     6,   136,    -1,    -1,
       7,   137,   114,   123,    -1,    -1,    66,   114,   138,   123,
      -1,     8,   140,   123,    -1,   113,    -1,     9,    -1,    10,
     124,   142,   125,    -1,    -1,   142,   143,    -1,    -1,    11,
     111,   144,   123,    -1,    -1,    12,   111,   145,   123,    -1,
      -1,    13,   110,   146,   123,    -1,    -1,    14,   111,   147,
     123,    -1,    -1,    15,   111,   148,   123,    -1,    16,   124,
     150,   125,    -1,    -1,   150,   151,    -1,    -1,    17,   159,
     152,   123,    -1,    -1,    18,   159,   153,   123,    -1,    -1,
      19,   154,    96,   123,    -1,    -1,    21,   114,   114,   114,
     110,   155,   123,    -1,    -1,    21,   114,   156,   123,    -1,
      -1,    21,    22,   157,   123,    -1,    -1,    20,   158,   123,
      -1,   115,   160,    -1,    -1,    96,    -1,    23,   124,   162,
     125,    -1,    -1,   162,   163,    -1,    -1,    24,   115,   164,
     123,    -1,    -1,    25,   115,   165,   123,    -1,    -1,    26,
     115,   166,   123,    -1,    -1,    27,   115,   167,   123,    -1,
      -1,    28,    29,   168,   123,    -1,    -1,    28,    30,   169,
     123,    -1,    -1,    33,    32,   170,   123,    -1,    -1,    33,
      30,   171,   123,    -1,    -1,    36,   110,   172,   123,    -1,
      -1,    37,   110,   173,   123,    -1,    -1,    34,    31,   174,
     123,    -1,    -1,    34,    30,   175,   123,    -1,    -1,    35,
     114,   176,   123,    -1,    38,   124,   178,   125,    -1,    -1,
     178,   179,    -1,    -1,    39,   110,   180,   123,    -1,    -1,
      40,   110,   262,   181,   123,    -1,    -1,    41,   110,   182,
     123,    -1,    -1,    42,   110,   262,   183,   123,    -1,    -1,
      43,   110,   262,   184,   123,    -1,    -1,    44,   110,   262,
     185,   123,    -1,    -1,    -1,    48,   187,   189,   191,   124,
     192,   188,   125,    -1,    51,    -1,   190,   190,    -1,    66,
     115,   203,   204,   205,    -1,    66,   114,    -1,    -1,    49,
      66,   253,    -1,    -1,   192,   193,    -1,    -1,   102,   252,
     194,   123,    -1,    -1,   103,   104,   110,   262,   195,   123,
      -1,    -1,   103,   105,   110,   263,   196,   123,    -1,    -1,
      45,   197,   200,   123,    -1,    -1,     6,    66,   198,   123,
      -1,    -1,    67,    66,   114,   199,   123,    -1,   202,    -1,
      -1,   202,   201,   126,   200,    -1,    46,   206,    -1,    -1,
      95,    -1,    -1,    96,    -1,    97,    -1,   110,    -1,    98,
      -1,    99,    -1,    -1,   110,    -1,    -1,    50,   211,    52,
     211,   208,   210,    -1,    -1,    50,   211,   209,   210,    -1,
     124,   212,   125,    -1,    51,   160,    -1,   159,    -1,    -1,
     212,   213,    -1,    -1,    53,   214,   248,   123,    -1,    -1,
      55,    56,   215,   123,    -1,    -1,    57,    58,   216,   123,
      -1,    59,   249,    -1,    -1,    61,   114,   217,   123,    -1,
      -1,    62,    71,   114,   218,   123,    -1,    -1,    61,    71,
     114,   219,   123,    -1,    -1,    61,    72,   114,   220,   123,
      -1,    -1,    61,    70,   221,   123,    -1,    -1,    63,   111,
     222,   123,    -1,    -1,    64,   111,   223,   123,    -1,    -1,
      65,   111,   224,   123,    -1,    -1,    67,    66,   253,   225,
     123,    -1,    -1,    68,    66,   253,   226,   123,    -1,    -1,
      69,   111,   227,   123,    -1,    -1,    73,   110,   228,   123,
      -1,    -1,    74,   229,   252,   123,    -1,    -1,    76,   111,
     230,   123,    -1,    -1,   100,   111,   231,   123,    -1,    -1,
     107,   114,   108,   232,   123,    -1,    -1,   107,   114,   109,
     233,   123,    -1,    -1,   101,   111,   234,   123,    -1,    -1,
      82,   111,   235,   123,    -1,    -1,    83,   111,   236,   123,
      -1,    -1,    77,   111,   237,   123,    -1,    -1,    78,   111,
     238,   123,    -1,    -1,    78,    79,   239,   123,    -1,    -1,
      91,   111,   240,   123,    -1,    -1,    92,   110,   241,   123,
      -1,    -1,    93,   110,   242,   123,    -1,    -1,    94,   110,
     243,   123,    -1,    -1,   103,   104,   110,   262,   244,   123,
      -1,    -1,    80,    81,   245,   123,    -1,    -1,   103,   105,
     110,   263,   246,   123,    -1,    -1,    84,   247,   124,   254,
     125,    -1,    -1,   248,    54,    -1,    -1,    71,   114,   114,
     250,   123,    -1,    -1,    72,   114,   251,   123,    -1,    46,
      -1,   110,    -1,    -1,   115,    -1,   114,    -1,    -1,   254,
     255,    -1,    -1,   106,   256,    47,   123,    -1,    -1,   103,
     104,   110,   262,   257,   123,    -1,    -1,   103,   105,   110,
     263,   258,   123,    -1,    -1,    74,   252,   259,   123,    -1,
      -1,    89,   114,   260,   123,    -1,    -1,    45,    46,   206,
     261,   123,    -1,   120,    -1,   121,    -1,   122,    -1,   116,
      -1,   117,    -1,   118,    -1,   119,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   247,   247,   249,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   268,   267,   287,   287,   292,
     306,   310,   309,   315,   314,   330,   333,   342,   355,   357,
     359,   362,   362,   363,   363,   364,   364,   365,   365,   366,
     366,   371,   373,   375,   379,   378,   384,   383,   393,   392,
     398,   397,   407,   406,   416,   415,   424,   424,   427,   439,
     440,   444,   446,   448,   452,   451,   463,   462,   474,   473,
     485,   484,   496,   495,   505,   504,   518,   517,   527,   526,
     540,   539,   549,   548,   558,   557,   567,   566,   580,   579,
     594,   596,   598,   602,   601,   607,   606,   612,   611,   617,
     616,   622,   621,   627,   626,   640,   648,   639,   682,   687,
     694,   743,   772,   776,   802,   804,   808,   807,   813,   812,
     818,   817,   829,   829,   834,   833,   839,   838,   846,   851,
     850,   857,   913,   914,   917,   918,   919,   922,   923,   924,
     927,   928,   934,   933,   960,   959,   983,  1054,  1060,  1069,
    1071,  1075,  1074,  1079,  1079,  1080,  1080,  1081,  1083,  1082,
    1092,  1091,  1100,  1099,  1107,  1106,  1130,  1129,  1139,  1139,
    1140,  1140,  1141,  1141,  1143,  1142,  1152,  1151,  1168,  1168,
    1169,  1169,  1171,  1170,  1176,  1176,  1177,  1177,  1178,  1178,
    1181,  1181,  1184,  1184,  1185,  1185,  1186,  1186,  1187,  1187,
    1189,  1188,  1197,  1196,  1205,  1204,  1213,  1212,  1222,  1221,
    1231,  1230,  1240,  1239,  1244,  1244,  1246,  1245,  1260,  1259,
    1270,  1272,  1296,  1295,  1305,  1304,  1326,  1334,  1346,  1347,
    1348,  1350,  1352,  1356,  1355,  1360,  1359,  1365,  1364,  1377,
    1376,  1382,  1381,  1391,  1390,  1488,  1489,  1490,  1493,  1494,
    1495,  1496
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PATH", "PATHTYPE", "INCLUDE", 
  "IDENTIFIER", "VENDORID", "LOGGING", "LOGLEV", "PADDING", 
  "PAD_RANDOMIZE", "PAD_RANDOMIZELEN", "PAD_MAXLEN", "PAD_STRICT", 
  "PAD_EXCLTAIL", "LISTEN", "X_ISAKMP", "X_ISAKMP_NATT", "X_ADMIN", 
  "STRICT_ADDRESS", "ADMINSOCK", "DISABLED", "MODECFG", "CFG_NET4", 
  "CFG_MASK4", "CFG_DNS4", "CFG_NBNS4", "CFG_AUTH_SOURCE", "CFG_SYSTEM", 
  "CFG_RADIUS", "CFG_LOCAL", "CFG_NONE", "CFG_ACCOUNTING", 
  "CFG_CONF_SOURCE", "CFG_MOTD", "CFG_POOL_SIZE", "CFG_AUTH_THROTTLE", 
  "RETRY", "RETRY_COUNTER", "RETRY_INTERVAL", "RETRY_PERSEND", 
  "RETRY_PHASE1", "RETRY_PHASE2", "NATT_KA", "ALGORITHM_CLASS", 
  "ALGORITHMTYPE", "STRENGTHTYPE", "SAINFO", "FROM", "REMOTE", 
  "ANONYMOUS", "INHERIT", "EXCHANGE_MODE", "EXCHANGETYPE", "DOI", 
  "DOITYPE", "SITUATION", "SITUATIONTYPE", "CERTIFICATE_TYPE", "CERTTYPE", 
  "PEERS_CERTFILE", "CA_TYPE", "VERIFY_CERT", "SEND_CERT", "SEND_CR", 
  "IDENTIFIERTYPE", "MY_IDENTIFIER", "PEERS_IDENTIFIER", 
  "VERIFY_IDENTIFIER", "DNSSEC", "CERT_X509", "CERT_PLAINRSA", 
  "NONCE_SIZE", "DH_GROUP", "KEEPALIVE", "PASSIVE", "INITIAL_CONTACT", 
  "NAT_TRAVERSAL", "NAT_TRAVERSAL_LEVEL", "PROPOSAL_CHECK", 
  "PROPOSAL_CHECK_LEVEL", "GENERATE_POLICY", "SUPPORT_PROXY", "PROPOSAL", 
  "EXEC_PATH", "EXEC_COMMAND", "EXEC_SUCCESS", "EXEC_FAILURE", 
  "GSSAPI_ID", "COMPLEX_BUNDLE", "DPD", "DPD_DELAY", "DPD_RETRY", 
  "DPD_MAXFAIL", "PREFIX", "PORT", "PORTANY", "UL_PROTO", "ANY", 
  "IKE_FRAG", "MODE_CFG", "PFS_GROUP", "LIFETIME", "LIFETYPE_TIME", 
  "LIFETYPE_BYTE", "STRENGTH", "SCRIPT", "PHASE1_UP", "PHASE1_DOWN", 
  "NUMBER", "SWITCH", "BOOLEAN", "HEXSTRING", "QUOTEDSTRING", 
  "ADDRSTRING", "UNITTYPE_BYTE", "UNITTYPE_KBYTES", "UNITTYPE_MBYTES", 
  "UNITTYPE_TBYTES", "UNITTYPE_SEC", "UNITTYPE_MIN", "UNITTYPE_HOUR", 
  "EOS", "BOC", "EOC", "COMMA", "$accept", "statements", "statement", 
  "path_statement", "@1", "special_statement", "@2", "include_statement", 
  "identifier_statement", "identifier_stmt", "@3", "@4", 
  "logging_statement", "log_level", "padding_statement", "padding_stmts", 
  "padding_stmt", "@5", "@6", "@7", "@8", "@9", "listen_statement", 
  "listen_stmts", "listen_stmt", "@10", "@11", "@12", "@13", "@14", "@15", 
  "@16", "ike_addrinfo_port", "ike_port", "modecfg_statement", 
  "modecfg_stmts", "modecfg_stmt", "@17", "@18", "@19", "@20", "@21", 
  "@22", "@23", "@24", "@25", "@26", "@27", "@28", "@29", 
  "timer_statement", "timer_stmts", "timer_stmt", "@30", "@31", "@32", 
  "@33", "@34", "@35", "sainfo_statement", "@36", "@37", "sainfo_name", 
  "sainfo_id", "sainfo_peer", "sainfo_specs", "sainfo_spec", "@38", "@39", 
  "@40", "@41", "@42", "@43", "algorithms", "@44", "algorithm", "prefix", 
  "port", "ul_proto", "keylength", "remote_statement", "@45", "@46", 
  "remote_specs_block", "remote_index", "remote_specs", "remote_spec", 
  "@47", "@48", "@49", "@50", "@51", "@52", "@53", "@54", "@55", "@56", 
  "@57", "@58", "@59", "@60", "@61", "@62", "@63", "@64", "@65", "@66", 
  "@67", "@68", "@69", "@70", "@71", "@72", "@73", "@74", "@75", "@76", 
  "@77", "@78", "@79", "@80", "exchange_types", "cert_spec", "@81", "@82", 
  "dh_group_num", "identifierstring", "isakmpproposal_specs", 
  "isakmpproposal_spec", "@83", "@84", "@85", "@86", "@87", "@88", 
  "unittype_time", "unittype_byte", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   127,   128,   128,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   131,   130,   133,   132,   134,
     135,   137,   136,   138,   136,   139,   140,   140,   141,   142,
     142,   144,   143,   145,   143,   146,   143,   147,   143,   148,
     143,   149,   150,   150,   152,   151,   153,   151,   154,   151,
     155,   151,   156,   151,   157,   151,   158,   151,   159,   160,
     160,   161,   162,   162,   164,   163,   165,   163,   166,   163,
     167,   163,   168,   163,   169,   163,   170,   163,   171,   163,
     172,   163,   173,   163,   174,   163,   175,   163,   176,   163,
     177,   178,   178,   180,   179,   181,   179,   182,   179,   183,
     179,   184,   179,   185,   179,   187,   188,   186,   189,   189,
     190,   190,   191,   191,   192,   192,   194,   193,   195,   193,
     196,   193,   197,   193,   198,   193,   199,   193,   200,   201,
     200,   202,   203,   203,   204,   204,   204,   205,   205,   205,
     206,   206,   208,   207,   209,   207,   210,   211,   211,   212,
     212,   214,   213,   215,   213,   216,   213,   213,   217,   213,
     218,   213,   219,   213,   220,   213,   221,   213,   222,   213,
     223,   213,   224,   213,   225,   213,   226,   213,   227,   213,
     228,   213,   229,   213,   230,   213,   231,   213,   232,   213,
     233,   213,   234,   213,   235,   213,   236,   213,   237,   213,
     238,   213,   239,   213,   240,   213,   241,   213,   242,   213,
     243,   213,   244,   213,   245,   213,   246,   213,   247,   213,
     248,   248,   250,   249,   251,   249,   252,   252,   253,   253,
     253,   254,   254,   256,   255,   257,   255,   258,   255,   259,
     255,   260,   255,   261,   255,   262,   262,   262,   263,   263,
     263,   263
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     5,     0,     4,     3,
       2,     0,     4,     0,     4,     3,     1,     1,     4,     0,
       2,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     4,     0,     2,     0,     4,     0,     4,     0,     4,
       0,     7,     0,     4,     0,     4,     0,     3,     2,     0,
       1,     4,     0,     2,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       4,     0,     2,     0,     4,     0,     5,     0,     4,     0,
       5,     0,     5,     0,     5,     0,     0,     8,     1,     2,
       5,     2,     0,     3,     0,     2,     0,     4,     0,     6,
       0,     6,     0,     4,     0,     4,     0,     5,     1,     0,
       4,     2,     0,     1,     0,     1,     1,     1,     1,     1,
       0,     1,     0,     6,     0,     4,     3,     2,     1,     0,
       2,     0,     4,     0,     4,     0,     4,     2,     0,     4,
       0,     5,     0,     5,     0,     5,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     5,     0,     5,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     5,
       0,     5,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     6,     0,     4,     0,     6,     0,     5,
       0,     2,     0,     5,     0,     4,     1,     1,     0,     1,
       1,     0,     2,     0,     4,     0,     6,     0,     6,     0,
       4,     0,     4,     0,     5,     1,     1,     1,     1,     1,
       1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,   105,     0,     0,     3,     4,    14,     5,     6,     7,
       8,     9,    10,    11,    12,    13,     0,     0,    21,     0,
      20,    27,    26,     0,    29,    42,    62,    91,     0,    59,
      59,   148,   144,    17,    15,    19,     0,    23,    25,     0,
       0,     0,     0,   108,     0,   112,     0,    60,   147,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,    30,     0,     0,    48,    56,     0,    41,    43,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      61,    63,     0,     0,     0,     0,     0,     0,    90,    92,
     111,   132,     0,     0,   109,   142,   149,   145,    18,    16,
      22,    24,    31,    33,    35,    37,    39,    44,    46,     0,
       0,    54,    52,    64,    66,    68,    70,    72,    74,    78,
      76,    86,    84,    88,    80,    82,    93,     0,    97,     0,
       0,     0,   133,   134,   228,   114,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    57,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   245,   246,   247,    95,     0,    99,
     101,   103,   135,   136,     0,   230,   229,   113,   106,   143,
     151,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   182,     0,     0,     0,     0,     0,     0,
     218,     0,     0,     0,     0,     0,     0,     0,     0,   146,
     150,    32,    34,    36,    38,    40,    45,    47,    49,    55,
       0,    53,    65,    67,    69,    71,    73,    75,    79,    77,
      87,    85,    89,    81,    83,    94,     0,    98,     0,     0,
       0,   138,   139,   137,   110,     0,   122,     0,     0,     0,
       0,   115,   220,   153,   155,     0,     0,   157,   166,     0,
       0,   158,     0,   168,   170,   172,   228,   228,   178,   180,
       0,   184,   198,   202,   200,   214,   194,   196,     0,   204,
     206,   208,   210,   186,   192,     0,     0,     0,    50,    96,
     100,   102,   104,   124,     0,     0,   226,   227,   116,     0,
       0,   107,     0,     0,     0,     0,   224,     0,   162,   164,
       0,   160,     0,     0,     0,   174,   176,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   231,     0,     0,
       0,     0,     0,     0,     0,     0,   188,   190,     0,     0,
     140,     0,   128,   126,     0,     0,     0,   221,   152,   154,
     156,   222,     0,   167,     0,     0,   159,     0,   169,   171,
     173,     0,     0,   179,   181,   183,   185,   199,   203,   201,
     215,   195,   197,     0,   205,   207,   209,   211,   187,   193,
     212,   248,   249,   250,   251,   216,     0,     0,    51,   125,
     141,   131,   123,     0,     0,   117,   118,   120,     0,   225,
     163,   165,   161,   175,   177,     0,     0,     0,     0,   233,
     219,   232,     0,     0,   189,   191,     0,   127,     0,     0,
     223,   140,   239,   241,     0,     0,     0,   213,   217,   130,
     119,   121,   243,     0,     0,     0,     0,     0,     0,   240,
     242,   235,   237,   234,   244,     0,     0,   236,   238
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,    14,    15,    63,    16,    62,    17,    18,    30,
      46,    65,    19,    33,    20,    49,    72,   148,   149,   150,
     151,   152,    21,    50,    79,   153,   154,   119,   348,   159,
     157,   120,    41,    58,    22,    51,    91,   160,   161,   162,
     163,   164,   165,   167,   166,   171,   172,   169,   168,   170,
      23,    52,    99,   173,   246,   178,   248,   249,   250,    24,
      38,   260,    55,    56,   103,   188,   261,   354,   428,   429,
     304,   349,   404,   351,   403,   352,   143,   184,   254,   401,
      25,   146,    61,   107,    42,   147,   220,   262,   313,   314,
     320,   367,   364,   365,   317,   322,   323,   324,   371,   372,
     327,   328,   280,   330,   342,   396,   397,   343,   335,   336,
     331,   333,   332,   338,   339,   340,   341,   422,   334,   423,
     288,   312,   267,   408,   362,   308,   187,   383,   421,   436,
     455,   456,   443,   444,   448,   177,   395
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -340
static const short yypact[] =
{
    -340,    33,  -340,    50,   -77,    20,     6,   -50,   -44,   -39,
     -32,  -340,   -33,    -9,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,    11,   -18,  -340,    13,
    -340,  -340,  -340,   -14,  -340,  -340,  -340,  -340,   -16,    16,
      16,  -340,    72,  -340,  -340,  -340,    15,  -340,  -340,    -7,
      -8,    -5,    18,  -340,   -67,    82,    67,  -340,  -340,  -340,
     -33,    21,    29,    31,    36,    37,    47,    51,    34,    52,
      57,  -340,  -340,    46,    46,  -340,  -340,    -6,  -340,  -340,
      54,    55,    56,    60,    35,    10,    42,    58,    38,    68,
    -340,  -340,    69,    71,    74,    75,    76,    77,  -340,  -340,
    -340,    87,   111,    59,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,    92,
      66,  -340,    78,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,   -53,  -340,   -53,
     -53,   -53,  -340,    -2,   -24,  -340,    21,    73,    70,    79,
      80,    81,    83,    84,    85,    86,  -340,    88,    96,    89,
      90,    91,    94,    95,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,  -340,  -340,  -340,  -340,   107,  -340,
    -340,  -340,  -340,  -340,   -47,  -340,  -340,  -340,     8,  -340,
    -340,   134,   133,    25,   -26,   123,   108,   120,   121,   129,
     130,   122,   124,  -340,   125,   126,   -45,   116,   127,   128,
    -340,   131,   135,   136,   137,   132,   138,    -4,   139,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
     140,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,   112,  -340,   117,   118,
     141,  -340,  -340,  -340,  -340,   149,  -340,   178,   -21,    -1,
     142,  -340,  -340,  -340,  -340,   143,   144,  -340,  -340,   145,
     146,  -340,   147,  -340,  -340,  -340,   -24,   -24,  -340,  -340,
     -21,  -340,  -340,  -340,  -340,  -340,  -340,  -340,   148,  -340,
    -340,  -340,  -340,  -340,  -340,   152,   153,   -10,  -340,  -340,
    -340,  -340,  -340,  -340,   154,   151,  -340,  -340,  -340,   156,
     158,  -340,   -30,   150,   155,   157,  -340,   159,  -340,  -340,
     160,  -340,   161,   162,   163,  -340,  -340,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,  -340,   174,   175,
     176,   177,   179,   180,   -53,   -40,  -340,  -340,   181,   182,
     191,   184,   183,  -340,   185,   -53,   -40,  -340,  -340,  -340,
    -340,  -340,   187,  -340,   188,   189,  -340,   190,  -340,  -340,
    -340,   192,   193,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,   -19,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,   194,   195,  -340,  -340,
    -340,  -340,  -340,   196,   197,  -340,  -340,  -340,   198,  -340,
    -340,  -340,  -340,  -340,  -340,   202,   -21,   200,     9,  -340,
    -340,  -340,   201,   203,  -340,  -340,   154,  -340,   204,   205,
    -340,   191,  -340,  -340,   209,   213,   207,  -340,  -340,  -340,
    -340,  -340,  -340,   206,   208,   -53,   -40,   210,   211,  -340,
    -340,  -340,  -340,  -340,  -340,   212,   214,  -340,  -340
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,    48,   215,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,   218,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -227,  -340,  -340,  -340,  -340,  -340,  -230,
    -340,  -340,  -340,   110,   216,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -277,  -161,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -139,  -339
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -130
static const short yytable[] =
{
     179,   180,   181,   329,    66,    67,    68,    69,    70,    73,
      74,    75,    76,    77,   255,    31,   121,   407,    39,    80,
      81,    82,    83,    84,   357,   306,   415,    28,    85,    86,
      87,    88,    89,     2,   283,    53,     3,    27,     4,     5,
     129,     6,   130,     7,   268,   269,   270,   100,   101,     8,
      54,   251,   252,   256,    26,   416,     9,    92,    93,    94,
      95,    96,    97,   253,   127,   128,   284,   174,   175,   176,
     417,    10,   131,   132,    34,   257,   391,   392,   393,   394,
      35,    11,    40,    12,   418,    36,    29,   419,   271,   307,
     185,   186,    37,   358,   182,   183,   265,   266,   346,   347,
     295,   296,    43,   309,   310,    45,   420,   452,   122,    48,
     258,   259,    57,   434,   435,   325,   326,    78,    71,    32,
      90,   117,   118,    13,    60,    44,   190,    47,   191,    64,
     192,   102,   193,    54,   194,   195,   196,   197,   198,   432,
     199,   200,   201,    98,   114,   106,   202,   203,   134,   204,
     205,   206,   108,   207,   109,   208,   209,   210,   112,   110,
     111,    40,   113,   115,   211,   212,   213,   214,   116,   123,
     124,   125,   133,   215,   216,   126,   217,   144,   135,   136,
     218,   137,   142,   145,   138,   139,   140,   141,   155,   156,
     263,   264,   158,   221,   272,   276,   277,   285,   219,   439,
     350,   442,   222,   223,   224,   390,   225,   226,   227,   228,
     230,   229,   231,   232,   233,   303,   406,   234,   235,   273,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     247,   274,   275,   278,   279,   299,   281,   282,   286,   287,
     300,   301,   289,   293,   305,   290,   291,   292,   431,   294,
     298,     0,     0,   297,   447,    59,   189,   315,   316,   318,
     319,   321,   344,   345,   302,   353,   355,   311,   356,     0,
       0,   361,   337,   359,   104,     0,   105,     0,   360,     0,
       0,     0,   363,   366,   368,   369,   370,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   384,   385,   386,
     387,   400,   388,   389,   398,   399,   451,   402,   405,  -129,
     409,   410,   411,   412,   433,   413,   414,   424,   425,   445,
     427,   430,   426,   446,   437,     0,   438,   440,   441,   449,
       0,   450,     0,   453,   454,   457,     0,   458
};

static const short yycheck[] =
{
     139,   140,   141,   280,    11,    12,    13,    14,    15,    17,
      18,    19,    20,    21,     6,     9,    22,   356,    51,    24,
      25,    26,    27,    28,    54,    46,    45,     7,    33,    34,
      35,    36,    37,     0,    79,    51,     3,   114,     5,     6,
      30,     8,    32,    10,    70,    71,    72,   114,   115,    16,
      66,    98,    99,    45,     4,    74,    23,    39,    40,    41,
      42,    43,    44,   110,    29,    30,   111,   120,   121,   122,
      89,    38,    30,    31,   124,    67,   116,   117,   118,   119,
     124,    48,   115,    50,   103,   124,    66,   106,   114,   110,
     114,   115,   124,   123,    96,    97,    71,    72,   108,   109,
     104,   105,   111,   104,   105,   123,   125,   446,   114,   123,
     102,   103,    96,   104,   105,   276,   277,   125,   125,   113,
     125,    73,    74,    90,    52,   114,    53,   114,    55,   114,
      57,    49,    59,    66,    61,    62,    63,    64,    65,   416,
      67,    68,    69,   125,   110,   124,    73,    74,   110,    76,
      77,    78,   123,    80,   123,    82,    83,    84,   111,   123,
     123,   115,   111,   111,    91,    92,    93,    94,   111,   115,
     115,   115,   114,   100,   101,   115,   103,    66,   110,   110,
     107,   110,    95,   124,   110,   110,   110,   110,    96,   123,
      56,    58,   114,   123,    71,    66,    66,    81,   125,   426,
      46,   431,   123,   123,   123,   344,   123,   123,   123,   123,
     114,   123,   123,   123,   123,    66,   355,   123,   123,   111,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   111,   111,   111,   110,   123,   111,   111,   111,   111,
     123,   123,   111,   111,    66,   110,   110,   110,    46,   111,
     110,    -1,    -1,   114,    47,    40,   146,   114,   114,   114,
     114,   114,   110,   110,   123,   114,   110,   125,   110,    -1,
      -1,   114,   124,   123,    56,    -1,    60,    -1,   123,    -1,
      -1,    -1,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   110,   123,   123,   123,   123,   445,   123,   123,   126,
     123,   123,   123,   123,   114,   123,   123,   123,   123,   110,
     123,   123,   126,   110,   123,    -1,   123,   123,   123,   123,
      -1,   123,    -1,   123,   123,   123,    -1,   123
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   128,     0,     3,     5,     6,     8,    10,    16,    23,
      38,    48,    50,    90,   129,   130,   132,   134,   135,   139,
     141,   149,   161,   177,   186,   207,     4,   114,     7,    66,
     136,     9,   113,   140,   124,   124,   124,   124,   187,    51,
     115,   159,   211,   111,   114,   123,   137,   114,   123,   142,
     150,   162,   178,    51,    66,   189,   190,    96,   160,   160,
      52,   209,   133,   131,   114,   138,    11,    12,    13,    14,
      15,   125,   143,    17,    18,    19,    20,    21,   125,   151,
      24,    25,    26,    27,    28,    33,    34,    35,    36,    37,
     125,   163,    39,    40,    41,    42,    43,    44,   125,   179,
     114,   115,    49,   191,   190,   211,   124,   210,   123,   123,
     123,   123,   111,   111,   110,   111,   111,   159,   159,   154,
     158,    22,   114,   115,   115,   115,   115,    29,    30,    30,
      32,    30,    31,   114,   110,   110,   110,   110,   110,   110,
     110,   110,    95,   203,    66,   124,   208,   212,   144,   145,
     146,   147,   148,   152,   153,    96,   123,   157,   114,   156,
     164,   165,   166,   167,   168,   169,   171,   170,   175,   174,
     176,   172,   173,   180,   120,   121,   122,   262,   182,   262,
     262,   262,    96,    97,   204,   114,   115,   253,   192,   210,
      53,    55,    57,    59,    61,    62,    63,    64,    65,    67,
      68,    69,    73,    74,    76,    77,    78,    80,    82,    83,
      84,    91,    92,    93,    94,   100,   101,   103,   107,   125,
     213,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     114,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   181,   123,   183,   184,
     185,    98,    99,   110,   205,     6,    45,    67,   102,   103,
     188,   193,   214,    56,    58,    71,    72,   249,    70,    71,
      72,   114,    71,   111,   111,   111,    66,    66,   111,   110,
     229,   111,   111,    79,   111,    81,   111,   111,   247,   111,
     110,   110,   110,   111,   111,   104,   105,   114,   110,   123,
     123,   123,   123,    66,   197,    66,    46,   110,   252,   104,
     105,   125,   248,   215,   216,   114,   114,   221,   114,   114,
     217,   114,   222,   223,   224,   253,   253,   227,   228,   252,
     230,   237,   239,   238,   245,   235,   236,   124,   240,   241,
     242,   243,   231,   234,   110,   110,   108,   109,   155,   198,
      46,   200,   202,   114,   194,   110,   110,    54,   123,   123,
     123,   114,   251,   123,   219,   220,   123,   218,   123,   123,
     123,   225,   226,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   254,   123,   123,   123,   123,   123,   123,
     262,   116,   117,   118,   119,   263,   232,   233,   123,   123,
     110,   206,   123,   201,   199,   123,   262,   263,   250,   123,
     123,   123,   123,   123,   123,    45,    74,    89,   103,   106,
     125,   255,   244,   246,   123,   123,   126,   123,   195,   196,
     123,    46,   252,   114,   104,   105,   256,   123,   123,   200,
     123,   123,   206,   259,   260,   110,   110,    47,   261,   123,
     123,   262,   263,   123,   123,   257,   258,   123,   123
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 15:
#line 268 "./cfparse.y"
    {
			if (yyvsp[-1].num >= LC_PATHTYPE_MAX) {
				yyerror("invalid path type %d", yyvsp[-1].num);
				return -1;
			}

			/* free old pathinfo */
			if (lcconf->pathinfo[yyvsp[-1].num])
				racoon_free(lcconf->pathinfo[yyvsp[-1].num]);

			/* set new pathinfo */
			lcconf->pathinfo[yyvsp[-1].num] = strdup(yyvsp[0].val->v);
			vfree(yyvsp[0].val);
		}
    break;

  case 17:
#line 287 "./cfparse.y"
    { lcconf->complex_bundle = yyvsp[0].num; }
    break;

  case 19:
#line 293 "./cfparse.y"
    {
			char path[MAXPATHLEN];

			getpathname(path, sizeof(path),
				LC_PATHTYPE_INCLUDE, yyvsp[-1].val->v);
			vfree(yyvsp[-1].val);
			if (yycf_switch_buffer(path) != 0)
				return -1;
		}
    break;

  case 21:
#line 310 "./cfparse.y"
    {
			/*XXX to be deleted */
		}
    break;

  case 23:
#line 315 "./cfparse.y"
    {
			/*XXX to be deleted */
			yyvsp[0].val->l--;	/* nuke '\0' */
			lcconf->ident[yyvsp[-1].num] = yyvsp[0].val;
			if (lcconf->ident[yyvsp[-1].num] == NULL) {
				yyerror("failed to set my ident: %s",
					strerror(errno));
				return -1;
			}
		}
    break;

  case 26:
#line 334 "./cfparse.y"
    {
			/*
			 * XXX ignore it because this specification
			 * will be obsoleted.
			 */
			yywarn("see racoon.conf(5), such a log specification will be obsoleted.");
			vfree(yyvsp[0].val);
		}
    break;

  case 27:
#line 343 "./cfparse.y"
    {
			/*
			 * set the loglevel by configuration file only when
			 * the command line did not specify any loglevel.
			 */
			if (loglevel <= LLV_BASE)
				loglevel += yyvsp[0].num;
		}
    break;

  case 31:
#line 362 "./cfparse.y"
    { lcconf->pad_random = yyvsp[0].num; }
    break;

  case 33:
#line 363 "./cfparse.y"
    { lcconf->pad_randomlen = yyvsp[0].num; }
    break;

  case 35:
#line 364 "./cfparse.y"
    { lcconf->pad_maxsize = yyvsp[0].num; }
    break;

  case 37:
#line 365 "./cfparse.y"
    { lcconf->pad_strict = yyvsp[0].num; }
    break;

  case 39:
#line 366 "./cfparse.y"
    { lcconf->pad_excltail = yyvsp[0].num; }
    break;

  case 44:
#line 379 "./cfparse.y"
    {
			listen_addr (yyvsp[0].saddr, 0);
		}
    break;

  case 46:
#line 384 "./cfparse.y"
    {
#ifdef ENABLE_NATT
			listen_addr (yyvsp[0].saddr, 1);
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
    break;

  case 48:
#line 393 "./cfparse.y"
    {
			yyerror("admin directive is obsoleted.");
		}
    break;

  case 50:
#line 398 "./cfparse.y"
    {
#ifdef ENABLE_ADMINPORT
			adminsock_conf(yyvsp[-3].val, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].num);
#else
			yywarn("admin port support not compiled in");
#endif
		}
    break;

  case 52:
#line 407 "./cfparse.y"
    {
#ifdef ENABLE_ADMINPORT
			adminsock_conf(yyvsp[0].val, NULL, NULL, -1);
#else
			yywarn("admin port support not compiled in");
#endif
		}
    break;

  case 54:
#line 416 "./cfparse.y"
    {
#ifdef ENABLE_ADMINPORT
			adminsock_path = NULL;
#else
			yywarn("admin port support not compiled in");
#endif
		}
    break;

  case 56:
#line 424 "./cfparse.y"
    { lcconf->strict_address = TRUE; }
    break;

  case 58:
#line 428 "./cfparse.y"
    {
			char portbuf[10];

			snprintf(portbuf, sizeof(portbuf), "%ld", yyvsp[0].num);
			yyval.saddr = str2saddr(yyvsp[-1].val->v, portbuf);
			vfree(yyvsp[-1].val);
			if (!yyval.saddr)
				return -1;
		}
    break;

  case 59:
#line 439 "./cfparse.y"
    { yyval.num = PORT_ISAKMP; }
    break;

  case 60:
#line 440 "./cfparse.y"
    { yyval.num = yyvsp[0].num; }
    break;

  case 64:
#line 452 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
		 if (inet_pton(AF_INET, yyvsp[0].val->v,
		     &isakmp_cfg_config.network4) != 1)
			yyerror("bad IPv4 network address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 66:
#line 463 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			if (inet_pton(AF_INET, yyvsp[0].val->v,
			    &isakmp_cfg_config.netmask4) != 1)
				yyerror("bad IPv4 netmask address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 68:
#line 474 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			if (inet_pton(AF_INET, yyvsp[0].val->v,
			    &isakmp_cfg_config.dns4) != 1)
				yyerror("bad IPv4 DNS address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 70:
#line 485 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			if (inet_pton(AF_INET, yyvsp[0].val->v,
			    &isakmp_cfg_config.nbns4) != 1)
				yyerror("bad IPv4 WINS address.");
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 72:
#line 496 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.authsource = ISAKMP_CFG_AUTH_SYSTEM;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 74:
#line 505 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.authsource = ISAKMP_CFG_AUTH_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 76:
#line 518 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.accounting = ISAKMP_CFG_ACCT_NONE;
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 78:
#line 527 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.accounting = ISAKMP_CFG_ACCT_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 80:
#line 540 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.pool_size = yyvsp[0].num;
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 82:
#line 549 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.auth_throttle = yyvsp[0].num;
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 84:
#line 558 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			isakmp_cfg_config.confsource = ISAKMP_CFG_CONF_LOCAL;
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 86:
#line 567 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
#ifdef HAVE_LIBRADIUS
			isakmp_cfg_config.confsource = ISAKMP_CFG_CONF_RADIUS;
#else /* HAVE_LIBRADIUS */
			yyerror("racoon not configured with --with-libradius");
#endif /* HAVE_LIBRADIUS */
#else /* ENABLE_HYBRID */
			yyerror("racoon not configured with --enable-hybrid");
#endif /* ENABLE_HYBRID */
		}
    break;

  case 88:
#line 580 "./cfparse.y"
    {
#ifdef ENABLE_HYBRID
			strncpy(&isakmp_cfg_config.motd[0], yyvsp[0].val->v, MAXPATHLEN);
			isakmp_cfg_config.motd[MAXPATHLEN] = '\0';
			vfree(yyvsp[0].val);
#else
			yyerror("racoon not configured with --enable-hybrid");
#endif
		}
    break;

  case 93:
#line 602 "./cfparse.y"
    {
			lcconf->retry_counter = yyvsp[0].num;
		}
    break;

  case 95:
#line 607 "./cfparse.y"
    {
			lcconf->retry_interval = yyvsp[-1].num * yyvsp[0].num;
		}
    break;

  case 97:
#line 612 "./cfparse.y"
    {
			lcconf->count_persend = yyvsp[0].num;
		}
    break;

  case 99:
#line 617 "./cfparse.y"
    {
			lcconf->retry_checkph1 = yyvsp[-1].num * yyvsp[0].num;
		}
    break;

  case 101:
#line 622 "./cfparse.y"
    {
			lcconf->wait_ph2complete = yyvsp[-1].num * yyvsp[0].num;
		}
    break;

  case 103:
#line 627 "./cfparse.y"
    {
#ifdef ENABLE_NATT
			lcconf->natt_ka_interval = yyvsp[-1].num * yyvsp[0].num;
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
    break;

  case 105:
#line 640 "./cfparse.y"
    {
			cur_sainfo = newsainfo();
			if (cur_sainfo == NULL) {
				yyerror("failed to allocate sainfo");
				return -1;
			}
		}
    break;

  case 106:
#line 648 "./cfparse.y"
    {
			struct sainfo *check;

			/* default */
			if (cur_sainfo->algs[algclass_ipsec_enc] == 0) {
				yyerror("no encryption algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}
			if (cur_sainfo->algs[algclass_ipsec_auth] == 0) {
				yyerror("no authentication algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}
			if (cur_sainfo->algs[algclass_ipsec_comp] == 0) {
				yyerror("no compression algorithm at %s",
					sainfo2str(cur_sainfo));
				return -1;
			}

			/* duplicate check */
			check = getsainfo(cur_sainfo->idsrc,
					  cur_sainfo->iddst,
					  cur_sainfo->id_i);
			if (check && (!check->idsrc && !cur_sainfo->idsrc)) {
				yyerror("duplicated sainfo: %s",
					sainfo2str(cur_sainfo));
				return -1;
			}
			inssainfo(cur_sainfo);
		}
    break;

  case 108:
#line 683 "./cfparse.y"
    {
			cur_sainfo->idsrc = NULL;
			cur_sainfo->iddst = NULL;
		}
    break;

  case 109:
#line 688 "./cfparse.y"
    {
			cur_sainfo->idsrc = yyvsp[-1].val;
			cur_sainfo->iddst = yyvsp[0].val;
		}
    break;

  case 110:
#line 695 "./cfparse.y"
    {
			char portbuf[10];
			struct sockaddr *saddr;

			if ((yyvsp[0].num == IPPROTO_ICMP || yyvsp[0].num == IPPROTO_ICMPV6)
			 && (yyvsp[-1].num != IPSEC_PORT_ANY || yyvsp[-1].num != IPSEC_PORT_ANY)) {
				yyerror("port number must be \"any\".");
				return -1;
			}

			snprintf(portbuf, sizeof(portbuf), "%lu", yyvsp[-1].num);
			saddr = str2saddr(yyvsp[-3].val->v, portbuf);
			vfree(yyvsp[-3].val);
			if (saddr == NULL)
				return -1;

			switch (saddr->sa_family) {
			case AF_INET:
				if (yyvsp[0].num == IPPROTO_ICMPV6) {
					yyerror("upper layer protocol mismatched.\n");
					racoon_free(saddr);
					return -1;
				}
				yyval.val = ipsecdoi_sockaddr2id(saddr,
					yyvsp[-2].num == ~0 ? (sizeof(struct in_addr) << 3): yyvsp[-2].num,
					yyvsp[0].num);
				break;
#ifdef INET6
			case AF_INET6:
				if (yyvsp[0].num == IPPROTO_ICMP) {
					yyerror("upper layer protocol mismatched.\n");
					racoon_free(saddr);
					return -1;
				}
				yyval.val = ipsecdoi_sockaddr2id(saddr,
					yyvsp[-2].num == ~0 ? (sizeof(struct in6_addr) << 3) : yyvsp[-2].num,
					yyvsp[0].num);
				break;
#endif
			default:
				yyerror("invalid family: %d", saddr->sa_family);
				yyval.val = NULL;
				break;
			}
			racoon_free(saddr);
			if (yyval.val == NULL)
				return -1;
		}
    break;

  case 111:
#line 744 "./cfparse.y"
    {
			struct ipsecdoi_id_b *id_b;

			if (yyvsp[-1].num == IDTYPE_ASN1DN) {
				yyerror("id type forbidden: %d", yyvsp[-1].num);
				yyval.val = NULL;
				return -1;
			}

			yyvsp[0].val->l--;

			yyval.val = vmalloc(sizeof(*id_b) + yyvsp[0].val->l);
			if (yyval.val == NULL) {
				yyerror("failed to allocate identifier");
				return -1;
			}

			id_b = (struct ipsecdoi_id_b *)yyval.val->v;
			id_b->type = idtype2doi(yyvsp[-1].num);

			id_b->proto_id = 0;
			id_b->port = 0;

			memcpy(yyval.val->v + sizeof(*id_b), yyvsp[0].val->v, yyvsp[0].val->l);
		}
    break;

  case 112:
#line 772 "./cfparse.y"
    {
			cur_sainfo->id_i = NULL;
		}
    break;

  case 113:
#line 777 "./cfparse.y"
    {
			struct ipsecdoi_id_b *id_b;
			vchar_t *idv;

			if (set_identifier(&idv, yyvsp[-1].num, yyvsp[0].val) != 0) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
			cur_sainfo->id_i = vmalloc(sizeof(*id_b) + idv->l);
			if (cur_sainfo->id_i == NULL) {
				yyerror("failed to allocate identifier");
				return -1;
			}

			id_b = (struct ipsecdoi_id_b *)cur_sainfo->id_i->v;
			id_b->type = idtype2doi(yyvsp[-1].num);

			id_b->proto_id = 0;
			id_b->port = 0;

			memcpy(cur_sainfo->id_i->v + sizeof(*id_b),
			       idv->v, idv->l);
			vfree(idv);
		}
    break;

  case 116:
#line 808 "./cfparse.y"
    {
			cur_sainfo->pfs_group = yyvsp[0].num;
		}
    break;

  case 118:
#line 813 "./cfparse.y"
    {
			cur_sainfo->lifetime = yyvsp[-1].num * yyvsp[0].num;
		}
    break;

  case 120:
#line 818 "./cfparse.y"
    {
#if 1
			yyerror("byte lifetime support is deprecated");
			return -1;
#else
			cur_sainfo->lifebyte = fix_lifebyte(yyvsp[-1].num * yyvsp[0].num);
			if (cur_sainfo->lifebyte == 0)
				return -1;
#endif
		}
    break;

  case 122:
#line 829 "./cfparse.y"
    {
			cur_algclass = yyvsp[0].num;
		}
    break;

  case 124:
#line 834 "./cfparse.y"
    {
			yyerror("it's deprecated to specify a identifier in phase 2");
		}
    break;

  case 126:
#line 839 "./cfparse.y"
    {
			yyerror("it's deprecated to specify a identifier in phase 2");
		}
    break;

  case 128:
#line 847 "./cfparse.y"
    {
			inssainfoalg(&cur_sainfo->algs[cur_algclass], yyvsp[0].alg);
		}
    break;

  case 129:
#line 851 "./cfparse.y"
    {
			inssainfoalg(&cur_sainfo->algs[cur_algclass], yyvsp[0].alg);
		}
    break;

  case 131:
#line 858 "./cfparse.y"
    {
			int defklen;

			yyval.alg = newsainfoalg();
			if (yyval.alg == NULL) {
				yyerror("failed to get algorithm allocation");
				return -1;
			}

			yyval.alg->alg = algtype2doi(cur_algclass, yyvsp[-1].num);
			if (yyval.alg->alg == -1) {
				yyerror("algorithm mismatched");
				racoon_free(yyval.alg);
				yyval.alg = NULL;
				return -1;
			}

			defklen = default_keylen(cur_algclass, yyvsp[-1].num);
			if (defklen == 0) {
				if (yyvsp[0].num) {
					yyerror("keylen not allowed");
					racoon_free(yyval.alg);
					yyval.alg = NULL;
					return -1;
				}
			} else {
				if (yyvsp[0].num && check_keylen(cur_algclass, yyvsp[-1].num, yyvsp[0].num) < 0) {
					yyerror("invalid keylen %d", yyvsp[0].num);
					racoon_free(yyval.alg);
					yyval.alg = NULL;
					return -1;
				}
			}

			if (yyvsp[0].num)
				yyval.alg->encklen = yyvsp[0].num;
			else
				yyval.alg->encklen = defklen;

			/* check if it's supported algorithm by kernel */
			if (!(cur_algclass == algclass_ipsec_auth && yyvsp[-1].num == algtype_non_auth)
			 && pk_checkalg(cur_algclass, yyvsp[-1].num, yyval.alg->encklen)) {
				int a = algclass2doi(cur_algclass);
				int b = algtype2doi(cur_algclass, yyvsp[-1].num);
				if (a == IPSECDOI_ATTR_AUTH)
					a = IPSECDOI_PROTO_IPSEC_AH;
				yyerror("algorithm %s not supported by the kernel (missing module?)",
					s_ipsecdoi_trns(a, b));
				racoon_free(yyval.alg);
				yyval.alg = NULL;
				return -1;
			}
		}
    break;

  case 132:
#line 913 "./cfparse.y"
    { yyval.num = ~0; }
    break;

  case 133:
#line 914 "./cfparse.y"
    { yyval.num = yyvsp[0].num; }
    break;

  case 134:
#line 917 "./cfparse.y"
    { yyval.num = IPSEC_PORT_ANY; }
    break;

  case 135:
#line 918 "./cfparse.y"
    { yyval.num = yyvsp[0].num; }
    break;

  case 136:
#line 919 "./cfparse.y"
    { yyval.num = IPSEC_PORT_ANY; }
    break;

  case 137:
#line 922 "./cfparse.y"
    { yyval.num = yyvsp[0].num; }
    break;

  case 138:
#line 923 "./cfparse.y"
    { yyval.num = yyvsp[0].num; }
    break;

  case 139:
#line 924 "./cfparse.y"
    { yyval.num = IPSEC_ULPROTO_ANY; }
    break;

  case 140:
#line 927 "./cfparse.y"
    { yyval.num = 0; }
    break;

  case 141:
#line 928 "./cfparse.y"
    { yyval.num = yyvsp[0].num; }
    break;

  case 142:
#line 934 "./cfparse.y"
    {
			struct remoteconf *new;
			struct proposalspec *prspec;

			new = copyrmconf(yyvsp[0].saddr);
			if (new == NULL) {
				yyerror("failed to get remoteconf for %s.", saddr2str (yyvsp[0].saddr));
				return -1;
			}

			new->remote = yyvsp[-2].saddr;
			new->inherited_from = getrmconf_strict(yyvsp[0].saddr, 1);
			new->proposal = NULL;
			new->prhead = NULL;
			cur_rmconf = new;

			prspec = newprspec();
			if (prspec == NULL || !cur_rmconf->inherited_from 
				|| !cur_rmconf->inherited_from->proposal)
				return -1;
			prspec->lifetime = cur_rmconf->inherited_from->proposal->lifetime;
			prspec->lifebyte = cur_rmconf->inherited_from->proposal->lifebyte;
			insprspec(prspec, &cur_rmconf->prhead);
		}
    break;

  case 144:
#line 960 "./cfparse.y"
    {
			struct remoteconf *new;
			struct proposalspec *prspec;

			new = newrmconf();
			if (new == NULL) {
				yyerror("failed to get new remoteconf.");
				return -1;
			}

			new->remote = yyvsp[0].saddr;
			cur_rmconf = new;

			prspec = newprspec();
			if (prspec == NULL)
				return -1;
			prspec->lifetime = oakley_get_defaultlifetime();
			insprspec(prspec, &cur_rmconf->prhead);
		}
    break;

  case 146:
#line 984 "./cfparse.y"
    {
			/* check a exchange mode */
			if (cur_rmconf->etypes == NULL) {
				yyerror("no exchange mode specified.\n");
				return -1;
			}

			if (cur_rmconf->idvtype == IDTYPE_UNDEFINED)
				cur_rmconf->idvtype = IDTYPE_ADDRESS;


			if (cur_rmconf->idvtype == IDTYPE_ASN1DN) {
				if (cur_rmconf->mycertfile) {
					if (cur_rmconf->idv)
						yywarn("Both CERT and ASN1 ID "
						       "are set. Hope this is OK.\n");
					/* TODO: Preparse the DN here */
				} else if (cur_rmconf->idv) {
					/* OK, using asn1dn without X.509. */
				} else {
					yyerror("ASN1 ID not specified "
						"and no CERT defined!\n");
					return -1;
				}
			}
			
			if (cur_rmconf->prhead->spspec == NULL
				&& cur_rmconf->inherited_from
				&& cur_rmconf->inherited_from->prhead) {
				cur_rmconf->prhead->spspec = cur_rmconf->inherited_from->prhead->spspec;
			}
			if (set_isakmp_proposal(cur_rmconf, cur_rmconf->prhead) != 0)
				return -1;

			/* DH group settting if aggressive mode is there. */
			if (check_etypeok(cur_rmconf, ISAKMP_ETYPE_AGG) != NULL) {
				struct isakmpsa *p;
				int b = 0;

				/* DH group */
				for (p = cur_rmconf->proposal; p; p = p->next) {
					if (b == 0 || (b && b == p->dh_group)) {
						b = p->dh_group;
						continue;
					}
					yyerror("DH group must be equal "
						"in all proposals "
						"when aggressive mode is "
						"used.\n");
					return -1;
				}
				cur_rmconf->dh_group = b;

				if (cur_rmconf->dh_group == 0) {
					yyerror("DH group must be set in the proposal.\n");
					return -1;
				}

				/* DH group settting if PFS is required. */
				if (oakley_setdhgroup(cur_rmconf->dh_group,
						&cur_rmconf->dhgrp) < 0) {
					yyerror("failed to set DH value.\n");
					return -1;
				}
			}

			insrmconf(cur_rmconf);
		}
    break;

  case 147:
#line 1055 "./cfparse.y"
    {
			yyval.saddr = newsaddr(sizeof(struct sockaddr *));
			yyval.saddr->sa_family = AF_UNSPEC;
			((struct sockaddr_in *)yyval.saddr)->sin_port = htons(yyvsp[0].num);
		}
    break;

  case 148:
#line 1061 "./cfparse.y"
    {
			yyval.saddr = yyvsp[0].saddr;
			if (yyval.saddr == NULL) {
				yyerror("failed to allocate sockaddr");
				return -1;
			}
		}
    break;

  case 151:
#line 1075 "./cfparse.y"
    {
			cur_rmconf->etypes = NULL;
		}
    break;

  case 153:
#line 1079 "./cfparse.y"
    { cur_rmconf->doitype = yyvsp[0].num; }
    break;

  case 155:
#line 1080 "./cfparse.y"
    { cur_rmconf->sittype = yyvsp[0].num; }
    break;

  case 158:
#line 1083 "./cfparse.y"
    {
			yywarn("This directive without certtype will be removed!\n");
			yywarn("Please use 'peers_certfile x509 \"%s\";' instead\n", yyvsp[0].val->v);
			cur_rmconf->getcert_method = ISAKMP_GETCERT_LOCALFILE;
			cur_rmconf->peerscertfile = strdup(yyvsp[0].val->v);
			vfree(yyvsp[0].val);
		}
    break;

  case 160:
#line 1092 "./cfparse.y"
    {
			cur_rmconf->cacerttype = yyvsp[-1].num;
			cur_rmconf->getcacert_method = ISAKMP_GETCERT_LOCALFILE;
			cur_rmconf->cacertfile = strdup(yyvsp[0].val->v);
			vfree(yyvsp[0].val);
		}
    break;

  case 162:
#line 1100 "./cfparse.y"
    {
			cur_rmconf->getcert_method = ISAKMP_GETCERT_LOCALFILE;
			cur_rmconf->peerscertfile = strdup(yyvsp[0].val->v);
			vfree(yyvsp[0].val);
		}
    break;

  case 164:
#line 1107 "./cfparse.y"
    {
			char path[MAXPATHLEN];
			int ret = 0;

			getpathname(path, sizeof(path),
				LC_PATHTYPE_CERT, yyvsp[0].val->v);
			vfree(yyvsp[0].val);

			if (cur_rmconf->getcert_method == ISAKMP_GETCERT_DNS) {
				yyerror("Different peers_certfile method "
					"already defined: %d!\n",
					cur_rmconf->getcert_method);
				return -1;
			}
			cur_rmconf->getcert_method = ISAKMP_GETCERT_LOCALFILE;
			if (rsa_parse_file(cur_rmconf->rsa_public, path, RSA_TYPE_PUBLIC)) {
				yyerror("Couldn't parse keyfile.\n", path);
				return -1;
			}
			plog(LLV_DEBUG, LOCATION, NULL, "Public PlainRSA keyfile parsed: %s\n", path);
		}
    break;

  case 166:
#line 1130 "./cfparse.y"
    {
			if (cur_rmconf->getcert_method) {
				yyerror("Different peers_certfile method already defined!\n");
				return -1;
			}
			cur_rmconf->getcert_method = ISAKMP_GETCERT_DNS;
			cur_rmconf->peerscertfile = NULL;
		}
    break;

  case 168:
#line 1139 "./cfparse.y"
    { cur_rmconf->verify_cert = yyvsp[0].num; }
    break;

  case 170:
#line 1140 "./cfparse.y"
    { cur_rmconf->send_cert = yyvsp[0].num; }
    break;

  case 172:
#line 1141 "./cfparse.y"
    { cur_rmconf->send_cr = yyvsp[0].num; }
    break;

  case 174:
#line 1143 "./cfparse.y"
    {
			if (set_identifier(&cur_rmconf->idv, yyvsp[-1].num, yyvsp[0].val) != 0) {
				yyerror("failed to set identifer.\n");
				return -1;
			}
			cur_rmconf->idvtype = yyvsp[-1].num;
		}
    break;

  case 176:
#line 1152 "./cfparse.y"
    {
			struct idspec  *id;
			id = newidspec();
			if (id == NULL) {
				yyerror("failed to allocate idspec");
				return -1;
			}
			if (set_identifier(&id->id, yyvsp[-1].num, yyvsp[0].val) != 0) {
				yyerror("failed to set identifer.\n");
				racoon_free(id);
				return -1;
			}
			id->idtype = yyvsp[-1].num;
			genlist_append (cur_rmconf->idvl_p, id);
		}
    break;

  case 178:
#line 1168 "./cfparse.y"
    { cur_rmconf->verify_identifier = yyvsp[0].num; }
    break;

  case 180:
#line 1169 "./cfparse.y"
    { cur_rmconf->nonce_size = yyvsp[0].num; }
    break;

  case 182:
#line 1171 "./cfparse.y"
    {
			yyerror("dh_group cannot be defined here.");
			return -1;
		}
    break;

  case 184:
#line 1176 "./cfparse.y"
    { cur_rmconf->passive = yyvsp[0].num; }
    break;

  case 186:
#line 1177 "./cfparse.y"
    { cur_rmconf->ike_frag = yyvsp[0].num; }
    break;

  case 188:
#line 1178 "./cfparse.y"
    { 
			cur_rmconf->script[SCRIPT_PHASE1_UP] = vdup(yyvsp[-1].val);
		}
    break;

  case 190:
#line 1181 "./cfparse.y"
    { 
			cur_rmconf->script[SCRIPT_PHASE1_DOWN] = vdup(yyvsp[-1].val);
		}
    break;

  case 192:
#line 1184 "./cfparse.y"
    { cur_rmconf->mode_cfg = yyvsp[0].num; }
    break;

  case 194:
#line 1185 "./cfparse.y"
    { cur_rmconf->gen_policy = yyvsp[0].num; }
    break;

  case 196:
#line 1186 "./cfparse.y"
    { cur_rmconf->support_proxy = yyvsp[0].num; }
    break;

  case 198:
#line 1187 "./cfparse.y"
    { cur_rmconf->ini_contact = yyvsp[0].num; }
    break;

  case 200:
#line 1189 "./cfparse.y"
    {
#ifdef ENABLE_NATT
			cur_rmconf->nat_traversal = yyvsp[0].num;
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
    break;

  case 202:
#line 1197 "./cfparse.y"
    {
#ifdef ENABLE_NATT
			cur_rmconf->nat_traversal = yyvsp[0].num;
#else
			yyerror("NAT-T support not compiled in.");
#endif
		}
    break;

  case 204:
#line 1205 "./cfparse.y"
    {
#ifdef ENABLE_DPD
			cur_rmconf->dpd = yyvsp[0].num;
#else
			yyerror("DPD support not compiled in.");
#endif
		}
    break;

  case 206:
#line 1213 "./cfparse.y"
    {
#ifdef ENABLE_DPD
			cur_rmconf->dpd_interval = yyvsp[0].num;
#else
			yyerror("DPD support not compiled in.");
#endif
		}
    break;

  case 208:
#line 1222 "./cfparse.y"
    {
#ifdef ENABLE_DPD
			cur_rmconf->dpd_retry = yyvsp[0].num;
#else
			yyerror("DPD support not compiled in.");
#endif
		}
    break;

  case 210:
#line 1231 "./cfparse.y"
    {
#ifdef ENABLE_DPD
			cur_rmconf->dpd_maxfails = yyvsp[0].num;
#else
			yyerror("DPD support not compiled in.");
#endif
		}
    break;

  case 212:
#line 1240 "./cfparse.y"
    {
			cur_rmconf->prhead->lifetime = yyvsp[-1].num * yyvsp[0].num;
		}
    break;

  case 214:
#line 1244 "./cfparse.y"
    { cur_rmconf->pcheck_level = yyvsp[0].num; }
    break;

  case 216:
#line 1246 "./cfparse.y"
    {
#if 1
			yyerror("byte lifetime support is deprecated in Phase1");
			return -1;
#else
			yywarn("the lifetime of bytes in phase 1 "
				"will be ignored at the moment.");
			cur_rmconf->prhead->lifebyte = fix_lifebyte(yyvsp[-1].num * yyvsp[0].num);
			if (cur_rmconf->prhead->lifebyte == 0)
				return -1;
#endif
		}
    break;

  case 218:
#line 1260 "./cfparse.y"
    {
			struct secprotospec *spspec;

			spspec = newspspec();
			if (spspec == NULL)
				return -1;
			insspspec(spspec, &cur_rmconf->prhead);
		}
    break;

  case 221:
#line 1273 "./cfparse.y"
    {
			struct etypes *new;
			new = racoon_malloc(sizeof(struct etypes));
			if (new == NULL) {
				yyerror("filed to allocate etypes");
				return -1;
			}
			new->type = yyvsp[0].num;
			new->next = NULL;
			if (cur_rmconf->etypes == NULL)
				cur_rmconf->etypes = new;
			else {
				struct etypes *p;
				for (p = cur_rmconf->etypes;
				     p->next != NULL;
				     p = p->next)
					;
				p->next = new;
			}
		}
    break;

  case 222:
#line 1296 "./cfparse.y"
    {
			cur_rmconf->certtype = yyvsp[-2].num;
			cur_rmconf->mycertfile = strdup(yyvsp[-1].val->v);
			vfree(yyvsp[-1].val);
			cur_rmconf->myprivfile = strdup(yyvsp[0].val->v);
			vfree(yyvsp[0].val);
		}
    break;

  case 224:
#line 1305 "./cfparse.y"
    {
			char path[MAXPATHLEN];
			int ret = 0;

			getpathname(path, sizeof(path),
				LC_PATHTYPE_CERT, yyvsp[0].val->v);
			vfree(yyvsp[0].val);

			cur_rmconf->certtype = yyvsp[-1].num;
			cur_rmconf->send_cr = FALSE;
			cur_rmconf->send_cert = FALSE;
			cur_rmconf->verify_cert = FALSE;
			if (rsa_parse_file(cur_rmconf->rsa_private, path, RSA_TYPE_PRIVATE)) {
				yyerror("Couldn't parse keyfile.\n", path);
				return -1;
			}
			plog(LLV_DEBUG, LOCATION, NULL, "Private PlainRSA keyfile parsed: %s\n", path);
		}
    break;

  case 226:
#line 1327 "./cfparse.y"
    {
			yyval.num = algtype2doi(algclass_isakmp_dh, yyvsp[0].num);
			if (yyval.num == -1) {
				yyerror("must be DH group");
				return -1;
			}
		}
    break;

  case 227:
#line 1335 "./cfparse.y"
    {
			if (ARRAYLEN(num2dhgroup) > yyvsp[0].num && num2dhgroup[yyvsp[0].num] != 0) {
				yyval.num = num2dhgroup[yyvsp[0].num];
			} else {
				yyerror("must be DH group");
				yyval.num = 0;
				return -1;
			}
		}
    break;

  case 228:
#line 1346 "./cfparse.y"
    { yyval.val = NULL; }
    break;

  case 229:
#line 1347 "./cfparse.y"
    { yyval.val = yyvsp[0].val; }
    break;

  case 230:
#line 1348 "./cfparse.y"
    { yyval.val = yyvsp[0].val; }
    break;

  case 233:
#line 1356 "./cfparse.y"
    {
			yyerror("strength directive is obsoleted.");
		}
    break;

  case 235:
#line 1360 "./cfparse.y"
    {
			cur_rmconf->prhead->spspec->lifetime = yyvsp[-1].num * yyvsp[0].num;
		}
    break;

  case 237:
#line 1365 "./cfparse.y"
    {
#if 1
			yyerror("byte lifetime support is deprecated");
			return -1;
#else
			cur_rmconf->prhead->spspec->lifebyte = fix_lifebyte(yyvsp[-1].num * yyvsp[0].num);
			if (cur_rmconf->prhead->spspec->lifebyte == 0)
				return -1;
#endif
		}
    break;

  case 239:
#line 1377 "./cfparse.y"
    {
			cur_rmconf->prhead->spspec->algclass[algclass_isakmp_dh] = yyvsp[0].num;
		}
    break;

  case 241:
#line 1382 "./cfparse.y"
    {
			if (cur_rmconf->prhead->spspec->vendorid != VENDORID_GSSAPI) {
				yyerror("wrong Vendor ID for gssapi_id");
				return -1;
			}
			cur_rmconf->prhead->spspec->gssid = strdup(yyvsp[0].val->v);
		}
    break;

  case 243:
#line 1391 "./cfparse.y"
    {
			int doi;
			int defklen;

			doi = algtype2doi(yyvsp[-2].num, yyvsp[-1].num);
			if (doi == -1) {
				yyerror("algorithm mismatched 1");
				return -1;
			}

			switch (yyvsp[-2].num) {
			case algclass_isakmp_enc:
			/* reject suppressed algorithms */
#ifndef HAVE_OPENSSL_RC5_H
				if (yyvsp[-1].num == algtype_rc5) {
					yyerror("algorithm %s not supported",
					    s_attr_isakmp_enc(doi));
					return -1;
				}
#endif
#ifndef HAVE_OPENSSL_IDEA_H
				if (yyvsp[-1].num == algtype_idea) {
					yyerror("algorithm %s not supported",
					    s_attr_isakmp_enc(doi));
					return -1;
				}
#endif

				cur_rmconf->prhead->spspec->algclass[algclass_isakmp_enc] = doi;
				defklen = default_keylen(yyvsp[-2].num, yyvsp[-1].num);
				if (defklen == 0) {
					if (yyvsp[0].num) {
						yyerror("keylen not allowed");
						return -1;
					}
				} else {
					if (yyvsp[0].num && check_keylen(yyvsp[-2].num, yyvsp[-1].num, yyvsp[0].num) < 0) {
						yyerror("invalid keylen %d", yyvsp[0].num);
						return -1;
					}
				}
				if (yyvsp[0].num)
					cur_rmconf->prhead->spspec->encklen = yyvsp[0].num;
				else
					cur_rmconf->prhead->spspec->encklen = defklen;
				break;
			case algclass_isakmp_hash:
				cur_rmconf->prhead->spspec->algclass[algclass_isakmp_hash] = doi;
				break;
			case algclass_isakmp_ameth:
				cur_rmconf->prhead->spspec->algclass[algclass_isakmp_ameth] = doi;
				/*
				 * We may have to set the Vendor ID for the
				 * authentication method we're using.
				 */
				switch (yyvsp[-1].num) {
				case algtype_gssapikrb:
					if (cur_rmconf->prhead->spspec->vendorid !=
					    VENDORID_UNKNOWN) {
						yyerror("Vendor ID mismatch "
						    "for auth method");
						return -1;
					}
					/*
					 * For interoperability with Win2k,
					 * we set the Vendor ID to "GSSAPI".
					 */
					cur_rmconf->prhead->spspec->vendorid =
					    VENDORID_GSSAPI;
					break;
				case algtype_rsasig:
					if (cur_rmconf->certtype == ISAKMP_CERT_PLAINRSA) {
						if (rsa_list_count(cur_rmconf->rsa_private) == 0) {
							yyerror ("Private PlainRSA key not set. "
								"Use directive 'certificate_type plainrsa ...'\n");
							return -1;
						}
						if (rsa_list_count(cur_rmconf->rsa_public) == 0) {
							yyerror ("Public PlainRSA keys not set. "
								"Use directive 'peers_certfile plainrsa ...'\n");
							return -1;
						}
					}
					break;
				default:
					break;
				}
				break;
			default:
				yyerror("algorithm mismatched 2");
				return -1;
			}
		}
    break;

  case 245:
#line 1488 "./cfparse.y"
    { yyval.num = 1; }
    break;

  case 246:
#line 1489 "./cfparse.y"
    { yyval.num = 60; }
    break;

  case 247:
#line 1490 "./cfparse.y"
    { yyval.num = (60 * 60); }
    break;

  case 248:
#line 1493 "./cfparse.y"
    { yyval.num = 1; }
    break;

  case 249:
#line 1494 "./cfparse.y"
    { yyval.num = 1024; }
    break;

  case 250:
#line 1495 "./cfparse.y"
    { yyval.num = (1024 * 1024); }
    break;

  case 251:
#line 1496 "./cfparse.y"
    { yyval.num = (1024 * 1024 * 1024); }
    break;


    }

/* Line 991 of yacc.c.  */
#line 3313 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__) \
    && !defined __cplusplus
  __attribute__ ((__unused__))
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1498 "./cfparse.y"


static struct proposalspec *
newprspec()
{
	struct proposalspec *new;

	new = racoon_calloc(1, sizeof(*new));
	if (new == NULL)
		yyerror("failed to allocate proposal");

	return new;
}

/*
 * insert into head of list.
 */
static void
insprspec(prspec, head)
	struct proposalspec *prspec;
	struct proposalspec **head;
{
	if (*head != NULL)
		(*head)->prev = prspec;
	prspec->next = *head;
	*head = prspec;
}

static struct secprotospec *
newspspec()
{
	struct secprotospec *new;

	new = racoon_calloc(1, sizeof(*new));
	if (new == NULL) {
		yyerror("failed to allocate spproto");
		return NULL;
	}

	new->encklen = 0;	/*XXX*/

	/*
	 * Default to "uknown" vendor -- we will override this
	 * as necessary.  When we send a Vendor ID payload, an
	 * "unknown" will be translated to a KAME/racoon ID.
	 */
	new->vendorid = VENDORID_UNKNOWN;

	return new;
}

/*
 * insert into head of list.
 */
static void
insspspec(spspec, head)
	struct secprotospec *spspec;
	struct proposalspec **head;
{
	spspec->back = *head;

	if ((*head)->spspec != NULL)
		(*head)->spspec->prev = spspec;
	spspec->next = (*head)->spspec;
	(*head)->spspec = spspec;
}

/* set final acceptable proposal */
static int
set_isakmp_proposal(rmconf, prspec)
	struct remoteconf *rmconf;
	struct proposalspec *prspec;
{
	struct proposalspec *p;
	struct secprotospec *s;
	int prop_no = 1; 
	int trns_no = 1;
	u_int32_t types[MAXALGCLASS];

	p = prspec;
	if (p->next != 0) {
		plog(LLV_ERROR, LOCATION, NULL,
			"multiple proposal definition.\n");
		return -1;
	}

	/* mandatory check */
	if (p->spspec == NULL) {
		yyerror("no remote specification found: %s.\n",
			saddr2str(rmconf->remote));
		return -1;
	}
	for (s = p->spspec; s != NULL; s = s->next) {
		/* XXX need more to check */
		if (s->algclass[algclass_isakmp_enc] == 0) {
			yyerror("encryption algorithm required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_hash] == 0) {
			yyerror("hash algorithm required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_dh] == 0) {
			yyerror("DH group required.");
			return -1;
		}
		if (s->algclass[algclass_isakmp_ameth] == 0) {
			yyerror("authentication method required.");
			return -1;
		}
	}

	/* skip to last part */
	for (s = p->spspec; s->next != NULL; s = s->next)
		;

	while (s != NULL) {
		plog(LLV_DEBUG2, LOCATION, NULL,
			"lifetime = %ld\n", (long)
			(s->lifetime ? s->lifetime : p->lifetime));
		plog(LLV_DEBUG2, LOCATION, NULL,
			"lifebyte = %d\n",
			s->lifebyte ? s->lifebyte : p->lifebyte);
		plog(LLV_DEBUG2, LOCATION, NULL,
			"encklen=%d\n", s->encklen);

		memset(types, 0, ARRAYLEN(types));
		types[algclass_isakmp_enc] = s->algclass[algclass_isakmp_enc];
		types[algclass_isakmp_hash] = s->algclass[algclass_isakmp_hash];
		types[algclass_isakmp_dh] = s->algclass[algclass_isakmp_dh];
		types[algclass_isakmp_ameth] =
		    s->algclass[algclass_isakmp_ameth];

		/* expanding spspec */
		clean_tmpalgtype();
		trns_no = expand_isakmpspec(prop_no, trns_no, types,
				algclass_isakmp_enc, algclass_isakmp_ameth + 1,
				s->lifetime ? s->lifetime : p->lifetime,
				s->lifebyte ? s->lifebyte : p->lifebyte,
				s->encklen, s->vendorid, s->gssid,
				rmconf);
		if (trns_no == -1) {
			plog(LLV_ERROR, LOCATION, NULL,
				"failed to expand isakmp proposal.\n");
			return -1;
		}

		s = s->prev;
	}

	if (rmconf->proposal == NULL) {
		plog(LLV_ERROR, LOCATION, NULL,
			"no proposal found.\n");
		return -1;
	}

	return 0;
}

static void
clean_tmpalgtype()
{
	int i;
	for (i = 0; i < MAXALGCLASS; i++)
		tmpalgtype[i] = 0;	/* means algorithm undefined. */
}

static int
expand_isakmpspec(prop_no, trns_no, types,
		class, last, lifetime, lifebyte, encklen, vendorid, gssid,
		rmconf)
	int prop_no, trns_no;
	int *types, class, last;
	time_t lifetime;
	int lifebyte;
	int encklen;
	int vendorid;
	char *gssid;
	struct remoteconf *rmconf;
{
	struct isakmpsa *new;

	/* debugging */
    {
	int j;
	char tb[10];
	plog(LLV_DEBUG2, LOCATION, NULL,
		"p:%d t:%d\n", prop_no, trns_no);
	for (j = class; j < MAXALGCLASS; j++) {
		snprintf(tb, sizeof(tb), "%d", types[j]);
		plog(LLV_DEBUG2, LOCATION, NULL,
			"%s%s%s%s\n",
			s_algtype(j, types[j]),
			types[j] ? "(" : "",
			tb[0] == '0' ? "" : tb,
			types[j] ? ")" : "");
	}
	plog(LLV_DEBUG2, LOCATION, NULL, "\n");
    }

#define TMPALGTYPE2STR(n) \
	s_algtype(algclass_isakmp_##n, types[algclass_isakmp_##n])
		/* check mandatory values */
		if (types[algclass_isakmp_enc] == 0
		 || types[algclass_isakmp_ameth] == 0
		 || types[algclass_isakmp_hash] == 0
		 || types[algclass_isakmp_dh] == 0) {
			yyerror("few definition of algorithm "
				"enc=%s ameth=%s hash=%s dhgroup=%s.\n",
				TMPALGTYPE2STR(enc),
				TMPALGTYPE2STR(ameth),
				TMPALGTYPE2STR(hash),
				TMPALGTYPE2STR(dh));
			return -1;
		}
#undef TMPALGTYPE2STR

	/* set new sa */
	new = newisakmpsa();
	if (new == NULL) {
		yyerror("failed to allocate isakmp sa");
		return -1;
	}
	new->prop_no = prop_no;
	new->trns_no = trns_no++;
	new->lifetime = lifetime;
	new->lifebyte = lifebyte;
	new->enctype = types[algclass_isakmp_enc];
	new->encklen = encklen;
	new->authmethod = types[algclass_isakmp_ameth];
	new->hashtype = types[algclass_isakmp_hash];
	new->dh_group = types[algclass_isakmp_dh];
	new->vendorid = vendorid;
#ifdef HAVE_GSSAPI
	if (gssid != NULL) {
		new->gssid = vmalloc(strlen(gssid) + 1);
		memcpy(new->gssid->v, gssid, new->gssid->l);
		racoon_free(gssid);
	} else
		new->gssid = NULL;
#endif
	insisakmpsa(new, rmconf);

	return trns_no;
}

static int
listen_addr (struct sockaddr *addr, int udp_encap)
{
	struct myaddrs *p;

	p = newmyaddr();
	if (p == NULL) {
		yyerror("failed to allocate myaddrs");
		return -1;
	}
	p->addr = addr;
	if (p->addr == NULL) {
		yyerror("failed to copy sockaddr ");
		delmyaddr(p);
		return -1;
	}
	p->udp_encap = udp_encap;

	insmyaddr(p, &lcconf->myaddrs);

	lcconf->autograbaddr = 0;
	return 0;
}

#if 0
/*
 * fix lifebyte.
 * Must be more than 1024B because its unit is kilobytes.
 * That is defined RFC2407.
 */
static int
fix_lifebyte(t)
	unsigned long t;
{
	if (t < 1024) {
		yyerror("byte size should be more than 1024B.");
		return 0;
	}

	return(t / 1024);
}
#endif

int
cfparse()
{
	int error;

	yycf_init_buffer();

	if (yycf_switch_buffer(lcconf->racoon_conf) != 0)
		return -1;

	error = yyparse();
	if (error != 0) {
		if (yyerrorcount) {
			plog(LLV_ERROR, LOCATION, NULL,
				"fatal parse failure (%d errors)\n",
				yyerrorcount);
		} else {
			plog(LLV_ERROR, LOCATION, NULL,
				"fatal parse failure.\n");
		}
		return -1;
	}

	if (error == 0 && yyerrorcount) {
		plog(LLV_ERROR, LOCATION, NULL,
			"parse error is nothing, but yyerrorcount is %d.\n",
				yyerrorcount);
		exit(1);
	}

	yycf_clean_buffer();

	plog(LLV_DEBUG2, LOCATION, NULL, "parse successed.\n");

	return 0;
}

int
cfreparse()
{
	flushph2();
	flushph1();
	flushrmconf();
	flushsainfo();
	clean_tmpalgtype();
	yycf_init_buffer();

	if (yycf_switch_buffer(lcconf->racoon_conf) != 0)
		return -1;

	return(cfparse());
}

#ifdef ENABLE_ADMINPORT
static void
adminsock_conf(path, owner, group, mode_dec)
	vchar_t *path;
	vchar_t *owner;
	vchar_t *group;
	int mode_dec;
{
	struct passwd *pw = NULL;
	struct group *gr = NULL;
	mode_t mode = 0;
	uid_t uid;
	gid_t gid;
	int isnum;

	adminsock_path = path->v;

	if (owner == NULL)
		return;

	errno = 0;
	uid = atoi(owner->v);
	isnum = !errno;
	if (((pw = getpwnam(owner->v)) == NULL) && !isnum)
		yyerror("User \"%s\" does not exist", owner->v);

	if (pw)
		adminsock_owner = pw->pw_uid;
	else
		adminsock_owner = uid;

	if (group == NULL)
		return;

	errno = 0;
	gid = atoi(group->v);
	isnum = !errno;
	if (((gr = getgrnam(group->v)) == NULL) && !isnum)
		yyerror("Group \"%s\" does not exist", group->v);

	if (gr)
		adminsock_group = gr->gr_gid;
	else
		adminsock_group = gid;

	if (mode_dec == -1)
		return;

	if (mode_dec > 777)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 400) { mode += 0400; mode_dec -= 400; }
	if (mode_dec >= 200) { mode += 0200; mode_dec -= 200; }
	if (mode_dec >= 100) { mode += 0200; mode_dec -= 100; }

	if (mode_dec > 77)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 40) { mode += 040; mode_dec -= 40; }
	if (mode_dec >= 20) { mode += 020; mode_dec -= 20; }
	if (mode_dec >= 10) { mode += 020; mode_dec -= 10; }

	if (mode_dec > 7)
		yyerror("Mode 0%03o is invalid", mode_dec);
	if (mode_dec >= 4) { mode += 04; mode_dec -= 4; }
	if (mode_dec >= 2) { mode += 02; mode_dec -= 2; }
	if (mode_dec >= 1) { mode += 02; mode_dec -= 1; }
	
	adminsock_mode = mode;

	return;
}
#endif

