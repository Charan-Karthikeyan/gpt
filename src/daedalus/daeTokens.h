
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DEFINE = 258,
     PDOMAIN = 259,
     CDOMAIN = 260,
     PROBLEM = 261,
     REQUIREMENTS = 262,
     TYPES = 263,
     FUNCTION = 264,
     FUNCTIONS = 265,
     PREDICATE = 266,
     PREDICATES = 267,
     PROCEDURE = 268,
     EXTERNAL = 269,
     OBJECTS = 270,
     ACTION = 271,
     PARAMETERS = 272,
     PRECONDITION = 273,
     EFFECT = 274,
     COST = 275,
     OBSERVATION = 276,
     WHEN = 277,
     INIT = 278,
     GOAL = 279,
     PACKAGE = 280,
     RAMIFICATION = 281,
     FORMULA = 282,
     SET = 283,
     IN = 284,
     ONEOF = 285,
     PRINT = 286,
     PRINTSTATE = 287,
     ASSERT = 288,
     HOOKS = 289,
     BELIEF = 290,
     HEURISTIC = 291,
     MODEL = 292,
     PLANNING = 293,
     CONFORMANT1 = 294,
     CONFORMANT2 = 295,
     ND_MDP = 296,
     MDP = 297,
     ND_POMDP1 = 298,
     ND_POMDP2 = 299,
     POMDP1 = 300,
     POMDP2 = 301,
     DYNAMICS = 302,
     DETERMINISTIC = 303,
     NON_DETERMINISTIC = 304,
     PROBABILISTIC = 305,
     FEEDBACK = 306,
     COMPLETE = 307,
     PARTIAL = 308,
     NULLF = 309,
     INTEGER = 310,
     BOOLEAN = 311,
     ARRAY = 312,
     LEFTPAR = 313,
     RIGHTPAR = 314,
     LEFTSQPAR = 315,
     RIGHTSQPAR = 316,
     LEFTCURLY = 317,
     RIGHTCURLY = 318,
     COLON = 319,
     STAR = 320,
     HYPHEN = 321,
     COMMA = 322,
     NOT = 323,
     AND = 324,
     OR = 325,
     IF = 326,
     IFF = 327,
     EQUAL = 328,
     LT = 329,
     LE = 330,
     GE = 331,
     GT = 332,
     PLUS = 333,
     EXISTS = 334,
     FORALL = 335,
     VECTOR = 336,
     SUM = 337,
     FOREACH = 338,
     INLINE = 339,
     CERTAINTY = 340,
     TOK_IDENT = 341,
     TOK_VIDENT = 342,
     TOK_INTEGER = 343,
     TOK_FLOAT = 344,
     TOK_BOOLEAN = 345,
     TOK_STRING = 346,
     TOK_FUNCTION = 347,
     TOK_PREDICATE = 348,
     TOK_PROCEDURE = 349,
     BOGUS = 350,
     BOGUS2 = 351,
     BOGUS3 = 352,
     BOGUS4 = 353
   };
#endif
/* Tokens.  */
#define DEFINE 258
#define PDOMAIN 259
#define CDOMAIN 260
#define PROBLEM 261
#define REQUIREMENTS 262
#define TYPES 263
#define FUNCTION 264
#define FUNCTIONS 265
#define PREDICATE 266
#define PREDICATES 267
#define PROCEDURE 268
#define EXTERNAL 269
#define OBJECTS 270
#define ACTION 271
#define PARAMETERS 272
#define PRECONDITION 273
#define EFFECT 274
#define COST 275
#define OBSERVATION 276
#define WHEN 277
#define INIT 278
#define GOAL 279
#define PACKAGE 280
#define RAMIFICATION 281
#define FORMULA 282
#define SET 283
#define IN 284
#define ONEOF 285
#define PRINT 286
#define PRINTSTATE 287
#define ASSERT 288
#define HOOKS 289
#define BELIEF 290
#define HEURISTIC 291
#define MODEL 292
#define PLANNING 293
#define CONFORMANT1 294
#define CONFORMANT2 295
#define ND_MDP 296
#define MDP 297
#define ND_POMDP1 298
#define ND_POMDP2 299
#define POMDP1 300
#define POMDP2 301
#define DYNAMICS 302
#define DETERMINISTIC 303
#define NON_DETERMINISTIC 304
#define PROBABILISTIC 305
#define FEEDBACK 306
#define COMPLETE 307
#define PARTIAL 308
#define NULLF 309
#define INTEGER 310
#define BOOLEAN 311
#define ARRAY 312
#define LEFTPAR 313
#define RIGHTPAR 314
#define LEFTSQPAR 315
#define RIGHTSQPAR 316
#define LEFTCURLY 317
#define RIGHTCURLY 318
#define COLON 319
#define STAR 320
#define HYPHEN 321
#define COMMA 322
#define NOT 323
#define AND 324
#define OR 325
#define IF 326
#define IFF 327
#define EQUAL 328
#define LT 329
#define LE 330
#define GE 331
#define GT 332
#define PLUS 333
#define EXISTS 334
#define FORALL 335
#define VECTOR 336
#define SUM 337
#define FOREACH 338
#define INLINE 339
#define CERTAINTY 340
#define TOK_IDENT 341
#define TOK_VIDENT 342
#define TOK_INTEGER 343
#define TOK_FLOAT 344
#define TOK_BOOLEAN 345
#define TOK_STRING 346
#define TOK_FUNCTION 347
#define TOK_PREDICATE 348
#define TOK_PROCEDURE 349
#define BOGUS 350
#define BOGUS2 351
#define BOGUS3 352
#define BOGUS4 353




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


