//  Theseus
//  thePlanningModel.cc -- Planning Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <typeinfo>

using namespace std;

#include <theseus/thePlanningModel.h>
#include <theseus/thePlanningBelief.h>
#include <theseus/theRtStandard.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Planning Model Class
//

void
planningModelClass::setup( problemHandleClass *handle, beliefClass* (*belCtor)( void ) )
{
  register int number;
  register stateListClass *stateList, *state;
  register beliefClass *belief;

  // fundamental vars
  numActions = handle->numActions;
  
  // bootstrap problem 
  stateList = handle->bootstrap();
  
  // set initial state
  number = 0;
  belief = (*belCtor)();
  for( state = stateList; state->probability != -1; ++state )
    if( state->state->valid )
      {
	++number;
	belief->insert( (int)state, 1.0 );
      }
  assert( number == 1 );
  theInitialBelief = belief;
}

void
planningModelClass::printState( int state, ostream& os, int indent ) const
{
  os << "Error: not yet suppoerted" << endl;
}

void
planningModelClass::printObservation( int obs, ostream& os, int indent ) const
{
  os << "Error: not yet suppoerted" << endl;
}

void
planningModelClass::printData( ostream& os, const char *prefix ) const
{
  os << (prefix?prefix:"%model ") << "numberActions " << numActions << endl;
}

void
planningModelClass::checkModelFor( int stateIndex )
{
  // XXXXXXX
}

void
planningModelClass::computeModelFor( int stateIndex, list<int> &newStates )
{
  // XXXXXXX
}

int
planningModelClass::getInitialState( const beliefClass *belief ) const
{
  // XXXXXXX
  return( 0 );

}
float
planningModelClass::cost( int state, int action ) const
{
  return( 1.0 );
}

float
planningModelClass::cost( const beliefClass *belief, int action ) const
{
  return( 1.0 );
}

bool
planningModelClass::applicable( int state, int action ) const
{
  // XXXXXXX
  return( false );
}

bool
planningModelClass::applicable( const beliefClass* belief, int action ) const
{
  // XXXXXXX
  return( false );
}

bool
planningModelClass::inGoal( int state ) const
{
  // this function shouldn't be called
  cerr << "Warning: planningModelClass::inGoal() called." << endl;
  return( false );
}

bool
planningModelClass::inGoal( const beliefClass* belief ) const
{
  return( ((planningBeliefClass*)belief)->state->goal() );
}

int
planningModelClass::nextState( int state, int action ) const
{
  // XXXXXXX
  return( 0 );
}

int
planningModelClass::nextObservation( int state, int action ) const
{
  // XXXXXXX
  return( 0 );
}

int
planningModelClass::newState( void )
{
  // XXXXXXX
  return( 0 );
}

void
planningModelClass::newTransition( int state, int action, int statePrime, float probability )
{
  // XXXXXXX
}


///////////////////////
//
// serialization

void
planningModelClass::checkIn( void )
{
  signRegistration( typeid( planningModelClass ).name(),
		    (void (*)( istream&, modelClass * )) &planningModelClass::fill,
		    (modelClass *(*)( void )) &planningModelClass::constructor );
}

void
planningModelClass::write( ostream& os ) const
{
  const char* id;
  unsigned len;

  // write registrationId
  id = typeid( *this ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  modelClass::write( os );
}

planningModelClass *
planningModelClass::read( istream& is )
{
  planningModelClass *model;
  
  model = new planningModelClass;
  fill( is, model );

  return( model );
}

planningModelClass *
planningModelClass::constructor( void )
{
  return( new planningModelClass );
}

void
planningModelClass::fill( istream& is, planningModelClass *model )
{
  modelClass::fill( is, model );
}
