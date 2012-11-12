//  Theseus
//  theBeliefCache.cc -- Belief Cache Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace std;

#include <theseus/theBeliefCache.h>


///////////////////////////////////////////////////////////////////////////////
//
// Belief Cache Class
//

beliefCacheClass::beliefCacheClass( const char *pIdent, 
				    int pCacheSize, 
				    int pClusterSize,
				    beliefClass *(*pBeliefConstructor)( void ),
				    void *(*pCachedDataConstructor)( void ),
				    void (*pCachedDataDestructor)( void * ) )
{
  assert( (pCacheSize == 0) || (pClusterSize > 0) );
  assert( (pCacheSize == 0) || (pCacheSize % pClusterSize == 0) );

  ident = new char[strlen( pIdent ) + 1];
  strcpy( ident, pIdent );
  if( pCacheSize == 0 )
    {
      cacheSize = 1;
      clusterSize = 1;
    }
  else
    {
      cacheSize = pCacheSize;
      clusterSize = pClusterSize;
    }
  beliefConstructor = pBeliefConstructor;
  cachedDataConstructor = pCachedDataConstructor;
  cachedDataDestructor = pCachedDataDestructor;

  cacheTable = new beliefCacheElementClass[cacheSize];
  for( int i = 0; i < cacheSize; ++i )
    {
      cacheTable[i].key = (beliefClass*)(beliefConstructor!=NULL?(*beliefConstructor)():NULL);
      cacheTable[i].cachedData = (*cachedDataConstructor)();
    }

  effectiveSize = cacheSize / clusterSize;
  hits = 0;
  lookups = 0;
  stamp = 0;
}

beliefCacheClass::beliefCacheClass( const beliefCacheClass& cache )
{
  cerr << "Warning: beliefCacheClass::beliefCacheClass() called" << endl;
}

beliefCacheClass::~beliefCacheClass( void )
{
  delete[] ident;
  for( int i = 0; i < cacheSize; ++i )
    {
      delete cacheTable[i].key;
      (*cachedDataDestructor)( cacheTable[i].cachedData );
    }
  delete[] cacheTable;
}

void *
beliefCacheClass::lookup( const beliefClass *belief, unsigned tag )
{
  register int i;
  register beliefCacheElementClass* cacheEntry;

  // cache lookup
  ++lookups;
  cacheEntry = &cacheTable[(belief->hashDataFunction() % effectiveSize) * clusterSize];
  for( i = 0; i < clusterSize; ++i )
    if( (cacheEntry[i].use > 0) && (cacheEntry[i].tag == tag) &&
	(*(cacheEntry[i].key) == *belief) )
      {
	++hits;
	cacheEntry[i].use = stamp++;
	return( cacheEntry[i].cachedData );
      }
  return( NULL );
}

void *
beliefCacheClass::freeSlot( const beliefClass *belief, unsigned tag )
{
  register int i, minIndex;
  register unsigned minStamp;
  register beliefCacheElementClass *cacheEntry;

  // cache lookup
  minIndex = -1;
  minStamp = 0; // to remove warning
  cacheEntry = &cacheTable[(belief->hashDataFunction() % effectiveSize) * clusterSize];
  for( i = 0; i < clusterSize; ++i )
    if( (minIndex == -1) || (cacheEntry[i].use < minStamp) )
      {
	minIndex = i;
	minStamp = cacheEntry[i].use;
      }

  // setup cache entry
  cacheEntry[minIndex].use = stamp++;
  *cacheEntry[minIndex].key = *belief;
  cacheEntry[minIndex].tag = tag;
  return( cacheEntry[minIndex].cachedData );
}

void
beliefCacheClass::clean( void )
{
  register int i;

  // clean statistics
  hits = 0;
  lookups = 0;

  // clean entries
  for( i = 0; i < cacheSize; ++i )
    cacheTable[i].use = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Belief Cached Data Class
//

beliefClass *(*beliefCachedDataClass::beliefConstructor)( void );

