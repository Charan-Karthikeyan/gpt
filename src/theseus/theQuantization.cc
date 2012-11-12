//  Theseus
//  theQuantization.cc -- Belief Quantization
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <map>
#include <math.h>

using namespace std;

#include <theseus/theQuantization.h>
#include <theseus/theTopLevel.h>
#include <theseus/theStandardBelief.h>
#include <theseus/theBeliefCache.h>

///////////////////////////////////////////////////////////////////////////////
//
// Quatization Cached Data Class
//

class quantizationCachedDataClass {
public:
  hashEntryClass* quantization;
  quantizationCachedDataClass() : quantization(0) { }
  virtual ~quantizationCachedDataClass() { }

  // static methods
  static quantizationCachedDataClass* constructor() { return(new quantizationCachedDataClass); }
  static void destructor( void *cachedData ) { delete (quantizationCachedDataClass*)cachedData; }
};


///////////////////////////////////////////////////////////////////////////////
//
// Quantization Class
//

// structure
struct pair_s {
  int   idx;
  float val;
};

// static data
static float*                x = 0;
static standardBeliefClass*  result = 0;

// static members
int                          quantizationClass::dimensions = 0;
float                        quantizationClass::levels = 0.0;
beliefCacheClass*            quantizationClass::cache = 0;

void
quantizationClass::initialize( int pLevels, int pDimensions )
{
  // set parameters
  levels = (float)pLevels;
  dimensions = pDimensions;

  // allocate working space
  x = new float[dimensions];
  result = new standardBeliefClass;
}

void
quantizationClass::setCache( int cacheSize, int clusterSize, beliefClass* (*beliefConstructor)() )
{
  delete cache;
  cache = new beliefCacheClass( "quantizationCache", 
				cacheSize,
				clusterSize, 
				beliefConstructor,
				(void *(*)()) &quantizationCachedDataClass::constructor,
				&quantizationCachedDataClass::destructor );
}

void
quantizationClass::finalize()
{
  delete[] x;
  delete result;
  delete cache;
  x = NULL;
  result = 0;
  cache = 0;
}

void
quantizationClass::clean()
{
  if( cache ) cache->clean();
}

void
quantizationClass::quantize( beliefHashClass* beliefHash,
			     bool useCache,
			     bool saveInHash,
			     const standardBeliefClass* belief, 
			     hashEntryClass* &quantization )
{
  static hashEntryClass localQuantization;

  // cache lookup
  quantizationCachedDataClass* cachedBel = 0;
  if( useCache && ((cachedBel = (quantizationCachedDataClass*)cache->lookup(belief,0)) != 0) ) { // return data. O(1).
    quantization = cachedBel->quantization;
    return;
  }
  else if( saveInHash ) { // evict someone from cache
    cachedBel = (quantizationCachedDataClass*)cache->freeSlot(belief,0);
  }

  if( (PD.version == problemClass::VERSION_GRID) || (PD.version == problemClass::VERSION_ORIGINAL) ) {
    // compute un-normalized belief
    register float sum = 0.0;
    const map<int,float>* m = belief->cast();
    for( std::map<int,float>::const_iterator it = m->begin(); it != m->end(); ++it )
      sum += (float)floor(1.5+(double)(levels*(*it).second));

    // normalize 
    result->clean();
    for( std::map<int,float>::const_iterator it = m->begin(); it != m->end(); ++it )
      result->insert((*it).first,(float)(floor(1.5+(double)(levels*(*it).second))/sum));
    assert( result->check() );
  }
  //std::cout << "quantization:" << std::endl << "  " << *belief << std::endl << "  " << *result << std::endl;

  // populate belief hash
  hashEntryClass* entry = beliefHash->lookup((void*)result);
  if( entry == 0 ) {
    if( saveInHash ) {
      entry = beliefHash->insert((void*)result,result->heuristicValue(),(void*)false);
      cachedBel->quantization = entry;
      quantization = entry;
    }
    else {
      localQuantization.setValue(result->heuristicValue());
      quantization = &localQuantization;
    }
  }
  else {
    if( saveInHash ) cachedBel->quantization = entry;
    quantization = entry;
  }
}

