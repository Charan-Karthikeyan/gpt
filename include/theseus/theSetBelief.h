//  Theseus
//  theSetBelief.h -- Set Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theSetBelief_INCLUDE_
#define _theSetBelief_INCLUDE_

#include <iostream>
#include <set>
#include <map>

#include <theseus/theBelief.h>
#include <theseus/theBeliefCache.h>
#include <theseus/theNonDetModel.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Set Belief Class
//

class setBeliefClass : public beliefClass
{
private:
  // members
  set<int,less<int>,gpt_alloc> bel;

  // static members
  static nonDetModelClass* model;

  // private methods
  setBeliefClass( const setBeliefClass& belief )
    {
      *this = belief;
    }

  // private virtual methods
  virtual void print( ostream& os ) const
    {
      set<int>::const_iterator it;
      os << "[ ";
      for( it = bel.begin(); it != bel.end(); ++it )
	os << *it << " ";
      os << "]";
    }

public:
  // constructors/destructors
  setBeliefClass() { }
  virtual ~setBeliefClass() { }

  // methods
  const nonDetModelClass* getModel( void ) const { return( model ); }
  void clean( void )
    {
      bel.erase( bel.begin(), bel.end() );
    }

  // virtual methods
  virtual void setModel( modelClass* m )
    {
      model = dynamic_cast<nonDetModelClass*>( m );
    }

  virtual beliefClass::constructor_t getConstructor( void ) const
    {
      return( (beliefClass *(*)(void)) &setBeliefClass::constructor );
    }

  virtual bool check( void ) const
    {
      return( bel.size() > 0 );
    }

  virtual bool check( int state ) const
    {
      set<int>::const_iterator it;
      for( it = bel.begin(); it != bel.end(); ++it )
	if( state == *it )
	  return( true );
      return( false );
    }

  virtual void checkModelAvailability( modelClass* model ) const
    {
      static set<int>::const_iterator it;
      for( it = bel.begin(); it != bel.end(); ++it )
	model->checkModelFor( *it );
    }

  virtual const map<int,float,less<int>,gpt_alloc>* cast( void ) const;

  virtual void insert( int state, float probability )
    {
      if( !check( state ) )
	bel.insert( state );
    }

  virtual unsigned randomSampling( void ) const
    {
      return( ::randomSampling( bel ) );
    }

  virtual void         nextPossibleObservations( int action, map<int,float> &result ) const;
  virtual int          plausibleState( int action, int observation ) const;
  virtual beliefClass* updateByA( beliefCacheClass* cache, bool useCache, int action ) const;
  virtual beliefClass* updateByAO( beliefCacheClass* cache, bool useCache, int action, int observation ) const;

  virtual beliefClass* clone( void ) const
    {
      return( new setBeliefClass( *this ) );
    }

  virtual float heuristicValue( void ) const
    {
      return( heuristic == NULL ? 0.0 : heuristic->value( this ) );
    }

  virtual void support( set<int> &result ) const
    {
      set<int>::const_iterator it;
      result.clear();
      for( it = bel.begin(); it != bel.end(); ++it )
	result.insert( *it );
    }

  virtual int supportSize( void ) const
    {
      return( bel.size() );
    }

  // operator overload
  virtual beliefClass& operator=( const setBeliefClass& b )
    {
      bel = b.bel;
      return( *this );
    }

  virtual beliefClass& operator=( const beliefClass& b )
    {
      *this = (setBeliefClass&)b;
      return( *this );
    }

  virtual bool operator==( const setBeliefClass& b ) const
    {
      return( (bel == b.bel) );
    }

  virtual bool operator==( const beliefClass& b ) const
    {
      return( *this == (setBeliefClass&)b );
    }

  // hash virtual methods
  virtual unsigned hashDataFunction( void ) const;
  virtual void* hashDataCopy( void ) const
    {
      return( (void*)clone() );
    }

  virtual bool hashDataCompare( const void* data ) const
    {
      return( !(bel == ((setBeliefClass*)data)->bel) );
    }

  virtual void hashDataWrite( ostream& os ) const
    {
      write( os );
    }

  virtual void* hashDataRead( istream& is ) const
    {
      return( beliefClass::read( is ) );
    }

  // serialization
  static  void            checkIn( void );
  virtual void            write( ostream& os ) const;
  static  setBeliefClass* read( istream& is );
  static  setBeliefClass* constructor( void );
  static  void            fill( istream& is, beliefClass* bel );
  static  void            fill( istream& is, setBeliefClass* bel );

  // friends
  friend class setBeliefHashClass;
  friend class searchPOMDPClass;
  friend class nonDetModelClass;
};


///////////////////////////////////////////////////////////////////////////////
//
// Set Belief Hash Class
//

class setBeliefHashClass : public beliefHashClass
{
private:
  // private methods
  setBeliefHashClass( const setBeliefHashClass& hash )
    {
      *this = hash;
    }

public:
  // constructors/destructors
  setBeliefHashClass() { }
  virtual ~setBeliefHashClass() { clean(); }

  // virtual hash functions (since this is a hashClass derived class)
  virtual unsigned dataFunction( const void* data )
    {
      return( ((setBeliefClass*)data)->hashDataFunction() );
    }

  virtual void* dataCopy( const void* data )
    {
      return( ((setBeliefClass*)data)->hashDataCopy() );
    }

  virtual bool dataCompare( const void* newData, const void* dataInHash )
    {
      return( !(*((setBeliefClass*)newData) == *((setBeliefClass*)dataInHash)) );
    }

  virtual void dataDelete( void* data )
    {
      delete (setBeliefClass*)data;
    }

  virtual void dataPrint( ostream& os, const void* data )
    {
      os << "[ " << *((setBeliefClass*)data) << "] ";
    } 

  virtual void dataWrite( ostream& os, const void* data ) const
    {
      ((setBeliefClass*)data)->write( os );
    }

  virtual void* dataRead( istream& is )
    {
      return( beliefClass::read( is ) );
    }

  virtual void statistics( ostream& os )
    {
      hashClass::statistics( os );
    }

  // virtual quantization functions
  virtual void quantize( bool useCache, bool saveInHash, const beliefClass* belief,
			 hashEntryClass*& quantization );

  // serialization
  static  void                checkIn( void );
  virtual void                write( ostream& os ) const;
  static  setBeliefHashClass* read( istream& is );
  static  setBeliefHashClass* constructor( void );
  static  void                fill( istream& is, setBeliefHashClass* hash );
};

#endif // _theSetBelief_INCLUDE
