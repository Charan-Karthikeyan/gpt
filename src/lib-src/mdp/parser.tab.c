
# line 2 "parser.y"
/* parser.y

 * Copyright 1996,1997, Brown University, Providence, RI.
 * 
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose other than its incorporation into a
 * commercial product is hereby granted without fee, provided that the
 * above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Brown University not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * BROWN UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR ANY
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL BROWN UNIVERSITY BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#include <stdio.h>
#include <stdlib.h>
#include "parse_err.h"
#include "mdp.h"
#include "parse_hash.h"
#include "parse_constant.h"
#include "sparse-matrix.h"
#include "imm-reward.h"

#define YACCtrace(X)       /*   printf(X);fflush(stdout)    */ 

/* When reading in matrices we need to know what type we are reading
   and also we need to keep track of where in the matrix we are
   including how to update the row and col after each entry is read. */
typedef enum { mc_none, mc_trans_single, mc_trans_row, mc_trans_all,
               mc_obs_single, mc_obs_row, mc_obs_all,
               mc_reward_single, mc_reward_row, 
               mc_reward_all, mc_reward_mdp_only,
               mc_start_belief, mc_mdp_start, 
               mc_start_include, mc_start_exclude } Matrix_Context;

#ifdef __cplusplus
extern "C" int yylex();
extern "C" void yyerror(char *string);
#else
extern int yylex();
extern void yyerror(char *string);
#endif

/* Forward declaration for action routines which appear at end of file */
void checkMatrix();
void enterString( Constant_Block *block );
void enterUniformMatrix( );
void enterIdentityMatrix( );
void enterResetMatrix( );
void enterMatrix( double value );
void setMatrixContext( Matrix_Context context, 
                      int a, int i, int j, int obs );
void enterStartState( int i );
void setStartStateUniform();
void endStartStates();
void verifyPreamble();
void checkProbs();

/*  Helps to give more meaningful error messages */
long currentLineNumber = 1;

/* This sets the context needed when names are given the the states, 
   actions and/or observations */
Mnemonic_Type curMnemonic = nt_unknown;

Matrix_Context curMatrixContext = mc_none;

/* These variable are used to keep track what type of matrix is being entered and
   which element is currently being processed.  They are initialized by the
   setMatrixContext() routine and updated by the enterMatrix() routine. */
int curRow;
int curCol;
int minA, maxA;
int minI, maxI;
int minJ, maxJ;
int minObs, maxObs;

/*  These variables will keep the intermediate representation for the
    matrices.  We cannot know how to set up the sparse matrices until
    all entries are read in, so we must have this intermediate 
    representation, which will will convert when it has all been read in.
    We allocate this memory once we know how big they must be and we
    will free all of this when we convert it to its final sparse format.
    */
I_Matrix *IP;   /* For transition matrices. */
I_Matrix *IR;   /* For observation matrices. */
I_Matrix **IW;  /* For reward matrices */

/* These variables are used by the parser only, to keep some state
   information. 
*/
/* These are set when the appropriate preamble line is encountered.  This will
   allow us to check to make sure each is specified.  If observations are not
   defined then we will assume it is a regular MDP, and otherwise assume it 
   is a POMDP
   */
int discountDefined = 0;
int valuesDefined = 0;
int statesDefined = 0;
int actionsDefined = 0;
int observationsDefined = 0;
int goalsDefined = 0;

/* We only want to check when observation probs. are specified, but
   there was no observations in preamble. */
int observationSpecDefined = 0;

/* When we encounter a matrix with too many entries.  We would like
   to only generate one error message, instead of one for each entry.
   This variable is cleared at the start of reading  a matrix and
   set when there are too many entries. */
int gTooManyEntries = 0;

/* These will hold the goal states. */
int gGoalList[100];
int gNumGoals = 0;


# line 139 "parser.y"
typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
 {
  Constant_Block *constBlk;
  int i_num;
  double f_num;
} YYSTYPE;
# define INTTOK 1
# define FLOATTOK 2
# define COLONTOK 3
# define MINUSTOK 4
# define PLUSTOK 5
# define STRINGTOK 6
# define ASTERICKTOK 7
# define DISCOUNTTOK 8
# define VALUESTOK 9
# define STATETOK 10
# define ACTIONTOK 11
# define OBSTOK 12
# define TTOK 13
# define OTOK 14
# define RTOK 15
# define UNIFORMTOK 16
# define IDENTITYTOK 17
# define REWARDTOK 18
# define COSTTOK 19
# define RESETTOK 20
# define STARTTOK 21
# define INCLUDETOK 22
# define EXCLUDETOK 23
# define GOALTOK 24
# define EOFTOK 258

#include <inttypes.h>

#ifdef __STDC__
#include <stdlib.h>
#include <string.h>
#define	YYCONST	const
#else
#include <malloc.h>
#include <memory.h>
#define	YYCONST
#endif

#include <values.h>

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
#ifndef yyerror
#if defined(__cplusplus)
	void yyerror(YYCONST char *);
#endif
#endif
#ifndef yylex
	int yylex(void);
#endif
	int yyparse(void);
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif

#endif

#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
YYSTYPE yylval;
YYSTYPE yyval;
typedef int yytabelem;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#if YYMAXDEPTH > 0
int yy_yys[YYMAXDEPTH], *yys = yy_yys;
YYSTYPE yy_yyv[YYMAXDEPTH], *yyv = yy_yyv;
#else	/* user does initial allocation */
int *yys;
YYSTYPE *yyv;
#endif
static int yymaxdepth = YYMAXDEPTH;
# define YYERRCODE 256

# line 797 "parser.y"


/********************************************************************/
/*              External Routines                                   */
/********************************************************************/

#define EPSILON  0.00001  /* tolerance for sum of probs == 1 */

Constant_Block *aConst;

/******************************************************************************/
void yyerror(char *string)
{
   ERR_enter("Parser<yyparse>", currentLineNumber, PARSE_ERR,"");
}  /* yyerror */
/******************************************************************************/
void checkMatrix() {
/* When a matrix is finished being read for the exactly correct number of
   values, curRow should be 0 and curCol should be -1.  For the cases
   where we are only interested in a row of entries curCol should be -1.
   If we get too many entries, then we will catch this as we parse the 
   extra entries.  Therefore, here we only need to check for too few 
   entries.
   */

   switch( curMatrixContext ) {
   case mc_trans_row:
      if( curCol < gNumStates )
         ERR_enter("Parser<checkMatrix>:", currentLineNumber, 
                   TOO_FEW_ENTRIES, "");
      break;
   case mc_trans_all:
      if((curRow < (gNumStates-1) )
	 || ((curRow == (gNumStates-1))
	     && ( curCol < gNumStates ))) 
	ERR_enter("Parser<checkMatrix>:", currentLineNumber,  
                   TOO_FEW_ENTRIES, "" );
      break;
   case mc_obs_row:
      if( curCol < gNumObservations )
         ERR_enter("Parser<checkMatrix>:", currentLineNumber, 
                   TOO_FEW_ENTRIES, "");
      break;
   case mc_obs_all:
      if((curRow < (gNumStates-1) )
	 || ((curRow == (gNumStates-1))
	     && ( curCol < gNumObservations ))) 
         ERR_enter("Parser<checkMatrix>:", currentLineNumber,  
                   TOO_FEW_ENTRIES, "" );
      break;
   case mc_start_belief:
      if( curCol < gNumStates )
	ERR_enter("Parser<checkMatrix>:", currentLineNumber, 
		  TOO_FEW_ENTRIES, "");
      break;

    case mc_mdp_start:
      /* We will check for invalid multiple entries for MDP in 
	 enterMatrix() */
      break;

    case mc_reward_row:
      if( gProblemType == POMDP_problem_type )
	if( curCol < gNumObservations )
	  ERR_enter("Parser<checkMatrix>:", currentLineNumber, 
		    TOO_FEW_ENTRIES, "");
      break;

    case mc_reward_all:
      if( gProblemType == POMDP_problem_type ) {
	if((curRow < (gNumStates-1) )
	   || ((curRow == (gNumStates-1))
	       && ( curCol < gNumObservations ))) 
	  ERR_enter("Parser<checkMatrix>:", currentLineNumber,  
		    TOO_FEW_ENTRIES, "" );
      }
      else
	if( curCol < gNumStates )
	  ERR_enter("Parser<checkMatrix>:", currentLineNumber, 
		    TOO_FEW_ENTRIES, "");
      
      break;
    case mc_reward_single:
      /* Don't need to do anything */
      break;

    case mc_reward_mdp_only:
      if((curRow < (gNumStates-1) )
	 || ((curRow == (gNumStates-1))
	     && ( curCol < gNumStates ))) 
	ERR_enter("Parser<checkMatrix>:", currentLineNumber,  
		  TOO_FEW_ENTRIES, "" );
      break;

   default:
      ERR_enter("Parser<checkMatrix>:", currentLineNumber, 
                BAD_MATRIX_CONTEXT, "" );
      break;
   }  /* switch */

   if( gTooManyEntries )
     ERR_enter("Parser<checkMatrix>:", currentLineNumber, 
	       TOO_MANY_ENTRIES, "" );

   /* After reading a line for immediate rewards for a pomdp, we must tell
      the data structures for the special representation that we are done */
   switch( curMatrixContext ) {
   case mc_reward_row:
   case mc_reward_all:
   case mc_reward_mdp_only:
     doneImmReward();
     break;

     /* This case is only valid for POMDPs, so if we have an MDP, we
	never would have started a new immediate reward, so calling 
	the doneImmReward will be in error.  */
   case mc_reward_single:
     if( gProblemType == POMDP_problem_type )
       doneImmReward();
     break;
   default:
     break;
   }  /* switch */
   

   curMatrixContext = mc_none;  /* reset this as a safety precaution */
}  /* checkMatrix */
/******************************************************************************/
void enterString( Constant_Block *block ) {
   
   if( H_enter( block->theValue.theString, curMnemonic ) == 0 )
      ERR_enter("Parser<enterString>:", currentLineNumber, 
                DUPLICATE_STRING, block->theValue.theString );

   free( block->theValue.theString );
   free( block );
}  /* enterString */
/******************************************************************************/
void enterUniformMatrix( ) {
   int a, i, j, obs;
   double prob;

   switch( curMatrixContext ) {
   case mc_trans_row:
      prob = 1.0/gNumStates;
      for( a = minA; a <= maxA; a++ )
         for( i = minI; i <= maxI; i++ )
            for( j = 0; j < gNumStates; j++ )
	       addEntryToIMatrix( IP[a], i, j, prob );
      break;
   case mc_trans_all:
      prob = 1.0/gNumStates;
      for( a = minA; a <= maxA; a++ )
         for( i = 0; i < gNumStates; i++ )
            for( j = 0; j < gNumStates; j++ )
 	       addEntryToIMatrix( IP[a], i, j, prob );
      break;
   case mc_obs_row:
      prob = 1.0/gNumObservations;
      for( a = minA; a <= maxA; a++ )
         for( j = minJ; j <= maxJ; j++ )
            for( obs = 0; obs < gNumObservations; obs++ )
 	       addEntryToIMatrix( IR[a], j, obs, prob );
      break;
   case mc_obs_all:
      prob = 1.0/gNumObservations;
      for( a = minA; a <= maxA; a++ )
         for( j = 0; j < gNumStates; j++ )
            for( obs = 0; obs < gNumObservations; obs++ )
 	       addEntryToIMatrix( IR[a], j, obs, prob );
      break;
   case mc_start_belief:
      setStartStateUniform();
      break;
   case mc_mdp_start:
      /* This is meaning less for an MDP */
      ERR_enter("Parser<enterUniformMatrix>:", currentLineNumber, 
                BAD_START_STATE_TYPE, "" );
      break;
   default:
      ERR_enter("Parser<enterUniformMatrix>:", currentLineNumber, 
                BAD_MATRIX_CONTEXT, "" );
      break;
   }  /* switch */
}  /* enterUniformMatrix */
/******************************************************************************/
void enterIdentityMatrix( ) {
   int a, i,j;

   switch( curMatrixContext ) {
   case mc_trans_all:
      for( a = minA; a <= maxA; a++ )
         for( i = 0; i < gNumStates; i++ )
            for( j = 0; j < gNumStates; j++ )
               if( i == j )
		 addEntryToIMatrix( IP[a], i, j, 1.0 );
               else
		 addEntryToIMatrix( IP[a], i, j, 0.0 );
      break;
   default:
      ERR_enter("Parser<enterIdentityMatrix>:", currentLineNumber, 
                BAD_MATRIX_CONTEXT, "" );
      break;
   }  /* switch */
}  /* enterIdentityMatrix */
/******************************************************************************/
void enterResetMatrix( ) {
  int a, i, j;

  if( curMatrixContext != mc_trans_row ) {
    ERR_enter("Parser<enterMatrix>:", currentLineNumber, 
	      BAD_RESET_USAGE, "" );
    return;
  }

  if( gProblemType == POMDP_problem_type )
    for( a = minA; a <= maxA; a++ )
      for( i = minI; i <= maxI; i++ )
	for( j = 0; j < gNumStates; j++ )
	  addEntryToIMatrix( IP[a], i, j, gInitialBelief[j] );
  
  else  /* It is an MDP */
    for( a = minA; a <= maxA; a++ )
      for( i = minI; i <= maxI; i++ )
	addEntryToIMatrix( IP[a], i, gInitialState, 1.0 );
  

}  /* enterResetMatrix */
/******************************************************************************/
void enterMatrix( double value ) {
/*
  For the '_single' context types we never have to worry about setting or 
  checking the bounds on the current row or col.  For all other we do and
  how this is done depends on the context.  Notice that we are filling in the 
  elements in reverse order due to the left-recursive grammar.  Thus
  we need to update the col and row backwards 
  */
   int a, i, j, obs;

   switch( curMatrixContext ) {
   case mc_trans_single:
      for( a = minA; a <= maxA; a++ )
         for( i = minI; i <= maxI; i++ )
            for( j = minJ; j <= maxJ; j++ )
	      addEntryToIMatrix( IP[a], i, j, value );
      break;
   case mc_trans_row:
      if( curCol < gNumStates ) {
         for( a = minA; a <= maxA; a++ )
            for( i = minI; i <= maxI; i++ )
	      addEntryToIMatrix( IP[a], i, curCol, value );
         curCol++;
      }
      else
	gTooManyEntries = 1;

      break;
   case mc_trans_all:
      if( curCol >= gNumStates ) {
         curRow++;
         curCol = 0;;
      }

      if( curRow < gNumStates ) {
         for( a = minA; a <= maxA; a++ )
	   addEntryToIMatrix( IP[a], curRow, curCol, value );
         curCol++;
      }
      else
	gTooManyEntries = 1;

      break;

   case mc_obs_single:

      if( gProblemType == POMDP_problem_type )
	/* We ignore this if it is an MDP */

	for( a = minA; a <= maxA; a++ )
	  for( j = minJ; j <= maxJ; j++ )
            for( obs = minObs; obs <= maxObs; obs++ )
	      addEntryToIMatrix( IR[a], j, obs, value );
      break;

   case mc_obs_row:
      if( gProblemType == POMDP_problem_type )
	/* We ignore this if it is an MDP */

	if( curCol < gNumObservations ) {

	  for( a = minA; a <= maxA; a++ )
            for( j = minJ; j <= maxJ; j++ )
	      addEntryToIMatrix( IR[a], j, curCol, value );
	  
	  curCol++;
	}
	else
	  gTooManyEntries = 1;

      break;

   case mc_obs_all:
      if( curCol >= gNumObservations ) {
         curRow++;
         curCol = 0;
      }

      if( gProblemType == POMDP_problem_type )
	/* We ignore this if it is an MDP */

	if( curRow < gNumStates ) {
	  for( a = minA; a <= maxA; a++ )
	    addEntryToIMatrix( IR[a], curRow, curCol, value );
	  
	  curCol++;
	}
	else
	  gTooManyEntries = 1;

      break;

/* This is a special case for POMDPs, since we need a special 
   representation for immediate rewards for POMDP's.  Note that this 
   is not valid syntax for an MDP, but we flag this error when we set 
   the matrix context, so we ignore the MDP case here.
   */
   case mc_reward_single:
      if( gProblemType == POMDP_problem_type ) {

	if( curCol == 0 ) {
	  enterImmReward( 0, 0, 0, value );
	  curCol++;
	}
	else
	  gTooManyEntries = 1;

      }
     break;

    case mc_reward_row:
      if( gProblemType == POMDP_problem_type ) {

	/* This is a special case for POMDPs, since we need a special 
	   representation for immediate rewards for POMDP's */
   
	if( curCol < gNumObservations ) {
	  enterImmReward( 0, 0, curCol, value );
	  curCol++;
	}
	else
	  gTooManyEntries = 1;

      }  /* if POMDP problem */

      else /* we are dealing with an MDP, so there should only be 
	      a single entry */
	if( curCol == 0 ) {
	  enterImmReward( 0, 0, 0, value );
	  curCol++;
	}
	else
	  gTooManyEntries = 1;


     break;

   case mc_reward_all:

      /* This is a special case for POMDPs, since we need a special 
	 representation for immediate rewards for POMDP's */

      if( gProblemType == POMDP_problem_type ) {
	if( curCol >= gNumObservations ) {
	  curRow++;
	  curCol = 0;
	}
	if( curRow < gNumStates ) {
	  enterImmReward( 0, curRow, curCol, value );
	  curCol++;
	}
	else
	  gTooManyEntries = 1;

      }  /* If POMDP problem */

      /* Otherwise it is an MDP and we should be expecting an entire
	 row of rewards. */

      else  /* MDP */
	if( curCol < gNumStates ) {
	  enterImmReward( 0, curCol, 0, value );
	  curCol++;
	}
	else
	  gTooManyEntries = 1;

      break;

      /* This is a special case for an MDP only where we specify
	 the entire matrix of rewards. If we are erroneously 
	 definining a POMDP, this error will be flagged in the 
	 setMatrixContext() routine.
	 */

    case mc_reward_mdp_only:
      if( gProblemType == MDP_problem_type ) {
	if( curCol >= gNumStates ) {
	  curRow++;
	  curCol = 0;
	}
	if( curRow < gNumStates ) {
	  enterImmReward( curRow, curCol, 0, value );
	  curCol++;
	}
	else
	  gTooManyEntries = 1;

      }
      break;

    case mc_mdp_start:

      /* For an MDP we only want to see a single value and */
      /* we want it to correspond to a valid state number. */

      if( curCol > 0 )
	gTooManyEntries = 1;

      else {
	gInitialState = (int) value;
	curCol++;
      }
      break;
	  
   case mc_start_belief:

      /* This will process the individual entries when a starting */
      /* belief state is fully specified.  When it is a POMDP, we need */
      /* an entry for each state, so we keep the curCol variable */
      /* updated.  */

      if( curCol < gNumStates ) {
	gInitialBelief[curCol] = value;
	curCol++;
      }
      else
	gTooManyEntries = 1;

      break;

   default:
      ERR_enter("Parser<enterMatrix>:", currentLineNumber, 
                BAD_MATRIX_CONTEXT, "");
      break;
   }  /* switch */

}  /* enterMatrix */
/******************************************************************************/
void setMatrixContext( Matrix_Context context, 
                      int a, int i, int j, int obs ) {
/* 
   Note that we must enter the matrix entries in reverse order because
   the matrices are defined with left-recursive rules.  Set the a, i,
   and j parameters to be less than zero when you want to define it
   for all possible values.  

   Rewards for MDPs and POMDPs differ since in the former, rewards are not
   based upon an observations.  This complicates things since not only is one 
   of the reward syntax options not valid, but the semantics of all the
   rewards change as well.  I have chosen to handle this in this routine.  
   I will check for the appropriate type and set the context to handle the
   proper amount of entries.
*/
  int state;

   curMatrixContext = context;
   gTooManyEntries = 0;  /* Clear this out before reading any */

   curRow = 0;  /* This is ignored for some contexts */
   curCol = 0;

   switch( curMatrixContext ) {

   mc_start_belief:
     
     break;

   case mc_start_include:

     /* When we specify the starting belief state as a list of states */
     /* to include, we initialize all state to 0.0, since as we read */
     /* the states we will set that particular value to 1.0.  After it */
     /* is all done we can then just normalize the belief state */

     if( gProblemType == POMDP_problem_type )
       for( state = 0; state < gNumStates; state++ )
	 gInitialBelief[state] = 0.0;

     else  /* It is an MDP which is not valid */
       ERR_enter("Parser<setMatrixContext>:", currentLineNumber, 
		 BAD_START_STATE_TYPE, "");
      
     break;

   case mc_start_exclude:

     /* When we are specifying the starting belief state as a a list */
     /* of states, we initialize all states to 1.0 and as we read each */
     /* in the list we clear it out to be zero.  fter it */
     /* is all done we can then just normalize the belief state */

     if( gProblemType == POMDP_problem_type )
       for( state = 0; state < gNumStates; state++ )
	 gInitialBelief[state] = 1.0;

     else  /* It is an MDP which is not valid */
       ERR_enter("Parser<setMatrixContext>:", currentLineNumber, 
		 BAD_START_STATE_TYPE, "");

     break;

  /* We need a special representation for the immediate rewards.
     These four cases initialize the data structure that will be
     needed for immediate rewards by calling newImmReward.  Note that
     the arguments will differe depending upon whether it is an
     MDP or POMDP.
     */
  case mc_reward_mdp_only:
    if( gProblemType == POMDP_problem_type )  {
       ERR_enter("Parser<setMatrixContext>:", currentLineNumber, 
		 BAD_REWARD_SYNTAX, "");
    }
    else {
      newImmReward( a, NOT_PRESENT, NOT_PRESENT, 0 );
    } 
    break;
 
  case mc_reward_all:	
    if( gProblemType == POMDP_problem_type ) 
      newImmReward( a, i, NOT_PRESENT, NOT_PRESENT );

    else {
      newImmReward( a, i, NOT_PRESENT, 0 );
    }
    break;
  case mc_reward_row:
    if( gProblemType == POMDP_problem_type ) 
      newImmReward( a, i, j, NOT_PRESENT );
    
    else {
      newImmReward( a, i, j, 0 );
    } 
    break;
  case mc_reward_single:

    if( gProblemType == MDP_problem_type ) {
       ERR_enter("Parser<setMatrixContext>:", currentLineNumber, 
		 BAD_REWARD_SYNTAX, "");
    }
    else {
       newImmReward( a, i, j, obs );
     }
    break;

   default:
     break;
   }

  /* These variable settings will define the range over which the current 
     matrix context will have effect.  This accounts for wildcards by
     setting the range to include everything.  When a single entry was
     specified, the range is that single number.  When we actually 
     start to read the matrix, each entry we see will apply for the
     entire range specified, though for specific entries the range 
     will be a single number.
     */
   if( a < 0 ) {
      minA = 0;
      maxA = gNumActions - 1;
   }
   else
      minA = maxA = a;

   if( i < 0 ) {
      minI = 0;
      maxI = gNumStates - 1;
   }
   else
      minI = maxI = i;

   if( j < 0 ) {
      minJ = 0;
      maxJ = gNumStates - 1;
   }
   else
      minJ = maxJ = j;

   if( obs < 0 ) {
      minObs = 0;
      maxObs = gNumObservations - 1;
   }
   else
      minObs = maxObs = obs;

}  /* setMatrixContext */
/******************************************************************************/
void enterStartState( int i ) {
/*
   This is not valid for an MDP, but the error has already been flagged
   in the setMatrixContext() routine.  Therefore, if just igore this if 
   it is an MDP.
*/

  if( gProblemType == MDP_problem_type )
    return;

  switch( curMatrixContext ) {
  case mc_start_include:
    gInitialBelief[i] = 1.0;
    break;
  case mc_start_exclude:
    gInitialBelief[i] = 0.0;
    break;
  default:
    ERR_enter("Parser<enterStartState>:", currentLineNumber, 
	      BAD_MATRIX_CONTEXT, "");
      break;
  } /* switch */
}  /* enterStartState */
/******************************************************************************/
void setStartStateUniform() {
  int i;
  double prob;

  if( gProblemType != POMDP_problem_type )
    return;

  prob = 1.0/gNumStates;
  for( i = 0; i < gNumStates; i++ )
    gInitialBelief[i] = prob;

}  /*  setStartStateUniform*/
/******************************************************************************/
void endStartStates() {
/*
   There are a few cases where the matrix context will not be
   set at this point.  When there is a list of probabilities
   or if it is an MDP the context will have been cleared.
   */
  int i;
  double prob;

  if( gProblemType == MDP_problem_type ) {
    curMatrixContext = mc_none;  /* just to be sure */
    return;
  }
    
  switch( curMatrixContext ) {
  case mc_start_include:
  case mc_start_exclude:
    /* At this point gInitialBelief should be a vector of 1.0's and 0.0's
       being set as each is either included or excluded.  Now we need to
       normalized them to make it a true probability distribution */
    prob = 0.0;
    for( i = 0; i < gNumStates; i++ )
      prob += gInitialBelief[i];
    if( prob <= 0.0 ) {
      ERR_enter("Parser<endStartStates>:", currentLineNumber, 
                BAD_START_PROB_SUM, "" );
      return;
    }
    for( i = 0; i < gNumStates; i++ )
      gInitialBelief[i] /= prob;
    break;

  default:  /* Make sure we have a valid prob. distribution */
    prob = 0.0;
    for( i = 0; i < gNumStates; i++ ) 
      prob += gInitialBelief[i];
    if((prob < ( 1.0 - EPSILON)) || (prob > (1.0 + EPSILON))) {
      ERR_enter("Parser<endStartStates>:", NO_LINE, 
		BAD_START_PROB_SUM, "" );
    }
    break;
  }  /* switch */

  curMatrixContext = mc_none;

}  /* endStartStates */
/******************************************************************************/
void verifyPreamble() {
/* 
   When a param is not defined, set these to non-zero so parsing can
   proceed even in the absence of specifying these values.  When an
   out of range value is encountered the parser will flag the error,
   but return 0 so that more errors can be detected 
   */

   if( discountDefined == 0 )
      ERR_enter("Parser<verifyPreamble>:", currentLineNumber, 
                MISSING_DISCOUNT, "" );
   if( valuesDefined == 0 )
      ERR_enter("Parser<verifyPreamble>:", currentLineNumber,
                MISSING_VALUES, "" );
   if( statesDefined == 0 ) {
      ERR_enter("Parser<verifyPreamble>:", currentLineNumber, 
                MISSING_STATES, "" );
      gNumStates = 1;
   }
   if( actionsDefined == 0 ) {
      ERR_enter("Parser<verifyPreamble>:", currentLineNumber, 
                MISSING_ACTIONS, "" );
      gNumActions = 1;
   }

   /* If we do not see this, them we must be parsing an MDP */
   if( observationsDefined == 0 ) {
     gNumObservations = 0;
     gProblemType = MDP_problem_type;
   }

   else
     gProblemType = POMDP_problem_type;

#if 0
   if( goalsDefined == 0 ) {
      ERR_enter("Parser<verifyPreamble>:", currentLineNumber, 
                MISSING_GOALS, "" );
      gNumGoals = 0;
   }
#endif

}  /* verifyPreamble */
/******************************************************************************/
void checkProbs() {
   int a,i,j,obs;
   double sum;
   char str[40];

   
   for( a = 0; a < gNumActions; a++ )
      for( i = 0; i < gNumStates; i++ ) {
	 sum = sumIMatrixRowValues( IP[a], i );
         if((sum < ( 1.0 - EPSILON)) || (sum > (1.0 + EPSILON))) {
            sprintf( str, "action=%d, state=%d (%.5lf)", a, i, sum );
            ERR_enter("Parser<checkProbs>:", NO_LINE, 
                      BAD_TRANS_PROB_SUM, str );
         }
      } /* for i */

   if( gProblemType == POMDP_problem_type )
     for( a = 0; a < gNumActions; a++ )
       for( j = 0; j < gNumStates; j++ ) {
	 sum = sumIMatrixRowValues( IR[a], j );
         if((sum < ( 1.0 - EPSILON)) || (sum > (1.0 + EPSILON))) {
	   sprintf( str, "action=%d, state=%d (%.5lf)", a, j, sum );
	   ERR_enter("Parser<checkProbs>:", NO_LINE, 
		     BAD_OBS_PROB_SUM, str );
         } /* if sum not == 1 */
       }  /* for j */

   /* Now see if we had observation specs defined in an MDP */

   if( observationSpecDefined && (gProblemType == MDP_problem_type))
     ERR_enter("Parser<checkProbs>:", NO_LINE, 
	       OBS_IN_MDP_PROBLEM, "" );

}  /* checkProbs */
/************************************************************************/
void initParser() {
/*
   This routine will reset all the state variables used by the parser
   in case it will parse multiple files.
*/
   observationSpecDefined = 0;
   discountDefined = 0;
   valuesDefined = 0;
   statesDefined = 0;
   actionsDefined = 0;
   observationsDefined = 0;
   observationSpecDefined = 0;
   currentLineNumber = 1;
   curMnemonic = nt_unknown;
   curMatrixContext = mc_none;

}  /* initParser */
/************************************************************************/
int readMDPFile( FILE *file ) {
   int returnValue;
   extern FILE *yyin;

   initParser();

   ERR_initialize();
   H_create();
   yyin = file;

   returnValue = yyparse();

   /* If there are syntax errors, then we have to do something if we 
      want to parse another file without restarting.  It seems that
      a syntax error bombs the code out, but leaves the file pointer
      at the place it bombed.  Thus, another call to yyparse() will
      pick up where it left off and not necessarily at the start of a 
      new file.

      Unfortunately, I do not know how to do this yet.
      */
   if (returnValue != 0) {
      printf("\nParameter file contains syntax errors!\n");
    }

   if (ERR_dump() || returnValue ) 
      return( 0 );

   ERR_cleanUp();
   H_destroy();

   /* This is where intermediate matrix representation are
      converted into their final representation */
   convertMatrices();

   return( 1 );
}  /* readPomdpFile */
/************************************************************************/
static YYCONST yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 113,
	1, 99,
	2, 99,
	-2, 69,
-1, 130,
	1, 99,
	2, 99,
	-2, 67,
-1, 138,
	1, 99,
	2, 99,
	-2, 65,
	};
# define YYNPROD 100
# define YYLAST 151
static YYCONST yytabelem yyact[]={

   114,   113,    72,   124,    84,    90,    68,    83,    49,    71,
    11,    12,    13,    14,    15,    26,    18,    73,    74,    77,
    29,    42,    73,    74,    34,    35,    16,    65,    66,    67,
    32,    31,    69,   133,    27,    28,    70,   107,   108,   125,
    73,    74,    78,   119,   126,   127,    53,    56,    59,    85,
   117,    91,   115,    60,    86,    87,    92,    93,    50,    55,
    52,    48,    45,    46,    50,    50,    50,   103,   101,    99,
    81,    80,    79,    44,    82,    43,    24,    23,    22,    21,
    20,    19,   104,   120,    88,   134,    95,    97,    98,   139,
    96,   102,   118,    98,   132,    94,   106,   100,   116,   131,
    89,    64,    63,    62,   105,    61,   110,    76,   112,   111,
   109,    75,    82,    41,   121,    58,    57,    39,    54,    38,
   122,    51,   130,   123,   129,   128,    37,    47,    36,    33,
    10,   121,     9,     8,   135,   136,   138,   137,     7,   121,
   140,     6,     5,     4,    40,    25,    17,     3,     2,     1,
    30 };
static YYCONST yytabelem yypact[]={

-10000000,-10000000,     2,    -5,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,    78,    77,    76,    75,    74,    73,-10000000,    12,    26,
     6,-10000000,-10000000,-10000000,-10000000,-10000000,    15,    72,    70,-10000000,
    61,-10000000,-10000000,-10000000,-10000000,-10000000,    60,    59,    58,    52,
    14,    16,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,    13,
-10000000,-10000000,-10000000,    13,-10000000,-10000000,    13,-10000000,    41,    13,
-10000000,-10000000,-10000000,-10000000,-10000000,    69,    68,    67,-10000000,-10000000,
-10000000,    39,-10000000,-10000000,-10000000,    48,    48,-10000000,-10000000,    50,
    50,    50,-10000000,    48,-10000000,-10000000,-10000000,-10000000,    48,-10000000,
    66,-10000000,-10000000,-10000000,-10000000,    65,-10000000,    64,-10000000,    48,
    21,    48,    16,    48,    26,    49,-10000000,-10000000,-10000000,    39,
    47,-10000000,    40,    26,-10000000,    48,    16,    38,    16,    48,
    26,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,    30,
    26,    39,    39,    38,    26,-10000000,-10000000,-10000000,    26,    26,
-10000000 };
static YYCONST yytabelem yypgo[]={

     0,     5,     4,     3,   150,     0,     2,   149,   148,   147,
   146,   145,   144,   143,   142,   141,   138,   133,   132,   130,
   129,   128,   127,     8,   126,   121,   119,   118,   117,   116,
   115,   113,     6,   111,     7,   107,   105,   103,   102,   101,
   100,    99,    98,    97,    96,    95,    94,    92,    91,    90,
    89,    85,     1,    83,    82,     9 };
static YYCONST yytabelem yyr1[]={

     0,     9,    11,     7,     8,     8,    13,    13,    13,    13,
    13,    13,    14,    15,    20,    20,    21,    16,    22,    22,
    24,    17,    25,    25,    26,    18,    27,    27,    28,    19,
    29,    29,    31,    10,    10,    33,    10,    35,    10,    10,
    34,    34,    12,    12,    36,    36,    36,    37,    41,    40,
    42,    40,    43,    40,    38,    46,    45,    47,    45,    48,
    45,    39,    50,    49,    51,    49,    53,    49,    54,    49,
    44,    44,    44,    32,    32,    32,    55,    55,    52,    52,
     2,     2,     2,     1,     1,     1,     3,     3,     3,    23,
    23,    30,    30,     6,     6,     5,     5,     4,     4,     4 };
static YYCONST yytabelem yyr2[]={

     0,     1,     1,    11,     5,     0,     2,     2,     2,     2,
     2,     2,     7,     7,     3,     3,     1,     9,     3,     2,
     1,     9,     3,     2,     1,     9,     3,     2,     1,     9,
     2,     2,     1,     8,     7,     1,    10,     1,    10,     1,
     5,     3,     4,     0,     2,     3,     2,     7,     1,    15,
     1,    11,     1,     7,     7,     1,    15,     1,    11,     1,
     7,     7,     1,    19,     1,    15,     1,    11,     1,     7,
     3,     3,     3,     3,     3,     3,     5,     3,     5,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     5,
     3,     5,     3,     3,     3,     5,     5,     3,     3,     1 };
static YYCONST yytabelem yychk[]={

-10000000,    -7,    -8,    -9,   -13,   -14,   -15,   -16,   -17,   -18,
   -19,     8,     9,    10,    11,    12,    24,   -10,    21,     3,
     3,     3,     3,     3,     3,   -11,     3,    22,    23,    -5,
    -4,     5,     4,   -20,    18,    19,   -21,   -24,   -26,   -28,
   -12,   -31,     6,     3,     3,     1,     2,   -22,     1,   -23,
     6,   -25,     1,   -23,   -27,     1,   -23,   -29,   -30,   -23,
     1,   -36,   -37,   -38,   -39,    13,    14,    15,   -32,    16,
    20,   -55,    -6,     1,     2,   -33,   -35,     6,     1,     3,
     3,     3,    -6,   -34,    -2,     1,     6,     7,   -34,   -40,
    -1,     1,     6,     7,   -45,    -1,   -49,    -1,    -2,     3,
   -43,     3,   -48,     3,   -54,    -2,   -44,    16,    17,   -55,
    -2,   -32,    -2,   -52,    -5,     3,   -42,     3,   -47,     3,
   -53,    -5,    -2,   -32,    -3,     1,     6,     7,   -32,    -2,
   -52,   -41,   -46,     3,   -51,    -6,    -6,    -3,   -52,   -50,
    -5 };
static YYCONST yytabelem yydef[]={

     5,    -2,     1,    39,     4,     6,     7,     8,     9,    10,
    11,     0,     0,     0,     0,     0,     0,     2,     0,    99,
     0,    16,    20,    24,    28,    43,    32,     0,     0,    12,
     0,    97,    98,    13,    14,    15,     0,     0,     0,     0,
     3,     0,    34,    35,    37,    95,    96,    17,    18,    19,
    90,    21,    22,    23,    25,    26,    27,    29,    30,    31,
    92,    42,    44,    45,    46,     0,     0,     0,    33,    73,
    74,    75,    77,    93,    94,     0,     0,    89,    91,     0,
     0,     0,    76,    36,    41,    80,    81,    82,    38,    47,
    52,    83,    84,    85,    54,    59,    61,    68,    40,     0,
     0,     0,     0,     0,    99,    50,    53,    70,    71,    72,
    57,    60,    66,    -2,    79,     0,     0,     0,     0,     0,
    99,    78,    48,    51,    55,    86,    87,    88,    58,    64,
    -2,     0,     0,     0,    99,    49,    56,    62,    -2,    99,
    63 };
typedef struct
#ifdef __cplusplus
	yytoktype
#endif
{
#ifdef __cplusplus
const
#endif
char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"INTTOK",	1,
	"FLOATTOK",	2,
	"COLONTOK",	3,
	"MINUSTOK",	4,
	"PLUSTOK",	5,
	"STRINGTOK",	6,
	"ASTERICKTOK",	7,
	"DISCOUNTTOK",	8,
	"VALUESTOK",	9,
	"STATETOK",	10,
	"ACTIONTOK",	11,
	"OBSTOK",	12,
	"TTOK",	13,
	"OTOK",	14,
	"RTOK",	15,
	"UNIFORMTOK",	16,
	"IDENTITYTOK",	17,
	"REWARDTOK",	18,
	"COSTTOK",	19,
	"RESETTOK",	20,
	"STARTTOK",	21,
	"INCLUDETOK",	22,
	"EXCLUDETOK",	23,
	"GOALTOK",	24,
	"EOFTOK",	258,
	"-unknown-",	-1	/* ends search */
};

#ifdef __cplusplus
const
#endif
char * yyreds[] =
{
	"-no such reduction-",
	"pomdp_file : preamble",
	"pomdp_file : preamble start_state",
	"pomdp_file : preamble start_state param_list",
	"preamble : preamble param_type",
	"preamble : /* empty */",
	"param_type : discount_param",
	"param_type : value_param",
	"param_type : state_param",
	"param_type : action_param",
	"param_type : obs_param",
	"param_type : goal_param",
	"discount_param : DISCOUNTTOK COLONTOK number",
	"value_param : VALUESTOK COLONTOK value_tail",
	"value_tail : REWARDTOK",
	"value_tail : COSTTOK",
	"state_param : STATETOK COLONTOK",
	"state_param : STATETOK COLONTOK state_tail",
	"state_tail : INTTOK",
	"state_tail : ident_list",
	"action_param : ACTIONTOK COLONTOK",
	"action_param : ACTIONTOK COLONTOK action_tail",
	"action_tail : INTTOK",
	"action_tail : ident_list",
	"obs_param : OBSTOK COLONTOK",
	"obs_param : OBSTOK COLONTOK obs_param_tail",
	"obs_param_tail : INTTOK",
	"obs_param_tail : ident_list",
	"goal_param : GOALTOK COLONTOK",
	"goal_param : GOALTOK COLONTOK goal_param_tail",
	"goal_param_tail : goal_list",
	"goal_param_tail : ident_list",
	"start_state : STARTTOK COLONTOK",
	"start_state : STARTTOK COLONTOK u_matrix",
	"start_state : STARTTOK COLONTOK STRINGTOK",
	"start_state : STARTTOK INCLUDETOK COLONTOK",
	"start_state : STARTTOK INCLUDETOK COLONTOK start_state_list",
	"start_state : STARTTOK EXCLUDETOK COLONTOK",
	"start_state : STARTTOK EXCLUDETOK COLONTOK start_state_list",
	"start_state : /* empty */",
	"start_state_list : start_state_list state",
	"start_state_list : state",
	"param_list : param_list param_spec",
	"param_list : /* empty */",
	"param_spec : trans_prob_spec",
	"param_spec : obs_prob_spec",
	"param_spec : reward_spec",
	"trans_prob_spec : TTOK COLONTOK trans_spec_tail",
	"trans_spec_tail : action COLONTOK state COLONTOK state",
	"trans_spec_tail : action COLONTOK state COLONTOK state prob",
	"trans_spec_tail : action COLONTOK state",
	"trans_spec_tail : action COLONTOK state u_matrix",
	"trans_spec_tail : action",
	"trans_spec_tail : action ui_matrix",
	"obs_prob_spec : OTOK COLONTOK obs_spec_tail",
	"obs_spec_tail : action COLONTOK state COLONTOK obs",
	"obs_spec_tail : action COLONTOK state COLONTOK obs prob",
	"obs_spec_tail : action COLONTOK state",
	"obs_spec_tail : action COLONTOK state u_matrix",
	"obs_spec_tail : action",
	"obs_spec_tail : action u_matrix",
	"reward_spec : RTOK COLONTOK reward_spec_tail",
	"reward_spec_tail : action COLONTOK state COLONTOK state COLONTOK obs",
	"reward_spec_tail : action COLONTOK state COLONTOK state COLONTOK obs number",
	"reward_spec_tail : action COLONTOK state COLONTOK state",
	"reward_spec_tail : action COLONTOK state COLONTOK state num_matrix",
	"reward_spec_tail : action COLONTOK state",
	"reward_spec_tail : action COLONTOK state num_matrix",
	"reward_spec_tail : action",
	"reward_spec_tail : action num_matrix",
	"ui_matrix : UNIFORMTOK",
	"ui_matrix : IDENTITYTOK",
	"ui_matrix : prob_matrix",
	"u_matrix : UNIFORMTOK",
	"u_matrix : RESETTOK",
	"u_matrix : prob_matrix",
	"prob_matrix : prob_matrix prob",
	"prob_matrix : prob",
	"num_matrix : num_matrix number",
	"num_matrix : number",
	"state : INTTOK",
	"state : STRINGTOK",
	"state : ASTERICKTOK",
	"action : INTTOK",
	"action : STRINGTOK",
	"action : ASTERICKTOK",
	"obs : INTTOK",
	"obs : STRINGTOK",
	"obs : ASTERICKTOK",
	"ident_list : ident_list STRINGTOK",
	"ident_list : STRINGTOK",
	"goal_list : goal_list INTTOK",
	"goal_list : INTTOK",
	"prob : INTTOK",
	"prob : FLOATTOK",
	"number : optional_sign INTTOK",
	"number : optional_sign FLOATTOK",
	"optional_sign : PLUSTOK",
	"optional_sign : MINUSTOK",
	"optional_sign : /* empty */",
};
#endif /* YYDEBUG */
# line	1 "/usr/ccs/bin/yaccpar"
/*
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#pragma ident	"@(#)yaccpar	6.16	99/01/20 SMI"

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#define YYNEW(type)	malloc(sizeof(type) * yynewmax)
#define YYCOPY(to, from, type) \
	(type *) memcpy(to, (char *) from, yymaxdepth * sizeof (type))
#define YYENLARGE( from, type) \
	(type *) realloc((char *) from, yynewmax * sizeof(type))
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-10000000)

/*
** global variables used by the parser
*/
YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



#ifdef YYNMBCHARS
#define YYLEX()		yycvtok(yylex())
/*
** yycvtok - return a token if i is a wchar_t value that exceeds 255.
**	If i<255, i itself is the token.  If i>255 but the neither 
**	of the 30th or 31st bit is on, i is already a token.
*/
#if defined(__STDC__) || defined(__cplusplus)
int yycvtok(int i)
#else
int yycvtok(i) int i;
#endif
{
	int first = 0;
	int last = YYNMBCHARS - 1;
	int mid;
	wchar_t j;

	if(i&0x60000000){/*Must convert to a token. */
		if( yymbchars[last].character < i ){
			return i;/*Giving up*/
		}
		while ((last>=first)&&(first>=0)) {/*Binary search loop*/
			mid = (first+last)/2;
			j = yymbchars[mid].character;
			if( j==i ){/*Found*/ 
				return yymbchars[mid].tvalue;
			}else if( j<i ){
				first = mid + 1;
			}else{
				last = mid -1;
			}
		}
		/*No entry in the table.*/
		return i;/* Giving up.*/
	}else{/* i is already a token. */
		return i;
	}
}
#else/*!YYNMBCHARS*/
#define YYLEX()		yylex()
#endif/*!YYNMBCHARS*/

/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
#if defined(__STDC__) || defined(__cplusplus)
int yyparse(void)
#else
int yyparse()
#endif
{
	register YYSTYPE *yypvt = 0;	/* top of value stack for $vars */

#if defined(__cplusplus) || defined(lint)
/*
	hacks to please C++ and lint - goto's inside
	switch should never be executed
*/
	static int __yaccpar_lint_hack__ = 0;
	switch (__yaccpar_lint_hack__)
	{
		case 1: goto yyerrlab;
		case 2: goto yynewstate;
	}
#endif

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

#if YYMAXDEPTH <= 0
	if (yymaxdepth <= 0)
	{
		if ((yymaxdepth = YYEXPAND(0)) <= 0)
		{
			yyerror("yacc initialization error");
			YYABORT;
		}
	}
#endif

	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */
	goto yystack;	/* moved from 6 lines above to here to please C++ */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			long yyps_index = (yy_ps - yys);
			long yypv_index = (yy_pv - yyv);
			long yypvt_index = (yypvt - yyv);
			int yynewmax;
#ifdef YYEXPAND
			yynewmax = YYEXPAND(yymaxdepth);
#else
			yynewmax = 2 * yymaxdepth;	/* double table size */
			if (yymaxdepth == YYMAXDEPTH)	/* first time growth */
			{
				char *newyys = (char *)YYNEW(int);
				char *newyyv = (char *)YYNEW(YYSTYPE);
				if (newyys != 0 && newyyv != 0)
				{
					yys = YYCOPY(newyys, yys, int);
					yyv = YYCOPY(newyyv, yyv, YYSTYPE);
				}
				else
					yynewmax = 0;	/* failed */
			}
			else				/* not first time */
			{
				yys = YYENLARGE(yys, int);
				yyv = YYENLARGE(yyv, YYSTYPE);
				if (yys == 0 || yyv == 0)
					yynewmax = 0;	/* failed */
			}
#endif
			if (yynewmax <= yymaxdepth)	/* tables not expanded */
			{
				yyerror( "yacc stack overflow" );
				YYABORT;
			}
			yymaxdepth = yynewmax;

			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register YYCONST int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
			skip_init:
				yynerrs++;
				/* FALLTHRU */
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 152 "parser.y"
{ 
		    /* The preamble is a section of the file which */
		    /* must come first and whcih contains some global */
		    /* properties of the MDP that the file */
		    /* specifies. (e.g., number of states).  The */
		    /* observations are optional and its presence or */
		    /* absence is what first tells the parser whether */
		    /* it is parsing an MDP or a POMDP. */

		    verifyPreamble();  /* make sure all things are */
				       /* defined */

		    /* While we parse we use an intermediate */
		    /* representation which will be converted to the */
		    /* sparse representation when we are finished */
		    /* parsing.  After the preamble we are ready to */
		    /* start filling in values and we know how big the */
		    /* problem is, so we allocate the space for the */
		    /* intermediate forms */

		    allocateIntermediateMDP();  
		  } break;
case 2:
# line 176 "parser.y"
{ 
		    /* Some type of algorithms want a place to start */
		    /* off the problem, especially when doing */
		    /* simulation type experiments.  This is an */
		    /* optional argument that allows specification of */
		    /* this.   In a POMDP this is a belief state, but */
		    /* in an MDP this is a single state.  If none is */
		    /* specified for a POMDP, then the uniform */
		    /* distribution over all states is used.  If none */
		    /* is specified for an MDP, then random states */
		    /* will be assumed. */

		    endStartStates(); 
		  } break;
case 3:
# line 195 "parser.y"
{
		    /* This is the very last thing we do while */
		    /* parsing.  Even though the file may conform to */
		    /* the syntax, the semantics of the problem */
		    /* specification requires probability */
		    /* distributions.  This routine will make sure */
		    /* that the appropriate things sum to 1.0 to make */
		    /* a valid probability distribution. This will */
		    /* also generate the error message when */
		    /* observation probabilities are specified in an */
		    /* MDP problem, since this is illegal. */

                     checkProbs();
		     YACCtrace("pomdp_file -> preamble params\n");
                  } break;
case 4:
# line 212 "parser.y"
{
		   YACCtrace("preamble -> preamble param_type\n");
		} break;
case 12:
# line 225 "parser.y"
{
		  /* The discount factor only makes sense when in the */
		  /* range 0 to 1, so it is an error to specify */
		  /* anything outside this range. */

                   gDiscount = yypvt[-0].f_num;
                   if(( gDiscount < 0.0 ) || ( gDiscount > 1.0 ))
                      ERR_enter("Parser<ytab>:", currentLineNumber,
                                BAD_DISCOUNT_VAL, "");
                   discountDefined = 1;
		   YACCtrace("discount_param -> DISCOUNTTOK COLONTOK number\n");
	        } break;
case 13:
# line 239 "parser.y"
{
                   valuesDefined = 1;
		   YACCtrace("value_param -> VALUESTOK COLONTOK value_tail\n");
	        } break;
case 14:
# line 252 "parser.y"
{
                   gValueType = REWARD_value_type;
		} break;
case 15:
# line 256 "parser.y"
{
                   gValueType = COST_value_type;
		} break;
case 16:
# line 261 "parser.y"
{ 
		  /* Since are able to enumerate the states and refer */
		  /* to them by identifiers, we will need to set the */
		  /* current state to indicate that we are parsing */
		  /* states.  This is important, since we will parse */
		  /* observatons and actions in exactly the same */
		  /* manner with the same code.  */
 
		  curMnemonic = nt_state; 

		} break;
case 17:
# line 273 "parser.y"
{
                   statesDefined = 1;
                   curMnemonic = nt_unknown;
		   YACCtrace("state_param -> STATETOK COLONTOK state_tail\n");
		} break;
case 18:
# line 280 "parser.y"
{

		  /*  For the number of states, we can just have a */
		  /*  number indicating how many there are, or ... */

                   gNumStates = yypvt[-0].constBlk->theValue.theInt;
                   if( gNumStates < 1 ) {
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_NUM_STATES, "");
                      gNumStates = 1;
                   }

 		   /* Since we use some temporary storage to hold the
		      integer as we parse, we free the memory when we
		      are done with the value */

                   free( yypvt[-0].constBlk );
		} break;
case 20:
# line 302 "parser.y"
{
		  /* See state_param for explanation of this */

		  curMnemonic = nt_action;  
		} break;
case 21:
# line 308 "parser.y"
{
                   actionsDefined = 1;
                   curMnemonic = nt_unknown;
		   YACCtrace("action_param -> ACTIONTOK COLONTOK action_tail\n");
		} break;
case 22:
# line 315 "parser.y"
{

		  /*  For the number of actions, we can just have a */
		  /*  number indicating how many there are, or ... */

                   gNumActions = yypvt[-0].constBlk->theValue.theInt;
                   if( gNumActions < 1 ) {
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_NUM_ACTIONS, "" );
                      gNumActions = 1;
                   }
		   
		   /* Since we use some temporary storage to hold the
		      integer as we parse, we free the memory when we
		      are done with the value */

                   free( yypvt[-0].constBlk );
		} break;
case 24:
# line 337 "parser.y"
{ 
		  /* See state_param for explanation of this */

		  curMnemonic = nt_observation; 
		} break;
case 25:
# line 343 "parser.y"
{
                   observationsDefined = 1;
                   curMnemonic = nt_unknown;
		   YACCtrace("obs_param -> OBSTOK COLONTOK obs_param_tail\n");
		} break;
case 26:
# line 350 "parser.y"
{

		  /*  For the number of observation, we can just have a */
		  /*  number indicating how many there are, or ... */

                   gNumObservations = yypvt[-0].constBlk->theValue.theInt;
                   if( gNumObservations < 1 ) {
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_NUM_OBS, "" );
                      gNumObservations = 1;
                   }

		   /* Since we use some temporary storage to hold the
		      integer as we parse, we free the memory when we
		      are done with the value */

                   free( yypvt[-0].constBlk );
		} break;
case 28:
# line 372 "parser.y"
{ 
		  /* See state_param for explanation of this */
		  curMnemonic = nt_goal; 
		} break;
case 29:
# line 377 "parser.y"
{
                   goalsDefined = 1;
		   gGoalList[gNumGoals] = -1;
		   curMnemonic = nt_unknown;
		   YACCtrace("goal_param -> GOALTOK COLONTOK goal_param_tail\n");
		} break;
case 32:
# line 389 "parser.y"
{ 
		  /* There are a number of different formats for the */
		  /* start state.  This one is valid for either a */
		  /* POMDP or an MDP.  With a POMDP it will expect a */
		  /* list of probabilities, one for each state, */
		  /* representing the initial belief state.  For an */
		  /* MDP there can be only a single integer */
		  /* representing the starting state. */

		  if( gProblemType == POMDP_problem_type )
		    setMatrixContext(mc_start_belief, 0, 0, 0, 0); 
		  else
		    setMatrixContext(mc_mdp_start, 0, 0, 0, 0); 
		} break;
case 34:
# line 422 "parser.y"
{
                   int num;

		   num = H_lookup( yypvt[-0].constBlk->theValue.theString, nt_state );
		   if(( num < 0 ) || (num >= gNumStates )) {
		     ERR_enter("Parser<ytab>:", currentLineNumber, 
				 BAD_STATE_STR, yypvt[-0].constBlk->theValue.theString );
                   }
                   else
		     if( gProblemType == MDP_problem_type )
		       gInitialState = num;
		     else
		       gInitialBelief[num] = 1.0;


                   free( yypvt[-0].constBlk->theValue.theString );
                   free( yypvt[-0].constBlk );
                } break;
case 35:
# line 442 "parser.y"
{ 
		  setMatrixContext(mc_start_include, 0, 0, 0, 0); 
		} break;
case 37:
# line 448 "parser.y"
{ 
		  setMatrixContext(mc_start_exclude, 0, 0, 0, 0); 
		} break;
case 39:
# line 455 "parser.y"
{ 
		  setStartStateUniform(); 
		} break;
case 40:
# line 460 "parser.y"
{
		  enterStartState( yypvt[-0].i_num );
                } break;
case 41:
# line 464 "parser.y"
{
		  enterStartState( yypvt[-0].i_num );
                } break;
case 45:
# line 473 "parser.y"
{
		    /* If there are observation specifications defined,
		       but no observations listed in the preamble, then
		       this is an error, since regular MDPs don't have
		       the concept of observations.  However, it could 
		       be a POMDP that was just missing the preamble 
		       part.  The way we handle this is to go ahead 
		       and parse the observation specifications, but
		       always check before we actually enter values in
		       a matrix (see the enterMatrix() routine.)  This
		       way we can determine if there are any problems 
		       with the observation specifications.  We cannot
		       add entries to the matrices since there will be
		       no memory allocated for it.  We want to
		       generate an error for this case, but don't want
		       a separate error for each observation
		       specification, so we define a variable that is
		       just a flag for whether or not any observation
		       specificiations have been defined.  After we
		       are all done parsing we will check this flag
		       and generate an error if needed.
		       */

		      observationSpecDefined = 1;
		  } break;
case 47:
# line 501 "parser.y"
{
		   YACCtrace("trans_prob_spec -> TTOK COLONTOK trans_spec_tail\n");
		} break;
case 48:
# line 506 "parser.y"
{ setMatrixContext(mc_trans_single, yypvt[-4].i_num, yypvt[-2].i_num, yypvt[-0].i_num, 0); } break;
case 49:
# line 507 "parser.y"
{
                   enterMatrix( yypvt[-0].f_num );
		   YACCtrace("trans_spec_tail -> action COLONTOK state COLONTOK state prob \n");
		} break;
case 50:
# line 512 "parser.y"
{ setMatrixContext(mc_trans_row, yypvt[-2].i_num, yypvt[-0].i_num, 0, 0); } break;
case 51:
# line 513 "parser.y"
{
		   YACCtrace("trans_spec_tail -> action COLONTOK state ui_matrix \n");
		} break;
case 52:
# line 516 "parser.y"
{ setMatrixContext(mc_trans_all, yypvt[-0].i_num, 0, 0, 0); } break;
case 53:
# line 517 "parser.y"
{
		   YACCtrace("trans_spec_tail -> action ui_matrix\n");
		} break;
case 54:
# line 522 "parser.y"
{
		   YACCtrace("obs_prob_spec -> OTOK COLONTOK  obs_spec_tail\n");
		} break;
case 55:
# line 527 "parser.y"
{ setMatrixContext(mc_obs_single, yypvt[-4].i_num, 0, yypvt[-2].i_num, yypvt[-0].i_num); } break;
case 56:
# line 528 "parser.y"
{
                   enterMatrix( yypvt[-0].f_num );
		   YACCtrace("obs_spec_tail -> action COLONTOK state COLONTOK obs prob \n");
		} break;
case 57:
# line 533 "parser.y"
{ setMatrixContext(mc_obs_row, yypvt[-2].i_num, 0, yypvt[-0].i_num, 0); } break;
case 58:
# line 534 "parser.y"
{
		   YACCtrace("obs_spec_tail -> action COLONTOK state COLONTOK u_matrix\n");
		} break;
case 59:
# line 537 "parser.y"
{ setMatrixContext(mc_obs_all, yypvt[-0].i_num, 0, 0, 0); } break;
case 60:
# line 538 "parser.y"
{
		   YACCtrace("obs_spec_tail -> action u_matrix\n");
		} break;
case 61:
# line 543 "parser.y"
{
		   YACCtrace("reward_spec -> RTOK COLONTOK  reward_spec_tail\n");
		} break;
case 62:
# line 550 "parser.y"
{ setMatrixContext(mc_reward_single, yypvt[-6].i_num, yypvt[-4].i_num, yypvt[-2].i_num, yypvt[-0].i_num); } break;
case 63:
# line 551 "parser.y"
{
                   enterMatrix( yypvt[-0].f_num );

		   /* Only need this for the call to doneImmReward */
		   checkMatrix();  
		   YACCtrace("reward_spec_tail -> action COLONTOK state COLONTOK state COLONTOK obs number\n");
		} break;
case 64:
# line 559 "parser.y"
{ setMatrixContext(mc_reward_row, yypvt[-4].i_num, yypvt[-2].i_num, yypvt[-0].i_num, 0); } break;
case 65:
# line 560 "parser.y"
{
                   checkMatrix();
		   YACCtrace("reward_spec_tail -> action COLONTOK state COLONTOK state num_matrix\n");
		 } break;
case 66:
# line 565 "parser.y"
{ setMatrixContext(mc_reward_all, yypvt[-2].i_num, yypvt[-0].i_num, 0, 0); } break;
case 67:
# line 566 "parser.y"
{
                   checkMatrix();
		   YACCtrace("reward_spec_tail -> action COLONTOK state num_matrix\n");
		} break;
case 68:
# line 572 "parser.y"
{ setMatrixContext(mc_reward_mdp_only, yypvt[-0].i_num, 0, 0, 0); } break;
case 69:
# line 573 "parser.y"
{
                   checkMatrix();
		   YACCtrace("reward_spec_tail -> action num_matrix\n");
                } break;
case 70:
# line 579 "parser.y"
{
                   enterUniformMatrix();
                } break;
case 71:
# line 583 "parser.y"
{
                   enterIdentityMatrix();
                } break;
case 72:
# line 587 "parser.y"
{
                   checkMatrix();
                } break;
case 73:
# line 593 "parser.y"
{
                   enterUniformMatrix();
                } break;
case 74:
# line 597 "parser.y"
{
		  enterResetMatrix();
		} break;
case 75:
# line 601 "parser.y"
{
                   checkMatrix();
                } break;
case 76:
# line 606 "parser.y"
{
                   enterMatrix( yypvt[-0].f_num );
                } break;
case 77:
# line 610 "parser.y"
{
                   enterMatrix( yypvt[-0].f_num );
                } break;
case 78:
# line 615 "parser.y"
{
                   enterMatrix( yypvt[-0].f_num );
                } break;
case 79:
# line 619 "parser.y"
{
                   enterMatrix( yypvt[-0].f_num );
                } break;
case 80:
# line 624 "parser.y"
{
                   if(( yypvt[-0].constBlk->theValue.theInt < 0 ) 
                      || (yypvt[-0].constBlk->theValue.theInt >= gNumStates )) {
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_STATE_VAL, "");
                      yyval.i_num = 0;
                   }
                   else
                      yyval.i_num = yypvt[-0].constBlk->theValue.theInt;
                   free( yypvt[-0].constBlk );
                } break;
case 81:
# line 636 "parser.y"
{
                   int num;
                   num = H_lookup( yypvt[-0].constBlk->theValue.theString, nt_state );
                   if(( num < 0 ) || (num >= gNumStates )) {
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_STATE_STR, yypvt[-0].constBlk->theValue.theString );
                      yyval.i_num = 0;
                   }
                   else
                      yyval.i_num = num;
                   free( yypvt[-0].constBlk->theValue.theString );
                   free( yypvt[-0].constBlk );
                } break;
case 82:
# line 650 "parser.y"
{
                   yyval.i_num = WILDCARD_SPEC;
                } break;
case 83:
# line 655 "parser.y"
{
                   yyval.i_num = yypvt[-0].constBlk->theValue.theInt;
                   if(( yypvt[-0].constBlk->theValue.theInt < 0 ) 
                      || (yypvt[-0].constBlk->theValue.theInt >= gNumActions )) {
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_ACTION_VAL, "" );
                      yyval.i_num = 0;
                   }
                   else
                      yyval.i_num = yypvt[-0].constBlk->theValue.theInt;
                   free( yypvt[-0].constBlk );
                } break;
case 84:
# line 668 "parser.y"
{
                   int num;
                   num = H_lookup( yypvt[-0].constBlk->theValue.theString, nt_action );
                   if(( num < 0 ) || (num >= gNumActions )) {
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_ACTION_STR, yypvt[-0].constBlk->theValue.theString );
                      yyval.i_num = 0;
                   }
                   else
                      yyval.i_num = num;
                   free( yypvt[-0].constBlk->theValue.theString );
                   free( yypvt[-0].constBlk );
                } break;
case 85:
# line 682 "parser.y"
{
                   yyval.i_num = WILDCARD_SPEC;
                } break;
case 86:
# line 687 "parser.y"
{
                   if(( yypvt[-0].constBlk->theValue.theInt < 0 ) 
                      || (yypvt[-0].constBlk->theValue.theInt >= gNumObservations )) {
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_OBS_VAL, "");
                      yyval.i_num = 0;
                   }
                   else
                      yyval.i_num = yypvt[-0].constBlk->theValue.theInt;
                   free( yypvt[-0].constBlk );
                } break;
case 87:
# line 699 "parser.y"
{
                   int num;
                   num = H_lookup( yypvt[-0].constBlk->theValue.theString, nt_observation );
                   if(( num < 0 ) || (num >= gNumObservations )) { 
                      ERR_enter("Parser<ytab>:", currentLineNumber, 
                                BAD_OBS_STR, yypvt[-0].constBlk->theValue.theString);
                      yyval.i_num = 0;
                   }
                   else
                      yyval.i_num = num;
                   free( yypvt[-0].constBlk->theValue.theString );
                   free( yypvt[-0].constBlk );
               } break;
case 88:
# line 713 "parser.y"
{
                   yyval.i_num = WILDCARD_SPEC;
                } break;
case 89:
# line 718 "parser.y"
{
		   if (curMnemonic != nt_goal)
		     enterString( yypvt[-0].constBlk );
		   else
		     gGoalList[gNumGoals++] = H_lookup( yypvt[-0].constBlk->theValue.theString, nt_state );
		   
                } break;
case 90:
# line 726 "parser.y"
{
		   if (curMnemonic != nt_goal)
		     enterString( yypvt[-0].constBlk );
		   else
		     gGoalList[gNumGoals++] = H_lookup( yypvt[-0].constBlk->theValue.theString, nt_state );
                } break;
case 91:
# line 734 "parser.y"
{
		   gGoalList[gNumGoals++] = yypvt[-0].constBlk->theValue.theInt;
		} break;
case 92:
# line 738 "parser.y"
{
		   gGoalList[gNumGoals++] = yypvt[-0].constBlk->theValue.theInt;
		} break;
case 93:
# line 743 "parser.y"
{
		  yyval.f_num = yypvt[-0].constBlk->theValue.theInt;
		  if( curMatrixContext != mc_mdp_start )
		    if(( yyval.f_num < 0 ) || (yyval.f_num > 1 ))
		      ERR_enter("Parser<ytab>:", currentLineNumber, 
				BAD_PROB_VAL, "");
		  free( yypvt[-0].constBlk );
		} break;
case 94:
# line 752 "parser.y"
{
                   yyval.f_num = yypvt[-0].constBlk->theValue.theFloat;
		   if( curMatrixContext == mc_mdp_start )
		     ERR_enter("Parser<ytab>:", currentLineNumber, 
			       BAD_START_STATE_TYPE, "" );
		   else
		     if(( yyval.f_num < 0.0 ) || (yyval.f_num > 1.0 ))
		       ERR_enter("Parser<ytab>:", currentLineNumber, 
				 BAD_PROB_VAL, "" );
                   free( yypvt[-0].constBlk );
		} break;
case 95:
# line 765 "parser.y"
{
                   if( yypvt[-1].i_num )
                      yyval.f_num = yypvt[-0].constBlk->theValue.theInt * -1.0;
                   else
                      yyval.f_num = yypvt[-0].constBlk->theValue.theInt;
                   free( yypvt[-0].constBlk );
                } break;
case 96:
# line 773 "parser.y"
{
                   if( yypvt[-1].i_num )
                      yyval.f_num = yypvt[-0].constBlk->theValue.theFloat * -1.0;
                   else
                      yyval.f_num = yypvt[-0].constBlk->theValue.theFloat;
                   free( yypvt[-0].constBlk );
                } break;
case 97:
# line 782 "parser.y"
{
                   yyval.i_num = 0;
                } break;
case 98:
# line 786 "parser.y"
{
                   yyval.i_num = 1;
                } break;
case 99:
# line 790 "parser.y"
{
                   yyval.i_num = 0;
                } break;
# line	531 "/usr/ccs/bin/yaccpar"
	}
	goto yystack;		/* reset registers in driver code */
}

