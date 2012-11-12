//  Theseus
//  theSearchBelief.h -- Search Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theSearchBelief_INCLUDE_
#define _theSearchBelief_INCLUDE_

#include <iostream>
#include <set>
#include <map>

#include <theseus/theBelief.h>
#include <theseus/theBeliefCache.h>
#include <theseus/theSearchModel.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Search Belief Class
//

class searchBeliefClass : public beliefClass
{
private:
  // members
  set<int> bel;

  // static members
  static searchModelClass* model;

  // private methods
  searchBeliefClass( const searchBeliefClass& belief )
    {
      *this = belief;
    }

  // private virtual methods
  virtual void print( ostream& os ) const;

public:
  // constructors/destructors
  searchBeliefClass() { }
  virtual ~searchBeliefClass() { }

  // methods
  const searchModelClass*       getModel( void ) const { return( model ); }
  void                          clean( void )
    {
      bel.erase( bel.begin(), bel.end() );
    }

  // virtual methods
  virtual void                  setModel( modelClass* m )
    {
      model = dynamic_cast<searchModelClass*>( m );
    }
  virtual beliefClass::constructor_t getConstructor( void ) const
    {
      return( (beliefClass *(*)(void)) &searchBeliefClass::constructor );
    }
  virtual bool                  check( void ) const
    {
      return( bel.size() > 0 );
    }
  virtual bool                  check( int state ) const
    {
      return( bel.find( state ) != bel.end() );
    }
  virtual void                  checkModelAvailability( modelClass* model ) const;
  virtual const map<int,float>* cast( void ) const;
  virtual void                  insert( int state, float probability )
    {
      bel.insert( state );
    }
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
      return( new searchBeliefClass( *this ) );
    }
  virtual float                 heuristicValue( void ) const
    {
      return( heuristic == NULL ? 0.0 : heuristic->value( this ) );
    }
  virtual void                  support( set<int> &result ) const;
  virtual int                   supportSize( void ) const;

  // operator overload
  virtual beliefClass&          operator=( const searchBeliefClass& b )
    {
      bel = b.bel;
      return( *this );
    }
  virtual beliefClass&          operator=( const beliefClass& b )
    {
      *this = (searchBeliefClass&)b;
      return( *this );
    }
  virtual bool                  operator==( const searchBeliefClass& b ) const
    {
      return( (bel == b.bel) );
    }
  virtual bool                  operator==( const beliefClass& b ) const
    {
      return( *this == (searchBeliefClass&)b );
    }

  // hash virtual methods
  virtual unsigned              hashDataFunction( void ) const;
  virtual void*                 hashDataCopy( void ) const
    {
      return( (void*)clone() );
    }
  virtual bool                  hashDataCompare( const void* data ) const
    {
      return( !(bel == ((searchBeliefClass*)data)->bel) );
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
  static  searchBeliefClass*    read( istream& is );
  static  searchBeliefClass*    constructor( void );
  static  void                  fill( istream& is, beliefClass* bel );
  static  void                  fill( istream& is, searchBeliefClass* bel );

  // friends
  friend class searchBeliefHashClass;
  friend class searchPOMDPClass;
  friend class searchModelClass;
};


///////////////////////////////////////////////////////////////////////////////
//
// Search Belief Hash Class
//

class searchBeliefHashClass : public beliefHashClass
{
private:
  // private methods
  searchBeliefHashClass( const searchBeliefHashClass& hash )
    {
      *this = hash;
    }

public:
  // constructors/destructors
  searchBeliefHashClass() { }
  virtual ~searchBeliefHashClass() { clean(); }

  // virtual hash functions (since this is a hashClass derived class)
  virtual unsigned               dataFunction( const void* data )
    {
      return( ((searchBeliefClass*)data)->hashDataFunction() );
    }
  virtual void*                  dataCopy( const void* data )
    {
      return( ((searchBeliefClass*)data)->hashDataCopy() );
    }
  virtual bool                   dataCompare( const void* newData, const void* dataInHash )
    {
      return( !(*((searchBeliefClass*)newData) == *((searchBeliefClass*)dataInHash)) );
    }
  virtual void                   dataDelete( void* data )
    {
      delete (searchBeliefClass*)data;
    }
  virtual void                   dataPrint( ostream& os, const void* data )
    {
      os << "[ " << *((searchBeliefClass*)data) << "] ";
    } 
  virtual void                   dataWrite( ostream& os, const void* data ) const
    {
      ((searchBeliefClass*)data)->write( os );
    }
  virtual void*                  dataRead( istream& is )
    {
      return( beliefClass::read( is ) );
    }
  virtual void                   statistics( ostream& os )
    {
      hashClass::statistics( os );
    }

  // virtual quantization functions
  virtual void                   quantize( bool useCache, bool saveInHash, const beliefClass* belief,
					   hashEntryClass*& quantization );

  // serialization
  static  void                   checkIn( void );
  virtual void                   write( ostream& os ) const;
  static  searchBeliefHashClass* read( istream& is );
  static  searchBeliefHashClass* constructor( void );
  static  void                   fill( istream& is, searchBeliefHashClass* hash );
};

#endif // _theSearchBelief_INCLUDE
