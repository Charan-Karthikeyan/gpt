//  Theseus
//  theSetBelief.cc -- Set Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <typeinfo>

using namespace std;

#include <theseus/theSetBelief.h>
#include <theseus/theBeliefCache.h>
#include <theseus/theHeuristic.h>
#include <theseus/theUtils.h>
#include <theseus/md4.h>

#define  PACK(a,o)     (((a)<<16)|((o)&0xFFFF))


///////////////////////////////////////////////////////////////////////////////
//
// Set Belief Class
//

// static members
nonDetModelClass* setBeliefClass::model = NULL;

const map<int,float>*
setBeliefClass::cast( void ) const
{
  set<int>::const_iterator it;
  static map<int,float> result;
  result.erase( result.begin(), result.end() );
  for( it = bel.begin(); it != bel.end(); ++it )
    result.insert( make_pair( *it, (float)(1.0/bel.size()) ) );
  return( &result );
}

void
setBeliefClass::nextPossibleObservations( int action, map<int,float> &result ) const
{
  static set<int>::const_iterator it1;
  static map<int,float>::const_iterator it2;
  result.clear();
  for( it1 = bel.begin(); it1 != bel.end(); ++it1 )
    {
      assert( model->observationModel[action].find(*it1)!=model->observationModel[action].end() );
      it2 = model->observationModel[action][*it1].begin();
      for( ; it2 != model->observationModel[action][*it1].end(); ++it2 )
	result[(*it2).first] = 1.0;
    }
}

int
setBeliefClass::plausibleState( int action, int observation ) const
{
  static set<int>::const_iterator it1;
  static map<int,float>::const_iterator it2;
  static set<int> plausibleStates;

  plausibleStates.clear();
  for( it1 = bel.begin(); it1 != bel.end(); ++it1 )
    {
      assert( model->observationModel[action].find(*it1)!=model->observationModel[action].end() );
      it2 = model->observationModel[action][*it1].begin();
      for( ; it2 != model->observationModel[action][*it1].end(); ++it2 )
	if( ((*it2).first == observation) && ((*it2).second > 0.0) )
	  plausibleStates.insert( *it1 );
    }
  assert( !plausibleStates.empty() );
  return( ::randomSampling( plausibleStates ) );
}

beliefClass*
setBeliefClass::updateByA( beliefCacheClass *cache, bool useCache, int action ) const
{
  unsigned tag;
  setBeliefClass* bel_a; 
  beliefCachedDataClass* cachedBel;
  set<int>::const_iterator it1;
  map<int,float>::const_iterator it2;

  // cache lookup
  tag = PACK(action,0);
  if( useCache && ((cachedBel = (beliefCachedDataClass*)cache->lookup( this, tag )) != NULL ) )
    {
      return( cachedBel->belief );
    }
  else
    {
      if( cache )
	{
	  // we need to evict someone from the cache belief
	  cachedBel = (beliefCachedDataClass*)cache->freeSlot( this, tag );
	  bel_a = (setBeliefClass*)cachedBel->belief;
	}
      else
	{
	  bel_a = (setBeliefClass*)clone();
	}
    }
  assert( bel_a != this );

  // calculation only for those bel_a(s) different from zero (see docs)
  bel_a->bel.clear();
  for( it1 = bel.begin(); it1 != bel.end(); ++it1 )
    {
      assert( (model->transitionModel[action].find(*it1)!=model->transitionModel[action].end()) && 
      	      !model->transitionModel[action][*it1].empty() );
      it2 = model->transitionModel[action][*it1].begin();
      for( ; it2 != model->transitionModel[action][*it1].end(); ++it2 )
	if( !bel_a->check( (*it2).first ) )
	  bel_a->bel.insert( (*it2).first );
    }
  return( bel_a );
}

beliefClass*
setBeliefClass::updateByAO( beliefCacheClass *cache, bool useCache, int action, int observation ) const
{
  unsigned tag;
  setBeliefClass *bel_ao;
  beliefCachedDataClass *cachedBel;
  static set<int>::const_iterator it1;
  static map<int,float>::const_iterator it2;

  // cache lookup
  tag = PACK(action,observation);
  if( useCache && ((cachedBel = (beliefCachedDataClass*)cache->lookup( this, tag )) != NULL ) )
    {
      return( cachedBel->belief );
    }
  else
    {
      if( cache )
	{
	  // we need to evict someone from the cache belief and insert
	  // a new belief with id = baliefClass::beliefId
	  cachedBel = (beliefCachedDataClass*)cache->freeSlot( this, tag );
	  bel_ao = (setBeliefClass*)cachedBel->belief;
	}
      else
	{
	  bel_ao = (setBeliefClass*)clone();
	}
    }
  assert( bel_ao != this );

  // calculation only for those bel_a(s) different from zero (see docs)
  bel_ao->bel.clear();
  for( it1 = bel.begin(); it1 != bel.end(); ++it1 )
    {
      assert( model->observationModel[action].find(*it1)!=model->observationModel[action].end() );
      it2 = model->observationModel[action][*it1].begin();
      for( ; it2 != model->observationModel[action][*it1].end(); ++it2 )
	if( ((*it2).first == observation) && !bel_ao->check(*it1) )
	  bel_ao->bel.insert( *it1 );
    }

  // return
  return( bel_ao );
}

unsigned
setBeliefClass::hashDataFunction( void ) const
{
  register unsigned *ptr, result;
  unsigned char digest[16];

  static MD4_CTX context;
  static unsigned buffSize = 0;
  static unsigned *buff = NULL;
  static set<int>::const_iterator it;

  // space allocation (if needed)
  if( bel.size() > buffSize )
    {
      for( ; bel.size() > buffSize; buffSize = (buffSize == 0 ? 64 : 2 * buffSize) );
      if( buff != NULL )
	delete[] buff;
      buff = new unsigned[buffSize];
    }

  // copy belief into buffer
  ptr = buff;
  for( it = bel.begin(); it != bel.end(); ++it )
    {
      // this assumes that sizeof( float ) = sizeof( int )
      *ptr++ = (unsigned)*it;
    }

  // compute MD4 digests
  MD4Init( &context );
  MD4Update( &context, (unsigned char*)buff, 4*(ptr - buff) );
  MD4Final( digest, &context );

  // compact digest into an unsigned and return it (this assumes that sizeof( unsigned ) = 4)
  ptr = (unsigned*)digest;
  result = (ptr[0] ^ ptr[1] ^ ptr[2] ^ ptr[3]);
  return( result );
}


///////////////////////
//
// serialization

void
setBeliefClass::checkIn( void )
{
  signRegistration( typeid( setBeliefClass ).name(),
		    (void (*)( istream&, beliefClass * )) &setBeliefClass::fill,
		    (beliefClass *(*)( void )) &setBeliefClass::constructor );
}

void
setBeliefClass::write( ostream& os ) const
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
  os << bel;
}

setBeliefClass *
setBeliefClass::read( istream& is )
{
  setBeliefClass *bel;

  bel = new setBeliefClass;
  fill( is, bel );

  return( bel );
}

setBeliefClass *
setBeliefClass::constructor( void )
{
  return( new setBeliefClass() );
}

void
setBeliefClass::fill( istream& is, beliefClass *bel )
{
  fill( is, (setBeliefClass*)bel );
}

void
setBeliefClass::fill( istream& is, setBeliefClass *bel )
{
  beliefClass::fill( is, bel );
  is >> bel->bel;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set Belief Hash Class
//

void
setBeliefHashClass::quantize( bool useCache, bool saveInHash, const beliefClass* belief,
				 hashEntryClass*& quantization )
{
  hashEntryClass *entry;
  static hashEntryClass localQuantization;

  if( (entry = lookup( (void*)belief )) == NULL )
    {
      if( saveInHash )
	{
	  entry = insert( (void*)belief, belief->heuristicValue(), (void*)false );
	  quantization = entry;
	}
      else
	{
	  localQuantization.setValue( belief->heuristicValue() );
	  quantization = &localQuantization;
	}
    }
  else
    quantization = entry;
}


///////////////////////
//
// serialization

void
setBeliefHashClass::checkIn( void )
{
  signRegistration( typeid( setBeliefHashClass ).name(),
		    (void (*)( istream&, hashClass * )) &setBeliefHashClass::fill,
		    (hashClass *(*)( void )) &setBeliefHashClass::constructor );
}

void
setBeliefHashClass::write( ostream& os ) const
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

setBeliefHashClass *
setBeliefHashClass::read( istream& is )
{
  setBeliefHashClass *result;

  result = new setBeliefHashClass;
  fill( is, result );
  return( result );
}

setBeliefHashClass *
setBeliefHashClass::constructor( void )
{
  return( new setBeliefHashClass );
}

void
setBeliefHashClass::fill( istream& is, setBeliefHashClass *hash )
{
  beliefHashClass::fill( is, hash );
}
