/* A Bison parser, made by GNU Bison 2.1.  */

/* C++ Skeleton parser for LALR(1) parsing with Bison,
   Copyright (C) 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */


#include "./calc++-parser.hh"

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* A pseudo ostream that takes yydebug_ into account. */
# define YYCDEBUG							\
  for (bool yydebugcond_ = yydebug_; yydebugcond_; yydebugcond_ = false)	\
    (*yycdebug_)

/* Enable debugging if requested.  */
#if YYDEBUG

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << (Title) << ' ';			\
      yysymprint_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (0)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yyreduce_print_ (Rule);		\
} while (0)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (0)

#else /* !YYDEBUG */

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab

#if YYERROR_VERBOSE

/* Return YYSTR after stripping away unnecessary quotes and
   backslashes, so that it's suitable for yyerror.  The heuristic is
   that double-quoting is unnecessary unless the string contains an
   apostrophe, a comma, or backslash (other than backslash-backslash).
   YYSTR is taken from yytname.  */
std::string
yy::calcxx_parser::yytnamerr_ (const char *yystr)
{
  if (*yystr == '"')
    {
      std::string yyr = "";
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    yyr += *yyp;
	    break;

	  case '"':
	    return yyr;
	  }
    do_not_strip_quotes: ;
    }

  return yystr;
}

#endif

#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

void
yy::calcxx_parser::yysymprint_ (int yytype,
                         const semantic_type* yyvaluep, const location_type* yylocationp)
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;
  (void) yylocationp;
  /* Backward compatibility, but should be removed eventually. */
  std::ostream& cdebug_ = *yycdebug_;
  (void) cdebug_;

  *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	     << ' ' << yytname_[yytype] << " ("
             << *yylocationp << ": ";
  switch (yytype)
    {
      case 4: /* "\"identifier\"" */
#line 7379 "../../doc/bison.texinfo"
        { debug_stream () << *(yyvaluep->sval); };
#line 144 "./calc++-parser.cc"
        break;
      case 5: /* "\"number\"" */
#line 7382 "../../doc/bison.texinfo"
        { debug_stream () << (yyvaluep->ival); };
#line 149 "./calc++-parser.cc"
        break;
      case 6: /* "\"expression\"" */
#line 7382 "../../doc/bison.texinfo"
        { debug_stream () << (yyvaluep->ival); };
#line 154 "./calc++-parser.cc"
        break;
      default:
        break;
    }
  *yycdebug_ << ')';
}
#endif /* ! YYDEBUG */

void
yy::calcxx_parser::yydestruct_ (const char* yymsg,
                         int yytype, semantic_type* yyvaluep, location_type* yylocationp)
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yymsg;
  (void) yyvaluep;
  (void) yylocationp;

  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 4: /* "\"identifier\"" */
#line 7380 "../../doc/bison.texinfo"
        { delete (yyvaluep->sval); };
#line 179 "./calc++-parser.cc"
        break;

      default:
        break;
    }
}

void
yy::calcxx_parser::yypop_ (unsigned int n)
{
  yystate_stack_.pop (n);
  yysemantic_stack_.pop (n);
  yylocation_stack_.pop (n);
}

std::ostream&
yy::calcxx_parser::debug_stream () const
{
  return *yycdebug_;
}

void
yy::calcxx_parser::set_debug_stream (std::ostream& o)
{
  yycdebug_ = &o;
}


yy::calcxx_parser::debug_level_type
yy::calcxx_parser::debug_level () const
{
  return yydebug_;
}

void
yy::calcxx_parser::set_debug_level (debug_level_type l)
{
  yydebug_ = l;
}


int
yy::calcxx_parser::parse ()
{
  /* Look-ahead and look-ahead in internal form.  */
  int yylooka;
  int yyilooka;

  /// Semantic value of the look-ahead.
  semantic_type yylval;
  /// Location of the look-ahead.
  location_type yylloc;
  /// The locations where the error started and ended.
  location yyerror_range[2];

  /// $$.
  semantic_type yyval;
  /// @$.
  location_type yyloc;

  int yyresult_;

  YYCDEBUG << "Starting parse" << std::endl;

  yynerrs_ = 0;
  yyerrstatus_ = 0;

  /* Start.  */
  yystate_ = 0;
  yylooka = yyempty_;


  /* User initialization code. */
  #line 7327 "../../doc/bison.texinfo"
{
  // Initialize the initial location.
  yylloc.begin.filename = yylloc.end.filename = &driver.file;
}
/* Line 548 of yacc.c.  */
#line 259 "./calc++-parser.cc"
  /* Initialize the stacks.  The initial state will be pushed in
     yynewstate, since the latter expects the semantical and the
     location values to have been already stored, initialize these
     stacks with a primary value.  */
  yystate_stack_ = state_stack_type (0);
  yysemantic_stack_ = semantic_stack_type (0);
  yylocation_stack_ = location_stack_type (0);
  yysemantic_stack_.push (yylval);
  yylocation_stack_.push (yylloc);

  /* New state.  */
yynewstate:
  yystate_stack_.push (yystate_);
  YYCDEBUG << "Entering state " << yystate_ << std::endl;
  goto yybackup;

  /* Backup.  */
yybackup:

  /* Try to take a decision without look-ahead.  */
  yyn_ = yypact_[yystate_];
  if (yyn_ == yypact_ninf_)
    goto yydefault;

  /* Read a look-ahead token.  */
  if (yylooka == yyempty_)
    {
      YYCDEBUG << "Reading a token: ";
      yylooka = yylex (&yylval, &yylloc, driver);
    }


  /* Convert token to internal form.  */
  if (yylooka <= yyeof_)
    {
      yylooka = yyilooka = yyeof_;
      YYCDEBUG << "Now at end of input." << std::endl;
    }
  else
    {
      yyilooka = yytranslate_ (yylooka);
      YY_SYMBOL_PRINT ("Next token is", yyilooka, &yylval, &yylloc);
    }

  /* If the proper action on seeing token ILOOKA_ is to reduce or to
     detect an error, take that action.  */
  yyn_ += yyilooka;
  if (yyn_ < 0 || yylast_ < yyn_ || yycheck_[yyn_] != yyilooka)
    goto yydefault;

  /* Reduce or error.  */
  yyn_ = yytable_[yyn_];
  if (yyn_ < 0)
    {
      if (yyn_ == yytable_ninf_)
	goto yyerrlab;
      else
	{
	  yyn_ = -yyn_;
	  goto yyreduce;
	}
    }
  else if (yyn_ == 0)
    goto yyerrlab;

  /* Accept?  */
  if (yyn_ == yyfinal_)
    goto yyacceptlab;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yyilooka, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yylooka != yyeof_)
    yylooka = yyempty_;

  yysemantic_stack_.push (yylval);
  yylocation_stack_.push (yylloc);

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus_)
    --yyerrstatus_;

  yystate_ = yyn_;
  goto yynewstate;

/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn_ = yydefact_[yystate_];
  if (yyn_ == 0)
    goto yyerrlab;
  goto yyreduce;

/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  yylen_ = yyr2_[yyn_];
  /* If LEN_ is nonzero, implement the default value of the action:
     `$$ = $1'.  Otherwise, use the top of the stack.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  */
  if (yylen_)
    yyval = yysemantic_stack_[yylen_ - 1];
  else
    yyval = yysemantic_stack_[0];

  {
    slice<location_type, location_stack_type> slice (yylocation_stack_, yylen_);
    YYLLOC_DEFAULT (yyloc, slice, yylen_);
  }
  YY_REDUCE_PRINT (yyn_);
  switch (yyn_)
    {
        case 2:
#line 7392 "../../doc/bison.texinfo"
    { driver.result = (yysemantic_stack_[0].ival); ;}
    break;

  case 3:
#line 7394 "../../doc/bison.texinfo"
    {;}
    break;

  case 4:
#line 7395 "../../doc/bison.texinfo"
    {;}
    break;

  case 5:
#line 7397 "../../doc/bison.texinfo"
    { driver.variables[*(yysemantic_stack_[2].sval)] = (yysemantic_stack_[0].ival); ;}
    break;

  case 6:
#line 7401 "../../doc/bison.texinfo"
    { (yyval.ival) = (yysemantic_stack_[2].ival) + (yysemantic_stack_[0].ival); ;}
    break;

  case 7:
#line 7402 "../../doc/bison.texinfo"
    { (yyval.ival) = (yysemantic_stack_[2].ival) - (yysemantic_stack_[0].ival); ;}
    break;

  case 8:
#line 7403 "../../doc/bison.texinfo"
    { (yyval.ival) = (yysemantic_stack_[2].ival) * (yysemantic_stack_[0].ival); ;}
    break;

  case 9:
#line 7404 "../../doc/bison.texinfo"
    { (yyval.ival) = (yysemantic_stack_[2].ival) / (yysemantic_stack_[0].ival); ;}
    break;

  case 10:
#line 7405 "../../doc/bison.texinfo"
    { (yyval.ival) = driver.variables[*(yysemantic_stack_[0].sval)]; ;}
    break;

  case 11:
#line 7406 "../../doc/bison.texinfo"
    { (yyval.ival) = (yysemantic_stack_[0].ival); ;}
    break;


      default: break;
    }

/* Line 676 of lalr1.cc.  */
#line 434 "./calc++-parser.cc"

  yypop_ (yylen_);

  YY_STACK_PRINT ();

  yysemantic_stack_.push (yyval);
  yylocation_stack_.push (yyloc);

  /* Shift the result of the reduction.  */
  yyn_ = yyr1_[yyn_];
  yystate_ = yypgoto_[yyn_ - yyntokens_] + yystate_stack_[0];
  if (0 <= yystate_ && yystate_ <= yylast_
      && yycheck_[yystate_] == yystate_stack_[0])
    yystate_ = yytable_[yystate_];
  else
    yystate_ = yydefgoto_[yyn_ - yyntokens_];
  goto yynewstate;

/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus_)
    {
      ++yynerrs_;
      error (yylloc, yysyntax_error_ (YYERROR_VERBOSE_IF (yyilooka)));
    }

  yyerror_range[0] = yylloc;
  if (yyerrstatus_ == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yylooka <= yyeof_)
        {
	  /* Return failure if at end of input.  */
	  if (yylooka == yyeof_)
	    YYABORT;
        }
      else
        {
          yydestruct_ ("Error: discarding", yyilooka, &yylval, &yylloc);
          yylooka = yyempty_;
        }
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (false)
    goto yyerrorlab;

  yyerror_range[0] = yylocation_stack_[yylen_ - 1];
  yypop_ (yylen_);
  yystate_ = yystate_stack_[0];
  goto yyerrlab1;

/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn_ = yypact_[yystate_];
      if (yyn_ != yypact_ninf_)
	{
	  yyn_ += yyterror_;
	  if (0 <= yyn_ && yyn_ <= yylast_ && yycheck_[yyn_] == yyterror_)
	    {
	      yyn_ = yytable_[yyn_];
	      if (0 < yyn_)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yystate_stack_.height () == 1)
	YYABORT;

      yyerror_range[0] = yylocation_stack_[0];
      yydestruct_ ("Error: popping",
                   yystos_[yystate_],
                   &yysemantic_stack_[0], &yylocation_stack_[0]);
      yypop_ ();
      yystate_ = yystate_stack_[0];
      YY_STACK_PRINT ();
    }

  if (yyn_ == yyfinal_)
    goto yyacceptlab;

  yyerror_range[1] = yylloc;
  // Using YYLLOC is tempting, but would change the location of
  // the look-ahead.  YYLOC is available though.
  YYLLOC_DEFAULT (yyloc, yyerror_range - 1, 2);
  yysemantic_stack_.push (yylval);
  yylocation_stack_.push (yyloc);

  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos_[yyn_],
		   &yysemantic_stack_[0], &yylocation_stack_[0]);

  yystate_ = yyn_;
  goto yynewstate;

  /* Accept.  */
yyacceptlab:
  yyresult_ = 0;
  goto yyreturn;

  /* Abort.  */
yyabortlab:
  yyresult_ = 1;
  goto yyreturn;

yyreturn:
  if (yylooka != yyeof_ && yylooka != yyempty_)
    yydestruct_ ("Cleanup: discarding lookahead", yyilooka, &yylval, &yylloc);

  while (yystate_stack_.height () != 1)
    {
      yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
      yypop_ ();
    }

  return yyresult_;
}

// Generate an error message.
std::string
yy::calcxx_parser::yysyntax_error_ (YYERROR_VERBOSE_IF (int tok))
{
  std::string res;
#if YYERROR_VERBOSE
  yyn_ = yypact_[yystate_];
  if (yypact_ninf_ < yyn_ && yyn_ < yylast_)
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  */
      int yyxbegin = yyn_ < 0 ? -yyn_ : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = yylast_ - yyn_;
      int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
      int count = 0;
      for (int x = yyxbegin; x < yyxend; ++x)
        if (yycheck_[x + yyn_] == x && x != yyterror_)
          ++count;

      // FIXME: This method of building the message is not compatible
      // with internationalization.  It should work like yacc.c does it.
      // That is, first build a string that looks like this:
      // "syntax error, unexpected %s or %s or %s"
      // Then, invoke YY_ on this string.
      // Finally, use the string as a format to output
      // yytname_[tok], etc.
      // Until this gets fixed, this message appears in English only.
      res = "syntax error, unexpected ";
      res += yytnamerr_ (yytname_[tok]);
      if (count < 5)
        {
          count = 0;
          for (int x = yyxbegin; x < yyxend; ++x)
            if (yycheck_[x + yyn_] == x && x != yyterror_)
              {
                res += (!count++) ? ", expecting " : " or ";
                res += yytnamerr_ (yytname_[x]);
              }
        }
    }
  else
#endif
    res = YY_("syntax error");
  return res;
}


/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
const signed char yy::calcxx_parser::yypact_ninf_ = -9;
const signed char
yy::calcxx_parser::yypact_[] =
{
      -9,    15,     5,    -9,    13,    -9,    -9,    -2,     7,     7,
       7,     7,     7,    -9,    -2,     4,     4,    -9,    -9
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
const unsigned char
yy::calcxx_parser::yydefact_[] =
{
       4,     0,     0,     1,    10,    11,     3,     2,     0,     0,
       0,     0,     0,    10,     5,     6,     7,     8,     9
};

/* YYPGOTO[NTERM-NUM].  */
const signed char
yy::calcxx_parser::yypgoto_[] =
{
      -9,    -9,    -9,    -9,    -8
};

/* YYDEFGOTO[NTERM-NUM].  */
const signed char
yy::calcxx_parser::yydefgoto_[] =
{
      -1,     1,     2,     6,     7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.  */
const signed char yy::calcxx_parser::yytable_ninf_ = -1;
const unsigned char
yy::calcxx_parser::yytable_[] =
{
      14,    15,    16,    17,    18,     9,    10,    11,    12,     4,
       5,    13,     5,    11,    12,     3,     8
};

/* YYCHECK.  */
const unsigned char
yy::calcxx_parser::yycheck_[] =
{
       8,     9,    10,    11,    12,     7,     8,     9,    10,     4,
       5,     4,     5,     9,    10,     0,     3
};

/* STOS_[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
const unsigned char
yy::calcxx_parser::yystos_[] =
{
       0,    12,    13,     0,     4,     5,    14,    15,     3,     7,
       8,     9,    10,     4,    15,    15,    15,    15,    15
};

#if YYDEBUG
/* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
   to YYLEX-NUM.  */
const unsigned short int
yy::calcxx_parser::yytoken_number_[] =
{
       0,   256,   257,   258,   259,   260,   261,    43,    45,    42,
      47
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
const unsigned char
yy::calcxx_parser::yyr1_[] =
{
       0,    11,    12,    13,    13,    14,    15,    15,    15,    15,
      15,    15
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
const unsigned char
yy::calcxx_parser::yyr2_[] =
{
       0,     2,     2,     2,     0,     3,     3,     3,     3,     3,
       1,     1
};

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at \a yyntokens_, nonterminals. */
const char*
const yy::calcxx_parser::yytname_[] =
{
  "\"end of file\"", "error", "$undefined", "\":=\"", "\"identifier\"",
  "\"number\"", "\"expression\"", "'+'", "'-'", "'*'", "'/'", "$accept",
  "unit", "assignments", "assignment", "exp", 0
};
#endif

#if YYDEBUG
/* YYRHS -- A `-1'-separated list of the rules' RHS. */
const yy::calcxx_parser::rhs_number_type
yy::calcxx_parser::yyrhs_[] =
{
      12,     0,    -1,    13,    15,    -1,    13,    14,    -1,    -1,
       4,     3,    15,    -1,    15,     7,    15,    -1,    15,     8,
      15,    -1,    15,     9,    15,    -1,    15,    10,    15,    -1,
       4,    -1,     5,    -1
};

/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
const unsigned char
yy::calcxx_parser::yyprhs_[] =
{
       0,     0,     3,     6,     9,    10,    14,    18,    22,    26,
      30,    32
};

/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
const unsigned short int
yy::calcxx_parser::yyrline_[] =
{
       0,  7392,  7392,  7394,  7395,  7397,  7401,  7402,  7403,  7404,
    7405,  7406
};

// Print the state stack on the debug stream.
void
yy::calcxx_parser::yystack_print_ ()
{
  *yycdebug_ << "Stack now";
  for (state_stack_type::const_iterator i = yystate_stack_.begin ();
       i != yystate_stack_.end (); ++i)
    *yycdebug_ << ' ' << *i;
  *yycdebug_ << std::endl;
}

// Report on the debug stream that the rule \a yyrule is going to be reduced.
void
yy::calcxx_parser::yyreduce_print_ (int yyrule)
{
  unsigned int yylno = yyrline_[yyrule];
  /* Print the symbols being reduced, and their result.  */
  *yycdebug_ << "Reducing stack by rule " << yyn_ - 1
             << " (line " << yylno << "), ";
  for (unsigned char i = yyprhs_[yyn_];
       0 <= yyrhs_[i]; ++i)
    *yycdebug_ << yytname_[yyrhs_[i]] << ' ';
  *yycdebug_ << "-> " << yytname_[yyr1_[yyn_]] << std::endl;
}
#endif // YYDEBUG

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
yy::calcxx_parser::token_number_type
yy::calcxx_parser::yytranslate_ (int token)
{
  static
  const token_number_type
  translate_table[] =
  {
         0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     9,     7,     2,     8,     2,    10,     2,     2,
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
       5,     6
  };
  if ((unsigned int) token <= yyuser_token_number_max_)
    return translate_table[token];
  else
    return yyundef_token_;
}

const int yy::calcxx_parser::yyeof_ = 0;
const int yy::calcxx_parser::yylast_ = 16;
const int yy::calcxx_parser::yynnts_ = 5;
const int yy::calcxx_parser::yyempty_ = -2;
const int yy::calcxx_parser::yyfinal_ = 3;
const int yy::calcxx_parser::yyterror_ = 1;
const int yy::calcxx_parser::yyerrcode_ = 256;
const int yy::calcxx_parser::yyntokens_ = 11;

const unsigned int yy::calcxx_parser::yyuser_token_number_max_ = 261;
const yy::calcxx_parser::token_number_type yy::calcxx_parser::yyundef_token_ = 2;

#line 7407 "../../doc/bison.texinfo"

#line 7416 "../../doc/bison.texinfo"
void
yy::calcxx_parser::error (const yy::calcxx_parser::location_type& l,
                          const std::string& m)
{
  driver.error (l, m);
}

