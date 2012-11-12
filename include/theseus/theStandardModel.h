//  Theseus
//  theStandardModel.h -- Standard Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theStandardModel_INCLUDE_
#define _theStandardModel_INCLUDE_

#include <iostream>
#include <set>
#include <map>
#include <vector>

#include <theseus/theModel.h>
#include <theseus/theRtStandard.h>
#include <theseus/theMemoryMgmt.h>

class stateClass;
class beliefClass;
class stateHashClass;
class invStateHashClass;
class observationHashClass;
class invObservationHashClass;


///////////////////////////////////////////////////////////////////////////////
//
// Standard Model Class
//

class standardModelClass : public modelClass
{
  // private members
#ifndef ALTERNATE_HASH
  stateHashClass*            stateHash;
  invStateHashClass*         invStateHash;             
#endif
  observationHashClass*      observationHash;
  invObservationHashClass*   invObservationHash;             

  // private methods
  standardModelClass( const standardModelClass& model );

public:
  // members
  int  stateIndex;
  int  observationIndex;
  int  numInitialStates;

#ifdef ALTERNATE_HASH
  // alternate state hash
  stateClass *rootState;
  stateClass::node_t *root;
  map<int,const stateClass::node_t*,less<int>,gpt_alloc> shash;
#endif

  // simple models
  char* goalModel;
  int   goalModelSize;
  char* availabilityModel;
  int   availabilityModelSize;
  set<int,less<int>,gpt_alloc>*       applicationModel;
  map<int,float,less<int>,gpt_alloc>* costModel;

  // complex models
  map<int,map<int,float,less<int>,gpt_alloc>,less<int>,gpt_alloc>* transitionModel;
  map<int,map<int,float,less<int>,gpt_alloc>,less<int>,gpt_alloc>* observationModel;
  //  map<int,set<int,less<int>,gpt_alloc>,less<int>,gpt_alloc> invTransitionModel;

  // constructors/destructors
  standardModelClass( void );
  virtual ~standardModelClass( void );

  // methods
  void                        setup( char *cassandraFilename, beliefClass* (*belCtor)( void ) );
  void                        computeWholeModelFrom( const beliefClass *belief );
  void                        computeWholeModelFrom( int stateIndex );
  void                        printModel( ostream &os ) const;
  const stateClass*           getState( int index ) const;
  void                        setGoal( int stateIndex );
  void                        setAvailability( int stateIndex );
  bool                        isGoal( int stateIndex ) const;
  bool                        isAvailable( int stateIndex ) const;

  // virtual functions
  virtual void                setup( problemHandleClass *handle, beliefClass* (*belCtor)( void ) );
  virtual void                printState( int state, ostream& os, int indent ) const;
  virtual void                printObservation( int obs, ostream& os, int indent ) const;
  virtual void                printData( ostream& os, const char *prefix = NULL ) const;
  virtual void                checkModelFor( int stateIndex );
  virtual void                computeModelFor( int stateIndex, list<int> &newStates );
  virtual int                 getInitialState( const beliefClass *belief ) const;
  virtual float               cost( int state, int action ) const;
  virtual float               cost( const beliefClass *belief, int action ) const;
  virtual bool                applicable( int state, int action ) const;
  virtual bool                applicable( const beliefClass* belief, int action ) const;
  virtual bool                inGoal( int state ) const;
  virtual bool                inGoal( const beliefClass *belief ) const;
  virtual int                 currentNumberStates() const { return stateIndex; }
  virtual int                 currentNumberObservations() const { return observationIndex; }
  virtual int                 nextState( int state, int action ) const;
  virtual int                 nextObservation( int state, int action ) const;
  virtual int                 newState( void );
  virtual void                newTransition( int state, int action, int statePrime, float probability );

  // serialization
  static  void                checkIn( void );
  virtual void                write( ostream& os ) const;
  static  standardModelClass* read( istream& is );
  static  standardModelClass* constructor( void );
  static  void                fill( istream& is, standardModelClass *model );
};

inline void
standardModelClass::setGoal( int stateIndex )
{
  if( stateIndex >= goalModelSize )
    {
      int oldsz = goalModelSize/8;
      while( stateIndex >= goalModelSize )
	goalModelSize = (goalModelSize == 0 ? 8 : 2 * goalModelSize);
      goalModel = (char*)realloc( goalModel, goalModelSize/8 );
      memset( &goalModel[oldsz], 0, goalModelSize/8 - oldsz );
    }
  goalModel[stateIndex/8] |= (1<<(stateIndex%8));
}

inline void
standardModelClass::setAvailability( int stateIndex )
{
  if( stateIndex >= availabilityModelSize )
    {
      int oldsz = availabilityModelSize/8;
      while( stateIndex >= availabilityModelSize )
	availabilityModelSize = (availabilityModelSize == 0 ? 8 : 2 * availabilityModelSize);
      availabilityModel = (char*)realloc( availabilityModel, availabilityModelSize/8 );
      memset( &availabilityModel[oldsz], 0, availabilityModelSize/8 - oldsz );
    }
  availabilityModel[stateIndex/8] |= (1<<(stateIndex%8));
}

inline bool
standardModelClass::isGoal( int stateIndex ) const
{
  if( stateIndex >= goalModelSize )
    return( false );
  else
    return( goalModel[stateIndex/8] & (1<<(stateIndex%8)) );
}

inline bool
standardModelClass::isAvailable( int stateIndex ) const
{
  if( stateIndex >= availabilityModelSize )
    return( false );
  else
    return( availabilityModel[stateIndex/8] & (1<<(stateIndex%8)) );
}


#endif // _theStandardModel_INCLUDE
