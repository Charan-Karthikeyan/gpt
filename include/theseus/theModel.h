//  Theseus
//  theModel.cc -- Abstract Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theModel_INCLUDE_
#define _theModel_INCLUDE_

#include <iostream>
#include <list>

class problemHandleClass;
class registerClass;
class beliefClass;


///////////////////////////////////////////////////////////////////////////////
//
// Model Class
//

class modelClass
{
  // static members
  static registerClass* registeredElements;

  // private methods
  modelClass( const modelClass& model )
    {
      *this = model;
    }

public:
  int                   numActions;
  float                 discountFactor;
  beliefClass*          theInitialBelief;

  // constructors
  modelClass()
    {
      numActions = -1;
      theInitialBelief = NULL;
    }

  // destructors
  virtual ~modelClass() = 0;

  // virtual functions
  virtual void          setup( problemHandleClass *handle, beliefClass* (*belCtor)( void ) ) = 0;
  virtual void          printState( int state, ostream& os, int indent ) const = 0;
  virtual void          printObservation( int obs, ostream& os, int indent ) const = 0;
  virtual void          printData( ostream& os, const char *prefix = NULL ) const = 0;
  virtual void          checkModelFor( int stateIndex ) = 0;
  virtual void          computeModelFor( int stateIndex, list<int> &newStates ) = 0;
  virtual beliefClass*  getTheInitialBelief( void );
  virtual int           getInitialState( const beliefClass *belief ) const = 0;
  virtual float         cost( int state, int action ) const = 0;
  virtual float         cost( const beliefClass *belief, int action ) const = 0;
  virtual bool          applicable( int state, int action ) const = 0;
  virtual bool          applicable( const beliefClass* belief, int action ) const = 0;
  virtual bool          inGoal( int state ) const = 0;
  virtual bool          inGoal( const beliefClass *belief ) const = 0;
  virtual int           currentNumberStates() const = 0;
  virtual int           currentNumberObservations() const = 0;
  virtual int           nextState( int state, int action ) const = 0;
  virtual int           nextObservation( int state, int action ) const = 0;
  virtual int           newState( void ) = 0;
  virtual void          newTransition( int state, int action, int statePrime, float probability ) = 0;

  // serialization
  static  void          signRegistration( const char *registrationId,
					  void (*fillFunction)( istream&, modelClass * ), 
					  modelClass *(*constructor)( void ) );
  static  void          cleanup( void );  
  virtual void          write( ostream& os ) const;
  static modelClass*    read( istream& is );
  static void           fill( istream& is, modelClass *model );
};


#endif // _theModel_INCLUDE
