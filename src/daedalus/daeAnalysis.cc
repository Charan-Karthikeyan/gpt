/*
**  Daedalus
**  daeAnalysis.cc -- Domain Analysis
**
**  Blai Bonet, Hector Geffner
**  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002
**
*/

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


extern map<const char*,const predicateClass*,ltstr> globalPredicateHash;
extern map<const char*,const functionClass*,ltstr> globalFunctionHash;


bool
predicateClass::staticPredicate( const problemClass *problem ) const
{
  list<const actionClass*>::const_iterator it;
  list<const ramificationClass*>::const_iterator it2;

  // check that it is not modified by any action
  for( it = problem->domain->actionList.begin(); it != problem->domain->actionList.end(); ++it )
    if( (*it)->dirtyPredicates.find( ident ) != (*it)->dirtyPredicates.end() )
      return( false );

  // check that it is not modified by any action
  for( it2 = problem->domain->ramificationList.begin(); it2 != problem->domain->ramificationList.end(); ++it2 )
    if( (*it2)->dirtyPredicates.find( ident ) != (*it2)->dirtyPredicates.end() )
      return( false );

  // check that the value is uniquely set in the initial situation
  if( problem->dirtyPredicates.find( ident ) != problem->dirtyPredicates.end() )
    return( false );

  return( true );
}

bool
functionClass::staticFunction( const problemClass *problem ) const
{
  list<const actionClass*>::const_iterator it;
  list<const ramificationClass*>::const_iterator it2;

  // check that it is not modified by any action
  for( it = problem->domain->actionList.begin(); it != problem->domain->actionList.end(); ++it )
    if( (*it)->dirtyFunctions.find( ident ) != (*it)->dirtyFunctions.end() )
      return( false );

  // check that it is not modified by any action
  for( it2 = problem->domain->ramificationList.begin(); it2 != problem->domain->ramificationList.end(); ++it2 )
    if( (*it2)->dirtyFunctions.find( ident ) != (*it2)->dirtyFunctions.end() )
      return( false );

  // check that the value is uniquely set in the initial situation
  if( problem->dirtyFunctions.find( ident ) != problem->dirtyFunctions.end() )
    return( false );

  return( true );
}

void
predicateStaticPartition( problemClass *problem )
{
  int n[domainClass::MAX_ARITY];
  map<const char*,const predicateClass*,ltstr>::const_iterator it;

  memset( n, 0, domainClass::MAX_ARITY * sizeof( int ) );
  for( it = problem->domain->predicateHash.begin(); it != problem->domain->predicateHash.end(); ++it )
    if( !(*it).second->external && !(*it).second->internal &&
	(globalPredicateHash.find( (*it).first ) == globalPredicateHash.end()) )
      n[(*it).second->numArguments] += (int)(*it).second->staticPredicate( problem );

  for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
    if( problem->domain->numPredicates[arity] > 0 )
      {
	problem->numPredicates[arity] = problem->domain->numPredicates[arity] - n[arity];
	problem->numStaticPredicates[arity] = n[arity];
      }
}

void
functionStaticPartition( problemClass *problem )
{
  int n[domainClass::MAX_ARITY];
  map<const char*,const functionClass*,ltstr>::const_iterator it;

  memset( n, 0, domainClass::MAX_ARITY * sizeof( int ) );
  for( it = problem->domain->functionHash.begin(); it != problem->domain->functionHash.end(); ++it )
    if( !(*it).second->external && !(*it).second->internal &&
	(globalFunctionHash.find( (*it).first ) == globalFunctionHash.end()) )
      n[(*it).second->numArguments] += (int)(*it).second->staticFunction( problem );

  for( int arity = 0; arity < domainClass::MAX_ARITY; ++arity )
    if( problem->domain->numFunctions[arity] > 0 )
      {
	problem->numFunctions[arity] = problem->domain->numFunctions[arity] - n[arity];
	problem->numStaticFunctions[arity] = n[arity];
      }
}
