//  Theseus
//  theBeliefCache.h -- Belief Cache Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theBeliefCache_INCLUDE_
#define _theBeliefCache_INCLUDE_

#include <theseus/theBelief.h>


///////////////////////////////////////////////////////////////////////////////
//
// Belief Cache Element Class
//

class beliefCacheElementClass
{
  // members
  unsigned tag;
  unsigned use;
  beliefClass *key;
  void *cachedData;

  // constructors and destructors
  beliefCacheElementClass( void )
    {
      tag = (unsigned)-1;
      use = 0;
      key = NULL;
      cachedData = NULL;
    }
  ~beliefCacheElementClass( void )
    {
    }
  void clean( void ) { use = 0; }

  // friends
  friend class beliefCacheClass;
};


///////////////////////////////////////////////////////////////////////////////
//
// Belief Cache Class
//

class beliefCacheClass
{
  // members
  char* ident;
  int   hits;
  int   lookups;
  int   cacheSize;
  int   clusterSize;
  int   effectiveSize;
  unsigned stamp;
  beliefCacheElementClass *cacheTable;
  beliefClass* (*beliefConstructor)( void );
  void*        (*cachedDataConstructor)( void );
  void         (*cachedDataDestructor)( void * );

  // private methods
  beliefCacheClass( const beliefCacheClass& cache );

public:
  // contructors
  beliefCacheClass( const char *ident, 
		    int cacheSize , int clusterSize,
		    beliefClass *(*beliefConstructor)( void ),
		    void *(*cacheDataConstructor)( void ),
		    void (*cachedDataDestructor)( void * ) );

  // destructor
  virtual ~beliefCacheClass( void );

  // methods
  int   getCacheSize( void ) const { return( cacheSize ); }
  int   getClusterSize( void ) const { return( clusterSize ); }
  float missRatio( void )
    {
      return( (float)(lookups - hits) / (float)lookups );
    }
  void* lookup( const beliefClass *belief, unsigned tag );
  void* freeSlot( const beliefClass *belief, unsigned tag );
  void  clean( void );
};


///////////////////////////////////////////////////////////////////////////////
//
// Belief Cached Data
//

class beliefCachedDataClass
{
public:
  // members
  beliefClass *belief;

  // static members
  static beliefClass* (*beliefConstructor)( void );

  // constructors
  beliefCachedDataClass()
    {
      belief = (*beliefConstructor)();
    }

  // destructors
  ~beliefCachedDataClass()
    {
      delete belief;
    }

  // static methods
  static void                   setBeliefConstructor( beliefClass *(*ctor)( void ) )
    {
      beliefConstructor = ctor;
    }
  static beliefCachedDataClass* constructor( void )
    {
      return( new beliefCachedDataClass );
    }
  static void                   destructor( void *cachedData )
    {
      delete (beliefCachedDataClass*)cachedData;
    }
};


#endif // _theBeliefCache_INCLUDE
