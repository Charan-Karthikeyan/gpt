//  Theseus
//  theNonDetModel.cc -- Non-Deterministic Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <typeinfo>

using namespace std;

#include <theseus/theNonDetModel.h>
#include <theseus/theTopLevel.h>
#include <theseus/theSetBelief.h>
#include <theseus/theRtStandard.h>


///////////////////////////////////////////////////////////////////////////////
//
// Non-Deterministic Model Class
//

float
nonDetModelClass::cost( const beliefClass *belief, int action ) const
{
  register float result;
  const set<int>* v;
  set<int>::const_iterator it;

  assert( belief != NULL );
  assert( dynamic_cast<const setBeliefClass*>(belief) != NULL );

  result = 0.0;
  v = &((setBeliefClass*)belief)->bel;
  for( it = v->begin(); it != v->end(); ++it )
    {
      float value = standardModelClass::cost( *it, action );
      result = (value > result ? value : result);
    }
  return( result );
}

bool
nonDetModelClass::applicable( const beliefClass* belief, int action ) const
{
  const set<int>* v;
  set<int>::const_iterator it;

  assert( belief != NULL );
  assert( dynamic_cast<const setBeliefClass*>(belief) != NULL );

  v = &((setBeliefClass*)belief)->bel;
  for( it = v->begin(); it != v->end(); ++it )
    if( !standardModelClass::applicable( *it, action ) )
      return( false );
  return( true );
}

bool
nonDetModelClass::inGoal( const beliefClass* belief ) const
{
  const set<int>* v;
  set<int>::const_iterator it;
  assert( belief != NULL );

  if( PD.pddlProblemType &&
      (PD.handle->otherInfo & (1<<problemHandleClass::CERTAINTY_GOAL)) )
    {
      return( belief->supportSize() == 1 );
    }
  else
    {
      assert( dynamic_cast<const setBeliefClass*>(belief) != NULL );
      v = &((setBeliefClass*)belief)->bel;
      for( it = v->begin(); it != v->end(); ++it )
	if( !standardModelClass::inGoal( *it ) )
	  return( false );
      return( true );
    }
}


///////////////////////
//
// serialization

void
nonDetModelClass::checkIn( void )
{
  signRegistration( typeid( nonDetModelClass ).name(),
		    (void (*)( istream&, modelClass * )) &nonDetModelClass::fill,
		    (modelClass* (*)( void )) &nonDetModelClass::constructor );
}

nonDetModelClass*
nonDetModelClass::read( istream& is )
{
  nonDetModelClass *model;

  model = new nonDetModelClass;
  fill( is, model );
  return( model );
}

nonDetModelClass*
nonDetModelClass::constructor( void )
{
  return( new nonDetModelClass );
}

void
nonDetModelClass::fill( istream& is, nonDetModelClass *model )
{
  standardModelClass::fill( is, model );
}
