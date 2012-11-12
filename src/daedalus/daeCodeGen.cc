/*
**  Daedalus
**  daeCodeGen.cc -- Code Generation
**
**  Blai Bonet, Hector Geffner
**  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002
**
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

using namespace std;

#include <daedalus/daeTypes.h>
#include <daedalus/daeTokens.h>
#include <daedalus/daeAnalysis.h>

extern "C" double pow( double, double );
extern "C" double ceil( double );

#define emit        fprintf


// global vars
static int          maxStatesAfter = 0;                  
static char         *actionName[10*1024];
static problemClass *problem = NULL;
static const char   *insideInternal = NULL;
static bool         insideQuantifier = false;
static bool         insideGenerateFields = false;

static set<const existsFormulaClass*> ePending, eGenerated;
static set<const forallFormulaClass*> fPending, fGenerated;
static set<const sumTermClass*> sPending, sGenerated;

extern map<const char*,const predicateClass*,ltstr> globalPredicateHash;
extern map<const char*,const functionClass*,ltstr> globalFunctionHash;


static void
emitArguments( FILE *out, 
	       const list<pair<const char*,const typeClass*> > &parameters,
	       const map<const char*,pair<const char*,const typeClass*>,ltstr> &instantiation )
{
  list<pair<const char*,const typeClass*> >::const_iterator it;
  map<const char*,pair<const char*,const typeClass*>,ltstr>::const_iterator it2;

  for( it = parameters.begin(); it != parameters.end(); )
    {
      it2 = instantiation.find( (*it).first );
      assert( it2 != instantiation.end() );
      emit( out, " %s", (*it2).second.first );
      ++it;
      emit( out, (it==parameters.end()?" ":",") );
    }
}


static void
emitArguments( char *buffer, 
	       const list<pair<const char*,const typeClass*> > &parameters,
	       const map<const char*,pair<const char*,const typeClass*>,ltstr> &instantiation )
{
  list<pair<const char*,const typeClass*> >::const_iterator it;
  map<const char*,pair<const char*,const typeClass*>,ltstr>::const_iterator it2;

  // this is called for generating actionNames, so no spaces between arguments
  for( it = parameters.begin(); it != parameters.end(); )
    {
      it2 = instantiation.find( (*it).first );
      assert( it2 != instantiation.end() );
      strcat( buffer, (*it2).second.first );
      ++it;
      if( it != parameters.end() )
	strcat( buffer, "," );
    }
}


void
andFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral,
			       map<const char*,const char*,ltstr> &environment ) const
{
  list<const formulaClass*>::const_iterator it;

  emit( out, "%*s( ", indent, "" );
  for( it = formulaList->begin(); it != formulaList->end(); )
    {
      (*it)->generateCode( out, 0, base, neutral, environment );
      ++it;
      if( it != formulaList->end() )
	emit( out, " && " );
    }
  emit( out, " )" );
}


void
orFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral,
			      map<const char*,const char*,ltstr> &environment ) const
{
  list<const formulaClass*>::const_iterator it;

  emit( out, "%*s( ", indent, "" );
  for( it = formulaList->begin(); it != formulaList->end(); )
    {
      (*it)->generateCode( out, 0, base, neutral, environment );
      ++it;
      if( it != formulaList->end() )
	emit( out, " || " );
    }
  emit( out, " )" );
}


void
notFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral, 
			       map<const char*,const char*,ltstr> &environment ) const
{
  emit( out, "%*s!", indent, "" );
  formula->generateCode( out, 0, base, !neutral, environment );
}


void
ifFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral, 
			      map<const char*,const char*,ltstr> &environment ) const
{
  emit( out, "%*s(!", indent, "" );
  formula1->generateCode( out, 0, base, !neutral, environment );
  emit( out, " || " );
  formula2->generateCode( out, 0, base, !neutral, environment );
  emit( out, ")" );
}


void
iffFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral, 
			      map<const char*,const char*,ltstr> &environment ) const
{
  emit( out, "%*s((!", indent, "" );
  formula1->generateCode( out, 0, base, !neutral, environment );
  emit( out, " || " );
  formula2->generateCode( out, 0, base, !neutral, environment );
  emit( out, ") && (" );
  formula1->generateCode( out, 0, base, !neutral, environment );
  emit( out, " || !" );
  formula2->generateCode( out, 0, base, !neutral, environment );
  emit( out, "))" );
}


void
atomFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral,
				map<const char*,const char*,ltstr> &environment ) const
{
  if( predicate->external || predicate->internal )
    {
      if( predicate->external )
	{
	  if( insideGenerateFields )
	    emit( out, "%*s%s( *ptr->state", indent, "", predicate->ident );
	  else
	    emit( out, "%*s%s( *this", indent, "", predicate->ident );
	}
      else
	{
	  emit( out, "%*s%sinternalPredicate%s( ", indent, "", base, predicate->ident );
	  if( insideInternal && !strcmp( insideInternal, predicate->ident ) )
	    emit( out, "1+depth, %scurrent, %s", (insideQuantifier?"":"&"), (neutral?"true":"false") );
	  else
	    emit( out, "0, NULL, false" );
	}
      emit( out, "%s", (predicate->numArguments>0?", ":"") );
      termList->generateCode( out, 0, base, true, false, environment );
      emit( out, " )" );
    }
  else
    {
      emit( out, "%*s", indent, "" );
      emit( out, "bitValue( &(%s%s%d[%s][0]), ", base, 
	    (predicate->staticPredicate(problem)?"staticPredicate":"predicate"), 
	    predicate->numArguments, predicate->ident );
      termList->generateCode( out, 0, base, false, false, environment );
      emit( out, " )" );
    }
}


void
equalFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral,
				 map<const char*,const char*,ltstr> &environment ) const
{
  emit( out, "%*s(", indent, "" );
  lterm->generateCode( out, 0, base, true /*irrelevant*/, false, environment );
  emit( out, " == " );
  rterm->generateCode( out, 0, base, true /*irrelevant*/, false, environment );
  emit( out, ")" );
}


void
inclusionFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral,
				     map<const char*,const char*,ltstr> &environment ) const
{
  list<const termClass*>::const_iterator it;

  emit( out, "%*s(", indent, "" );
  for( it = termList->begin(); it != termList->end(); )
    {
      emit( out, "(" );
      term->generateCode( out, 0, base, true /*irrelevant*/, false, environment );
      emit( out, " == " );
      (*it)->generateCode( out, 0, base, true /*irrelevant*/, false, environment );
      emit( out, ")" );
      ++it;
      if( it != termList->end() )
	emit( out, " || " );
    }
  emit( out, ")" );
}


void
existsFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral,
				  map<const char*,const char*,ltstr> &env ) const
{
  if( inlineFormula )
    {
      list<const char*> objectList;
      list<const char*>::const_iterator it;

      emit( out, "(" );
      problem->getObjectsOfType( varType, objectList );
      for( it = objectList.begin(); it != objectList.end(); )
	{
	  // extend environment with new var
	  assert( env.find( variable ) == env.end() );
	  env.insert( make_pair( variable, *it ) );

	  // generate code
	  formula->generateCode( out, 0, base, false, env );

	  // shrink environment
	  env.erase( variable );
	  free( (void*)*it );

	  // go over next value
	  if( ++it != objectList.end() )
	    emit( out, " || " );
	}
      emit( out, ")" );
    }
  else
    {
      list<map<const char*,const typeClass*,ltstr>*>::const_iterator vit;
      map<const char*,const typeClass*,ltstr>::const_iterator mit;
      map<const char*,const char*,ltstr>::const_iterator it;

      // mark for generation
      if( eGenerated.find( this ) == eGenerated.end() )
	ePending.insert( this );

      // generate call (first variable is our own var)
      if( insideInternal )
	emit( out, "%sexists%d( %s, depth, %scurrent" , 
	      base, id, (insideEffect?"ptr":"NULL"), (insideQuantifier?"":"&") );
      else
	emit( out, "%sexists%d( %s" , base, id, (insideEffect?"ptr":"NULL") );
      for( vit = environment->begin(), ++vit; vit != environment->end(); ++vit )
	{
	  for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
	    {
	      emit( out, ", " );
	      if( (it = env.find( (*mit).first )) != env.end() )
		emit( out, (*it).second );
	      else
		emit( out, (*mit).first );
	    }
	}
      emit( out, " )" );
    }
}


void
forallFormulaClass::generateCode( FILE *out, int indent, const char *base, bool neutral,
				  map<const char*,const char*,ltstr> &env ) const
{
  if( inlineFormula )
    {
      list<const char*> objectList;
      list<const char*>::const_iterator it;

      emit( out, "(" );
      problem->getObjectsOfType( varType, objectList );
      for( it = objectList.begin(); it != objectList.end(); )
	{
	  // extend environment with new var
	  assert( env.find( variable ) == env.end() );
	  env.insert( make_pair( variable, *it ) );

	  // generate code
	  formula->generateCode( out, 0, base, true, env );

	  // shrink environment
	  env.erase( variable );
	  free( (void*)*it );

	  // go over next value
	  ++it;
	  if( it != objectList.end() )
	    emit( out, " && " );
	}
      emit( out, ")" );
    }
  else
    {
      list<map<const char*,const typeClass*,ltstr>*>::const_iterator vit;
      map<const char*,const typeClass*,ltstr>::const_iterator mit;
      map<const char*,const char*,ltstr>::const_iterator it;

      // mark for generation
      if( fGenerated.find( this ) == fGenerated.end() )
	fPending.insert( this );

      // generate call (first variable is our own var)
      if( insideInternal )
	emit( out, "%sforall%d( %s, depth, %scurrent" , 
	      base, id, (insideEffect?"ptr":"NULL"), (insideQuantifier?"":"&") );
      else
	emit( out, "%sforall%d( %s" , base, id, (insideEffect?"ptr":"NULL") );
      for( vit = environment->begin(), ++vit; vit != environment->end(); ++vit )
	{
	  for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
	    {
	      emit( out, ", " );
	      if( (it = env.find( (*mit).first )) != env.end() )
		emit( out, (*it).second );
	      else
		emit( out, (*mit).first );
	    }
	}
      emit( out, " )" );
    }
}


void
certaintyGoalClass::generateCode( FILE *out, int indent, const char *base, bool neutral,
				  map<const char*,const char*,ltstr> &env ) const
{
  emit( out, "true" );
}


void
integerTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
				map<const char*,const char*,ltstr> &environment ) const
{
  assert( !lvalue );
  emit( out, "%*s%s", indent, "", integer );
}


void
sumTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
			    map<const char*,const char*,ltstr> &env ) const
{
  if( inlineTerm )
    {
      list<const char*> objectList;
      list<const char*>::const_iterator it;

      emit( out, "(" );
      problem->getObjectsOfType( varType, objectList );
      for( it = objectList.begin(); it != objectList.end(); )
	{
	  // extend environment with new var
	  assert( env.find( variable ) == env.end() );
	  env.insert( make_pair( variable, *it ) );

	  // generate code
	  term->generateCode( out, 0, base, commaSep, lvalue, env );

	  // shrink environment
	  env.erase( variable );
	  free( (void*)*it );

	  // go over next value
	  ++it;
	  if( it != objectList.end() )
	    emit( out, " + " );
	}
      emit( out, ")" );
    }
  else
    {
      list<map<const char*,const typeClass*,ltstr>*>::const_iterator vit;
      map<const char*,const typeClass*,ltstr>::const_iterator mit;
      map<const char*,const char*,ltstr>::const_iterator it;

      // mark for generation
      if( sGenerated.find( this ) == sGenerated.end() )
	sPending.insert( this );

      // generate call (first variable is our own var)
      emit( out, "%ssummation%d( %s" , base, id, (insideEffect?"ptr":"NULL") );
      for( vit = environment->begin(), ++vit; vit != environment->end(); )
	{
	  for( mit = (*vit)->begin(); mit != (*vit)->end(); )
	    {
	      if( (it = env.find( (*mit).first )) != env.end() )
		emit( out, " %s", (*it).second );
	      else
		emit( out, " %s", (*mit).first );
	      if( ++mit != (*vit)->end() ) emit( out, "," );
	    }
	  if( ++vit != environment->end() ) emit( out, "," );
	}
      emit( out, " )" );
    }
}


void
booleanTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
				map<const char*,const char*,ltstr> &environment ) const
{
  assert( !lvalue );
  emit( out, "%*s%s", indent, "", boolean );
}


void
variableTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
				 map<const char*,const char*,ltstr> &environment ) const
{
  if( environment.find( ident ) != environment.end() )
    emit( out, "%*s%s", indent, "", environment[ident] );
  else
    emit( out, "%*s%s", indent, "", ident );
}


void
objectTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
			       map<const char*,const char*,ltstr> &environment ) const
{
  map<const char*,int,ltstr>::const_iterator it;
  if( (it = problem->varDenotation.find( ident )) == problem->varDenotation.end() )
    emit( out, "%*s%s", indent, "", ident );
  else
    emit( out, "%*s%sobject[%d/*%s*/]", indent, "", base, (*it).second, ident );
}


void
termListClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const
{
  list<const termClass*>::const_iterator it;

  assert( !lvalue );
  emit( out, "%*s", indent, "" );
  if( commaSep )
    {
      for( it = termList->begin(); it != termList->end(); )
	{
	  (*it)->generateCode( out, 0, base, true /*irrelevant*/, false, environment );
	  ++it;
	  if( it != termList->end() )
	    emit( out, ", " );
	}
    }
  else
    {
      if( termList->size() == 0 )
	{
	  emit( out, "0" );
	}
      else
	{
	  for( unsigned i = 1; i < termList->size(); ++i )
	    emit( out, "(" );
	  it = termList->begin();
	  (*it)->generateCode( out, 0, base, true /*irrelevant*/, false, environment );
	  for( ++it; it != termList->end(); ++it )
	    {
	      emit( out, ")*NUMOBJECTS + " );
	      (*it)->generateCode( out, 0, base, true /*irrelevant*/, false, environment );
	    }
	}
    }
}


void
functionTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
				 map<const char*,const char*,ltstr> &environment ) const
{
  if( function->external || function->internal )
    {
      assert( !lvalue );
      if( function->external )
	{
	  if( insideGenerateFields )
	    emit( out, "%*s%s( *ptr->state", indent, "", function->ident );
	  else
	    emit( out, "%*s%s( *this", indent, "", function->ident );
	}
      else
	{
	  emit( out, "%*s%sinternalFunction%s( ", indent, "", base, function->ident );
	  if( insideInternal && !strcmp( insideInternal, function->ident ) )
	    emit( out, "&current" );
	  else
	    emit( out, "NULL" );
	}
      emit( out, "%s", (function->numArguments>0?", ":"") );
      argument->generateCode( out, 0, base, true, false, environment );

      // special functions (e.g. plus)
      if( special )
	{
	  emit( out, ", " );
	  ((specialFunctionClass*)function)->extraArgument->
	    generateCode( out, 0, base, true /*irrelevant*/, false, environment );
	}
      emit( out, " )" );
    }
  else
    {
      emit( out, "%*s%s%s%d[%s][", indent, "", base,
	    (function->staticFunction(problem)?"staticFunction":"function"), 
	    function->numArguments, function->ident );
      argument->generateCode( out, 0, base, false, false, environment );
      emit( out, "]" );
    }
}


void
predicateTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
				  map<const char*,const char*,ltstr> &environment ) const
{
  if( predicate->external || predicate->internal )
    {
      assert( !lvalue );
      if( predicate->external )
	{
	  if( insideGenerateFields )
	    emit( out, "%*s%s( *ptr->state", indent, "", predicate->ident );
	  else
	    emit( out, "%*s%s( *this", indent, "", predicate->ident );
	}
      else
	{
	  emit( out, "%*s%sinternalPredicate%s( ", indent, "", base, predicate->ident );
	  if( insideInternal && !strcmp( insideInternal, predicate->ident ) )
	    emit( out, "1+depth, %scurrent", (insideQuantifier?"":"&") );
	  else
	    emit( out, "0, NULL" );
	}
      emit( out, "%s", (predicate->numArguments>0?", ":"") );
      argument->generateCode( out, 0, base, true, false, environment );
    }
  else
    {
      if( !lvalue )
	{
	  emit( out, "%*s", indent, "" );
	  emit(out, "bitValue( &(%s%s%d[%s][0]), ", base,
	       (predicate->staticPredicate(problem)?"staticPredicate":"predicate"), 
	       predicate->numArguments, predicate->ident );
	  argument->generateCode( out, 0, base, false, false, environment );
	  emit( out, " )" );
	}
      else
	{
	  emit( out, "%*s", indent, "" );
	  emit( out, "setBit( &(%s%s%d[%s][0]), ", base,
		(predicate->staticPredicate(problem)?"staticPredicate":"predicate"), 
		predicate->numArguments, predicate->ident );
	  argument->generateCode( out, 0, base, false, false, environment );
	  emit( out, ", " );
	}
    }
}


void
arrayTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
			      map<const char*,const char*,ltstr> &environment ) const
{
  if( subscript == NULL )
    {
      //xxxx: array assignments
      cerr << "array assignments not supported. sorry." << endl;
      assert( 0 );
    }
  else
    {
      emit( out, "%*s%sarray[A%s][ ", indent, "", base, ident );
      subscript->generateCode( out, 0, base, true /*irrelevant*/, false, environment );
      emit( out, " ]" );
    }
}


void
conditionalTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
				    map<const char*,const char*,ltstr> &environment ) const
{
  emit( out, "%*s(", indent, "" );
  formula->generateCode( out, 0, base, /*irrelevant*/false, environment );
  emit( out, "?" );
  term1->generateCode( out, 0, base, commaSep, lvalue, environment );
  emit( out, ":" );
  term2->generateCode( out, 0, base, commaSep, lvalue, environment );
  emit( out, ")" );
}


void
formulaTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
				map<const char*,const char*,ltstr> &environment ) const
{
  formula->generateCode( out, 0, base, /*irrelevant*/false, environment );
}


void
starTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const
{
  char *nbase;

  if( insideEffect )
    {
      nbase = new char[strlen( problem->ident ) + 32];
      sprintf( nbase, "((stateFor%sClass*)ptr->state)->", problem->ident );
      term->generateCode( out, indent, nbase, commaSep, lvalue, environment );
      delete[] nbase;
    }
  else
    {
      term->generateCode( out, indent, base, commaSep, lvalue, environment );
    }
}


void
universalTermClass::generateCode( FILE *out, int indent, const char *base, bool commaSep, bool lvalue,
				  map<const char*,const char*,ltstr> &environment ) const
{
}


void
effectListClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
			       map<const char*,const char*,ltstr> &environment ) const
{
  list<const effectClass*>::const_iterator it;

  for( it = effectList->begin(); it != effectList->end(); ++it )
    {
      (*it)->generateCode( out, indent, base1, base2, environment );
      emit( out, "\n" );
    }
}


void
procedureEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				    map<const char*,const char*,ltstr> &environment ) const
{
  if( insideGenerateFields )
    emit( out, "%*s%s( *ptr->state );\n", indent, "", procedureIdent );
  else
    emit( out, "%*s%s( *this );\n", indent, "", procedureIdent );
}


void
whenEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
			       map<const char*,const char*,ltstr> &environment ) const
{
  emit( out, "%*sif( ", indent, "" );
  formula->generateCode( out, 0, base2, true /*irrelevant*/, environment );
  emit( out, " )\n" );
  emit( out, "%*s  {\n", indent, "" );
  effect->generateCode( out, indent + 4, base1, base2, environment );
  emit( out, "%*s  }\n", indent, "" );
}


void
forEachEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				  map<const char*,const char*,ltstr> &environment ) const
{
  list<const char*> objectList;
  list<const char*>::const_iterator it;

  problem->getObjectsOfType( varType, objectList );
  for( it = objectList.begin(); it != objectList.end(); ++it )
    {
      // extend environment with new var
      assert( environment.find( variable ) == environment.end() );
      environment.insert( make_pair( variable, *it ) );

      // generate effect list
      effectList->generateCode( out, indent, base1, base2, environment );

      // shrink environment
      environment.erase( variable );
      free( (void*)*it );
    }
}


void
termSetEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				  map<const char*,const char*,ltstr> &environment ) const
{
  lvalue->generateCode( out, indent, base1, true /*irrelevant*/, true, environment );
  emit( out, " = " );
  rvalue->generateCode( out, 0, base2, true /*irrelevant*/, false, environment );
  emit( out, ";" );
}


void
predicateSetEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				       map<const char*,const char*,ltstr> &environment ) const
{
  assert( (rvalue1 || rvalue2) && (!rvalue1 || !rvalue2) );

  lvalue->generateCode( out, indent, base1, true /*irrelevant*/, true, environment );
  if( rvalue1 != NULL )
    rvalue1->generateCode( out, 0, base2, true /*irrelevant*/, environment );
  if( rvalue2 != NULL )
    rvalue2->generateCode( out, 0, base2, true /*irrelevant*/, false, environment );
  emit( out, " );" );
}


void
printEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				map<const char*,const char*,ltstr> &environment ) const
{
  assert( (term || formula) && (!term || !formula) );
  if( term )
    {
      emit( out, "%*scout << \"term = \" << ", indent, "" );
      term->generateCode( out, 0, base2, true /*irrelevant*/, false, environment );
      emit( out, " << endl;\n" );
    }
  if( formula )
    {
      emit( out, "%*scout << \"formula = \" << ", indent, "" );
      formula->generateCode( out, 0, base2, true /*irrelevant*/, environment );
      emit( out, " << endl;\n" );
    }
}


void
printStateEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				     map<const char*,const char*,ltstr> &environment ) const
{
  emit( out, "%*s%sprint( cout, 0 );\n", indent, "", base2 );
}


void
initTermSetEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				      map<const char*,const char*,ltstr> &environment ) const
{
  list<const termClass*>::const_iterator it;

  for( it = termList->begin(); it != termList->end(); ++it )
    {
      emit( out, "%*s// new value (term)\n", indent, "" );
      emit( out, "%*s*(newElement->state) = *(ptr->state);\n", indent, "" );

      lvalue->generateCode( out, indent, base1, true /*irrelevant*/, true, environment );
      emit( out, " = " );
      (*it)->generateCode( out, 0, base2, true /*irrelevant*/, false, environment );
      emit( out, ";\n" );

      if( assertion != NULL )
	{
	  emit( out, "\n%*s// check assertion\n", indent, "" );
	  emit( out, "%*sif( ", indent, "" );
	  assertion->generateCode( out, 0, base1, true /*irrelevant*/, environment );
	  emit( out, " )\n" );
	  emit( out, "%*s  {\n", indent, "" );
	  emit( out, "%*s    concatLists( (stateListClass*&)result, newElement );\n", indent, "" );
	  emit( out, "%*s    newElement = new stateListFor%sClass;\n", indent, "", problem->ident );
	  emit( out, "%*s  }\n\n", indent, "" );
	}
      else
	{
	  emit( out, "%*sconcatLists( (stateListClass*&)result, newElement );\n", indent, "" );
	  emit( out, "%*snewElement = new stateListFor%sClass;\n\n", indent, "", problem->ident );
	}
    }
}


void
initPredicateSetEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
					   map<const char*,const char*,ltstr> &environment ) const
{
  list<const termClass*>::const_iterator it;

  for( it = termList->begin(); it != termList->end(); ++it )
    {
      emit( out, "%*s// new value (predicate)\n", indent, "" );
      emit( out, "%*s*(newElement->state) = *(ptr->state);\n", indent, "" );

      lvalue->generateCode( out, indent, base1, true /*irrelevant*/, true, environment );
      (*it)->generateCode( out, 0, base2, true /*irrelevant*/, false, environment );
      emit( out, " );\n" );

      if( assertion != NULL )
	{
	  emit( out, "\n%*s// check assertion\n", indent, "" );
	  emit( out, "%*sif( ", indent, "" );
	  assertion->generateCode( out, 0, base1, true /*irrelevant*/, environment );
	  emit( out, " )\n" );
	  emit( out, "%*s  {\n", indent, "" );
	  emit( out, "%*s    concatLists( (stateListClass*&)result, newElement );\n", indent, "" );
	  emit( out, "%*s    newElement = new stateListFor%sClass;\n", indent, "", problem->ident );
	  emit( out, "%*s  }\n\n", indent, "" );
	}
      else
	{
	  emit( out, "%*sconcatLists( (stateListClass*&)result, newElement );\n", indent, "" );
	  emit( out, "%*snewElement = new stateListFor%sClass;\n\n", indent, "", problem->ident );
	}
    }
}


void
oneOfEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				map<const char*,const char*,ltstr> &environment ) const
{
  list<const effectClass*>::const_iterator it;

  for( it = effectList->begin(); it != effectList->end(); ++it )
    {
      emit( out, "%*s// new value\n", indent, "" );
      emit( out, "%*s*(newElement->state) = *(ptr->state);\n", indent, "" );
      (*it)->generateCode( out, indent, base1, base2, environment );
      emit( out, "\n" );

      emit( out, "%*sconcatLists( (stateListClass*&)result, newElement );\n", indent, "" );
      emit( out, "%*snewElement = new stateListFor%sClass;\n\n", indent, "", problem->ident );
    }
}


void
formulaInitEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
				      map<const char*,const char*,ltstr> &environment ) const
{
  emit( out, "\n%*s// check assertion\n", indent, "" );
  emit( out, "%*sif( !", indent, "" );
  formula->generateCode( out, 0, base1, true /*irrelevant*/, environment );
  emit( out, " )\n" );
  emit( out, "%*s  toBeRemoved.push_front( ptr );\n", indent, "" );
}


void
probabilisticEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
					map<const char*,const char*,ltstr> &environment ) const
{
  float prob;
  list<pair<const char*,const effectClass*>*>::const_iterator it;

  prob = 0.0;
  for( it = effectList->begin(); it != effectList->end(); )
    {
      prob += atof( (*it)->first );
      (*it)->second->generateCode( out, indent, base1, base2, environment );
      emit( out, "%*s(ptr++)->probability = %s;\n", indent, "", (*it)->first );
      ++it;
      if( (it != effectList->end()) || (prob != 1.0) )
	emit( out, "\n" );
    }

  // remaining probability
  if( prob != 1.0 )
    emit( out, "%*s(ptr++)->probability = %f;\n", indent, "", 1.0 - prob );
}


void
nonDeterministicEffectClass::generateCode( FILE *out, int indent, const char *base1, const char *base2,
					   map<const char*,const char*,ltstr> &environment ) const
{
  list<const effectClass*>::const_iterator it;

  for( it = effectList->begin(); it != effectList->end(); )
    {
      (*it)->generateCode( out, indent, base1, base2, environment );
      emit( out, "%*s(ptr++)->probability = 0.5;   /* not used */\n", indent, "" );
      ++it;
      if( it != effectList->end() )
	emit( out, "\n" );
    }
}


void
termCostClass::generateCode( FILE *out, int indent, 
			     map<const char*,const char*,ltstr> &environment ) const
{
  cost->generateCode( out, 0, "", true /*irrelevant*/, false, environment );
}


void
observationListClass::generateCode( FILE *out, int indent, const char *prefix ) const
{
  list<const observationClass*>::const_iterator it;

  for( it = observationList->begin(); it != observationList->end(); ++it )
    {
      (*it)->generateCode( out, indent, prefix );
      emit( out, "\n" );
    }
}


void
formulaObservationClass::generateCode( FILE *out, int indent, const char *prefix ) const
{
  map<const char*,const char*,ltstr> environment;

  emit( out, "%*s%sobservation->setValue( offset++, ", indent, "", prefix );
  formula->generateCode( out, 0, "", true /*irrelevant*/, environment );
  emit( out, " );\n" );
}


void
termObservationClass::generateCode( FILE *out, int indent, const char *prefix ) const
{
  map<const char*,const char*,ltstr> environment;

  emit( out, "%*s%sobservation->setValue( offset++, ", indent, "", prefix );
  term->generateCode( out, 0, "", true /*irrelevant*/, false, environment );
  emit( out, " );\n" );
}


void
conditionalFormulaObservationClass::generateCode( FILE *out, int indent, const char *prefix ) const
{
  map<const char*,const char*,ltstr> environment;

  emit( out, "%*sif( ", indent, "" );
  formula->generateCode( out, 0, "", true /*irrelevant*/, environment );
  emit( out, " )\n" );
  observation->generateCode( out, indent + 2, prefix );
  emit( out, "%*selse\n", indent, "" );
  emit( out, "%*s%sobservation->setValue( offset++, -1 );\n", indent + 2, "", prefix );
}


void
conditionalTermObservationClass::generateCode( FILE *out, int indent, const char *prefix ) const
{
  map<const char*,const char*,ltstr> environment;

  emit( out, "%*sif( ", indent, "" );
  formula->generateCode( out, 0, "", true /*irrelevant*/, environment );
  emit( out, " )\n" );
  observation->generateCode( out, indent + 2, prefix );
  emit( out, "%*selse\n", indent, "" );
  emit( out, "%*s%sobservation->setValue( offset++, -1 );\n", indent + 2, "", prefix );
}


void
vectorObservationClass::generateCode( FILE *out, int indent, const char *prefix ) const
{
  list<const char*> objectList;
  list<const char*>::const_iterator it;
  map<const char*,const char*,ltstr> env;

  problem->getObjectsOfType( varType, objectList );
  for( it = objectList.begin(); it != objectList.end(); ++it )
    {
      // extend environment with new var
      assert( env.find( variable ) == env.end() );
      env.insert( make_pair( variable, *it ) );

      // generate code
      emit( out, "%*s%sobservation->setValue( offset++, ", indent, "", prefix );
      term->generateCode( out, 0, "", true /*irrelevant*/, false, env );
      emit( out, " );\n" );

      // shrink environment
      env.erase( variable );
      //xxxxx free( (void*)*it );
    }
}


void
probabilisticObservationClass::generateCode( FILE *out, int indent, const char *prefix ) const
{
  list<pair<const char*,const observationListClass*>*>::const_iterator it;

  for( it = observationList->begin(); it != observationList->end(); )
    {
      emit( out, "%*s// probabilistic observation for p = %s\n", indent, "", (*it)->first );
      emit( out, "%*sptr->probability = %s;\n", indent, "", (*it)->first );
      (*it)->second->generateCode( out, indent, prefix );
      ++it;
      if( it != observationList->end() )
	{
	  emit( out, "%*s++ptr;\n", indent, "" );
	  emit( out, "%*soffset = 0;\n", indent, "" );
	}
    }
}


void
nonDeterministicObservationClass::generateCode( FILE *out, int indent, const char *prefix ) const
{
  emit( out, "no yet suported\n" );
}


static void
generateParametersCode( FILE *out, 
			const list<pair<const char*,const typeClass*> > *parameters,
			... )
{
  int someParameter;
  char *parameter;
  list<pair<const char*,const typeClass*> >::const_iterator it;
  va_list ap;

  someParameter = 0;
  emit( out, "( " );

  // process variable argument list
  va_start( ap, parameters );
  parameter = va_arg( ap, char * );
  while( parameter != NULL )
    {
      someParameter = 1;
      emit( out, parameter );
      parameter = va_arg( ap, char * );
      if( (parameter != NULL) || !parameters->empty() )
	emit( out, ", " );
    }
  va_end( ap );

  // process parameters
  if( !parameters->empty() )
    {
      someParameter = 1;
      for( it = parameters->begin(); it != parameters->end(); )
	{
	  emit( out, "register int %s", (*it).first );
	  ++it;
	  if( it != parameters->end() )
	    emit( out, ", " );
	}
    }

  // closing
  if( someParameter == 0 )
    emit( out, "void" );
  emit( out, " )" );
}


static void
computeInstantiations( list<pair<const char*,const typeClass*> >::const_iterator &initialIt,
		       list<pair<const char*,const typeClass*> >::const_iterator &finalIt,
		       map<const char*,pair<const char*,const typeClass*>,ltstr> &instantiation,
		       list<map<const char*,pair<const char*,const typeClass*>,ltstr> > &instantiationList )
{
  list<const char*> objectList;
  list<const char*>::const_iterator it;

  if( initialIt == finalIt )
    {
      // this is the bottom of the recursion.
      // Insert current map into the list of instantiations and return.
      instantiationList.push_back( instantiation );
    }
  else
    {
      // get objects and insert them into map
      problem->getObjectsOfType( (*initialIt).second, objectList );
      for( it = objectList.begin(); it != objectList.end(); ++it )
	{
	  pair<map<const char*,pair<const char*, const typeClass*>,ltstr>::iterator, bool> rv;
	  rv = instantiation.insert( make_pair((*initialIt).first,make_pair(*it,(*initialIt).second)) );
	  ++initialIt;
	  computeInstantiations( initialIt, finalIt, instantiation, instantiationList );
	  --initialIt;
	  instantiation.erase( rv.first );
	}
    }
}


void
ramificationClass::generateRamificationCode( FILE *out ) const
{
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::ramificationCode%s", problem->ident, ident );
  generateParametersCode( out, &parameters, NULL );
  emit( out, "\n{\n" );
  generateCode( out );
  emit( out, "}\n\n" );
}


void
ramificationClass::generateInstantiations( FILE *out )
{
  list<map<const char*,pair<const char*,const typeClass*>,ltstr> >::const_iterator it;

  // generate parameter instantiations
  if( instantiationList.empty() )
    {
      list<pair<const char*,const typeClass*> >::const_iterator it1 = parameters.begin();
      list<pair<const char*,const typeClass*> >::const_iterator it2 = parameters.end();
      map<const char*,pair<const char*,const typeClass*>,ltstr> instantiation;
      computeInstantiations( it1, it2, instantiation, instantiationList );
    }

  // generate code
  for( it = instantiationList.begin(); it != instantiationList.end(); ++it )
    {
      // unique ramification identifier & name
      int ram = problem->numRamifications++;

      // ramification
      emit( out, "void\n" );
      emit( out, "%sRamification%d( stateFor%sClass& s )\n", problem->ident, ram, problem->ident );
      emit( out, "{\n" );
      emit( out, "  s.ramificationCode%s(", ident );
      emitArguments( out, parameters, *it );
      emit( out, ");\n" );
      emit( out, "}\n\n" );
    }
}


void
formulaRamificationClass::generateCode( FILE *out ) const
{
  map<const char*,const char*,ltstr> environment;

  emit( out, "  valid = valid && " );
  formula->generateCode( out, 0, "", true /*irrelevant*/, environment );
  emit( out, ";\n" );
}


void
effectRamificationClass::generateCode( FILE *out ) const
{
  map<const char*,const char*,ltstr> environment;

  effect->generateCode( out, 2, "", "", environment );
}


void
internalFunctionClass::generateCode( FILE *out ) const
{
  map<const char*,const char*,ltstr> environment;

  emit( out, "int\n" );
  emit( out, "stateFor%sClass::internalFunction%s", problem->ident, ident );
  generateParametersCode( out, &parameters, NULL );
  emit( out, "\n{\n" );

  emit( out, "  return( " );
  term->generateCode( out, 0, "", true /*irrelevant*/, false, environment );
  emit( out, " );\n" );
  emit( out, "}\n\n" );
}


void
internalPredicateClass::generateCode( FILE *out ) const
{
  map<const char*,const char*,ltstr> environment;
  list<pair<const char*,const typeClass*> >::const_iterator p;

  // prepare extra parameter & assert global flag
  char *extra = new char[strlen( ident ) + 64];
  sprintf( extra, "int depth, ARforPredicate%s *AR, bool defVal", ident );
  insideInternal = ident;

  emit( out, "bool\n" );
  emit( out, "stateFor%sClass::internalPredicate%s", problem->ident, ident );
  generateParametersCode( out, &parameters, extra, NULL );
  emit( out, " const\n{\n" );

  emit( out, "  bool rv;\n" );
  sprintf( extra, "ARforPredicate%s", ident );
  emit( out, "  %s current, *p;\n\n", extra );

  // standard computation
  if( parameters.size() > 0 )
    {
      // initialize current Activation Record
      emit( out, "  // initialize current Activation Record\n" );
      for( p = parameters.begin(); p != parameters.end(); ++p )
	emit( out, "  current.%s = %s;\n", (*p).first, (*p).first );
      emit( out, "  current.prev = AR;\n\n" );

      // check for branch loop
      emit( out, "  // check for branch loop\n" );
      emit( out, "  for( p = AR; p != NULL; p = p->prev )\n" );
      emit( out, "    if( " );
      for( p = parameters.begin(); p != parameters.end(); )
	{
	  emit( out, "(%s == p->%s)", (*p).first, (*p).first );
	  ++p;
	  if( p != parameters.end() )
	    emit( out, " && " );
	}
      emit( out, " )\n" );
      emit( out, "      return( defVal );\n\n" );
    }

  emit( out, "  rv = " );
  formula->generateCode( out, 0, "", true /*irrelevant*/, environment );
  emit( out, ";\n\n" );

  emit( out, "  return( rv );\n" );
  emit( out, "}\n\n" );

  // clean
  delete[] extra;
  insideInternal = NULL;
}


void
actionClass::generatePreconditionCode( FILE *out, int indent ) const
{
  map<const char*,const char*,ltstr> environment;

  if( precondition != NULL )
    {
      emit( out, "%*sprec = ", indent, "" );
      precondition->generateCode( out, 0, "", true /*irrelevant*/, environment );
      emit( out, ";\n" );
    }
  else
    {
      emit( out, "%*sprec = 1;\n", indent, "" );
    }
}


void
actionClass::generateEffectCode( FILE *out, int indent ) const
{
  static char base[1024];
  map<const char*,const char*,ltstr> environment;

  if ( effect != NULL )
    {
      sprintf( base, "((stateFor%sClass*)ptr->state)->", problem->ident );
      effect->generateCode( out, indent, base, "", environment );
    }
}


void
actionClass::generateActionCode( FILE *out ) const
{
  int i;

  // heading
  emit( out, "stateListFor%sClass *\n", problem->ident );
  emit( out, "stateFor%sClass::statesAfter%s", problem->ident, ident );
  generateParametersCode( out, &parameters, "register int opId", NULL );
  emit( out, " const \n{\n" );

  // local variables
  emit( out, "  register int prec;\n" );
  emit( out, "  register stateListFor%sClass *ptr;\n", problem->ident );
  emit( out, "  static stateListFor%sClass result[%d];\n\n", problem->ident, numResultingStates + 1 );
  //emit( out, "  stateListFor%sClass *result;\n\n", problem->ident );
  //emit( out, "  result = new stateListFor%sClass[%d];\n\n", problem->ident, numResultingStates + 1 );

  // setup resulting states
  emit( out, "  // setup resulting states\n" );
  for( i = 0; i < numResultingStates; ++i )
    {
      emit( out, "  *result[%d].state = *this;\n", i );
      emit( out, "  result[%d].action = opId;\n", i );
      emit( out, "  result[%d].probability = 1.0;\n", i );
      emit( out, "  result[%d].state->valid = 1;\n", i );
    }
  emit( out, "  result[%d].probability = -1.0;\n", i );
  emit( out, "  ptr = result;\n\n" );

  // precondition
  generatePreconditionCode( out, 2 );

  // effect
  emit( out, "  if( prec == 1 )\n" );
  emit( out, "    {\n" );
  generateEffectCode( out, 6 );
  emit( out, "    }\n" );

  // closing: if the precondition is not satisfied, return an empty list
  emit( out, "  else\n" );
  emit( out, "    result[0].probability = -1.0;\n" );
  emit( out, "  return( result );\n" );
  emit( out, "}\n\n" );
}


void
actionClass::generateCostCode( FILE *out ) const
{
  map<const char*,const char*,ltstr> environment;

  // heading
  emit( out, "float\n" );
  emit( out, "stateFor%sClass::costAfter%s", problem->ident, ident );
  generateParametersCode( out, &parameters, NULL );
  emit( out, " const\n{\n" );

  // code
  if( cost != NULL )
    {
      emit( out, "  return( " );
      cost->generateCode( out, 0, environment );
      emit( out, " );\n" );
    }
  else
    emit( out, "  return( 1.0 );\n" );

  // closing
  emit( out, "}\n\n" );
}


void
actionClass::generateObservationCode( FILE *out ) const
{
  // header
  emit( out, "observationListFor%sClass *\n", problem->ident );
  emit( out, "stateFor%sClass::observationsAfter%s", problem->ident, ident );
  generateParametersCode( out, &parameters, NULL );
  emit( out, " const\n{\n" );
  emit( out, "  register observationListFor%sClass *result, *ptr;\n", problem->ident );
  emit( out, "  register int offset;\n\n" );

  // setup resulting observations: remember that just one (yet?)
  int number = (observation ? observation->number( problem ) : 0);
  emit( out, "  // setup resulting observations\n" );
  emit( out, "  result = new observationListFor%sClass[%d+1];\n", problem->ident, number );
  if( number == 1 )
    emit( out, "  result[0].probability = 1.0;\n" );
  emit( out, "  result[%d].probability = -1.0;\n", number );
  emit( out, "  ptr = &result[0];\n" );
  emit( out, "  offset = 0;\n\n" );

  // observation code
  if( observation != NULL )
    {
      emit( out, "  // observation code\n" );
      observation->generateCode( out, 2, "ptr->" );
    }

  // closing
  emit( out, "  return( result );\n" );
  emit( out, "}\n\n" );
}


void
actionClass::generateInstantiations( FILE *out )
{
  int                action;
  static char        buffer[4096];
  list<map<const char*,pair<const char*,const typeClass*>,ltstr> >::const_iterator it;

  // generate parameter instantiations
  if( instantiationList.empty() )
    {
      list<pair<const char*,const typeClass*> >::const_iterator it1 = parameters.begin();
      list<pair<const char*,const typeClass*> >::const_iterator it2 = parameters.end();
      map<const char*,pair<const char*,const typeClass*>,ltstr> instantiation;
      computeInstantiations( it1, it2, instantiation, instantiationList );
    }

  for( it = instantiationList.begin(); it != instantiationList.end(); ++it )
    {
      // unique action identifier & name
      maxStatesAfter += numResultingStates;
      action = problem->numActions++;
      strcpy( buffer, &(ident)[1] );

#if 0
      for( char *p = buffer; *p; ++p ) if( *p == '_' ) *p = '-';
#endif
      strcat( buffer, "(" );
      emitArguments( buffer, parameters, *it );
      strcat( buffer, ")" );

#if 0
      // remove '_' from buffer
      for( char *p = buffer; *p; ++p )
	if( *p == '_' )
	  for( char *s = p; *s; ++s )
	    *s = *(s+1);
#endif
	  
      actionName[action] = strdup( buffer );

      // heading
      emit( out, "stateListClass *\n" );
      emit( out, "%sAction%d( const stateFor%sClass& s )\n", problem->ident, action, problem->ident );
      emit( out, "{\n" );
      emit( out, "  register float cost;\n" );
      emit( out, "  register stateListFor%sClass *result, *p;\n\n", problem->ident );

      // cost
      if( problem->domain->model == PLANNING )
	{
	  emit( out, "  cost = 1;\n" );
	}
      else
	{
	  emit( out, "  cost = s.costAfter%s(", ident );
	  emitArguments( out, parameters, *it );
	  emit( out, ");\n" );
	}

      // effect
      emit( out, "  result = s.statesAfter%s( %d%s", ident, action, (parameters.empty()?" ":",") );
      emitArguments( out, parameters, *it );
      emit( out, ");\n" );

      emit( out, "  for( p = result; p->probability != -1; ++p )\n" );
      emit( out, "    {\n" );
      emit( out, "      p->cost = cost;\n" );

      // ramifications
      emit( out, "      p->state->applyRamifications();\n" );

      // observations
      if( (problem->domain->model != ND_POMDP1) &&
	  (problem->domain->model != ND_POMDP2) &&
	  (problem->domain->model != POMDP1) &&
	  (problem->domain->model != POMDP2) )
	{
	  emit( out, "      p->observations = NULL;\n" );
	}
      else
	{
	  emit( out, "      p->observations = ((stateFor%sClass*)p->state)->observationsAfter%s(", 
		problem->ident, ident );
	  emitArguments( out, parameters, *it );
	  emit( out, ");\n" );
	}

      // closing
      emit( out, "    }\n" );
      emit( out, "  return( result );\n" );
      emit( out, "}\n\n" );
    }
}


void
problemClass::generateClasses( FILE *out )
{
  // forward references
  emit( out, "// forward references\n" );
  emit( out, "class stateFor%sClass;\n", ident );
  emit( out, "class stateListFor%sClass;\n", ident );
  emit( out, "class observationFor%sClass;\n", ident );
  emit( out, "class observationListFor%sClass;\n\n\n", ident );

  //
  // definition for class stateClass
  //
  emit( out, "class stateFor%sClass : public stateClass\n", ident );
  emit( out, "{\n" );
  emit( out, "public:\n" );

  // objects
  if( !varDenotation.empty() )
    {
      emit( out, "  // objects\n" );
      emit( out, "  %-25sobject[%d];\n\n", "int", varDenotation.size() );
    }

  // functions
  if( numObjects > 0 )
    {
      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	{
	  if( numFunctions[arity] > 0 )
	    {
	      emit( out, "  // functions %d argument(s)\n", arity );
	      emit( out, "  %-25sfunction%d[%d][%d];\n\n", "int", 
		    arity, numFunctions[arity], (int)pow((double)numObjects,(double)arity) );
	    }

	  if( numStaticFunctions[arity] > 0 )
	    {
	      emit( out, "  // static functions %d argument(s)\n", arity );
	      emit( out, "  %-25sstaticFunction%d[%d][%d];\n\n", "static int", 
		    arity, numStaticFunctions[arity], (int)pow((double)numObjects,(double)arity) );
	    }
	}
    }

  // predicates
  if( numObjects > 0 )
    {
      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	{
	  if( numPredicates[arity] > 0 )
	    {
	      emit( out, "  // predicates %d argument(s)\n", arity );
	      emit( out, "  %-25spredicate%d[%d][%d];\n\n", "unsigned", 
		    arity, numPredicates[arity], 
		    (int)ceil(pow((double)numObjects,(double)arity)/(double)(8*sizeof(unsigned))) );
	    }

	  if( numStaticPredicates[arity] > 0 )
	    {
	      emit( out, "  // static predicates %d argument(s)\n", arity );
	      emit( out, "  %-25sstaticPredicate%d[%d][%d];\n\n", "static unsigned", 
		    arity, numStaticPredicates[arity], 
		    (int)ceil(pow((double)numObjects,(double)arity)/(double)(8*sizeof(unsigned))) );
	    }
	}
    }

  // arrays
  if( numArrays > 0 )
    {
      emit( out, "  // arrays\n" );
      emit( out, "  %-25sarray[%d][%d];\n\n", "int", numArrays, maxArraySize  );
    }

  // basic methods
  emit( out, "  // basics methods\n" );
  emit( out, "  %-25sstateFor%sClass();\n", " ", ident );
  emit( out, "  %-25s~stateFor%sClass();\n", "virtual", ident );
  emit( out, "  %-25soperator=( register const stateClass& s );\n", "virtual stateClass&" );
  emit( out, "  %-25soperator=( register const stateFor%sClass& s );\n", "virtual stateClass&", ident );
  emit( out, "  %-25soperator==( register const stateClass& s ) const;\n", "virtual bool" );
  emit( out, "  %-25soperator==( register const stateFor%sClass& s ) const;\n", "virtual bool", ident );
  emit( out, "  %-25sclone( void ) const;\n\n", "virtual stateClass*" );

  // static members and methods
  emit( out, "  // static members and methods\n" );
  emit( out, "  %-25snumActions;\n", "static int" );
  emit( out, "  %-25sactionName;\n", "static const char**" );
  emit( out, "  %-25s(**ramificationTable)( stateFor%sClass& );\n", "static void", ident );
  emit( out, "  %-25s(**actionTable)( const stateFor%sClass& );\n", "static stateListClass*", ident );
  emit( out, "  %-25sfillTables( void );\n", "static void" );
  emit( out, "  %-25scleanUp( void );\n", "static void" );
  emit( out, "  %-25sbootstrap( void );\n", "static stateListClass*" );
  emit( out, "  %-25sprintAction( ostream& os, int action );\n", "static void" );
  emit( out, "  %-25sgenerateField( int idx, stateListFor%sClass *&list );\n", "static void", ident );
  emit( out, "  %-25sgetInitialSituations( void );\n", "static stateListClass*" );
  emit( out, "  %-25salloc( register int number, register stateClass **pool );\n", "static void" );
  emit( out, "  %-25sdealloc( register stateClass *pool );\n", "static void" );
  emit( out, "  %-25sinitialize( void );\n\n", "static void" );

  // virtual methods
  emit( out, "  // virtual methods\n" );
  emit( out, "  %-25sapplyRamifications( void );\n", "virtual void" );
  emit( out, "  %-25sgoal( void ) const;\n", "virtual bool" );
  emit( out, "  %-25sapplyAllActions( void ) const;\n", "virtual stateListClass*" );
  emit( out, "  %-25sprint( ostream& os, int indent ) const;\n", "virtual void" );
  emit( out, "  %-25slookup( const node_t *node ) const;\n", "virtual const stateClass::node_t*" );
  emit( out, "  %-25sinsert( node_t *node, int index ) const;\n", "virtual const stateClass::node_t*" );
  emit( out, "  %-25srecover( const node_t *node );\n\n", "virtual void" );

  // methods
  generateMethodList( out );
  emit( out, "};\n\n" );



  //
  // definition for class stateListClass
  //
  emit( out, "class stateListFor%sClass : public stateListClass\n", ident );
  emit( out, "{\n" );
  emit( out, "public:\n" );
  emit( out, "  stateListFor%sClass();\n", ident );
  emit( out, "  virtual stateListClass* alloc( register int size );\n" );
  emit( out, "};\n\n" );



  //
  // definition for class observationClass
  //
  emit( out, "class observationFor%sClass : public observationClass\n", ident );
  emit( out, "{\n" );
  emit( out, "public:\n" );
  emit( out, "  int obs[%d+1];\n", observationSize() );
  emit( out, "  static int observationSize;\n\n" );
  emit( out, "  observationFor%sClass();\n", ident );
  emit( out, "  %-30soperator=( register const observationClass& o );\n", "virtual observationClass&" );
  emit( out, "  %-30soperator=( register const observationFor%sClass& o );\n", "virtual observationClass&", ident );
  emit( out, "  %-30soperator==( register const observationClass& o ) const;\n", "virtual bool" );
  emit( out, "  %-30soperator==( register const observationFor%sClass& o ) const;\n\n", "virtual bool", ident );
  emit( out, "  %-30ssetValue( register int offset, register int value );\n", "virtual void"  );
  emit( out, "  %-30sclone( void ) const;\n", "virtual observationClass*" );
  emit( out, "  %-30sprint( ostream& os, int indent ) const;\n", "virtual void" );
  emit( out, "};\n\n" );



  //
  // definition for class observationListClass
  //
  emit( out, "class observationListFor%sClass : public observationListClass\n", ident );
  emit( out, "{\n" );
  emit( out, "public:\n" );
  emit( out, "  observationListFor%sClass();\n", ident );
  emit( out, "  ~observationListFor%sClass();\n", ident );
  emit( out, "  virtual void print( ostream& os, int indent ) const;\n" );
  emit( out, "};\n\n" );
}


void
problemClass::generateMethodList( FILE *out )
{
  list<const internalFunctionClass*>::const_iterator it1;
  list<const internalPredicateClass*>::const_iterator it4;
  list<const ramificationClass*>::const_iterator it2;
  list<const actionClass*>::const_iterator it3;
  list<pair<const char*,const typeClass*> >::const_iterator p;

  emit( out, "  // Activation Records\n" );
  for( it1 = domain->internalFunctionList.begin(); it1 != domain->internalFunctionList.end(); ++it1 )
    {
      emit( out, "  struct ARforFunction%s {\n", (*it1)->ident );
      for( p = (*it1)->parameters.begin(); p != (*it1)->parameters.end(); ++p )
	emit( out, "    int %s;\n", (*p).first );
      emit( out, "    struct ARforFunction%s *prev;\n", (*it1)->ident );
      emit( out, "  };\n" );
    }
  for( it4 = domain->internalPredicateList.begin(); it4 != domain->internalPredicateList.end(); ++it4 )
    {
      emit( out, "  struct ARforPredicate%s {\n", (*it4)->ident );
      for( p = (*it4)->parameters.begin(); p != (*it4)->parameters.end(); ++p )
	emit( out, "    int %s;\n", (*p).first );
      emit( out, "    struct ARforPredicate%s *prev;\n", (*it4)->ident );
      emit( out, "  };\n" );
    }
  if( (domain->internalFunctionList.size() > 0) || (domain->internalPredicateList.size() > 0) )
    emit( out, "\n" );

  emit( out, "  // methods\n" );
  for( it2 = domain->ramificationList.begin(); it2 != domain->ramificationList.end(); ++it2 )
    {
      emit( out, "  void ramificationCode%s", (*it2)->ident );
      generateParametersCode( out, &(*it2)->parameters, NULL );
      emit( out, ";\n" );
    }

  for( it1 = domain->internalFunctionList.begin(); it1 != domain->internalFunctionList.end(); ++it1 )
    {
      emit( out, "  int internalFunction%s", (*it1)->ident );
      generateParametersCode( out, &(*it1)->parameters, NULL );
      emit( out, ";\n" );
    }

  for( it4 = domain->internalPredicateList.begin(); it4 != domain->internalPredicateList.end(); ++it4 )
    {
      // prepare extra parameter
      char *extra = new char[strlen( ident ) + 64];
      sprintf( extra, "int depth, ARforPredicate%s *AR, bool defVal", (*it4)->ident );

      emit( out, "  bool internalPredicate%s", (*it4)->ident );
      generateParametersCode( out, &(*it4)->parameters, extra, NULL );
      emit( out, " const;\n" );

      // clean
      delete[] extra;
    }

  for( it3 = domain->actionList.begin(); it3 != domain->actionList.end(); ++it3 )
    {
      emit( out, "  float costAfter%s", (*it3)->ident );
      generateParametersCode( out, &(*it3)->parameters, NULL );
      emit( out, " const;\n" );

      emit( out, "  stateListFor%sClass* statesAfter%s", ident, (*it3)->ident );
      generateParametersCode( out, &(*it3)->parameters, "register int opId", NULL );
      emit( out, " const;\n" );

      emit( out, "  observationListFor%sClass* observationsAfter%s", ident, (*it3)->ident );
      generateParametersCode( out, &(*it3)->parameters, NULL );
      emit( out, " const;\n" );
    }
  emit( out, "\n" );

  // method list for summations, exists and foralls
  existsFormulaClass *eform;
  forallFormulaClass *fform;
  sumTermClass *sterm;
  list<map<const char*,const typeClass*,ltstr>*>::const_iterator vit;
  map<const char*,const typeClass*,ltstr>::const_iterator mit;

  // exists
  for( eform = existsChain; eform != NULL; eform = eform->nextChain )
    if( !eform->inlineFormula )
      {
	emit( out, "  bool  exists%d( stateListFor%sClass* ptr", eform->id, problem->ident );
	if( eform->internal )
	  emit( out, ", int depth, ARforPredicate%s *AR", eform->internal );
	for( vit = eform->environment->begin(), ++vit; vit != eform->environment->end(); ++vit )
	  {
	    for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
	      emit( out, ", int %s", (*mit).first );
	  }
	emit( out, " ) const;\n" );
      }

  // forall
  for( fform = forallChain; fform != NULL; fform = fform->nextChain )
    if( !fform->inlineFormula )
      {
	emit( out, "  bool  forall%d( stateListFor%sClass *ptr", fform->id, problem->ident );
	if( fform->internal )
	  emit( out, ", int depth, ARforPredicate%s *AR", fform->internal );
	for( vit = fform->environment->begin(), ++vit; vit != fform->environment->end(); ++vit )
	  {
	    for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
	      emit( out, ", int %s", (*mit).first );
	  }
	emit( out, " ) const;\n" );
      }

  // summations
  for( sterm = sumChain; sterm != NULL; sterm = sterm->nextChain )
    if( !sterm->inlineTerm )
      {
	emit( out, "  int  summation%d( stateListFor%sClass *ptr", sterm->id, problem->ident );
	for( vit = sterm->environment->begin(), ++vit; vit != sterm->environment->end(); ++vit )
	  {
	    for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
	    emit( out, ", int %s", (*mit).first );
	  }
	emit( out, " ) const;\n" );
      }
}


void
problemClass::generateBasicMethods( FILE *out )
{
  int osize = observationSize();
  map<const char*,int,ltstr>::const_iterator it;
  
  // constructors for stateClass
  emit( out, "stateFor%sClass::stateFor%sClass()\n", ident, ident );
  emit( out, "{\n" );
  emit( out, "  valid = 1;\n" );

  // objects
  for( it = varDenotation.begin(); it != varDenotation.end(); ++it )
    emit( out, "  object[%d/*%s*/] = 0;\n", (*it).second, (*it).first );

  // functions
  if( numObjects > 0 )
    {
      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	{
	  if( numFunctions[arity] > 0 )
	    {
	      emit( out, "  memset( function%d, 0, %d * %d * sizeof(int) );\n", 
		    arity, numFunctions[arity], (int)pow((double)numObjects,(double)arity) );
	    }
	  if( numPredicates[arity] > 0 )
	    {
	      emit( out, "  memset( predicate%d, 0, %d * %d * sizeof(unsigned) );\n", 
		    arity, numPredicates[arity],
		    (int)ceil(pow((double)numObjects,(double)arity)/(double)(8*sizeof(unsigned))) );
	    }
	}
    }

  // arrays
  if( numArrays > 0 )
    {
      emit( out, "  memset( array, 0, %d * %d * sizeof( int ) );\n", numArrays, maxArraySize );
    }

  emit( out, "}\n\n" );


  emit( out, "stateFor%sClass::~stateFor%sClass()\n", ident, ident );
  emit( out, "{\n" );
  emit( out, "}\n\n" );


  // operator= for stateClass
  emit( out, "stateClass&\n" );
  emit( out, "stateFor%sClass::operator=( register const stateClass& s )\n", ident );
  emit( out, "{\n" );
  emit( out, "  return( this->operator=( (const stateFor%sClass&)s ) );\n", ident );
  emit( out, "}\n\n" );

  emit( out, "stateClass&\n" );
  emit( out, "stateFor%sClass::operator=( register const stateFor%sClass& s )\n", ident, ident );
  emit( out, "{\n" );
  emit( out, "  valid = s.valid;\n" );

  if( numObjects > 0 )
    {
      if( varDenotation.size() > 0 )
	emit( out, "  memcpy( object, s.object, %d * sizeof( int ) );\n", varDenotation.size() );

      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	{
	  if( numFunctions[arity] > 0 )
	    {
	      emit( out, "  memcpy( &function%d[0][0], &s.function%d[0][0], %d * %d * sizeof(int) );\n", 
		    arity, arity, numFunctions[arity], (int)pow((double)numObjects,(double)arity) );
	    }
	  if( numPredicates[arity] > 0 )
	    {
	      emit( out, "  memcpy( &predicate%d[0][0], &s.predicate%d[0][0], %d * %d * sizeof(unsigned) );\n", 
		    arity, arity, numPredicates[arity],
		    (int)ceil(pow((double)numObjects,(double)arity)/(double)(8*sizeof(unsigned))) );
	    }
	}
    }

  if( numArrays > 0 )
    {
      emit( out, "  memcpy( array, s.array, %d * %d * sizeof( int ) );\n", numArrays, maxArraySize );
    }

  emit( out, "  return( *this );\n" );
  emit( out, "}\n\n" );


  // operator== for stateClass
  emit( out, "bool\n" );
  emit( out, "stateFor%sClass::operator==( register const stateClass& s ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  return( this->operator==( (const stateFor%sClass&)s ) );\n", ident );
  emit( out, "}\n\n" );

  emit( out, "bool\n" );
  emit( out, "stateFor%sClass::operator==( register const stateFor%sClass& s ) const\n", ident, ident );
  emit( out, "{\n" );
  emit( out, "  return( (valid == s.valid) " );

  if( numObjects > 0 )
    {
      if( varDenotation.size() > 0 )
	{
	  emit( out, "&&\n%10s", "" );
	  emit( out, "!memcmp( object, s.object, %d * sizeof( int ) ) ", varDenotation.size() );
	}

      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	{
	  if( numFunctions[arity] > 0 )
	    {
	      emit( out, "&&\n%10s", "" );
	      emit( out, "!memcmp( &function%d[0][0], &s.function%d[0][0], %d * %d * sizeof(int) ) ", 
		    arity, arity, numFunctions[arity], (int)pow((double)numObjects,(double)arity) );
	    }
	  if( numPredicates[arity] > 0 )
	    {
	      emit( out, "&&\n%10s", "" );
	      emit( out, "!memcmp( &predicate%d[0][0], &s.predicate%d[0][0], %d * %d * sizeof(unsigned) ) ", 
		    arity, arity, numPredicates[arity],
		    (int)ceil(pow((double)numObjects,(double)arity)/(double)(8*sizeof(unsigned))) );
	    }
	}
    }

  if( numArrays > 0 )
    {
      emit( out, "&&\n%10s", "" );
      emit( out, "!memcmp( array, s.array, %d * %d * sizeof( int ) ) ", numArrays, maxArraySize );
    }

  emit( out, ");\n" );
  emit( out, "}\n\n" );


  // clone method
  emit( out, "stateClass *\n" );
  emit( out, "stateFor%sClass::clone( void ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  register stateFor%sClass *state;\n\n", ident );
  emit( out, "  state = new stateFor%sClass;\n", ident );
  emit( out, "  return( &(*state = *this) );\n" );
  emit( out, "}\n\n" );


  // constructors for stateListClass
  emit( out, "stateListFor%sClass::stateListFor%sClass()\n", ident, ident );
  emit( out, "{\n" );
  emit( out, "  state = new stateFor%sClass;\n", ident );
  emit( out, "}\n\n" );


  // alloc method
  emit( out, "stateListClass *\n" );
  emit( out, "stateListFor%sClass::alloc( register int size )\n", ident );
  emit( out, "{\n" );
  emit( out, "  return( new stateListFor%sClass[size] );\n", ident );
  emit( out, "}\n\n" );


  // static members for observationClass
  emit( out, "int observationFor%sClass::observationSize = %d+1;\n\n", ident, osize );


  // constructor for observationClass
  emit( out, "observationFor%sClass::observationFor%sClass()\n", ident, ident );
  emit( out, "{\n" );
  emit( out, "  memset( obs, -1, observationSize * sizeof( int ) );\n" );
  emit( out, "}\n\n" );


  // operator= for observationClass
  emit( out, "observationClass&\n" );
  emit( out, "observationFor%sClass::operator=( register const observationClass& o )\n", ident );
  emit( out, "{\n" );
  emit( out, "  return( this->operator=( (const observationFor%sClass&)o ) );\n", ident );
  emit( out, "}\n\n" );

  emit( out, "observationClass&\n" );
  emit( out, "observationFor%sClass::operator=( register const observationFor%sClass& o )\n", 
	ident, ident );
  emit( out, "{\n" );
  emit( out, "  memcpy( obs, o.obs, observationSize * sizeof( int ) );\n" );
  emit( out, "  return( *this );\n" );
  emit( out, "}\n\n" );


  // operator== for observationClass
  emit( out, "bool\n" );
  emit( out, "observationFor%sClass::operator==( register const observationClass& o ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  return( this->operator==( (const observationFor%sClass&)o ) );\n", ident );
  emit( out, "}\n\n" );

  emit( out, "bool\n" );
  emit( out, "observationFor%sClass::operator==( register const observationFor%sClass& o ) const\n", 
	ident, ident );
  emit( out, "{\n" );
  emit( out, "  return( !memcmp( obs, o.obs, observationSize * sizeof( int ) ) );\n" );
  emit( out, "}\n\n" );


  // clone method
  emit( out, "observationClass *\n" );
  emit( out, "observationFor%sClass::clone( void ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  register observationFor%sClass *newObs;\n\n", ident );
  emit( out, "  newObs = new observationFor%sClass;\n", ident );
  emit( out, "  return( &(*newObs = *this) );\n" );
  emit( out, "}\n\n" );


  // setValue method
  emit( out, "void\n" );
  emit( out, "observationFor%sClass::setValue( register int offset, register int value )\n", ident );
  emit( out, "{\n" );
  emit( out, "  obs[offset] = value;\n" );
  emit( out, "}\n\n" );


  // print method
  emit( out, "void\n" );
  emit( out, "observationFor%sClass::print( ostream& os, int indent ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  os << \"[\";\n" );

  for( int i = 0; i < osize; ++i )
    if( i < osize-1 )
      emit( out, "  os << obs[%d] << \",\";\n", i );
    else
      emit( out, "  os << obs[%d] << \"]\" << endl;\n", i );

  emit( out, "}\n\n" );


  // constructors/destructor for observationListClass
  emit( out, "observationListFor%sClass::observationListFor%sClass()\n", ident, ident );
  emit( out, "{\n" );
  emit( out, "  observation = new observationFor%sClass;\n", ident );
  emit( out, "}\n\n" );

  emit( out, "observationListFor%sClass::~observationListFor%sClass()\n", ident, ident );
  emit( out, "{\n" );
  emit( out, "  delete observation;\n" );
  emit( out, "}\n\n" );


  // print method
  emit( out, "void\n" );
  emit( out, "observationListFor%sClass::print( ostream& os, int indent ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  os << \"hello world!\" << endl;\n" );
  emit( out, "}\n\n" );
}

static void
generateFields( FILE *out,
 		int& index,
	        list<const effectClass*>::const_iterator it, 
		list<const effectClass*>::const_iterator endit, 
		map<const char*,const char*,ltstr> &environment )
{
  char base[1024];
  const forEachEffectClass* iter;

  // end of recursion case
  if( it == endit )
    return;

  // if this is a forEachEffectClass, make a recursive call
  if( (iter = dynamic_cast<const forEachEffectClass*>(*it)) != NULL )
    {
      list<const char*> objectList;
      list<const char*>::const_iterator obj;

      problem->getObjectsOfType( iter->varType, objectList );
      for( obj = objectList.begin(); obj != objectList.end(); ++obj )
	{
	  // extend environment with new var
	  assert( environment.find( iter->variable ) == environment.end() );
	  environment.insert( make_pair( iter->variable, *obj ) );

	  // generate fields
	  generateFields( out, index, 
			  iter->effectList->effectList->begin(), 
			  iter->effectList->effectList->end(), 
			  environment );

	  // shrink environment
	  environment.erase( iter->variable );
	  free( (void*)*obj );
	}
    }
  else
    {
      if( (*it)->dimension > 1 )
	{
	  emit( out, "    case %d:\n", index );
	  emit( out, "      for( ptr = actualList; ptr != NULL; ptr = tmp )\n" );
	  emit( out, "        {\n" );

	  sprintf( base, "((stateFor%sClass*)newElement->state)->", problem->ident );
	  (*it)->generateCode( out, 10, base, base, environment );

	  emit( out, "          // next state\n" );
	  emit( out, "          tmp = (stateListFor%sClass*)ptr->next;\n", problem->ident );
	  emit( out, "          delete ptr;\n" );
	  emit( out, "        }\n" );
	  emit( out, "      actualList = result;\n" );
	  emit( out, "      break;\n" );
	}	  
      else
	{
	  emit( out, "    case %d:\n", index );
	  emit( out, "      for( ptr = actualList; ptr != NULL; ptr = tmp )\n" );
	  emit( out, "        {\n" );

	  sprintf( base, "((stateFor%sClass*)ptr->state)->", problem->ident );
	  (*it)->generateCode( out, 10, base, base, environment );

	  emit( out, "          // next state\n" );
	  emit( out, "          tmp = (stateListFor%sClass*)ptr->next;\n", problem->ident );
	  emit( out, "        }\n" );
	  emit( out, "      break;\n" );
	}
    }

  // generate next field
  generateFields( out, ++index, ++it, endit, environment );
}

void
problemClass::generateStaticMethods( FILE *out )
{
  int i, index;
  map<const char*,const char*,ltstr> environment;

  // static data
  emit( out, "int stateFor%sClass::numActions;\n", ident );
  emit( out, "const char **stateFor%sClass::actionName;\n", ident );
  emit( out, "void (**stateFor%sClass::ramificationTable)( stateFor%sClass& );\n\n", ident, ident );
  emit( out, "stateListClass *(**stateFor%sClass::actionTable)( const stateFor%sClass& );\n\n", ident, ident );

  // fillTables
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::fillTables( void )\n", ident );
  emit( out, "{\n" );

  // actionTable
  emit( out, "  // actionTable\n" );
  emit( out, "  actionTable = (stateListClass*(**)( const stateFor%sClass& ))new void*[%d];\n",
	ident, numActions + 1 );

  for( i = 0; i < numActions; ++i )
    emit( out, "  actionTable[%d] = &%sAction%d;\n", i, ident, i );

  emit( out, "  actionTable[%d] = NULL;\n", numActions );
  emit( out, "  numActions = %d;\n", numActions );
  emit( out, "  observationFor%sClass::observationSize = %d+1;\n\n", ident, observationSize() );
  emit( out, "  actionName = new const char*[%d];\n", numActions );

  if( (domain->model == MDP) || (domain->model == ND_MDP) )
    {
      emit( out, "  actionName[0] = \"INIT\";\n" );
      i = 1;
    }
  else
    {
      i = 0;
    }
  for( ; i < numActions; ++i )
    emit( out, "  actionName[%d] = \"%s\";\n", i, actionName[i] );
  emit( out, "\n" );

  // ramificationTable
  emit( out, "  // ramificationTable\n" );
  emit( out, "  ramificationTable = (void(**)( stateFor%sClass& ))new void*[%d];\n", 
	ident, numRamifications + 1 );

  for( i = 0; i < numRamifications; ++i )
    emit( out, "  ramificationTable[%d] = &%sRamification%d;\n", i, ident, i );

  emit( out, "  ramificationTable[%d] = NULL;\n", i );
  emit( out, "}\n\n" );


  // cleanUp
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::cleanUp( void )\n", ident );
  emit( out, "{\n" );
  emit( out, "  delete[] actionTable;\n" );
  emit( out, "  delete[] actionName;\n" );
  emit( out, "  delete[] ramificationTable;\n" );
  emit( out, "}\n\n" );


  // bootstrap
  emit( out, "stateListClass *\n" );
  emit( out, "stateFor%sClass::bootstrap( void )\n", ident );
  emit( out, "{\n" );
  emit( out, "  return( stateFor%sClass::getInitialSituations() );\n", problem->ident );
  emit( out, "}\n\n" );


  // printAction
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::printAction( ostream& os, int action )\n", ident );
  emit( out, "{\n" );
  emit( out, "  os << actionName[action];\n" );
  emit( out, "}\n\n" );


  // generateField
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::generateField( int index, stateListFor%sClass*& actualList )\n",
	ident, ident );
  emit( out, "{\n" );
  emit( out, "  register stateListFor%sClass *result, *ptr, *tmp;\n", ident );
  emit( out, "  stateListFor%sClass *newElement = NULL;\n", ident );
  emit( out, "  list<stateListFor%sClass*> toBeRemoved;\n", ident );
  emit( out, "  list<stateListFor%sClass*>::iterator it;\n\n", ident );

  emit( out, "  // allocate space\n" );
  emit( out, "  newElement = new stateListFor%sClass;\n\n", problem->ident );

  emit( out, "  // generate indicated field\n" );
  emit( out, "  result = NULL;\n" );
  emit( out, "  switch( index )\n" );
  emit( out, "    {\n" );

  index = 0;
  insideGenerateFields = true;
  generateFields( out, index,
		  initEffect->effectList->begin(), 
		  initEffect->effectList->end(), 
		  environment );
  insideGenerateFields = false;

  emit( out, "    }\n" );
  emit( out, "  delete newElement;\n\n" );
  emit( out, "  // clean actualList\n" );
  emit( out, "  for( it = toBeRemoved.begin(); it != toBeRemoved.end(); ++it )\n" );
  emit( out, "    removeElement( (stateListClass*&)actualList, *it );\n" );
  emit( out, "}\n\n" );


  // getInitialSituations
  emit( out, "stateListClass*\n" );
  emit( out, "stateFor%sClass::getInitialSituations( void )\n", ident );
  emit( out, "{\n" );
  emit( out, "  register int i;\n" );
  emit( out, "  register stateListFor%sClass *result, *ptr;\n\n", ident );

  emit( out, "  // initialize\n" );
  emit( out, "  result = new stateListFor%sClass;\n", ident );

  emit( out, "  // generate each field (pointer to list is passed by reference)\n" );
  emit( out, "  for( i = 0; i < %d; ++i )\n", index );
  emit( out, "    generateField( i, result );\n\n" );

  emit( out, "  // apply ramifications and count valid states\n" );
  emit( out, "  for( ptr = result, i = 0; ptr != NULL; ptr = (stateListFor%sClass*)ptr->next )\n", ident );
  emit( out, "    {\n" );
  emit( out, "      ptr->state->applyRamifications();\n" );
  emit( out, "      i += ptr->state->valid;\n" );
  emit( out, "    }\n\n" );

  emit( out, "  // assign probabilities\n" );
  emit( out, "  for( ptr = result; ptr != NULL; ptr = (stateListFor%sClass*)ptr->next )\n", ident );
  emit( out, "    ptr->probability = 1.0 / ((float)i);\n\n" );

  emit( out, "  // return list of initial states\n" );
  emit( out, "  return( result );\n" );
  emit( out, "}\n\n" );


  // alloc method
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::alloc( register int number, register stateClass **pool )\n", ident );
  emit( out, "{\n" );
  emit( out, "  register int i;\n" );
  emit( out, "  register stateFor%sClass *result;\n\n", ident );
  emit( out, "  result = new stateFor%sClass[number];\n", ident );
  emit( out, "  for( i = 0; i < number; ++i )\n" );
  emit( out, "    pool[i] = &result[i];\n" );
  emit( out, "}\n\n" );


  // dealloc method
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::dealloc( register stateClass *pool )\n", ident );
  emit( out, "{\n" );
  emit( out, "  delete[] (stateFor%sClass*)pool;\n", ident );
  emit( out, "}\n\n" );


  // initializeFunction
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::initialize( void )\n", ident );
  emit( out, "{\n" );

  // check in classes
  if( beliefHook != NULL )
    emit( out, "  %sBeliefClass::checkIn();\n", beliefHook->ident );
  else if( modelHook != NULL )
    emit( out, "  %sBeliefClass::checkIn();\n", modelHook->ident );

  emit( out, "}\n\n" );
}


void
problemClass::generateVirtualMethods( FILE *out )
{
  map<const char*,int,ltstr>::const_iterator it1;
  map<const char*,const functionClass*,ltstr>::const_iterator it2;
  map<const char*,const predicateClass*,ltstr>::const_iterator it3;
  map<const char*,const char*,ltstr> environment;

  // applyRamifications
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::applyRamifications( void )\n", ident );
  emit( out, "{\n" );
  emit( out, "  register void (**op)( stateFor%sClass& );\n\n", ident );
  emit( out, "  for( op = ramificationTable; *op != NULL; ++op )\n" );
  emit( out, "    (**op)( *this );\n" );
  emit( out, "}\n\n" );


  // goal
  emit( out, "bool\n" );
  emit( out, "stateFor%sClass::goal( void ) const\n", ident );
  emit( out, "{\n" );

  if( goalFormula != NULL )
    {
      emit( out, "  return( " );
      goalFormula->generateCode( out, 0, "", true /*irrelevant*/, environment );
      emit( out, " );\n" );
    }
  else
    emit( out, "  return( 0 );\n" );

  emit( out, "}\n\n" );


  // applyAllActions
  emit( out, "stateListClass *\n" );
  emit( out, "stateFor%sClass::applyAllActions( void ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  register float mass;\n" );
  emit( out, "  register stateListClass *resPtr, *tmp, *ptr;\n" );
  emit( out, "  register stateListClass *(**op)( const stateFor%sClass& );\n", ident );
  emit( out, "  static stateListFor%sClass result[%d];\n\n", ident, maxStatesAfter + 1 );
  //emit( out, "  stateListFor%sClass *result;\n\n", ident );
  //emit( out, "  result = new stateListFor%sClass[%d];\n\n", ident, maxStatesAfter + 1 );
  emit( out, "  resPtr = result;\n" );
  emit( out, "  for( op = actionTable; *op != NULL; ++op )\n" );
  emit( out, "    {\n" );
  emit( out, "      // apply action\n" );
  emit( out, "      tmp = (**op)( *this );\n\n" );

  emit( out, "      // normalize probabilities\n" );
  emit( out, "      for( mass = 0.0, ptr = tmp; ptr->probability != -1; ++ptr )\n" );
  emit( out, "        mass += (ptr->state->valid ? ptr->probability : 0.0);\n" );
  emit( out, "      for( ptr = tmp; ptr->probability != -1; ++ptr )\n" );
  emit( out, "        ptr->probability /= mass;\n\n" );

  emit( out, "      // store result\n" );
  emit( out, "      for( ptr = tmp; ptr->probability != -1; ++ptr )\n" );
  emit( out, "        {\n" );
  emit( out, "          if( ptr->state->valid )\n" );
  emit( out, "            {\n" );
  emit( out, "              resPtr->action = ptr->action;\n" );
  emit( out, "              resPtr->cost = ptr->cost;\n" );
  emit( out, "              resPtr->probability = ptr->probability;\n" );
  emit( out, "              *resPtr->state = *ptr->state;\n" );
  emit( out, "              resPtr->observations = ptr->observations;\n" );
  emit( out, "              ++resPtr;\n" );
  emit( out, "            }\n" );
  emit( out, "          else\n" );
  emit( out, "            {\n" );
  emit( out, "              delete[] ptr->observations;\n" );
  emit( out, "            }\n" );
  emit( out, "        }\n" );
  //emit( out, "      delete[] tmp;\n" );
  emit( out, "    }\n" );

  emit( out, "  // return\n" );
  emit( out, "  resPtr->probability = -1;\n" );
  emit( out, "  return( (stateListClass*)result );\n" );
  emit( out, "}\n\n" );


  // print
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::print( ostream& os, int indent ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  register int i, *ip;\n" );
  emit( out, "  register bool *bp;\n\n" );

  if( numObjects > 0 )
    {
      // objects
      for( it1 = varDenotation.begin(); it1 != varDenotation.end(); ++it1 )
	  {
	    emit( out, "  (os.width( indent ), os) << \"\";\n" );
	    emit( out, "  os << \"object[%d] = \" << object[%d] << endl;\t// %s\n", 
		  (*it1).second, (*it1).second, (*it1).first );
	  }

      // functions
      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	{
	  int len = (int)pow( (double)numObjects, (double)arity );
	  if( problem->numFunctions[arity] > 0 )
	    {
	      emit( out, "// functions of arity %d\n", arity );
	      it2 = problem->domain->functionHash.begin();
	      for( ; it2 != problem->domain->functionHash.end(); ++it2 )
		if( ((*it2).second->numArguments == arity) &&
		    !(*it2).second->external && !(*it2).second->internal &&
		    (globalFunctionHash.find( (*it2).first ) == globalFunctionHash.end()) &&
		    !(*it2).second->staticFunction( this ) )
		  {
		    emit( out, "  (os.width( indent ), os) << \"\";\n" );
		    emit( out, "  os << \"function%d[%s] = { \";\n", arity, (*it2).first );
		    emit( out, "  for( i = 0, ip = (int*)function%d + (%d*%s); i < %d; ++i, ++ip )\n", 
			  arity, len, (*it2).first, len );
		    emit( out, "    os << *ip << \" \";\n" );
		    emit( out, "  os << \"}\" << endl;\n" );
		  }
	    }
	}

      // predicates
      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	{
	  int len = (int)ceil(pow((double)problem->numObjects,(double)arity)/(double)(8*sizeof(unsigned)));
	  if( problem->numPredicates[arity] > 0 )
	    {
	      emit( out, "\n" );
	      emit( out, "  // predicates of arity %d\n", arity );
	      it3 = problem->domain->predicateHash.begin();
	      for( ; it3 != problem->domain->predicateHash.end(); ++it3 )
		if( ((*it3).second->numArguments == arity) &&
		    !(*it3).second->external && !(*it3).second->internal &&
		    (globalPredicateHash.find( (*it3).first ) == globalPredicateHash.end()) &&
		    !(*it3).second->staticPredicate( this ) )
		  {
		    emit( out, "  (os.width( indent ), os) << \"\";\n" );
		    emit( out, "  os << \"predicate%d[%s] = { \";\n", arity, (*it3).first );
		    emit( out, "  for( int i = 0; i < %d; ++i )\n", len ); 
		    emit( out, "    {\n" );
		    emit( out, "      os << \"[\" << i << \"]:\";\n" );
		    emit( out, "      printBits( os, predicate%d[%s][i] );\n", arity, (*it3).first );
		    emit( out, "      os << \" \";\n" );
		    emit( out, "    }\n" );
		    emit( out, "  os << \" }\" << endl;\n" );
		  }
	    }
	}
    }

  // arrays
  for( int i = 0; i < numArrays; ++i )
    {
      emit( out, "  (os.width( indent ), os) << \"\";\n" );
      emit( out, "  os << \"array[%d] = { \";\n", i );
      emit( out, "  for( i = 0; i < %d; ++i )\n", maxArraySize );
      emit( out, "    os << array[%d][i] << \" \";\n", i );
      emit( out, "  os << \"}\" << endl;\n" );
    }

    emit( out, "}\n\n" );
}

void
problemClass::generateStateHash( FILE *out )
{
  int depth;

  // lookup function
  emit( out, "const stateClass::node_t*\n" );
  emit( out, "stateFor%sClass::lookup( const node_t *node ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  int value;\n" );
  emit( out, "  node_t **ch;\n" );
  emit( out, "  switch( node->depth )\n" );
  emit( out, "    {\n" );

  depth = 0;
  for( unsigned i = 0; i < varDenotation.size(); ++i )
    {
      emit( out, "    case %d:\n", depth );
      emit( out, "      value = object[%d];\n", i );
      emit( out, "      break;\n" );
      ++depth;
    }

  for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
    {
      for( int i = 0; i < numFunctions[arity]; ++i )
	for( int j = 0; j < (int)pow((double)numObjects,(double)arity); ++j )
	  {
	    emit( out, "    case %d:\n", depth );
	    emit( out, "      value = function%d[%d][%d];\n", arity, i, j );
	    emit( out, "      break;\n" );
	    ++depth;
	  }

      for( int i = 0; i < numPredicates[arity]; ++i )
	for( int j = 0; j < (int)ceil(pow((double)numObjects,(double)arity)/(double)(8*sizeof(unsigned))); ++j )
	  {
	    emit( out, "    case %d:\n", depth );
	    emit( out, "      value = predicate%d[%d][%d];\n", arity, i, j );
	    emit( out, "      break;\n" );
	    ++depth;
	  }
    }

  for( int i = 0; i < numArrays * maxArraySize; ++i )
    {
      emit( out, "    case %d:\n", depth );
      emit( out, "      cerr << \"ERROR: not yet supported.\" << endl;\n" );
      emit( out, "      return( NULL );\n" );
      emit( out, "      break;\n" );
      ++depth;
    }

  emit( out, "    case %d:\n", depth );
  emit( out, "      return( node );\n" );
  emit( out, "      break;\n" );
  emit( out, "    default:\n" );
  emit( out, "      cerr << \"ERROR: bad depth index \" << node->depth << \".\" << endl;\n" );
  emit( out, "      return( NULL );\n" );
  emit( out, "      break;\n" );
  emit( out, "    }\n\n" );
  emit( out, "  if( !node->children )\n" );
  emit( out, "    return( NULL );\n" );
  emit( out, "  else\n" );
  emit( out, "    {\n" );
  emit( out, "      for( ch = node->children; *ch != NULL; ++ch )\n" );
  emit( out, "        if( (*ch)->value == value )\n" );
  emit( out, "          return( lookup( *ch ) );\n" );
  emit( out, "      return( NULL );\n" );
  emit( out, "    }\n" );
  emit( out, "}\n\n" );


  // insert function
  emit( out, "const stateClass::node_t*\n" );
  emit( out, "stateFor%sClass::insert( node_t *node, int index ) const\n", ident );
  emit( out, "{\n" );
  emit( out, "  int n, value;\n" );
  emit( out, "  node_t **ch;\n" );
  emit( out, "  switch( node->depth )\n" );
  emit( out, "    {\n" );

  depth = 0;
  for( unsigned i = 0; i < varDenotation.size(); ++i )
    {
      emit( out, "    case %d:\n", depth );
      emit( out, "      value = object[%d];\n", i );
      emit( out, "      break;\n" );
      ++depth;
    }

  for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
    {
      for( int i = 0; i < numFunctions[arity]; ++i )
	for( int j = 0; j < (int)pow((double)numObjects,(double)arity); ++j )
	  {
	    emit( out, "    case %d:\n", depth );
	    emit( out, "      value = function%d[%d][%d];\n", arity, i, j );
	    emit( out, "      break;\n" );
	    ++depth;
	  }

      for( int i = 0; i < numPredicates[arity]; ++i )
	for( int j = 0; j < (int)ceil(pow((double)numObjects,(double)arity)/(double)(8*sizeof(unsigned))); ++j )
	  {
	    emit( out, "    case %d:\n", depth );
	    emit( out, "      value = predicate%d[%d][%d];\n", arity, i, j );
	    emit( out, "      break;\n" );
	    ++depth;
	  }
    }

  for( int i = 0; i < numArrays * maxArraySize; ++i )
    {
      emit( out, "    case %d:\n", depth );
      emit( out, "      cerr << \"ERROR: not yet supported.\" << endl;\n" );
      emit( out, "      return( NULL );\n" );
      emit( out, "      break;\n" );
      ++depth;
    }

  emit( out, "    case %d:\n", depth );
  emit( out, "      node->children = (node_t**)index;\n" );
  emit( out, "      return( node );\n" );
  emit( out, "      break;\n" );
  emit( out, "    default:\n" );
  emit( out, "      cerr << \"ERROR: bad depth index \" << node->depth << \".\" << endl;\n" );
  emit( out, "      return( NULL );\n" );
  emit( out, "      break;\n" );
  emit( out, "    }\n\n" );
  emit( out, "  if( !node->children )\n" );
  emit( out, "    {\n" );
  emit( out, "      node->children = (node_t**)malloc( 1 * sizeof( node_t* ) );\n" );
  emit( out, "      node->children[0] = NULL;\n" );
  emit( out, "    }\n\n" );
  emit( out, "  for( ch = node->children, n = 0; *ch != NULL; ++ch, ++n )\n" );
  emit( out, "    if( (*ch)->value == value )\n" );
  emit( out, "      return( insert( *ch, index ) );\n" );
  emit( out, "  if( *ch == NULL )\n" );
  emit( out, "    {\n" );
  emit( out, "      node->children = (node_t**)realloc( node->children, (2+n) * sizeof( node_t* ) );\n" );
  emit( out, "      node->children[n+1] = NULL;\n" );
  emit( out, "      node->children[n] = new node_t;\n" );
  emit( out, "      node->children[n]->value = value;\n" );
  emit( out, "      node->children[n]->depth = 1 + node->depth;\n" );
  emit( out, "      node->children[n]->parent = node;\n" );
  emit( out, "      return( insert( node->children[n], index ) );\n" );
  emit( out, "    }\n" );
  emit( out, "}\n\n" );


  // recover function
  emit( out, "void\n" );
  emit( out, "stateFor%sClass::recover( const node_t *node )\n", ident );
  emit( out, "{\n" );
  emit( out, "  switch( node->depth - 1 )\n" );
  emit( out, "    {\n" );

  depth = 0;
  for( unsigned i = 0; i < varDenotation.size(); ++i )
    {
      emit( out, "    case %d:\n", depth );
      emit( out, "      object[%d] = node->value;\n", i );
      emit( out, "      break;\n" );
      ++depth;
    }

  for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
    {
      for( int i = 0; i < numFunctions[arity]; ++i )
	for( int j = 0; j < (int)pow((double)numObjects,(double)arity); ++j )
	  {
	    emit( out, "    case %d:\n", depth );
	    emit( out, "      function%d[%d][%d] = node->value;\n", arity, i, j );
	    emit( out, "      break;\n" );
	    ++depth;
	  }

      for( int i = 0; i < numPredicates[arity]; ++i )
	for( int j = 0; j < (int)ceil(pow((double)numObjects,(double)arity)/(double)(8*sizeof(unsigned))); ++j )
	  {
	    emit( out, "    case %d:\n", depth );
	    emit( out, "      predicate%d[%d][%d] = node->value;\n", arity, i, j );
	    emit( out, "      break;\n" );
	    ++depth;
	  }
    }

  for( int i = 0; i < numArrays * maxArraySize; ++i )
    {
      emit( out, "    case %d:\n", depth );
      emit( out, "      cerr << \"ERROR: not yet supported.\" << endl;\n" );
      emit( out, "      return;\n" );
      emit( out, "      break;\n" );
      ++depth;
    }

  emit( out, "    case %d:\n", depth );
  emit( out, "      break;\n" );
  emit( out, "    default:\n" );
  emit( out, "      cerr << \"ERROR: bad depth index \" << node->depth << \".\" << endl;\n" );
  emit( out, "      break;\n" );
  emit( out, "    }\n\n" );
  emit( out, "}\n\n" );
}

void
problemClass::generateRegistrationFunctions( FILE *out )
{
  // problemRegister
  emit( out, "extern \"C\" problemHandleClass* problemRegister( void );\n\n" );
  emit( out, "problemHandleClass *\n" );
  emit( out, "problemRegister( void )\n" );
  emit( out, "{\n" );
  emit( out, "  problemHandleClass *handle;\n\n" );
  emit( out, "  stateFor%sClass::fillTables();\n", ident );
  emit( out, "  handle = new problemHandleClass;\n" );

  switch( domain->model )
    {
    case PLANNING:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_PLANNING;\n" );
      break;
    case CONFORMANT1:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_CONFORMANT1;\n" );
      break;
    case CONFORMANT2:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_CONFORMANT2;\n" );
      break;
    case MDP:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_MDP;\n" );
      break;
    case ND_MDP:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_ND_MDP;\n" );
      break;
    case POMDP1:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_POMDP1;\n" );
      break;
    case POMDP2:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_POMDP2;\n" );
      break;
    case ND_POMDP1:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_ND_POMDP1;\n" );
      break;
    case ND_POMDP2:
      emit( out, "  handle->problemType = problemHandleClass::PROBLEM_ND_POMDP2;\n" );
      break;
    }

  emit( out, "  handle->hookType = %d;\n", problem->hookType );
  emit( out, "  handle->numActions = stateFor%sClass::numActions;\n", ident );
  emit( out, "  handle->stateSize = sizeof( stateFor%sClass );\n", ident );
  emit( out, "  handle->observationSize = sizeof( observationFor%sClass );\n", ident );
  emit( out, "  handle->theNullObservation = new observationFor%sClass;\n", ident );
  emit( out, "  handle->bootstrap = &stateFor%sClass::bootstrap;\n", ident );
  emit( out, "  handle->cleanUp = &stateFor%sClass::cleanUp;\n", ident );
  emit( out, "  handle->printAction = &stateFor%sClass::printAction;\n", ident );
  emit( out, "  handle->stateAllocFunction = &stateFor%sClass::alloc;\n", ident );
  emit( out, "  handle->stateDeallocFunction = &stateFor%sClass::dealloc;\n", ident );
  emit( out, "  handle->initializeFunction = &stateFor%sClass::initialize;\n", ident );
  emit( out, "  handle->otherInfo = 0;\n" );

  // set other info
  if( dynamic_cast<const certaintyGoalClass*>( goalFormula ) != NULL )
    emit( out, "  handle->otherInfo = handle->otherInfo | (1<<problemHandleClass::CERTAINTY_GOAL);\n" );

  // model hook
  if( modelHook != NULL )
    emit( out, "  handle->modelHook = new %sModelClass;\n", modelHook->ident );
  else
    emit( out, "  handle->modelHook = NULL;\n" );

  // belief hook
  if( beliefHook != NULL )
    emit( out, "  handle->beliefHook = new %sBeliefClass;\n", beliefHook->ident );
  else if( modelHook != NULL )
    emit( out, "  handle->beliefHook = new %sBeliefClass;\n", modelHook->ident );
  else
    emit( out, "  handle->beliefHook = NULL;\n" );

  // heuristic hook
  if( heuristicHook != NULL )
    emit( out, "  handle->heuristicHook = new %sHeuristicClass;\n", heuristicHook->ident );
  else if( beliefHook != NULL )
    emit( out, "  handle->heuristicHook = new %sHeuristicClass;\n", beliefHook->ident );
  else if( modelHook != NULL )
    emit( out, "  handle->heuristicHook = new %sHeuristicClass;\n", modelHook->ident );
  else
    {
      if( domain->model == PLANNING )
	emit( out, "  handle->heuristicHook = NULL;\n" );
      else
	emit( out, "  handle->heuristicHook = NULL;\n" );
    }

  // return handle
  emit( out, "  return( handle );\n" );
  emit( out, "}\n\n" );
}


void
problemClass::generateMakefile( FILE *out, char *MName )
{
  // name
  emit( out, "OUTPUT=%s%s\n", &ident[1], domain->ident );

  // linkmap
  emit( out, "STDLIB=$(LIB)/standard.o " );
  emit( out, "\n" );

  // end of macros
  emit( out, "\n" );

  // dependencies/rules
  emit( out, "$(OUTPUT).o:\t\t$(OUTPUT)-state.o\n" );
  emit( out, "\t\t\t@$(LD) -o $(OUTPUT).o $(OUTPUT)-state.o $(LDFLAGS) $(STDLIB) $(LINKMAP)\n" );
  emit( out, "\n" );

  emit( out, "$(OUTPUT)-state.o:\t$(OUTPUT)-state.cc $(OUTPUT).h\n" );
  emit( out, "\t\t\t@$(CC) $(CCFLAGS) -c $(OUTPUT)-state.cc -I. $(INCLUDE)\n" );
  emit( out, "\n" );

  if( domain->model == PLANNING )
    {
      emit( out, "$(OUTPUT)-heuristic.o:\t$(OUTPUT)-heuristic.cc $(OUTPUT).h\n" );
      emit( out, "\t\t\t@$(CC) $(CCFLAGS) -c $(OUTPUT)-heuristic.cc -I. $(INCLUDE)\n" );
      emit( out, "\n" );
    }

  emit( out, "$(OUTPUT)-state.cc \\\n" );
  if( domain->model == PLANNING )
    emit( out, "+ $(OUTPUT)-heuristic.cc \\\n" );
  emit( out, "+ $(OUTPUT).h:\t\t%s\n", domain->fileName );
  emit( out, "\t\t\t@$(GPTHOME)/bin/parser %s %s\n", &ident[1], domain->fileName );
  emit( out, "\n" );

  // clean
  emit( out, "clean:\n" );
  emit( out, "\t\t\t@rm -f $(OUTPUT)*.o $(OUTPUT)*.cc $(OUTPUT)*.h\n\n" );

  // deep-clean
  emit( out, "deep-clean:\t\tclean\n" );
  emit( out, "\t\t\t@rm -f %s\n", MName );
}


void
generateCode( FILE *HFile, char *HName, FILE *CFile, FILE *MFile, char *MName, problemClass *prob )
{
  int index1, index2;
  map<const char*,list<const char*>*,ltstr>::const_iterator it;
  list<const ramificationClass*>::const_iterator it1;
  list<const internalPredicateClass*>::const_iterator it5;
  list<const actionClass*>::const_iterator it2;
  map<const char*,const functionClass*,ltstr>::const_iterator it3;
  map<const char*,const predicateClass*,ltstr>::const_iterator it6;
  list<const char*>::const_iterator it7;
  set<const char*,ltstr>::const_iterator it8;

  extern void genStateCode( FILE *, char *, FILE *, struct problem_s * );
  extern void genHeuristicCode( FILE *, char *, FILE *, struct problem_s * );

  // set global data
  problem = prob;

  // partition predicates/functions into static/non-static
  predicateStaticPartition( problem );
  functionStaticPartition( problem );

  //
  // include file
  //
  emit( HFile, "#include <iostream>\n" );
  emit( HFile, "#include <stdio.h>\n\n" );
  emit( HFile, "using namespace std;\n" );
  emit( HFile, "#include <theseus/theRtStandard.h>\n\n" );

  // object labels
  emit( HFile, "// object labels\n" );
  index1 = 0;
  for( it = problem->objectHashByType.begin(); it != problem->objectHashByType.end(); ++it )
    for( it7 = (*it).second->begin(); it7 != (*it).second->end(); ++it7 )
      emit( HFile, "#define %-20s%d\n", (*it7), index1++ );
  problem->numObjects = index1;
  emit( HFile, "\n" );

  // numobjects
  emit( HFile, "#define %-20s%d\n\n", "NUMOBJECTS", problem->numObjects );

  // function labels
  for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
    if( (problem->numFunctions[arity] > 0) || (problem->numStaticFunctions[arity] > 0) )
      {
	index1 = 0;
	index2 = 0;
	emit( HFile, "// labels for functions of arity %d\n", arity );
	for( it3 = problem->domain->functionHash.begin(); it3 != problem->domain->functionHash.end(); ++it3 )
	  if( ((*it3).second->numArguments == arity) &&
	      !(*it3).second->external && !(*it3).second->internal &&
	      (globalFunctionHash.find( (*it3).first ) == globalFunctionHash.end()) )
	    {
	      if( !(*it3).second->staticFunction( problem ) )
		emit( HFile, "#define %-20s%d\t// non-static\n", (*it3).first, index1++ );
	      else
		emit( HFile, "#define %-20s%d\t// static\n", (*it3).first, index2++ );
	    }
	emit( HFile, "\n" );
      }

  // predicate labels
  for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
    if( (problem->numPredicates[arity] > 0) || (problem->numStaticPredicates[arity] > 0) )
      {
	index1 = 0;
	index2 = 0;
	emit( HFile, "// labels for predicates of arity %d\n", arity );
	for( it6 = problem->domain->predicateHash.begin(); it6!=problem->domain->predicateHash.end(); ++it6 )
	  if( ((*it6).second->numArguments == arity) &&
	      !(*it6).second->external && !(*it6).second->internal &&
	      (globalPredicateHash.find( (*it6).first ) == globalPredicateHash.end()) )
	    {
	      if( !(*it6).second->staticPredicate( problem ) )
		emit( HFile, "#define %-20s%d\t// non-static\n", (*it6).first, index1++ );
	      else
		emit( HFile, "#define %-20s%d\t// static\n", (*it6).first, index2++ );
	    }
	emit( HFile, "\n" );
      }

  // array labels
  if( (it = problem->objectHashByType.find( (const char*)":array" )) != problem->objectHashByType.end() )
    {
      index1 = 0;
      emit( HFile, "// array labels\n" );
      for( it7 = (*it).second->begin(); it7 != (*it).second->end(); ++it7 )
	emit( HFile, "#define A%-19s%d\n", (*it7), index1++ );
    }
  emit( HFile, "\n\n" );

  // class layouts
  problem->generateClasses( HFile );

  // external functions
  for( it3 = problem->domain->functionHash.begin(); it3 != problem->domain->functionHash.end(); ++it3 )
    if( (*it3).second->external &&
	(globalFunctionHash.find( (*it3).first ) == globalFunctionHash.end()) )
      {
	assert( ((*it3).second->domainType == functionClass::SIMPLE_DOMAIN) ||
		((*it3).second->domainType == functionClass::LIST_DOMAIN) );
	emit( HFile, "extern int %s( const stateClass&, ", (*it3).first );
	for( int i = 0; i < (*it3).second->numArguments - 1; ++i )
	  emit( HFile, "int, " );
	emit( HFile, "int );\n" );
      }

  for( it6 = problem->domain->predicateHash.begin(); it6 != problem->domain->predicateHash.end(); ++it6 )
    if( (*it6).second->external &&
	(globalPredicateHash.find( (*it6).first ) == globalPredicateHash.end()) )
      {
	assert( ((*it6).second->domainType == functionClass::SIMPLE_DOMAIN) ||
		((*it6).second->domainType == functionClass::LIST_DOMAIN) );
	emit( HFile, "extern bool %s( const stateClass&, ", (*it6).first );
	for( int i = 0; i < (*it6).second->numArguments - 1; ++i )
	  emit( HFile, "int, " );
	emit( HFile, "int );\n" );
      }
  for( it8 = problem->domain->externalProcedureList.begin(); it8 != problem->domain->externalProcedureList.end(); ++it8 )
    {
      emit( HFile, "extern void %s( stateClass& );\n", *it8 );
    }


  //
  // source file
  //
  emit( CFile, "#include <%s>\n", HName );
  emit( CFile, "#include <pkg/standard.h>\n" );
  emit( CFile, "#include <list>\n" );
  emit( CFile, "#include <map>\n" );
  //  for( entry = problem->domain->linkMapHash->sequential; entry != NULL; entry = entry->sequential )
  //    emit( CFile, "#include <%s.h>\n", entry->key );
  //  emit( CFile, "\n" );

  // include for hooks
  if( problem->modelHook != NULL )
    emit( CFile, "#include <%s.h>\n", problem->modelHook->ident );
  if( problem->beliefHook != NULL )
    emit( CFile, "#include <%s.h>\n", problem->beliefHook->ident );
  if( problem->heuristicHook != NULL )
    emit( CFile, "#include <%s.h>\n", problem->heuristicHook->ident );
  emit( CFile, "\n" );

  // static members
  if( problem->numObjects > 0 )
    {
      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	if( problem->numStaticPredicates[arity] > 0 )
	  {
	    emit( CFile, "// static predicates %d argument(s)\n", arity );
	    emit( CFile, "unsigned stateFor%sClass::staticPredicate%d[%d][%d];\n",
		  problem->ident, arity, problem->numStaticPredicates[arity], 
		  (int)ceil(pow((double)problem->numObjects,(double)arity)/(double)(8*sizeof(unsigned))) );
	    emit( CFile, "\n" );
	  }

      for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
	if( problem->numStaticFunctions[arity] > 0 )
	  {
	    emit( CFile, "// static functions %d argument(s)\n", arity );
	    emit( CFile, "int stateFor%sClass::staticFunction%d[%d][%d];\n",
		  problem->ident, arity, problem->numStaticFunctions[arity],
		  (int)pow((double)problem->numObjects,(double)arity) );
	    emit( CFile, "\n" );
	  }
    }

  // ramifications
  for( it1 = problem->domain->ramificationList.begin(); it1 != problem->domain->ramificationList.end(); ++it1 )
    {
      (*it1)->generateRamificationCode( CFile );
      ((ramificationClass*)(*it1))->generateInstantiations( CFile );
    }

  // logical definitions
  for( it5 = problem->domain->internalPredicateList.begin(); it5 != problem->domain->internalPredicateList.end(); ++it5 )
    {
      (*it5)->generateCode( CFile );
    }

  // actions
  if( (prob->domain->model == MDP) || (prob->domain->model == ND_MDP) )
    {
      // generate INIT action
      emit( CFile, "stateListClass *\n" );
      emit( CFile, "%sAction0( const stateFor%sClass& s )\n", problem->ident, problem->ident );
      emit( CFile, "{\n" );
      emit( CFile, "  stateListFor%sClass *result;\n\n", problem->ident );
      emit( CFile, "  result = new stateListFor%sClass[1];\n", problem->ident );
      emit( CFile, "  result[0].cost = 0.0;\n" );
      emit( CFile, "  result[0].probability = -1;\n" );
      emit( CFile, "  return( (stateListClass*)result );\n" );
      emit( CFile, "}\n\n" );
      ++prob->numActions;
    }

  for( it2 = problem->domain->actionList.begin(); it2 != problem->domain->actionList.end(); ++it2 )
    {
      (*it2)->generateActionCode( CFile );
      (*it2)->generateCostCode( CFile );
      (*it2)->generateObservationCode( CFile );
      ((actionClass*)(*it2))->generateInstantiations( CFile );
    }

  // methods
  problem->generateBasicMethods( CFile );
  problem->generateStaticMethods( CFile );
  problem->generateVirtualMethods( CFile );


  // generate pending methods
  set<const existsFormulaClass*> ePend;
  set<const forallFormulaClass*> fPend;
  set<const sumTermClass*> sPend;
  while( !ePending.empty() || !fPending.empty() || !sPending.empty() )
    {
      static set<const existsFormulaClass*>::const_iterator eit;
      static set<const forallFormulaClass*>::const_iterator fit;
      static set<const sumTermClass*>::const_iterator sit;
      list<map<const char*,const typeClass*,ltstr>*>::const_iterator vit;
      map<const char*,const typeClass*,ltstr>::const_iterator mit;

      map<const char*,const char*,ltstr> environment;
      list<const char*> objectList;
      list<const char*>::const_iterator it;

      // move pending list into fresh variables
      ePend.clear();
      ePend.insert( ePending.begin(), ePending.end() );
      ePending.clear();
      fPend.clear();
      fPend.insert( fPending.begin(), fPending.end() );
      fPending.clear();
      sPend.clear();
      sPend.insert( sPending.begin(), sPending.end() );
      sPending.clear();

      // exists
      for( eit = ePend.begin(); eit != ePend.end(); eGenerated.insert( *eit++ ) )
	if( !(*eit)->inlineFormula )
	  {
	    // generate prototype and recreate original environment (first var is our var)
	    environment.clear();
	    emit( CFile, "bool\n" );
	    emit( CFile, "stateFor%sClass::exists%d( stateListFor%sClass *ptr", 
		  problem->ident, (*eit)->id, problem->ident );
	    if( (*eit)->internal )
	      emit( CFile, ", int depth, ARforPredicate%s *current", (*eit)->internal );
	    for( vit = (*eit)->environment->begin(), ++vit; vit != (*eit)->environment->end(); ++vit )
	      {
		for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
		  {
		    environment.insert( make_pair( (*mit).first, (*mit).first ) );
		    emit( CFile, ", int %s", (*mit).first );
		  }
	      }
	    emit( CFile, " ) const\n" );
	    emit( CFile, "{\n" );

#if 0
	    if( (*eit)->internal )
	      {
		emit( CFile, "  ARforPredicate%s current, *p;\n\n", (*eit)->internal );
		if( !(*eit)->environment->empty() )
		  {
		    // initialize current Activation Record
		    emit( CFile, "  // initialize current Activation Record\n" );
		    for( vit = (*eit)->environment->begin(), ++vit; vit != (*eit)->environment->end(); ++vit)
		      for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
			emit( CFile, "  current.%s = %s;\n", (*mit).first, (*mit).first );
		    emit( CFile, "  current.prev = AR;\n\n" );

		    // check for branch loop
		    emit( CFile, "  // check for branch loop\n" );
		    emit( CFile, "  for( p = AR; p != NULL; p = p->prev )\n" );
		    emit( CFile, "    if( " );
		    for( vit = (*eit)->environment->begin(), ++vit; vit != (*eit)->environment->end(); )
		      {
			for( mit = (*vit)->begin(); mit != (*vit)->end(); )
			  {
			    emit( CFile, "(%s == p->%s)", (*mit).first, (*mit).first );
			    if( ++mit != (*vit)->end() ) emit( CFile, " && " );
			  }
			if( ++vit != (*eit)->environment->end() ) emit( CFile, " && " );
		      }
		    emit( CFile, " )\n" );
		    emit( CFile, "      return( false );\n\n" );
		  }
	      }
#endif

	    // generate code
	    emit( CFile, "  return( false" );
	    insideInternal = (*eit)->internal;
	    insideQuantifier = true;
	    objectList.clear();
	    problem->getObjectsOfType( (*eit)->varType, objectList );
	    for( it = objectList.begin(); it != objectList.end(); ++it )
	      {
		emit( CFile, " || " );

		// extend environment with new var
		assert( environment.find( (*eit)->variable ) == environment.end() );
		environment.insert( make_pair( (*eit)->variable, *it ) );

		// generate code
		(*eit)->formula->generateCode( CFile, 0, "", false, environment );

		// shrink environment
		environment.erase( (*eit)->variable );
		//xxxxx free( (void*)*it );
	      }
	    insideInternal = NULL;
	    insideQuantifier = false;

	    emit( CFile, " );\n" );
	    emit( CFile, "}\n\n" );
	  }

      // forall
      for( fit = fPend.begin(); fit != fPend.end(); fGenerated.insert( *fit++ ) )
	if( !(*fit)->inlineFormula )
	  {
	    // generate prototype and recreate original environment (first var is our var)
	    environment.clear();
	    emit( CFile, "bool\n" );
	    emit( CFile, "stateFor%sClass::forall%d( stateListFor%sClass *ptr", 
		  problem->ident, (*fit)->id, problem->ident );
	    if( (*fit)->internal )
	      emit( CFile, ", int depth, ARforPredicate%s *current", (*fit)->internal );
	    for( vit = (*fit)->environment->begin(), ++vit; vit != (*fit)->environment->end(); ++vit )
	      {
		for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
		  {
		    environment.insert( make_pair( (*mit).first, (*mit).first ) );
		    emit( CFile, ", int %s", (*mit).first );
		  }
	      }
	    emit( CFile, " ) const\n" );
	    emit( CFile, "{\n" );

#if 0
	    if( (*fit)->internal )
	      {
		emit( CFile, "  ARforPredicate%s current, *p;\n\n", (*fit)->internal );
		if( !(*fit)->environment->empty() )
		  {
		    // initialize current Activation Record
		    emit( CFile, "  // initialize current Activation Record\n" );
		    for( vit = (*fit)->environment->begin(), ++vit; vit != (*fit)->environment->end(); ++vit)
		      for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
			emit( CFile, "  current.%s = %s;\n", (*mit).first, (*mit).first );
		    emit( CFile, "  current.prev = AR;\n\n" );

		    // check for branch loop
		    emit( CFile, "  // check for branch loop\n" );
		    emit( CFile, "  for( p = AR; p != NULL; p = p->prev )\n" );
		    emit( CFile, "    if( " );
		    for( vit = (*fit)->environment->begin(), ++vit; vit != (*fit)->environment->end(); )
		      {
			for( mit = (*vit)->begin(); mit != (*vit)->end(); )
			  {
			    emit( CFile, "(%s == p->%s)", (*mit).first, (*mit).first );
			    if( ++mit != (*vit)->end() ) emit( CFile, " && " );
			  }
			if( ++vit != (*fit)->environment->end() ) emit( CFile, " && " );
		      }
		    emit( CFile, " )\n" );
		    emit( CFile, "      return( true );\n\n" );
		  }
	      }
#endif

	    // generate code
	    emit( CFile, "  return( true" );
	    insideInternal = (*fit)->internal;
	    insideQuantifier = true;
	    objectList.clear();
	    problem->getObjectsOfType( (*fit)->varType, objectList );
	    for( it = objectList.begin(); it != objectList.end(); ++it )
	      {
		emit( CFile, " && " );

		// extend environment with new var
		assert( environment.find( (*fit)->variable ) == environment.end() );
		environment.insert( make_pair( (*fit)->variable, *it ) );

		// generate code
		(*fit)->formula->generateCode( CFile, 0, "", true, environment );

		// shrink environment
		environment.erase( (*fit)->variable );
		//xxxxx free( (void*)*it );
	      }
	    insideInternal = NULL;
	    insideQuantifier = false;

	    emit( CFile, " );\n" );
	    emit( CFile, "}\n\n" );
	  }

      // summations
      for( sit = sPend.begin(); sit != sPend.end(); sGenerated.insert( *sit++ ) )
	if( !(*sit)->inlineTerm )
	  {
	    // generate prototype and recreate original environment (first var is our var)
	    environment.clear();
	    emit( CFile, "int\n" );
	    emit( CFile, "stateFor%sClass::summation%d( stateListFor%sClass *ptr",
		  problem->ident, (*sit)->id, problem->ident );
	    for( vit = (*sit)->environment->begin(), ++vit; vit != (*sit)->environment->end(); ++vit )
	      {
		for( mit = (*vit)->begin(); mit != (*vit)->end(); ++mit )
		  {
		    environment.insert( make_pair( (*mit).first, (*mit).first ) );
		    emit( CFile, ", int %s", (*mit).first );
		  }
	      }
	    emit( CFile, " ) const\n" );
	    emit( CFile, "{\n" );
	    emit( CFile, "  return( 0" );

	    // generate code
	    objectList.clear();
	    problem->getObjectsOfType( (*sit)->varType, objectList );
	    for( it = objectList.begin(); it != objectList.end(); ++it )
	      {
		emit( CFile, " + " );

		// extend environment with new var
		assert( environment.find( (*sit)->variable ) == environment.end() );
		environment.insert( make_pair( (*sit)->variable, *it ) );

		// generate code
		(*sit)->term->generateCode( CFile, 0, "", false, false, environment );

		// shrink environment
		environment.erase( (*sit)->variable );
		//xxxxx free( (void*)*it );
	      }

	    emit( CFile, " );\n" );
	    emit( CFile, "}\n\n" );
	  }
    }

  // generate state hash functions
  problem->generateStateHash( CFile );


  // problemRegister
  problem->generateRegistrationFunctions( CFile );


  //
  // Makefile
  //
  problem->generateMakefile( MFile, MName );
}
