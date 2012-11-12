//  Theseus
//  theSimpleBelief.h -- Simple Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theSimpleBelief_INCLUDE_
#define _theSimpleBelief_INCLUDE_

#include <iostream>
#include <map>

#include <theseus/theBelief.h>
#include <theseus/theStandardModel.h>


///////////////////////////////////////////////////////////////////////////////
//
// Simple Belief Class
//

class simpleBeliefClass : public beliefClass
{
private:
  // members
  int state;

  // static members
  static standardModelClass *model;

  // private methods
  simpleBeliefClass( const simpleBeliefClass& belief )
    {
      *this = belief;
    }

  // private virtual methods
  virtual void print( ostream& os ) const
    {
      os << state;
    }

public:
  // constructors/destructors
  simpleBeliefClass() { }
  simpleBeliefClass( int s ) { state = s; }
  virtual ~simpleBeliefClass( void ) { }

  // methods
  void                          setState( int s ) { state = s; }
  int                           getState( void )  { return( state ); } 

  // virtual methods
  virtual void                  setModel( modelClass* m )
    {
      model = dynamic_cast<standardModelClass*>( m );
    }
  virtual beliefClass::constructor_t getConstructor( void ) const
    {
      return( (beliefClass* (*)(void))&simpleBeliefClass::constructor );
    }
  virtual bool                  check( void ) const
    {
      return( true );
    }
  virtual bool                  check( int s ) const
    {
      return( state == s );
    }
  virtual const map<int,float>* cast( void ) const;
  virtual void                  insert( int s, float p )
    {
      state = s;
    }
  virtual void                  checkModelAvailability( modelClass* m ) const
    {
      m->checkModelFor( state );
    }
  virtual unsigned              randomSampling( void ) const
    {
      return( state );
    }
  virtual void                  nextPossibleObservations( int action, map<int,float> &result ) const;
  virtual int                   plausibleState( int action, int observation ) const
    {
      return( observation );
    }
  virtual beliefClass*          updateByA( beliefCacheClass* cache, bool useCache, int action ) const;
  virtual beliefClass*          updateByAO( beliefCacheClass* cache, bool useCache, int action, int observation ) const;
  virtual beliefClass*          clone( void ) const
    {
      return( new simpleBeliefClass( *this ) );
    }
  virtual float                 heuristicValue( void ) const
    {
      return( heuristic == NULL ? 0.0 : heuristic->value( this ) );
    }
  virtual void                  support( set<int> &result ) const
    {
      result.clear();
      result.insert( state );
    }
  virtual int                   supportSize( void ) const
    {
      return( 1 );
    }

  // operator overload
  virtual beliefClass&          operator=( const simpleBeliefClass& b )
    {
      state = b.state;
      return( *this );
    }
  virtual beliefClass&          operator=( const beliefClass& b )
    {
      *this = (simpleBeliefClass&)b;
      return( *this );
    }
  virtual bool                  operator==( const simpleBeliefClass& b ) const
    {
      return( state == b.state );
    }
  virtual bool                  operator==( const beliefClass& b ) const
    {
      return( *this == (simpleBeliefClass&)b );
    }

  // hash virtual methods
  virtual unsigned              hashDataFunction( void ) const
    {
      return( state );
    }
  virtual void*                 hashDataCopy( void ) const
    {
      return( (void*)clone() );
    }
  virtual bool                  hashDataCompare( const void* data ) const
    {
      return( !(state == ((simpleBeliefClass*)data)->state) );
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
  static  simpleBeliefClass*    read( istream& is );
  static  simpleBeliefClass*    constructor( void );
  static  void                  fill( istream& is, beliefClass* bel );
  static  void                  fill( istream& is, simpleBeliefClass* bel );

  // friends
  friend class simpleBeliefHashClass;
};


///////////////////////////////////////////////////////////////////////////////
//
// Simple Belief Hash Class
//

class simpleBeliefHashClass : public beliefHashClass
{
  // private methods
  simpleBeliefHashClass( const simpleBeliefHashClass& hash )
    {
      *this = hash;
    }

public:
  // constructors/destructors
  simpleBeliefHashClass() { }
  virtual ~simpleBeliefHashClass() { clean(); }

  // virtual hash functions (since this is a hashClass derived class)
  virtual void                     clean( void )
    {
      beliefHashClass::clean();
    }
  virtual unsigned                 dataFunction( const void* data )
    {
      return( (unsigned)data );
    }
  virtual void*                    dataCopy( const void* data )
    {
      return( (void*)data );
    }
  virtual bool                     dataCompare( const void* newData, const void* dataInHash )
    {
      return( !(newData == dataInHash) );
    }
  virtual void                     dataDelete( void* data ) { }
  virtual void                     dataPrint( ostream& os, const void* data )
    {
      os << (unsigned)data;
    } 
  virtual void                     dataWrite( ostream& os, const void* data ) const
    {
      os << (unsigned)data;
    }
  virtual void*                    dataRead( istream& is )
    {
      unsigned data;
      is >> data;
      return( (void*)data );
    }
  virtual void                     statistics( ostream& os )
    {
      hashClass::statistics( os );
    }

  // virtual quantization functions
  virtual void                     quantize( bool useCache, bool saveInHash, const beliefClass* belief, 
					     hashEntryClass*& quantization )
    {
    }

  // serialization
  static  void                     checkIn( void );
  virtual void                     write( ostream& os ) const;
  static  simpleBeliefHashClass*   read( istream& is );
  static  simpleBeliefHashClass*   constructor( void );
  static  void                     fill( istream& is, simpleBeliefHashClass* hash );
};


#endif // _theSimpleBelief_INCLUDE
