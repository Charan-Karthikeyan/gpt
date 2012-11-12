//  Theseus
//  theBelief.h -- Abstract Belief Class
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theBelief_INCLUDE_
#define _theBelief_INCLUDE_

#include <iostream>
#include <set>
#include <map>

#include <theseus/theHeuristic.h>
#include <theseus/theHash.h>
#include <theseus/theMemoryMgmt.h>

class heuristicClass;
class modelClass;
class beliefCacheClass;
class registerClass;


///////////////////////////////////////////////////////////////////////////////
//
// Belief Class
//

class beliefClass
{
 private:
  // private methods
  beliefClass( const beliefClass& belief ) { }

  // private virtual methods
  virtual void              print( ostream& os ) const = 0;

 public:
  // typedefs
  typedef beliefClass* (*constructor_t)( void );

  // static members
  static heuristicClass* heuristic;
  static registerClass*  registeredElements;

  // constructors/destructors
  beliefClass() { }
  virtual ~beliefClass();

  // setup methods
  static  void                  setHeuristic( heuristicClass* h ) { heuristic = h; }

  // virtual methods
  virtual void                  setModel( modelClass* model ) = 0;
  virtual beliefClass*          (*getConstructor( void ) const)( void ) = 0;
  virtual bool                  check( void ) const = 0;
  virtual bool                  check( int state ) const = 0;
  virtual void                  checkModelAvailability( modelClass *model ) const = 0;
  virtual const map<int,float>* cast( void ) const = 0;
  virtual void                  insert( int state, float probability ) = 0;
  virtual unsigned              randomSampling( void ) const = 0;
  virtual void                  nextPossibleObservations( int action, map<int,float> &result ) const = 0;
  virtual int                   plausibleState( int action, int observation ) const = 0;
  virtual beliefClass*          updateByA( beliefCacheClass* cache, bool useCache, int action ) const = 0;
  virtual beliefClass*          updateByAO( beliefCacheClass* cache, bool useCache,	int action, int observation ) const = 0;
  virtual beliefClass*          clone( void ) const = 0;
  virtual float                 heuristicValue( void ) const;
  virtual void                  support( set<int> &result ) const = 0;
  virtual int                   supportSize( void ) const = 0;

  // operator overload
  virtual beliefClass&          operator=( const beliefClass& bel ) = 0;
  virtual bool                  operator==( const beliefClass& bel ) const = 0;

  // hash virtual methods
  virtual unsigned              hashDataFunction( void ) const;
  virtual void*                 hashDataCopy( void ) const;
  virtual bool                  hashDataCompare( const void* data ) const;
  virtual void                  hashDataWrite( ostream& os ) const;
  virtual void*                 hashDataRead( istream& is ) const;

  // serialization
  static void                   signRegistration( const char* registrationId, 
						  void (*fillFunction)( istream& is, beliefClass * ), 
						  beliefClass* (*constructor)( void ) );
  static  void                  cleanup( void );
  virtual void                  write( ostream& os ) const;
  static beliefClass*           read( istream& is );
  static void                   fill( istream& is, beliefClass* bel );

  // friend functions
  friend ostream& operator<<( ostream& os, const beliefClass& belief );
};


///////////////////////////////////////////////////////////////////////////////
//
// Belief Hash Class
//

class beliefHashClass : public hashClass
{
  // private methods
  beliefHashClass( const beliefHashClass& hash );

public:
  // constructors
  beliefHashClass() : hashClass( 1024 ) { }

  // destructor
  virtual ~beliefHashClass();

  // virtual hash functions (since this is a hashClass derived class)
  virtual unsigned dataFunction( const void* data );
  virtual void*    dataCopy( const void* data );
  virtual bool     dataCompare( const void* newData, const void* dataInHash );
  virtual void     dataDelete( void* data ) = 0;
  virtual void     dataPrint( ostream& os, const void* data ) = 0;
  virtual void     dataWrite( ostream& os, const void* data ) const = 0;
  virtual void*    dataRead( istream& is ) = 0;
  virtual void     statistics( ostream& os );

  // virtual quantization functions
  virtual void     quantize( bool useCache, 
			     bool saveInHash,
			     const beliefClass* belief, 
			     hashEntryClass*& quantization ) = 0;

  // serialization
  virtual void             write( ostream& os ) const;
  static  beliefHashClass* read( istream& is );
  static  void             fill( istream& is, beliefHashClass* hash );
};


///////////////////////////////////////////////////////////////////////////////
//
// Hooked Belief Hash Class
//

class hookedBeliefHashClass : public beliefHashClass
{
  // private methods
  hookedBeliefHashClass( const hookedBeliefHashClass& hash );

public:
  // constructors/destructor
  hookedBeliefHashClass() { }
  virtual ~hookedBeliefHashClass();

  // virtual hash functions (using some parent virtual functions)
  virtual void                   dataDelete( void* data );
  virtual void                   dataPrint( ostream& os, const void* data );
  virtual void                   dataWrite( ostream& os, const void* data ) const;
  virtual void*                  dataRead( istream& is );

  // virtual quantization functions
  virtual void                   quantize( bool useCache, bool saveInHash, const beliefClass* belief, 
					   hashEntryClass*& quantization );

  // serialization
  static  void                   checkIn( void );
  virtual void                   write( ostream& os ) const;
  static  hookedBeliefHashClass* read( istream& is );
  static  hookedBeliefHashClass* constructor( void );
  static  void                   fill( istream& is, hookedBeliefHashClass* hash );
};

#endif // _theBelief_INCLUDE
