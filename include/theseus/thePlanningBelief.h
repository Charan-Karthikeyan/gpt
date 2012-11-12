//  Theseus
//  thePlanningBelief.h -- Planning Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _thePlanningBelief_INCLUDE_
#define _thePlanningBelief_INCLUDE_

#include <iostream>
#include <map>

#include <theseus/theBelief.h>
#include <theseus/theRtStandard.h>

class problemHandleClass;


///////////////////////////////////////////////////////////////////////////////
//
// Planning Belief Class
//

class planningBeliefClass : public beliefClass
{
 private:
  // static private members
  static void (*stateAllocFunction)( int, stateClass** );
  static void (*stateDeallocFunction)( stateClass* );

  // private methods
  planningBeliefClass( const planningBeliefClass& belief )
    {
      state = belief.state;
    }

  // private virtual methods
  virtual void print( ostream& os ) const;

 public:
  // members
  stateClass* state;

  // constructors/destructors
  planningBeliefClass() { state = NULL; }
  virtual ~planningBeliefClass() { }

  // static methods
  static void                   initialize( problemHandleClass* handle );
  static void                   alloc( int number, stateClass** pool )
    {
      (*stateAllocFunction)( number, pool );
    }
  static void                   dealloc( stateClass* pool )
    {
      (*stateDeallocFunction)( pool );
    }

  // virtual methods
  virtual void                  setModel( modelClass* model ) { }
  virtual beliefClass::constructor_t getConstructor( void ) const
    {
      return( (beliefClass *(*)(void)) &planningBeliefClass::constructor );
    }
  virtual bool                  check( void ) const { return( true ); }
  virtual bool                  check( int state ) const { return( true ); }
  virtual void                  checkModelAvailability( modelClass* model ) const;
  virtual const map<int,float,less<int>,gpt_alloc>* cast( void ) const;
  virtual void                  insert( int state, float probability );
  virtual unsigned              randomSampling( void ) const;
  virtual void                  nextPossibleObservations( int action, map<int,float> &result ) const;
  virtual int                   plausibleState( int action, int observation ) const;
  virtual beliefClass*          updateByA( beliefCacheClass* cache, bool useCache, int action ) const;
  virtual beliefClass*          updateByAO( beliefCacheClass* cache, bool useCache, int action, int observation ) const;
  virtual beliefClass*          clone( void ) const
    {
      return( new planningBeliefClass( *this ) );
    }
  virtual float                 heuristicValue( void ) const
    {
      return( heuristic == NULL ? 0.0 : heuristic->value( this ) );
    }
  virtual void                  support( set<int> &result ) const
    {
      result.clear();
    }
  virtual int                   supportSize( void ) const
    {
      return( 1 );
    }

  // operator overload
  virtual beliefClass&          operator=( const planningBeliefClass& bel )
    {
      state = bel.state->clone();
      return( *this );
    }
  virtual beliefClass&          operator=( const beliefClass& bel )
    {
      *this = (planningBeliefClass&)bel;
      return( *this );
    }
  virtual bool                  operator==( const planningBeliefClass& bel ) const
    {
      return( *state == *bel.state );
    }
  virtual bool                  operator==( const beliefClass& bel ) const
    {
      return( *this == (planningBeliefClass&) bel );
    }

  // serialization
  static  void                  checkIn( void );
  virtual void                  write( ostream& os ) const;
  static  planningBeliefClass*  read( istream& is );
  static  planningBeliefClass*  constructor( void );
  static  void                  fill( istream& is, planningBeliefClass* bel );

  // friends
  friend class planningBeliefHashClass;
};


///////////////////////////////////////////////////////////////////////////////
//
// Planning Belief Hash Class
//

class planningBeliefHashClass : public beliefHashClass
{
  // private methods
  planningBeliefHashClass( const planningBeliefHashClass& hash )
    {
      *this = hash;
    }

public:
  // constructors/destructors
  planningBeliefHashClass() { }
  virtual ~planningBeliefHashClass() { }

  // virtual hash functions (since this is a hashClass derived class)
  virtual void                     clean( void );
  virtual unsigned                 dataFunction( const void* data );
  virtual void*                    dataCopy( const void* data );
  virtual bool                     dataCompare( const void* newData, const void* dataInHash );
  virtual void                     dataDelete( void* data );
  virtual void                     dataPrint( ostream& os, const void* data );
  virtual void                     dataWrite( ostream& os, const void* data ) const;
  virtual void*                    dataRead( istream& is );

  // virtual quantization functions
  virtual void                     quantize( bool useCache, bool saveInHash, const beliefClass* belief,
					     hashEntryClass*& quantization );

  // serialization
  static  void                     checkIn( void );
  virtual void                     write( ostream& os ) const;
  static  planningBeliefHashClass* read( istream& is );
  static  planningBeliefHashClass* constructor( void );
  static  void                     fill( istream& is, planningBeliefHashClass* hash );
};


#endif // _thePlanningBelief_INCLUDE
