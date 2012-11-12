//  Theseus
//  thePlanningModel.h -- Planning Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _thePlanningModel_INCLUDE_
#define _thePlanningModel_INCLUDE_

#include <iostream>
#include <list>

#include <theseus/theModel.h>

class problemHandleClass;
class planningModelClass;


///////////////////////////////////////////////////////////////////////////////
//
// Planning Model Class
//

class planningModelClass : public modelClass
{
  // private methods
  planningModelClass( const planningModelClass& model ) { }

public:
  // constructors/destructors
  planningModelClass( void ) { }
  virtual ~planningModelClass( void ) { }

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
  virtual int                 currentNumberStates() const { return -1; }
  virtual int                 currentNumberObservations() const { return -1; }
  virtual int                 nextState( int state, int action ) const;
  virtual int                 nextObservation( int state, int action ) const;
  virtual int                 newState( void );
  virtual void                newTransition( int state, int action, int statePrime, float probability );

  // serialization
  static  void                checkIn( void );
  virtual void                write( ostream& os ) const;
  static  planningModelClass* read( istream& is );
  static  planningModelClass* constructor( void );
  static  void                fill( istream& is, planningModelClass *model );
};


#endif // _thePlanningModel_INCLUDE
