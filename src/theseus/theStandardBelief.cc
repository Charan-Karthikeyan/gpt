//  Theseus
//  theStandardBelief.cc -- Standard Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <math.h>
#include <iostream>
#include <typeinfo>

using namespace std;

#include <theseus/theStandardBelief.h>
#include <theseus/theTopLevel.h>
#include <theseus/theBeliefCache.h>
#include <theseus/theHeuristic.h>
#include <theseus/md4.h>

#define  PACK(a,o)     (((a)<<16)|((o)&0xFFFF))


///////////////////////////////////////////////////////////////////////////////
//
// Standard Belief Class
//

// static members
standardModelClass* standardBeliefClass::model = NULL;

void
standardBeliefClass::print( ostream& os ) const
{
  os << "[ ";
  for( map<int,float>::const_iterator it = bel.begin(); it != bel.end(); ++it )
    os << (*it).first << ":" << (*it).second << " ";
  os << "]";
}

// Check the consistency of a belief state. Probabilities
// must add to 1, and the registered number of nozero entries
// must be ok.
bool
standardBeliefClass::check() const
{
  register float sum = 0.0;
  for( map<int,float>::const_iterator it = bel.begin(); it != bel.end(); ++it )
    sum += (*it).second;
  return( fabs((double)sum -(double)1.0 ) < PD.epsilon );
}

void
standardBeliefClass::checkModelAvailability( modelClass* m ) const
{
  static map<int,float>::const_iterator it;
  for( it = bel.begin(); it != bel.end(); ++it )
    m->checkModelFor((*it).first);
}

void
standardBeliefClass::nextPossibleObservations( int action, map<int,float>& result ) const
{
  static map<int,map<int,float> >::const_iterator handle;
  static map<int,float>::const_iterator it1, it2;

  result.clear();
  for( it1 = bel.begin(); it1 != bel.end(); ++it1 ) {
    assert( model->observationModel[action].find((*it1).first)!=model->observationModel[action].end() );
    it2 = model->observationModel[action][(*it1).first].begin();
    for( ; it2 != model->observationModel[action][(*it1).first].end(); ++it2 ) {
      if( result.find( (*it2).first ) == result.end() )
        result[(*it2).first] = (*it1).second*(*it2).second;
      else
        result[(*it2).first] += (*it1).second*(*it2).second;
    }
  }
}

int
standardBeliefClass::plausibleState( int action, int observation ) const
{
  static map<int,float>::const_iterator it1, it2;
  static set<int> plausibleStates;

  plausibleStates.clear();
  for( it1 = bel.begin(); it1 != bel.end(); ++it1 ) {
    assert( model->observationModel[action].find((*it1).first)!=model->observationModel[action].end() );
    it2 = model->observationModel[action][(*it1).first].begin();
    for( ; it2 != model->observationModel[action][(*it1).first].end(); ++it2 )
      if( ((*it2).first == observation) && ((*it2).second > 0.0) )
        plausibleStates.insert((*it1).first);
  }
  assert( !plausibleStates.empty() );
  return( ::randomSampling(plausibleStates) );
}

beliefClass*
standardBeliefClass::updateByA( beliefCacheClass* cache, bool useCache, int action ) const
{
  unsigned tag;
  register float norm, pMass;
  standardBeliefClass *bel_a; 
  beliefCachedDataClass *cachedBel;
  static map<int,map<int,float> >::const_iterator handle;
  static map<int,float>::const_iterator it1;
  static map<int,float>::const_iterator it2;
  static map<int,float>::iterator it3;

  // cache lookup
  tag = PACK(action,0);
  if( useCache && ((cachedBel = (beliefCachedDataClass*)cache->lookup(this,tag)) != NULL ) ) {
    return(cachedBel->belief);
  }
  else {
    if( cache ) {
      // we need to evict someone from the cache belief and insert
      // a new belief with id = baliefClass::beliefId
      cachedBel = (beliefCachedDataClass*)cache->freeSlot( this, tag );
      bel_a = (standardBeliefClass*)cachedBel->belief;
    }
    else {
      bel_a = (standardBeliefClass*)clone();
    }
  }
  assert( bel_a != this );

  // calculation only for those bel_a(s) different from zero (see docs)
  norm = 0.0;
  bel_a->bel.clear();
  for( it1 = bel.begin(); it1 != bel.end(); ++it1 ) {
    assert( model->transitionModel[action].find((*it1).first)!=model->transitionModel[action].end() );
    assert( !model->transitionModel[action][(*it1).first].empty() );
    handle = model->transitionModel[action].find( (*it1).first );
    for( it2 = (*handle).second.begin(); it2 != (*handle).second.end(); ++it2 ) {
      pMass = (*it1).second * (*it2).second;
      bel_a->bel[(*it2).first] += pMass;
      /*
      ** if( (it3 = workingBel.find( (*it2).first )) == workingBel.end() )
      **   workingBel.insert( make_pair( (*it2).first, pMass ) );
      ** else
      **  (*it3).second += pMass;
      */
      norm += pMass;
    }
  }

  // normalization
  for( it3 = bel_a->bel.begin(); it3 != bel_a->bel.end(); ++it3 )
    (*it3).second /= norm;
  assert( bel_a->check() );

  return( bel_a );
}

beliefClass*
standardBeliefClass::updateByAO( beliefCacheClass* cache, bool useCache, int action, int observation ) const
{
  unsigned tag;
  register int hit;
  register float norm, pMass;
  standardBeliefClass *bel_ao;
  beliefCachedDataClass *cachedBel;
  static map<int,map<int,float> >::const_iterator handle;
  static map<int,float>::const_iterator it1, it2;
  static map<int,float>::iterator it3;

  // cache lookup
  tag = PACK(action,observation);
  if( useCache && ((cachedBel = (beliefCachedDataClass*)cache->lookup(this,tag)) != NULL ) ) {
    return( cachedBel->belief );
  }
  else {
    if( cache ) {
      // we need to evict someone from the cache belief and insert
      // a new belief with id = baliefClass::beliefId
      cachedBel = (beliefCachedDataClass*)cache->freeSlot( this, tag );
      bel_ao = (standardBeliefClass*)cachedBel->belief;
    }
    else {
      bel_ao = (standardBeliefClass*)clone();
    }
  }
  assert( bel_ao != this );

  // calculation only for those bel_a(s) different from zero (see docs)
  hit = 0;
  norm = 0.0;
  bel_ao->bel.clear();
  for( it1 = bel.begin(); it1 != bel.end(); ++it1 ) {
    assert( model->observationModel[action].find((*it1).first)!=model->observationModel[action].end() );
    handle = model->observationModel[action].find( (*it1).first );
    for( it2 = (*handle).second.begin(); it2 != (*handle).second.end(); ++it2 )
      if( (*it2).first == observation ) {
        ++hit;
        pMass = (*it1).second * (*it2).second;
        bel_ao->bel[(*it1).first] = pMass;
        norm += pMass;
        break;
      }
  }
  assert( hit != 0 );

  // normalization
  for( it3 = bel_ao->bel.begin(); it3 != bel_ao->bel.end(); ++it3 )
    (*it3).second /= norm;
  assert( bel_ao->check() );

  // return
  return( bel_ao );
}

void
standardBeliefClass::support( set<int> &result ) const
{
  map<int,float>::const_iterator it;
  result.clear();
  for( it = bel.begin(); it != bel.end(); ++it )
    result.insert( (*it).first );
}

// Get rid off zero entries
bool
standardBeliefClass::compact()
{
  return( false );
}

// After computing a new belief state, we normalize it in
// order to get rid off rounding errors.
void
standardBeliefClass::normalize()
{
  register float sum;
  static map<int,float>::iterator it;

  do {
    sum = 0.0;
    for( it = bel.begin(); it != bel.end(); ++it )
      sum += (*it).second;
    
    for( it = bel.begin(); it != bel.end(); ++it )
      (*it).second /= sum;
  } while( compact() );
}

// build a belief from a float array
void
standardBeliefClass::fill( const float* array, int dimension )
{
  register int i;

  clean();
  for( i = 0; i < dimension; ++i )
    if( array[i] > 0.0 )
      bel[i] = array[i];
}

bool
standardBeliefClass::operator==( const standardBeliefClass& b ) const
{
  static map<int,float>::const_iterator it1, it2;

  if( bel.size() == b.bel.size() ) {
    for( it1 = bel.begin(), it2 = b.bel.begin(); it1 != bel.end(); ++it1, ++it2 )
    if( ((*it1).first != (*it2).first) || (fabs( (*it1).second - (*it2).second ) > PD.epsilon) )
      return( false );
    return( true );
  }
  else
    return( false );
}

unsigned
standardBeliefClass::hashDataFunction() const
{
  register unsigned *ptr, result;
  unsigned char digest[16];
  MD4_CTX context;

  static unsigned buffSize = 0;
  static unsigned *buff = NULL;
  static map<int,float>::const_iterator it;

  // space allocation (if needed)
  if( 2 * bel.size() > buffSize ) {
    for( ; 2 * bel.size() > buffSize; buffSize = (buffSize == 0 ? 64 : 2 * buffSize) );
    delete[] buff;
    buff = new unsigned[buffSize];
  }

  // copy belief into buffer
  ptr = buff;
  for( it = bel.begin(); it != bel.end(); ++it ) {
    *ptr++ = (unsigned)(*it).first;
    *ptr++ = (unsigned)(100.0 * (*it).second);
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
standardBeliefClass::checkIn()
{
  signRegistration( typeid( standardBeliefClass ).name(),
		    (void (*)( istream&, beliefClass * )) &standardBeliefClass::fill,
		    (beliefClass* (*)()) &standardBeliefClass::constructor );
}

void
standardBeliefClass::write( ostream& os ) const
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

standardBeliefClass*
standardBeliefClass::read( istream& is )
{
  standardBeliefClass *bel;

  bel = new standardBeliefClass;
  fill( is, bel );

  return( bel );
}

standardBeliefClass*
standardBeliefClass::constructor()
{
  return( new standardBeliefClass() );
}



void
standardBeliefClass::fill( istream& is, beliefClass* bel )
{
  fill( is, (standardBeliefClass*)bel );
}

void
standardBeliefClass::fill( istream& is, standardBeliefClass* bel )
{
  beliefClass::fill( is, bel );
  is >> bel->bel;
}


///////////////////////////////////////////////////////////////////////////////
//
// Standard Belief Hash Class
//

void
standardBeliefHashClass::quantize( bool useCache, bool saveInHash, const beliefClass* belief,
				   hashEntryClass*& quantizationResult )
{
  hashEntryClass *entry;

  if( quantization->levels > 0.0 ) {
    quantization->quantize( this,
                            useCache,
                            saveInHash,
                            (const standardBeliefClass*)belief,
                            quantizationResult );
  }
  else {
    if( (entry = lookup( (void*)belief )) == NULL )
      entry = insert( (void*)belief, belief->heuristicValue() );
    quantizationResult = entry;
  }
}

void
standardBeliefHashClass::clean()
{
  beliefHashClass::clean();
  quantization->clean();
}

void
standardBeliefHashClass::statistics( ostream& os )
{
  os << "%pomdp quantizationCacheMissRatio ";
  os << (quantization->cache != NULL ? quantization->cache->missRatio() : -1.0) << endl;
  hashClass::statistics( os );
}


///////////////////////
//
// serialization

void
standardBeliefHashClass::checkIn()
{
  signRegistration( typeid( standardBeliefHashClass ).name(),
		    (void (*)( istream&, hashClass* )) &standardBeliefHashClass::fill,
		    (hashClass* (*)()) &standardBeliefHashClass::constructor );
}

void
standardBeliefHashClass::write( ostream& os ) const
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

standardBeliefHashClass*
standardBeliefHashClass::read( istream& is )
{
  standardBeliefHashClass *result;

  result = new standardBeliefHashClass;
  fill( is, result );
  return( result );
}

standardBeliefHashClass*
standardBeliefHashClass::constructor()
{
  return( new standardBeliefHashClass );
}

void
standardBeliefHashClass::fill( istream& is, standardBeliefHashClass* hash )
{
  beliefHashClass::fill( is, hash );
}
