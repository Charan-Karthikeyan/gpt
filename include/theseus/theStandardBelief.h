//  Theseus
//  theStandardBelief.h -- Standard Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theStandardBelief_INCLUDE_
#define _theStandardBelief_INCLUDE_

#include <iostream>
#include <map>

#include <theseus/theBelief.h>
#include <theseus/theStandardModel.h>
#include <theseus/theQuantization.h>
#include <theseus/theUtils.h>

class standardModelClass;


///////////////////////////////////////////////////////////////////////////////
//
// Standard Belief Class
//

class standardBeliefClass : public beliefClass
{
 private:
  // members
  map<int,float,less<int>,gpt_alloc> bel;

  // static members
  static standardModelClass* model;

  // private methods
  standardBeliefClass( const standardBeliefClass& belief ) { *this = belief; }

  // private virtual methods
  virtual void print( ostream& os ) const;

public:
  // constructors/destructors
  standardBeliefClass() { }
  virtual ~standardBeliefClass() { }

  // methods
  const standardModelClass*     getModel( void ) const
    {
      return( model );
    }
  void                          clean( void )
    {
      bel.clear();
    }
  bool                          compact( void );
  void                          normalize( void );
  void                          fill( const float* array, int dimension );

  // virtual methods
  virtual void                  setModel( modelClass* m )
    {
      model = dynamic_cast<standardModelClass*>( m );
    }
  virtual beliefClass::constructor_t getConstructor( void ) const
    {
      return( (beliefClass* (*)(void))&standardBeliefClass::constructor );
    }
  virtual bool                  check( void ) const;
  virtual bool                  check( int state ) const
    {
      return( bel.find( state ) != bel.end() );
    }
  virtual const map<int,float,less<int>,gpt_alloc>* cast( void ) const
    {
      return( &bel );
    }
  virtual void                  insert( int state, float probability )
    {
      bel.insert( make_pair( state, probability ) );
    }
  virtual void                  checkModelAvailability( modelClass* model ) const;
  virtual unsigned              randomSampling( void ) const
    {
      return( ::randomSampling( bel ) );
    }
  virtual void                  nextPossibleObservations( int action, map<int,float> &result ) const;
  virtual int                   plausibleState( int action, int observation ) const;
  virtual beliefClass*          updateByA( beliefCacheClass* cache, bool useCache, int action ) const;
  virtual beliefClass*          updateByAO( beliefCacheClass* cache, bool useCache, int action, int observation ) const;
  virtual beliefClass*          clone( void ) const
    {
      return( new standardBeliefClass( *this ) );
    }
  virtual float                 heuristicValue( void ) const
    {
      return( heuristic == NULL ? 0.0 : heuristic->value( this ) );
    }
  virtual void                  support( set<int> &result ) const;
  virtual int                   supportSize( void ) const
    {
      return( bel.size() );
    }

  // operator overload
  virtual beliefClass&          operator=( const standardBeliefClass& b )
    {
      bel = b.bel;
      return( *this );
    }
  virtual beliefClass&          operator=( const beliefClass& b )
    {
      *this = (standardBeliefClass&)b;
      return( *this );
    }
  virtual bool                  operator==( const standardBeliefClass& b ) const;
  virtual bool                  operator==( const beliefClass& b ) const
    {
      return( *this == (standardBeliefClass&)b );
    }

  // hash virtual methods
  virtual unsigned              hashDataFunction( void ) const;
  virtual void*                 hashDataCopy( void ) const
    {
      return( (void*)clone() );
    }
  virtual bool                  hashDataCompare( const void* data ) const
    {
      return( !(bel == ((standardBeliefClass*)data)->bel) );
    }
  virtual void                  hashDataWrite( ostream& os ) const
    {
      write( os );
    }
  virtual void*                 hashDataRead( istream& is ) const
    {
      return( beliefClass::read( is ) );
    }

  // serialization
  static  void                  checkIn( void );
  virtual void                  write( ostream& os ) const;
  static  standardBeliefClass*  read( istream& is );
  static  standardBeliefClass*  constructor( void );
  static  void                  fill( istream& is, beliefClass* bel );
  static  void                  fill( istream& is, standardBeliefClass* bel );

  // friends
  friend class standardBeliefHashClass;
};


///////////////////////////////////////////////////////////////////////////////
//
// Standard Belief Hash Class
//

class standardBeliefHashClass : public beliefHashClass
{
  // private methods
  standardBeliefHashClass( const standardBeliefHashClass& bhash ) { *this = bhash; }

public:
  // members
  quantizationClass*  quantization;

  // constructors/destructors
  standardBeliefHashClass() { quantization = new quantizationClass; }
  virtual ~standardBeliefHashClass()
    {
      clean(); 
      delete quantization;
    }

  // virtual hash functions (since this is a hashClass derived class)
  virtual void                     clean( void );
  virtual unsigned                 dataFunction( const void* data )
    {
      return( ((standardBeliefClass*)data)->hashDataFunction() );
    }
  virtual void*                    dataCopy( const void* data )
    {
      return( ((standardBeliefClass*)data)->hashDataCopy() );
    }
  virtual bool                     dataCompare( const void* newData, const void* dataInHash )
    {
      return( !(*((standardBeliefClass*)newData) == *((standardBeliefClass*)dataInHash)) );
    }
  virtual void                     dataDelete( void* data )
    {
      delete (standardBeliefClass*)data;
    }
  virtual void                     dataPrint( ostream& os, const void* data )
    {
      os << *((standardBeliefClass*)data) << " ";
    } 
  virtual void                     dataWrite( ostream& os, const void* data ) const
    {
      ((standardBeliefClass*)data)->write( os );
    }
  virtual void*                    dataRead( istream& is )
    {
      return( beliefClass::read( is ) );
    }
  virtual void                     statistics( ostream& os );

  // virtual quantization functions
  virtual void                     quantize( bool useCache, bool saveInHash, const beliefClass* belief, 
					     hashEntryClass*& quantization );

  // serialization
  static  void                     checkIn( void );
  virtual void                     write( ostream& os ) const;
  static  standardBeliefHashClass* read( istream& is );
  static  standardBeliefHashClass* constructor( void );
  static  void                     fill( istream& is, standardBeliefHashClass* hash );
};


#endif // _theStandardBelief_INCLUDE
