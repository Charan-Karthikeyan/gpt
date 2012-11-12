//  Theseus
//  theQuantization.h -- Belief Quantization
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theQuantization_INCLUDE_
#define _theQuantization_INCLUDE_

class beliefClass;
class beliefHashClass;
class standardBeliefClass;
class hashEntryClass;
class beliefCacheClass;


///////////////////////////////////////////////////////////////////////////////
//
// Quantization Class
//

class quantizationClass
{
public:
  // members
  static int               dimensions;
  static float             levels;
  static beliefCacheClass* cache;

  // constructors
  quantizationClass() { }

  // destructors
  ~quantizationClass() { }

  // static methods
  static void initialize( int levels, int dimensions );
  static void setCache( int cacheSize, int clusterSize,
			beliefClass* (*beliefConstructor)( void ) );
  static void finalize( void );
  

  // methods
  void        clean( void );
  void        quantize( beliefHashClass* beliefHash,
			bool useCache, 
			bool saveInHash,
			const standardBeliefClass* belief, 
			hashEntryClass* &quantization );
};

#endif // _theQuantization_INCLUDE
