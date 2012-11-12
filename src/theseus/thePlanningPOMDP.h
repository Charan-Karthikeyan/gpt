//  Theseus
//  thePlanningPOMDP.h -- Planning POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _thePlanningPOMDP_INCLUDE_
#define _thePlanningPOMDP_INCLUDE_

#include <iostream>

#include <theseus/thePOMDP.h>
#include <theseus/thePlanningBelief.h>
#include <theseus/theModel.h>

class stateListClass;


///////////////////////////////////////////////////////////////////////////////
//
// Planning POMDP Class
//

class planningPOMDPClass : public POMDPClass
{
  // private methods
  planningPOMDPClass( const planningPOMDPClass& pomdp );

  // private virtual methods
  virtual void algorithm( bool learning, resultClass& result );

public:
  // constructors/destcurtors
  planningPOMDPClass()
    {
      beliefHash = new planningBeliefHashClass;
    }
  virtual ~planningPOMDPClass( void )
    {
      delete beliefHash;
    }

  // static methods
  static stateListClass* expandState( const beliefClass *belief )
    {
      return( ((planningBeliefClass*)belief)->state->applyAllActions() );
    }

  // virtual methods
  virtual float cost( int state, int action ) const
    {
      return( 1.0 );
    }

  virtual float cost( const beliefClass *belief, int action ) const
    {
      return( 1.0 );
    }

  virtual bool inGoal( int state ) const
    {
      return( model->inGoal( state ) );
    }

  virtual bool inGoal( const beliefClass* belief ) const
    {
      return( model->inGoal( belief ) );
    }

  virtual bool applicable( int state, int action ) const;
  virtual bool applicable( const beliefClass* belief, int action ) const;
  virtual int  getInitialState( const beliefClass* belief ) const;
  virtual int  nextState( int state, int action ) const;
  virtual int  nextObservation( int state, int action ) const;

  // serialization
  static  void                checkIn( void );
  virtual void                write( ostream& os ) const;
  static  planningPOMDPClass* read( istream& is );
  static  planningPOMDPClass* constructor( void );
  static  void                fill( istream& is, planningPOMDPClass *pomdp );
};

#endif // _thePlanningPOMDP_INCLUDE
