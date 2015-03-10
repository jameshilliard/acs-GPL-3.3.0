%{                                            /* -*- C++ -*- */
# include <cstdlib>
# include <errno.h>
# include <limits.h>
# include <string>
# include "calc++-driver.hh"
# include "calc++-parser.hh"
%}

%option noyywrap nounput batch debug

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
blank [ \t]

%{
# define YY_USER_ACTION  yylloc->columns (yyleng);
%}
%%
%{
  yylloc->step ();
%}
{blank}+   yylloc->step ();
[\n]+      yylloc->lines (yyleng); yylloc->step ();

[-+*/]     return yytext[0];
":="       return TOKEN_ASSIGN;
{int}      {
  errno = 0;
  long n = strtol (yytext, NULL, 10);
  if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
    driver.error (*yylloc, "integer is out of range");
  yylval->ival = n;
  return TOKEN_NUMBER;
}
{id}       yylval->sval = new std::string (yytext); return TOKEN_IDENTIFIER;
.          driver.error (*yylloc, "invalid character");
%%

void
calcxx_driver::scan_begin ()
{
  yy_flex_debug = trace_scanning;
  if (!(yyin = fopen (file.c_str (), "r")))
    error (std::string ("cannot open ") + file);
}

void
calcxx_driver::scan_end ()
{
  fclose (yyin);
}
