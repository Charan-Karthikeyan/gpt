//  Theseus
//  theSimpleBelief.cc -- Simple Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <typeinfo>

using namespace std;

#include <theseus/theSimpleBelief.h>
#include <theseus/theBeliefCache.h>
#include <theseus/theUtils.h>

#define  PACK(a,o)     (((a)<<16)|((o)&0xFFFF))


///////////////////////////////////////////////////////////////////////////////
//
// Simple Belief Class
//

// static members
standardModelClass* simpleBeliefClass::model = NULL;

const map<int,float>*
simpleBeliefClass::cast( void ) const
{
  static map<int,float> m;
  m.clear();
  m[state] = 1.0;
  return( &m );
}

void
simpleBeliefClass::nextPossibleObservations( int action, map<int,float>& result ) const
{
  result.clear();
  assert( model->transitionModel[action].find( state ) != model->transitionModel[action].end() );
  result.insert( model->transitionModel[action][state].begin(), 
		 model->transitionModel[action][state].end() );
}

beliefClass*
simpleBeliefClass::updateByA( beliefCacheClass* cache, bool useCache, int action ) const
{
  unsigned tag;
  simpleBeliefClass *bel_a; 
  beliefCachedDataClass *cachedBel;

  tag = PACK(action,0);
  cachedBel = (beliefCachedDataClass*)cache->freeSlot( this, tag );
  bel_a = (simpleBeliefClass*)cachedBel->belief;
  assert( bel_a != this );

  *bel_a = *this;
  return( bel_a );
}

beliefClass*
simpleBeliefClass::updateByAO( beliefCacheClass* cache, bool useCache, int action, int observation ) const
{
  unsigned tag;
  simpleBeliefClass *bel_ao;
  beliefCachedDataClass *cachedBel;

  tag = PACK(action,observation);
  cachedBel = (beliefCachedDataClass*)cache->freeSlot( this, tag );
  bel_ao = (simpleBeliefClass*)cachedBel->belief;
  assert( bel_ao != this );

  bel_ao->setState( observation );
  return( bel_ao );
}


///////////////////////
//
// serialization

void
simpleBeliefClass::checkIn( void )
{
  signRegistration( typeid( simpleBeliefClass ).name(),
		    (void (*)( istream&, beliefClass * )) &simpleBeliefClass::fill,
		    (beliefClass* (*)( void )) &simpleBeliefClass::constructor );
}

void
simpleBeliefClass::write( ostream& os ) const
{
  const char *id;
  unsigned len;

  // write registrationId
  id = typeid( *this ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  beliefClass::write( os );

  // do it!
  os << state;
}

simpleBeliefClass*
simpleBeliefClass::read( istream& is )
{
  simpleBeliefClass *bel;

  bel = new simpleBeliefClass;
  fill( is, bel );

  return( bel );
}

simpleBeliefClass*
simpleBeliefClass::constructor( void )
{
  return( new simpleBeliefClass() );
}

void
simpleBeliefClass::fill( istream& is, beliefClass* bel )
{
  fill( is, (simpleBeliefClass*)bel );
}

void
simpleBeliefClass::fill( istream& is, simpleBeliefClass* bel )
{
  beliefClass::fill( is, bel );
  is >> bel->state;
}


///////////////////////////////////////////////////////////////////////////////
//
// Simple Belief Hash Class
//

void
simpleBeliefHashClass::checkIn( void )
{
  signRegistration( typeid( simpleBeliefHashClass ).name(),
		    (void (*)( istream&, hashClass* )) &simpleBeliefHashClass::fill,
		    (hashClass* (*)( void )) &simpleBeliefHashClass::constructor );
}

void
simpleBeliefHashClass::write( ostream& os ) const
{
  const char *id;
  unsigned len;

  // write registrationId
  id = typeid( *this ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  beliefHashClass::write( os );
}

simpleBeliefHashClass*
simpleBeliefHashClass::read( istream& is )
{
  simpleBeliefHashClass *result;

  result = new simpleBeliefHashClass;
  fill( is, result );
  return( result );
}

simpleBeliefHashClass*
simpleBeliefHashClass::constructor( void )
{
  return( new simpleBeliefHashClass );
}

void
simpleBeliefHashClass::fill( istream& is, simpleBeliefHashClass* hash )
{
  beliefHashClass::fill( is, hash );
}
