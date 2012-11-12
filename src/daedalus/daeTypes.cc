/*
**  Daedalus
**  daeTypes.cc -- Type's Basic Routines
**
**  Blai Bonet, Hector Geffner
**  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002
**
*/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

#include <daedalus/daeTokens.h>
#include <daedalus/daeTypes.h>
#include <daedalus/_daeErrors.h>


extern int lineno;
extern map<const char*,const functionClass*,ltstr> *functionHash;
extern map<const char*,const predicateClass*,ltstr> *predicateHash;
extern map<const char*,const namedTypeClass*,ltstr> *namedTypeHash;

extern booleanTypeClass          *theBooleanType;
extern genericIntegerTypeClass   *theGenericIntegerType;
extern domainClass               *declaredDomain;
extern problemClass              *declaredProblem;


domainClass::domainClass( const char *filen, const char *id )
{
  fileName = filen;
  ident = id;
  model = POMDP2;
  dynamics = PROBABILISTIC;
  feedback = PARTIAL;

  memset( numFunctions, 0, MAX_ARITY * sizeof( int ) );
  memset( numPredicates, 0, MAX_ARITY * sizeof( int ) );
  numArrays = 0;
  maxArraySize = 0;

  existsNumber = 0;
  forallNumber = 0;
  sumNumber = 0;
  existsChain = NULL;
  forallChain = NULL;
  sumChain = NULL;
}

problemClass::problemClass( const char *filen, const char *id, const domainClass *dom )
{
  map<const char*,const namedTypeClass*,ltstr>::const_iterator it1;
  map<const char*,const typeClass*,ltstr>::const_iterator it2;
  map<const char*,list<const char*>*,ltstr>::const_iterator it3;
  map<const char*,const arrayClass*,ltstr>::const_iterator it4;
  map<const char*,int,ltstr>::const_iterator it5;

  fileName = filen;
  ident = id;
  domain = dom;
  initEffect = NULL;
  goalFormula = NULL;
  hookType = NO_HOOK;

  // copy static chains
  existsNumber = dom->existsNumber;
  forallNumber = dom->forallNumber;
  sumNumber = dom->sumNumber;
  existsChain = dom->existsChain;
  forallChain = dom->forallChain;
  sumChain = dom->sumChain;

  // copy namedTypeHash
  for( it1 = domain->namedTypeHash.begin(); it1 != domain->namedTypeHash.end(); ++it1 )
    if( (*it1).second != NULL )
      namedTypeHash.insert( make_pair( (*it1).first, new namedTypeClass( *(*it1).second ) ) );
    else
      namedTypeHash.insert( make_pair( (*it1).first, (const namedTypeClass*)NULL ) );

  // copy objectHashByName
  for( it2 = domain->objectHashByName.begin(); it2 != domain->objectHashByName.end(); ++it2 )
    objectHashByName.insert( make_pair( strdup( (*it2).first ), (*it2).second->clone() ) );

  // copy objectHashByType
  for( it3 = domain->objectHashByType.begin(); it3 != domain->objectHashByType.end(); ++it3 )
    {
      list<const char*> *idList = new list<const char*>;
      list<const char*>::const_iterator it;
      for( it = (*it3).second->begin(); it != (*it3).second->end(); ++it )
	idList->push_back( strdup( *it ) );
      objectHashByType.insert( make_pair( strdup( (*it3).first ), idList ) );
    }

  // copy arrayHash
  for( it4 = domain->arrayHash.begin(); it4 != domain->arrayHash.end(); ++it4 )
    arrayHash.insert( make_pair( strdup( (*it4).first ), new arrayClass( *(*it4).second ) ) );

  // copy set of variable denotations
  for( it5 = domain->varDenotation.begin(); it5 != domain->varDenotation.end(); ++it5 )
    varDenotation.insert( make_pair( strdup( (*it5).first ), (*it5).second ) );

  numActions = 0;
  numRamifications = 0;
  numObjects = 0;
  numArrays = domain->numArrays;
  maxArraySize = domain->maxArraySize;
  memset( numFunctions, 0, domainClass::MAX_ARITY * sizeof( int ) );
  memset( numPredicates, 0, domainClass::MAX_ARITY * sizeof( int ) );
  memset( numStaticFunctions, 0, domainClass::MAX_ARITY * sizeof( int ) );
  memset( numStaticPredicates, 0, domainClass::MAX_ARITY * sizeof( int ) );

  modelHook = NULL;
  beliefHook = NULL;
  heuristicHook = NULL;
}

problemClass::~problemClass()
{
  map<const char*,const namedTypeClass*,ltstr>::const_iterator it1;
  map<const char*,const typeClass*,ltstr>::const_iterator it2;
  map<const char*,list<const char*>*,ltstr>::const_iterator it3;
  map<const char*,const arrayClass*,ltstr>::const_iterator it4;
  map<const char*,int,ltstr>::const_iterator it5;

  //  cout << "******* detructor for problemClass called" << endl;
  //  fprintf( stderr, "free of [%p]%s\n", fileName, fileName );
  //  fprintf( stderr, "free of [%p]%s\n", ident, ident );
  free( (void*)fileName );
  free( (void*)ident );
  delete initEffect;
  delete goalFormula;

  // clean namedTypeHash
  for( it1 = namedTypeHash.begin(); it1 != namedTypeHash.end(); ++it1 )
    if( (*it1).second != NULL )
      delete (*it1).second;

  // clean objectHashByName
  for( it2 = objectHashByName.begin(); it2 != objectHashByName.end(); ++it2 )
    {
      //      fprintf( stderr, "free of [%p]%s\n", (*it2).first, (*it2).first );
      free( (void*)(*it2).first );
      delete (*it2).second;
    }

  // clean objectHashByType
  for( it3 = objectHashByType.begin(); it3 != objectHashByType.end(); ++it3 )
    {
      list<const char*>::const_iterator it;
      for( it = (*it3).second->begin(); it != (*it3).second->end(); ++it )
	{
	  //	  fprintf( stderr, "free of [%p]%s\n", (*it), (*it) );
	  free( (void*)(*it) );
	}
      delete (*it3).second;
      //      fprintf( stderr, "free of [%p]%s\n", (*it3).first, (*it3).first );
      free( (void*)(*it3).first );
    }

  // clean arrayHash
  for( it4 = arrayHash.begin(); it4 != arrayHash.end(); ++it4 )
    {
      //      fprintf( stderr, "free of [%p]%s\n", (*it4).first, (*it4).first );
      free( (void*)(*it4).first );
      delete (*it4).second;
    }

  // clean varDenotation
  for( it5 = varDenotation.begin(); it5 != varDenotation.end(); ++it5 )
    free( (char*)(*it5).first );
}

void
problemClass::getObjectsOfType( const typeClass *type, list<const char*> &oList ) const
{
  const simpleTypeClass *stype;
  type->getBaseObjects( oList );
  if( oList.empty() && ((stype = dynamic_cast<const simpleTypeClass*>(type)) != NULL) )
    {
      map<const char*,const typeClass*,ltstr>::const_iterator it;
      for( it = objectHashByName.begin(); it != objectHashByName.end(); ++it )
	if( stype->checkType( (*it).second ) )
	  oList.push_back( strdup( (*it).first ) );

#if 0
      map<const char*,list<const char*>*,ltstr>::const_iterator it1;
      list<const char*>::const_iterator it2;
      for( it1 = objectHashByType.begin(); it1 != objectHashByType.end(); ++it1 )
	if( !strcmp( (*it1).first, stype->ident ) )
	  for( it2 = (*it1).second->begin(); it2 != (*it1).second->end(); ++it2 )
	    oList.push_back( strdup( (*it2) ) );
#endif
    }
}

int
problemClass::observationSize( void ) const
{
  list<const actionClass*>::const_iterator it;

  int size = 0;
  for( it = domain->actionList.begin(); it != domain->actionList.end(); ++it )
    if( (*it)->observation && ((*it)->observation->size( this ) > size) )
      size = (*it)->observation->size( this );
  return( size );
}

andFormulaClass::~andFormulaClass()
{
  list<const formulaClass*>::const_iterator it;
  //  cout << "destructor for andFormulaClass called" << endl;
  for( it = formulaList->begin(); it != formulaList->end(); ++it )
    delete (*it);
  delete formulaList;
}

orFormulaClass::~orFormulaClass()
{
  list<const formulaClass*>::const_iterator it;
  //  cout << "destructor for orFormulaClass called" << endl;
  for( it = formulaList->begin(); it != formulaList->end(); ++it )
    delete (*it);
  delete formulaList;
}

atomFormulaClass::atomFormulaClass( const char *ident, const termListClass *tList )
{
  map<const char*,const predicateClass*,ltstr>::const_iterator it;

  assert( predicateHash != NULL );
  it = predicateHash->find( ident );
  if( it == predicateHash->end() )
    {
      error( undefinedPredicate, ident );

      // avoid future errors
      predicate = newUniversalPredicate();
      termList = NULL;
      delete tList;
      return;
    }
  else if( !((*it).second->domain->checkType( tList )) ) // checkType ok
    {
      error( incompTypePredicate, ident );

      // avoid future errors
      predicate = newUniversalPredicate();
      termList = NULL;
      delete tList;
      return;
    }
  else
    {
      predicate = (*it).second->clone();
      termList = tList;
    }
  //  fprintf( stderr, "free of [%p]%s\n", ident, ident );
  free( (void*)ident );
}

equalFormulaClass::equalFormulaClass( const termClass *lt, const termClass *rt )
{
  lterm = lt;
  rterm = rt;

  if( !(lt->checkType( rt )) && !(rt->checkType( lt )) ) // checktype ok
    error( incompTypeEquality );
}

inclusionFormulaClass::inclusionFormulaClass( const termClass *t, const list<const termClass*> *tList )
{
  list<const termClass*>::const_iterator it;

  term = t;
  termList = tList;
  for( it = tList->begin(); it != tList->end(); ++it )
    if( !t->checkType( *it ) ) // checktype ok
      break;

  if( it != tList->end() )
    error( incompTypePredicate, ":in" );
}

inclusionFormulaClass::~inclusionFormulaClass()
{
  list<const termClass*>::const_iterator it;
  //  cout << "destructor for inclusionlFormulaClass called" << endl;
  for( it = termList->begin(); it != termList->end(); ++it )
    delete (*it);
  delete termList;
  delete term;
}

existsFormulaClass::existsFormulaClass( const char *var, 
					const simpleTypeClass *type, 
					const formulaClass *form,
					list<map<const char*,const typeClass*,ltstr>*> *env, 
					const char *intern,
					bool iEffect,
					bool iln )
{
  variable = var; 
  varType = type; 
  formula = form; 
  environment = env; 
  internal = intern;
  insideEffect = iEffect;
  inlineFormula = iln;

  if( declaredProblem == NULL )
    {
      id = declaredDomain->existsNumber++; 
      nextChain = declaredDomain->existsChain; 
      declaredDomain->existsChain = this; 
    }
  else
    {
      id = declaredProblem->existsNumber++; 
      nextChain = declaredProblem->existsChain; 
      declaredProblem->existsChain = this; 
    }
}

existsFormulaClass::~existsFormulaClass()
{
  free( (void*)variable ); 
  delete varType; 
  delete formula;
}

forallFormulaClass::forallFormulaClass( const char *var, 
					const simpleTypeClass *type, 
					const formulaClass *form,
					list<map<const char*,const typeClass*,ltstr>*> *env, 
					const char *intern,
					bool iEffect,
					bool iln )
{
  variable = var; 
  varType = type; 
  formula = form; 
  environment = env; 
  internal = intern;
  insideEffect = iEffect;
  inlineFormula = iln;

  if( declaredProblem == NULL )
    {
      id = declaredDomain->forallNumber++; 
      nextChain = declaredDomain->forallChain; 
      declaredDomain->forallChain = this; 
    }
  else
    {
      id = declaredProblem->forallNumber++; 
      nextChain = declaredProblem->forallChain; 
      declaredProblem->forallChain = this; 
    }
}

forallFormulaClass::~forallFormulaClass()
{
  free( (void*)variable ); 
  delete varType; 
  delete formula;
}

sumTermClass::sumTermClass( const char *var, const simpleTypeClass *vtype, const termClass *t,
			    list<map<const char*,const typeClass*,ltstr>*> *env,
			    bool iEffect, bool iln )
{
  insideEffect = iEffect;
  variable = var;
  varType = vtype;
  term = t;
  environment = env;
  inlineTerm = iln;

  if( !term->checkType( theGenericIntegerType ) ) // checktype ok
    error( incompTypeInSum );

  if( declaredProblem == NULL )
    {
      id = declaredDomain->sumNumber++; 
      nextChain = declaredDomain->sumChain; 
      declaredDomain->sumChain = this; 
    }
  else
    {
      id = declaredProblem->sumNumber++; 
      nextChain = declaredProblem->sumChain; 
      declaredProblem->sumChain = this; 
    }
}

sumTermClass::~sumTermClass()
{
  free( (void*)variable );
  delete varType;
  delete term;
}

termListClass::~termListClass()
{
  list<const termClass*>::const_iterator it;
  //  cout << "detructor for termListClass called" << endl;
  for( it = termList->begin(); it != termList->end(); ++it )
    delete (*it);
  delete termList;
}

int
termListClass::checkType( const termClass *term ) const
{
  const termListClass *tlist = dynamic_cast<const termListClass*>( term );
  if( termList->size() == 1 )
    {
      return( termList->front()->checkType( term ) ); // checktype ok
    }
  else if( tlist != NULL )
    {
      if( termList->size() == tlist->termList->size() )
	{
	  list<const termClass*>::const_iterator it1, it2;
	  it1 = termList->begin();
	  it2 = tlist->termList->begin();
	  for( ; (it1 != termList->end()) && (*it1)->checkType(*it2); ++it1, ++it2 ); // checktype ok
	  if( it1 == termList->end() )
	    return( 1 );
	}
      return( 0 );
    }
  else if( dynamic_cast<const universalTermClass*>(term) )
    return( 1 );
  else
    return( 0 );
}

int
termListClass::checkType( const typeListClass *type ) const
{
  list<const termClass*>::const_iterator it1;
  list<const typeClass*>::const_iterator it2;

  if( termList->size() != type->typeList->size() )
    return( 0 );
  else
    {
      it1 = termList->begin();
      it2 = type->typeList->begin();
      for( ; (it1 != termList->end()) && (*it2)->checkType( *it1 ); ++it1, ++it2 ); // checktype ok
      return( it1 == termList->end() );
    }
}

functionTermClass::functionTermClass( const char *ident, const termClass *arg )
{
  map<const char*,const functionClass*,ltstr>::const_iterator it;

  // lookup function
  assert( functionHash != NULL );
  it = functionHash->find( ident );
  if( it == functionHash->end() )
    {
      error( undefinedFunction, ident );

      // avoid future errors
      function = newUniversalFunction();
      argument = NULL;
      delete arg;
    }
  else if( !((*it).second->domain->checkType( arg )) ) // checktype ok
    {
      error( incompTypeFunction, ident );

      // avoid future errors
      function = newUniversalFunction();
      argument = NULL;
      delete arg;
    }
  else
    {
      function = (*it).second->clone();
      argument = arg;
      special = 0;
    }
  //  fprintf( stderr, "free of [%p]%s\n", ident, ident );
  free( (void*)ident );
}

functionTermClass::functionTermClass( const specialFunctionClass *fun, const termClass *arg )
{
  function = fun;
  argument = arg;
  special = 1;

  if( !(fun->domain->checkType( arg )) ) // checktype ok
    error( incompTypeFunction );
}

predicateTermClass::predicateTermClass( const char *ident, const termClass *arg )
{
  map<const char*,const predicateClass*,ltstr>::const_iterator it;

  // lookup function
  assert( predicateHash != NULL );
  it = predicateHash->find( ident );
  if( it == predicateHash->end() )
    {
      error( undefinedPredicate, ident );

      // avoid future errors
      predicate = newUniversalPredicate();
      argument = NULL;
      delete arg;
    }
  else if( !((*it).second->domain->checkType( arg )) ) // checktype ok
    {
      error( incompTypePredicate, ident );

      // avoid future errors
      predicate = newUniversalPredicate();
      argument = NULL;
      delete arg;
    }
  else
    {
      predicate = (*it).second->clone();
      argument = arg;
    }
  //  fprintf( stderr, "free of [%p]%s\n", ident, ident );
  free( (void*)ident );
}

int
predicateTermClass::checkType( const termClass *term ) const
{
  return( theBooleanType->checkType( term ) ); // checktype ok
}

int
predicateTermClass::checkType( const typeClass *type ) const
{ 
  return( theBooleanType->checkType( type ) ); // checktype ok
}

arrayTermClass::arrayTermClass( const char *id, const arrayTypeClass *t, const termClass *subs )
{
  ident = id;
  type = t;
  subscript = subs;
}

conditionalTermClass::conditionalTermClass( const formulaClass *form, 
					    const termClass *t1, const termClass *t2 )
{
  formula = form;
  term1 = t1;
  term2 = t2;
  if( !t1->checkType( t2 ) || !t2->checkType( t1 ) ) // checktype ok
    error( incompTypeInConditional );
}

termCostClass::termCostClass( const termClass *c )
{
  cost = c;
  if( !theGenericIntegerType->checkType( cost ) ) // checktype ok
    error( badCostTerm );
}

predicateClass::predicateClass( const char *id, const typeClass *dom, int dType, int ext )
{
  ident = id;
  domain = dom;
  domainType = dType;
  external = ext;
  internal = 0;

  // number of arguments
  if( (domainType == SIMPLE_DOMAIN) || (domainType == ARRAY_DOMAIN) )
    numArguments = 1;
  else
    numArguments = ((const typeListClass*)domain)->typeList->size();
}

predicateClass::predicateClass( const predicateClass &predicate )
{
  //  cout << "copy constructor for predicateClass called" << endl;
  ident = strdup( predicate.ident );
  domain = predicate.domain->clone();
  domainType = predicate.domainType;
  external = predicate.external;
  internal = predicate.internal;
  numArguments = predicate.numArguments;
}

effectListClass::~effectListClass()
{
  list<const effectClass*>::const_iterator it;
  for( it = effectList->begin(); it != effectList->end(); ++it )
    delete (*it);
  delete effectList;
}

termSetEffectClass::termSetEffectClass( const termClass *lv, const termClass *rv )
{
  lvalue = lv;
  rvalue = rv;

  // check valid lvalue & rvalue, and types in assignment
  if( !lv->validLValue() )
    error( invalidLValue );
  if( !rv->validRValue() )
    error( invalidRValue );
  if( !(lv->checkType( rvalue )) ) // checktype ok
    error( incompTypeAssignment );
}

predicateSetEffectClass::predicateSetEffectClass( const char *ident, const termClass *argument, 
						  const formulaClass *form )
{
  lvalue = new predicateTermClass( ident, argument );
  rvalue1 = form;
  rvalue2 = NULL;
}

predicateSetEffectClass::predicateSetEffectClass( const char *ident, const termClass *argument, 
						  const termClass *term )
{
  lvalue = new predicateTermClass( ident, argument );
  rvalue1 = NULL;
  rvalue2 = term;

  if( !theBooleanType->checkType( term ) ) // checktype ok
    error( incompTypeAssignment );
}

initTermSetEffectClass::initTermSetEffectClass( const termClass *lv, const list<const termClass*> *tList, 
						const formulaClass *ass )
{
  list<const termClass*>::const_iterator it;

  // set defaults
  lvalue = lv;
  termList = tList;
  dimension = termList->size();
  assertion = ass;

  // check valid lvalue & rvalue, and types in assignment
  if( !lv->validLValue() )
    error( invalidLValue );

  for( it = tList->begin(); it != tList->end(); ++it )
    {
      if( !(*it)->validRValue() )
	error( invalidRValue );
      if( !(lv->checkType( *it )) ) // checktype ok
	error( incompTypeAssignment );
    }
}

initTermSetEffectClass::initTermSetEffectClass( const termClass *lv, const char *min, const char *max,
						const formulaClass *ass )
{
  list<const termClass*> *tList;
  char integer[128]; // hope it is enough...

  // set defaults
  lvalue = lv;
  assertion = ass;

  // check valid lvalue, and types
  if( !lv->validLValue() )
    error( invalidLValue );
  if( atoi( min ) > atoi( max ) )
    error( badRangeLimits, min, max );

  tList = new list<const termClass*>;
  tList->push_back( new integerTermClass( strdup( max ) ) );
  if( !lv->checkType( tList->front() ) ) // checktype ok
    error( incompTypeAssignment );

  // expand range to a termList (maybe this can be done better)
  for( int i = atoi( min ); i < atoi( max ); ++i )
    {
      sprintf( integer, "%d", i );
      tList->push_front( new integerTermClass( strdup( integer ) ) );
    }
  termList = tList;
  dimension = termList->size();
  free( (void*)min );
  free( (void*)max );
}

initTermSetEffectClass::~initTermSetEffectClass()
{
  list<const termClass*>::const_iterator it;
  for( it = termList->begin(); it != termList->end(); ++it )
    delete (*it);
  delete termList;
  delete lvalue;
  delete assertion;
}

initPredicateSetEffectClass::initPredicateSetEffectClass( const char *ident, const termClass *argument, 
							  const list<const termClass*> *tList, 
							  const formulaClass *ass )
{
  list<const termClass*>::const_iterator it;

  termList = tList;
  assertion = ass;
  lvalue = new predicateTermClass( ident, argument );
  dimension = termList->size();

  // check types in the term list
  for( it = tList->begin(); it != tList->end(); ++it )
    {
      if( !(*it)->validRValue() )
	error( invalidRValue );
      if( !(theBooleanType->checkType( *it )) ) // checktype ok
	error( incompTypeAssignment );
    }
}

initPredicateSetEffectClass::~initPredicateSetEffectClass()
{
  list<const termClass*>::const_iterator it;
  for( it = termList->begin(); it != termList->end(); ++it )
    delete (*it);
  delete termList;
  delete lvalue;
  delete assertion;
}

oneOfEffectClass::~oneOfEffectClass()
{
  list<const effectClass*>::const_iterator it;
  for( it = effectList->begin(); it != effectList->end(); ++it )
    delete (*it);
  delete effectList;
}

formulaInitEffectClass::~formulaInitEffectClass()
{
  delete formula;
}

probabilisticEffectClass::~probabilisticEffectClass()
{
  list<pair<const char*,const effectClass*>*>::const_iterator it;
  for( it = effectList->begin(); it != effectList->end(); ++it )
    {
      //      fprintf( stderr, "free of [%p]%s\n", (*it)->first, (*it)->first );
      free( (void*)(*it)->first );
      delete (*it)->second;
      delete (*it);
    }
  delete effectList;
}

nonDeterministicEffectClass::~nonDeterministicEffectClass()
{
  list<const effectClass*>::const_iterator it;
  for( it = effectList->begin(); it != effectList->end(); ++it )
    delete (*it);
  delete effectList;
}

observationListClass::~observationListClass()
{
  list<const observationClass*>::const_iterator it;
  for( it = observationList->begin(); it != observationList->end(); ++it )
    delete (*it);
  delete observationList;
}

int
observationListClass::size( const problemClass *problem ) const
{
  int sz = 0;
  list<const observationClass*>::const_iterator it;
  if( observationList )
    for( it = observationList->begin(); it != observationList->end(); ++it )
      sz += (*it)->size( problem );
  return( sz );
}

int
vectorObservationClass::size( const problemClass *problem ) const
{
  list<const char*> objectList;
  problem->getObjectsOfType( varType, objectList );
  return( objectList.size() );
}

probabilisticObservationClass::~probabilisticObservationClass()
{
  list<pair<const char*,const observationListClass*>*>::const_iterator it;
  for( it = observationList->begin(); it != observationList->end(); ++it )
    {
      free( (void*)(*it)->first );
      delete (*it)->second;
      delete (*it);
    }
  delete observationList;
}

int
probabilisticObservationClass::size( const problemClass *problem ) const
{
  int sz = 0;
  list<pair<const char*,const observationListClass*>*>::const_iterator it;
  if( observationList )
    for( it = observationList->begin(); it != observationList->end(); ++it )
      if( (*it)->second->size( problem ) > sz )
	sz = (*it)->second->size( problem );
  return( sz );
}

nonDeterministicObservationClass::~nonDeterministicObservationClass()
{
  list<const observationListClass*>::const_iterator it;
  for( it = observationList->begin(); it != observationList->end(); ++it )
    delete (*it);
  delete observationList;
}

int
nonDeterministicObservationClass::size( const problemClass *problem ) const
{
  int sz = 0;
  list<const observationListClass*>::const_iterator it;
  if( observationList )
    for( it = observationList->begin(); it != observationList->end(); ++it )
      if( (*it)->size( problem ) > sz )
	sz = (*it)->size( problem );
  return( sz );
}

namedTypeClass::namedTypeClass( const namedTypeClass &named )
{
  list<const char*>::const_iterator it;
  list<const char*> *aList;

  //  cout << "copy contructor for namedTypeClass called" << endl;
  ident = strdup( named.ident );
  aList = new list<const char*>;
  for( it = named.ancestorList->begin(); it != named.ancestorList->end(); ++it )
    aList->push_back( strdup( *it ) );
  ancestorList = aList;
}

namedTypeClass::~namedTypeClass()
{
  list<const char*>::const_iterator it;

  //  cout << "destructor for namedTypeClass called" << endl;
  //  fprintf( stderr, "free of [%p]%s\n", ident, ident );
  free( (void*)ident );
  for( it = ancestorList->begin(); it != ancestorList->end(); ++it )
    {
      //      fprintf( stderr, "free of [%p]%s\n", (*it), (*it) );
      free( (void*)(*it) );
    }
  delete ancestorList;
}

ramificationClass::ramificationClass( const char *id, const list<pair<const char*,const typeClass*> > &par )
{
  ident = id;
  parameters.insert( parameters.end(), par.begin(), par.end() );
}

formulaRamificationClass::formulaRamificationClass( const char *id, 
						    const list<pair<const char*,const typeClass*> > &par,
						    const formulaClass *form )
  : ramificationClass( id, par )
{
  formula = form;
}

effectRamificationClass::effectRamificationClass( const char *id, 
						  const list<pair<const char*,const typeClass*> > &par,
						  const effectClass *eff )
  : ramificationClass( id, par )
{
  effect = eff;
}

internalFunctionClass::internalFunctionClass( const char *id,
					      const list<pair<const char*,const typeClass*> > &par, 
					      const termClass* t )
{
  ident = id;
  parameters.insert( parameters.end(), par.begin(), par.end() );
  term = t;
}

internalFunctionClass::~internalFunctionClass()
{
  list<pair<const char*,const typeClass*> >::const_iterator it;

  //  cout << "detructor for internalFunctionClass called" << endl;
  for( it = parameters.begin(); it != parameters.end(); ++it )
    {
      //      fprintf( stderr, "free of [%p]%s\n", (*it).first, (*it).first );
      free( (void*)(*it).first );
      delete (*it).second;
    }
  //  fprintf( stderr, "free of [%p]%s\n", ident, ident );
  free( (void*)ident );
  delete term;
}

internalPredicateClass::internalPredicateClass( const char *id, 
						const list<pair<const char*,const typeClass*> > &par, 
						const formulaClass* form )
{
  ident = id;
  parameters.insert( parameters.end(), par.begin(), par.end() );
  formula = form;
}

internalPredicateClass::~internalPredicateClass()
{
  list<pair<const char*,const typeClass*> >::const_iterator it;

  //  cout << "detructor for internalPredicateClass called" << endl;
  for( it = parameters.begin(); it != parameters.end(); ++it )
    {
      //      fprintf( stderr, "free of [%p]%s\n", (*it).first, (*it).first );
      free( (void*)(*it).first );
      delete (*it).second;
    }
  //  fprintf( stderr, "free of [%p]%s\n", ident, ident );
  free( (void*)ident );
  delete formula;
}

actionClass::actionClass( const char *ident )
{
  actionClass::ident = ident;
  numResultingStates = 1;
  precondition = NULL;
  effect = NULL;
  cost = NULL;
  observation = NULL;
}

actionClass::~actionClass()
{
  list<pair<const char*,const typeClass*> >::const_iterator it1;

  //  cout << "**** destructor for actionClass called" << endl;
  //  fprintf( stderr, "free of [%p]%s\n", ident, ident );
  free( (void*)ident );

  delete precondition;
  delete effect;
  delete cost;
  delete observation;

  for( it1 = parameters.begin(); it1 != parameters.end(); ++it1 )
    {
      //  fprintf( stderr, "free of [%p]%s\n", (*it1).first, (*it1).first );
      free( (void*)(*it1).first );
      delete (*it1).second;
    }
}

arrayClass::arrayClass( const char *id, const char *s, const typeClass *b )
{
  ident = id;
  size = s;
  base = b;
}

arrayClass::arrayClass( const arrayClass& array )
{
  //  cout << "copy constructor for arrayClass called" << endl;
  ident = strdup( array.ident );
  size = strdup( array.size );
  base = array.base->clone();
}

functionClass::functionClass( const char *id, const typeClass *dom, const typeClass *ran, int dType )
{
  ident = id;
  domain = dom;
  range = ran;
  domainType = dType;
  external = 0;
  internal = 0;

  // number of arguments
  if( (domainType == SIMPLE_DOMAIN) || (domainType == ARRAY_DOMAIN) )
    numArguments = 1;
  else
    numArguments = ((const typeListClass*)domain)->typeList->size();
}

functionClass::functionClass( const functionClass &function )
{
  ident = strdup( function.ident );
  domain = function.domain->clone();
  range = function.range->clone();
  domainType = function.domainType;
  external = function.external;
  internal = function.internal;
  numArguments = function.numArguments;
}

specialFunctionClass::specialFunctionClass( const char *ident, const typeClass *domain, 
					    const typeClass *range, const termListClass *extraArg )
  : functionClass( ident, domain, range, functionClass::LIST_DOMAIN )
{
  extraArgument = extraArg;
  external = 1;
}

int
genericIntegerTypeClass::checkType( const termClass *term ) const
{
  return( term->checkType( this ) ); // checktype ok
}

int 
genericIntegerTypeClass::checkType( const typeClass *type ) const
{
  if( dynamic_cast<const integerTypeClass*>(type) ||
      dynamic_cast<const genericIntegerTypeClass*>(type) || 
      dynamic_cast<const universalTypeClass*>(type) )
    return( 1 );
  else
    return( 0 );
}

integerTypeClass::integerTypeClass( const char *mi, const char *ma )
{
  min = mi;
  max = ma;
  minValue = atoi( min );
  maxValue = atoi( max );
  selfValue = 1;
  if( minValue > maxValue )
    error( badRangeLimits, min, max );
}

integerTypeClass::integerTypeClass( const integerTypeClass &integer )
{
  min = strdup( integer.min );
  max = strdup( integer.max );
  minValue = integer.minValue;
  maxValue = integer.maxValue;
  selfValue = integer.selfValue;
}

void
integerTypeClass::getBaseObjects( list<const char*> &objectList ) const
{
  static char value[16];
  for( int i = minValue; i <= maxValue; ++i )
    {
      sprintf( value, "%d", i );
      objectList.push_back( strdup( value ) );
    }
}

int
integerTypeClass::checkType( const termClass *term ) const
{
  return( term->checkType( this ) ); // checktype ok
}

int 
integerTypeClass::checkType( const typeClass *type ) const
{
  const integerTypeClass *itype = dynamic_cast<const integerTypeClass*>( type );
  if( itype != NULL )
    return( checkType( itype ) ); // checktype ok
  else if( dynamic_cast<const genericIntegerTypeClass*>(type) || 
	   dynamic_cast<const universalTypeClass*>(type) )
    return( 1 );
  else
    return( 0 );
}

void
booleanTypeClass::getBaseObjects( list<const char*> &objectList ) const
{
  objectList.push_back( strdup( "true" ) );
  objectList.push_back( strdup( "false" ) );
}

int
booleanTypeClass::checkType( const termClass *term ) const
{
  return( term->checkType( this ) ); // checktype ok
}

int 
booleanTypeClass::checkType( const typeClass *type ) const
{
  if( dynamic_cast<const booleanTypeClass*>(type) || dynamic_cast<const universalTypeClass*>(type) )
    return( 1 );
  else
    return( 0 );
}

simpleTypeClass::simpleTypeClass( const char *id )
{
  map<const char*,const namedTypeClass*,ltstr>::const_iterator it;
  assert( namedTypeHash != NULL );
  it = namedTypeHash->find( id );
  if( it == namedTypeHash->end() )
    error( undefinedType, id );
  ident = id;
  namedType = (*it).second;
}

simpleTypeClass::simpleTypeClass( const simpleTypeClass &simple )
{
  ident = strdup( simple.ident );
  namedType = simple.namedType;
}

int
simpleTypeClass::checkType( const termClass *term ) const
{
  return( term->checkType( this ) ); // checktype ok
}

int 
simpleTypeClass::checkType( const typeClass *type ) const
{
  const simpleTypeClass* stype = dynamic_cast<const simpleTypeClass*>( type );
  if( stype != NULL )
    return( checkType( stype ) ); // checktype ok
  else if( dynamic_cast<const universalTypeClass*>(type) )
    return( 1 );
  else
    return( 0 );
}

int
simpleTypeClass::checkType( const simpleTypeClass *type ) const
{
  list<const char*>::const_iterator it;
  for( it=type->namedType->ancestorList->begin(); it!=type->namedType->ancestorList->end(); ++it )
    if( !strcmp( *it, ident ) )
      return( 1 );
  return( 0 );
}

typeListClass::typeListClass( const list<const typeClass*> *tList )
{
  typeList = tList;
}

typeListClass::typeListClass( const typeListClass &tList )
{
  typeList = new list<const typeClass*>( *tList.typeList );
}

typeListClass::~typeListClass()
{
  //  cout << "detructor for typeListClass called" << endl;
  //xxx  list<const typeClass*>::const_iterator it;
  //xxx  for( it = typeList->begin(); it != typeList->end(); ++it )
  //xxx    delete (*it);
  delete typeList;
}

int
typeListClass::checkType( const termClass *term ) const
{
  return( term->checkType( this ) ); // checktype ok
}

int 
typeListClass::checkType( const typeClass *type ) const
{
  const typeListClass* tlist = dynamic_cast<const typeListClass*>( type );
  if( tlist != NULL )
    return( checkType( tlist ) ); // checktype ok
  else if( typeList->size() == 1 )
    return( tlist->typeList->front()->checkType( type ) ); // checktype ok
  else if( dynamic_cast<const universalTypeClass*>(type) )
    return( 1 );
  else
    return( 0 );
}

int
typeListClass::checkType( const typeListClass *type ) const
{
  list<const typeClass*>::const_iterator it1, it2;

  if( typeList->size() != type->typeList->size() )
    return( 0 );
  else
    {
      it1 = typeList->begin();
      it2 = type->typeList->begin();
      for( ; (it1 != typeList->end()) && (*it1)->checkType( *it2 ); ++it1, ++it2 ); // checktype ok
      return( it1 == typeList->end() );
    }
}

arrayTypeClass::arrayTypeClass( const char *s, const typeClass *b )
{
  size = s;
  base = b;
}

arrayTypeClass::arrayTypeClass( const arrayTypeClass &array )
{
  size = strdup( array.size );
  base = array.base->clone();
}

int
arrayTypeClass::checkType( const termClass *term ) const
{
  return( term->checkType( this ) ); // checktype ok
}

int 
arrayTypeClass::checkType( const typeClass *type ) const
{
  const arrayTypeClass* atype = dynamic_cast<const arrayTypeClass*>( type );
  if( atype != NULL )
    return( checkType( atype ) ); // checktype ok
  else if( dynamic_cast<const universalTypeClass*>(type) )
    return( 1 );
  else
    return( 0 );
}

hookClass::hookClass( int hType, const char *id )
{
  hookType = hType;
  ident = id;
}

functionClass *
newUniversalFunction( void )
{
  return( new functionClass( strdup("theUniversalFunction"), 
			     new universalTypeClass(), 
			     new universalTypeClass(), 
			     functionClass::SIMPLE_DOMAIN ) );
}

predicateClass *
newUniversalPredicate( void )
{
  return( new predicateClass( strdup("theUniversalPredicate"), 
			      new universalTypeClass(), 
			     functionClass::SIMPLE_DOMAIN ) );
}
