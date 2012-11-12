/*
**  Daedalus
**  daeLex.l -- Lexer
**
**  Blai Bonet, Hector Geffner
**  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002
**
*/

b     "  "
n     "\n"
t     "\t"
q     "\""
c     [a-zA-Z_]
i     [0-9]

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#include <daedalus/daeTypes.h>
#include <daedalus/daeTokens.h>

#include <map>


int lineno = 1;
int cpos = 1;

extern map<const char*,const functionClass*,ltstr> *functionHash;
extern map<const char*,const predicateClass*,ltstr> *predicateHash;
extern set<const char*,ltstr> *externalProcedureList;
extern YYSTYPE yylval;

#ifdef linux
static int yyinput( void );
#endif


void
skipcomments( void )  
{
#ifdef linux
  while( yyinput() != '\n' );
#else
  while( lex_input() != '\n' );
#endif
  ++lineno;
}

void
translateHyphens( char *s )
{
#if 0
  for( char *p = s; *p; ++p )
    if( *p == '-' ) *p = '_';
#endif
}
%}


%%

"\n"                   { lineno++; cpos = 1; }
"\t"                   { cpos += 8; }
" "                    { cpos++; }
";"                    { skipcomments(); }


:dynamics              { cpos += strlen( yytext ); return( DYNAMICS ); }
:deterministic         { cpos += strlen( yytext ); return( DETERMINISTIC ); }
:non-deterministic     { cpos += strlen( yytext ); return( NON_DETERMINISTIC ); }
:probabilistic         { cpos += strlen( yytext ); return( PROBABILISTIC ); }
:feedback              { cpos += strlen( yytext ); return( FEEDBACK ); }
:complete              { cpos += strlen( yytext ); return( COMPLETE ); }
:partial               { cpos += strlen( yytext ); return( PARTIAL ); }
:null                  { cpos += strlen( yytext ); return( NULLF ); }


define                 { cpos += strlen( yytext ); return( DEFINE ); }
domain                 { cpos += strlen( yytext ); return( PDOMAIN ); }
problem                { cpos += strlen( yytext ); return( PROBLEM ); }
true                   { cpos += strlen( yytext ); yylval.ident = strdup( yytext ); return( TOK_BOOLEAN ); }
false                  { cpos += strlen( yytext ); yylval.ident = strdup( yytext ); return( TOK_BOOLEAN ); }

:domain                { cpos += strlen( yytext ); return( CDOMAIN ); }
:requirements          { cpos += strlen( yytext ); return( REQUIREMENTS ); }
:types                 { cpos += strlen( yytext ); return( TYPES ); }
:functions             { cpos += strlen( yytext ); return( FUNCTIONS ); }
:function              { cpos += strlen( yytext ); return( FUNCTION ); }
:predicates            { cpos += strlen( yytext ); return( PREDICATES ); }
:predicate             { cpos += strlen( yytext ); return( PREDICATE ); }
:procedure             { cpos += strlen( yytext ); return( PROCEDURE ); }
:external              { cpos += strlen( yytext ); return( EXTERNAL ); }
:objects               { cpos += strlen( yytext ); return( OBJECTS ); }
:action                { cpos += strlen( yytext ); return( ACTION ); }
:parameters            { cpos += strlen( yytext ); return( PARAMETERS ); }
:precondition          { cpos += strlen( yytext ); return( PRECONDITION ); }
:effect                { cpos += strlen( yytext ); return( EFFECT ); }
:cost                  { cpos += strlen( yytext ); return( COST ); }
:observation           { cpos += strlen( yytext ); return( OBSERVATION ); }
:when                  { cpos += strlen( yytext ); return( WHEN ); }
:goal                  { cpos += strlen( yytext ); return( GOAL ); }
:package               { cpos += strlen( yytext ); return( PACKAGE ); }
:set                   { cpos += strlen( yytext ); return( SET ); }
:in                    { cpos += strlen( yytext ); return( IN ); }
:one-of                { cpos += strlen( yytext ); return( ONEOF ); }
:print                 { cpos += strlen( yytext ); return( PRINT ); }
:print-state           { cpos += strlen( yytext ); return( PRINTSTATE ); }
:assert                { cpos += strlen( yytext ); return( ASSERT ); }
:ramification          { cpos += strlen( yytext ); return( RAMIFICATION ); }
:predicate             { cpos += strlen( yytext ); return( PREDICATE ); }
:formula               { cpos += strlen( yytext ); return( FORMULA ); }
:init                  { cpos += strlen( yytext ); return( INIT ); }
:hooks                 { cpos += strlen( yytext ); return( HOOKS ); }
:model                 { cpos += strlen( yytext ); return( MODEL ); }
:belief                { cpos += strlen( yytext ); return( BELIEF ); }
:heuristic             { cpos += strlen( yytext ); return( HEURISTIC ); }
:integer               { cpos += strlen( yytext ); return( INTEGER ); }
:boolean               { cpos += strlen( yytext ); return( BOOLEAN ); }
:array                 { cpos += strlen( yytext ); return( ARRAY ); }
:vector                { cpos += strlen( yytext ); return( VECTOR ); }
:not                   { cpos += strlen( yytext ); return( NOT ); }
:and                   { cpos += strlen( yytext ); return( AND ); }
:or                    { cpos += strlen( yytext ); return( OR ); }
:if                    { cpos += strlen( yytext ); return( IF ); }
:iff                   { cpos += strlen( yytext ); return( IFF ); }

:exists                { cpos += strlen( yytext ); return( EXISTS ); }
:forall                { cpos += strlen( yytext ); return( FORALL ); }
:inline                { cpos += strlen( yytext ); return( INLINE ); }
:sum                   { cpos += strlen( yytext ); return( SUM ); }
:foreach               { cpos += strlen( yytext ); return( FOREACH ); }
:certainty             { cpos += strlen( yytext ); return( CERTAINTY ); }

"("                    { cpos += strlen( yytext ); return( LEFTPAR ); }
")"                    { cpos += strlen( yytext ); return( RIGHTPAR ); }
"["                    { cpos += strlen( yytext ); return( LEFTSQPAR ); }
"]"                    { cpos += strlen( yytext ); return( RIGHTSQPAR ); }
"{"                    { cpos += strlen( yytext ); return( LEFTCURLY ); }
"}"                    { cpos += strlen( yytext ); return( RIGHTCURLY ); }
"*"                    { cpos += strlen( yytext ); return( STAR ); }
"+"                    { cpos += strlen( yytext ); return( PLUS ); }
"-"                    { cpos += strlen( yytext ); return( HYPHEN ); }
","                    { cpos += strlen( yytext ); return( COMMA ); }
":"                    { cpos += strlen( yytext ); return( COLON ); }
"="                    { cpos += strlen( yytext ); return( EQUAL ); }
"<"                    { cpos += strlen( yytext ); yylval.ident = strdup( "_lessThan" ); return( LT ); }
"<="                   { cpos += strlen( yytext ); yylval.ident = strdup( "_lessEqual" ); return( LE ); }
">="                   { cpos += strlen( yytext ); yylval.ident = strdup( "_greaterEqual" ); return( GE ); }
">"                    { cpos += strlen( yytext ); yylval.ident = strdup( "_greaterThan" ); return( GT ); }

{c}({c}|{i})*          {
                         int token;
                         translateHyphens( yytext );  
                         cpos += strlen( yytext );
			 yylval.ident = (char*)malloc( strlen( yytext ) + 2 );
			 sprintf( (char*)yylval.ident, "_%s", yytext );

			 // check if token is FUNCTION, PREDICATE or PROCEDURE
			 if( functionHash && 
			     (functionHash->find(yylval.ident) != functionHash->end()) )
                           token = TOK_FUNCTION;
			 else if( predicateHash && 
				  (predicateHash->find(yylval.ident) != predicateHash->end()) )
                           token = TOK_PREDICATE;
                         else if( externalProcedureList &&
                                  (externalProcedureList->find(yylval.ident) != externalProcedureList->end()) )
                           token = TOK_PROCEDURE;
			 else
                           token = TOK_IDENT;
			 //fprintf( stderr, "yylval = [%p]%s, token = %d\n" , yylval.ident, yylval.ident, token );
                         return( token );
                       }
\?{c}({c}|{i})*        {
                         translateHyphens( yytext );  
                         cpos += strlen( yytext );
			 yylval.ident = (char*)malloc( strlen( yytext ) + 3 );
			 sprintf( (char*)yylval.ident, "_v_%s", &yytext[1] );
			 return( TOK_VIDENT );
                       }
-?{i}+                 { cpos += strlen( yytext ); yylval.ident = strdup( yytext ); return( TOK_INTEGER ); }
-?{i}+\.{i}+           { cpos += strlen( yytext ); yylval.ident = strdup( yytext ); return( TOK_FLOAT ); }
\"[^\"]*\"             { 
                         cpos += strlen( yytext );
			 yytext[strlen( yytext ) - 1] = '\0';
			 yylval.ident = strdup( &yytext[1] ); 
			 return( TOK_STRING );
                       }

.                      { return( yytext[0] ); }

%%

int
yywrap( void )
{
  return( 1 );
}
