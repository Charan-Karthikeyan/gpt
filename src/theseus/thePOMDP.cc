//  Theseus
//  thePOMDP.cc -- Abstract POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>

using namespace std;

#include <theseus/thePOMDP.h>
#include <theseus/theModel.h>
#include <theseus/theBeliefCache.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// POMDP Class
//

registerClass *POMDPClass::registeredElements = NULL;

POMDPClass::POMDPClass()
{
  actionCache = NULL;
  observationCache = NULL;

  beliefHash = NULL;
  theInitialBelief = NULL;
  numActions = -1;
  model = NULL;

  setParameters( 0, 1, 250, 1, 0.0 );
  reset();
}

POMDPClass::~POMDPClass()
{
  delete beliefHash;
  delete actionCache;
  delete observationCache;
}

void
POMDPClass::printHash( ostream& os )
{
  assert( beliefHash != NULL );
  beliefHash->print( os );
}

void 
POMDPClass::statistics( ostream& os )
{
  assert( beliefHash != NULL );
  os << "%pomdp initialBeliefValue " << getTheInitialBelief()->getValue() << endl;
  os << "%pomdp averageBranching " << ((float)branching / (float)expansions) << endl;
  os << "%pomdp actionCacheMissRatio " << (actionCache != NULL ? actionCache->missRatio() : -1.0) << endl;
  os << "%pomdp observationCacheMissRatio "<< (observationCache != NULL ? observationCache->missRatio() : -1.0) << endl;
  beliefHash->statistics( os );
}

void
POMDPClass::setHash( beliefHashClass *pBeliefHash )
{
  delete beliefHash;
  beliefHash = pBeliefHash;
}

beliefHashClass *
POMDPClass::removeHash( void )
{
  beliefHashClass *rv = beliefHash;
  beliefHash = NULL;
  return( rv );
}

void
POMDPClass::setCache( int achsize, int aclsize, int ochsize, int oclsize,
		      beliefClass *(*beliefConstructor)( void ) )
{
  // set belief constructor
  beliefCachedDataClass::setBeliefConstructor( beliefConstructor );
  //xxxxxx where is the destructor?

  // make action cache
  delete actionCache;
  actionCache = new beliefCacheClass( "actionCache",
				      achsize, 
				      aclsize,
				      beliefConstructor,
				      (void *(*)()) &beliefCachedDataClass::constructor,
				      &beliefCachedDataClass::destructor );

  // make observation cache
  delete observationCache;
  observationCache = new beliefCacheClass( "observationCache",
					   ochsize,
					   oclsize,
					   beliefConstructor,
					   (void *(*)()) &beliefCachedDataClass::constructor,
					   &beliefCachedDataClass::destructor );

  // keep cache sizes
  actionCacheSize = achsize;
  actionClusterSize = aclsize;
  observationCacheSize = ochsize;
  observationClusterSize = oclsize;
}

void
POMDPClass::setParameters( int nactions, int cpomdp, int mmoves, bool rties, float dfactor )
{
  numActions = nactions;
  costPOMDP = cpomdp;
  maxMoves = mmoves;
  randomTies = rties;
  discountFactor = dfactor;
}

void
POMDPClass::setTheInitialBelief( const beliefClass *belief )
{
  theInitialBelief = belief;
}

const hashEntryClass*
POMDPClass::getTheInitialBelief( void )
{
  return( NULL );
}

void
POMDPClass::initialize( void )
{
}

void
POMDPClass::finalize( void )
{
}

void
POMDPClass::initRun( int run )
{
}

void
POMDPClass::endRun( bool lastRun )
{
  if( !lastRun )
    {
      cleanHash();
      reset();
      // the action and observation caches don't need to be cleaned
    }
}

const hashEntryClass*
POMDPClass::getHashEntry( const beliefClass *belief )
{
  return( beliefHash->lookup( (const void*)belief ) );
}

POMDPClass::hashValueClass*
POMDPClass::hashValue( const beliefClass* belief )
{
  assert( 0 );
  return( NULL );
}

int
POMDPClass::getBestAction( const hashEntryClass *belief )
{
  assert( 0 );
  return( -1 );
}

void
POMDPClass::expandBeliefWithAction( const hashEntryClass *belief, int action,
				    deque<pair<pair<int,float>,const hashEntryClass*> >&result )
{
  assert( 0 );
}

void
POMDPClass::quantize( bool useCache, bool saveInHash, const beliefClass* belief,
		      hashEntryClass*& quantization )
{
  beliefHash->quantize( useCache, saveInHash, belief, quantization );
}


///////////////////////
//
// serialization

void
POMDPClass::signRegistration( const char *registrationId,
			      void (*fillFunction)( istream&, POMDPClass * ), 
			      POMDPClass *(*constructor)( void ) )
{
  registerClass::insertRegistration( registeredElements,
				     registrationId,
				     (void (*)( istream&, void * )) fillFunction,
				     (void *(*)( void )) constructor );
}

void
POMDPClass::cleanup( void )
{
  registerClass::cleanup( registeredElements );
}

void
POMDPClass::write( ostream& os ) const
{
  safeWrite( &costPOMDP, sizeof( int ), 1, os );
  safeWrite( &maxMoves, sizeof( int ), 1, os );
  safeWrite( &randomTies, sizeof( bool ), 1, os );
  safeWrite( &discountFactor, sizeof( float ), 1, os );
  safeWrite( &numActions, sizeof( int ), 1, os );
  safeWrite( &actionCacheSize, sizeof( int ), 1, os );
  safeWrite( &actionClusterSize, sizeof( int ), 1, os );
  safeWrite( &observationCacheSize, sizeof( int ), 1, os );
  safeWrite( &observationClusterSize, sizeof( int ), 1, os );

  //xxxx don't write the initial belief
  //xxxx theInitialBelief->write( os );

  beliefHash->write( os );
}

POMDPClass *
POMDPClass::read( istream& is )
{
  char *id;
  POMDPClass *POMDP;

  id = registerClass::getRegisteredId( is );
  POMDP = (POMDPClass *) registerClass::readElement( is, id, registeredElements );
  delete id;
  return( POMDP );
}

void
POMDPClass::fill( istream& is, POMDPClass *pomdp )
{
  safeRead( &pomdp->costPOMDP, sizeof( int ), 1, is );
  safeRead( &pomdp->maxMoves, sizeof( int ), 1, is );
  safeRead( &pomdp->randomTies, sizeof( bool ), 1, is );
  safeRead( &pomdp->discountFactor, sizeof( float ), 1, is ); 
  safeRead( &pomdp->numActions, sizeof( int ), 1, is );
  safeRead( &pomdp->actionCacheSize, sizeof( int ), 1, is );
  safeRead( &pomdp->actionClusterSize, sizeof( int ), 1, is );
  safeRead( &pomdp->observationCacheSize, sizeof( int ), 1, is );
  safeRead( &pomdp->observationClusterSize, sizeof( int ), 1, is );

  //xxxx don't read the initial belief
  //xxxx pomdp->theInitialBelief = beliefClass::read( is );

  pomdp->beliefHash = (beliefHashClass *) hashClass::read( is );

  //xxxx this can't be done here since still don't have an initial belief
  //xxxx pomdp->setCache( pomdp->actionCacheSize, pomdp->actionClusterSize,
  //xxxx                  pomdp->observationCacheSize, pomdp->observationClusterSize,
  //xxxx                  pomdp->theInitialBelief->getConstructor() );
}
