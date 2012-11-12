//  Theseus
//  thePlanningPOMDP.cc -- Planning POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <typeinfo>
#include <climits>

using namespace std;

#include <theseus/thePlanningPOMDP.h>
#include <theseus/theTopLevel.h>
#include <theseus/theResult.h>
#include <theseus/theUtils.h>

class childListClass;
static int selectBest( childListClass *children, int num, bool randomTies );


///////////////////////////////////////////////////////////////////////////////
//
// Child List Class
//

class childListClass
{
public:
  planningBeliefClass *belief;
  float value;
};


///////////////////////////////////////////////////////////////////////////////
//
// Planning POMDP Class
//

bool
planningPOMDPClass::applicable( int state, int action ) const
{
  // XXXXXX
  return( false );
}

bool
planningPOMDPClass::applicable( const beliefClass* belief, int action ) const
{
  // XXXXXX
  return( false );
}

int
planningPOMDPClass::getInitialState( const beliefClass* belief ) const
{
  // XXXXXX
  return( 0 );
}

int
planningPOMDPClass::nextState( int state, int action ) const
{
  // XXXXXX
  return( 0 );
}

int
planningPOMDPClass::nextObservation( int state, int action ) const
{
  // be sure this is never called
  assert( 0 );
  return( 0 );
}

void
planningPOMDPClass::algorithm( bool learning, resultClass& result )
{
  register int k, bestChild, bestAction, sim, depth;
  register float bestValue, actionCost;
  register const beliefClass *belief, *savedBelief;
  register stateListClass *children, *child;
  register hashEntryClass *entry;
  static childListClass *childBelief;
  static int initialized = 0;

  // initialization
  if( !initialized )
    {
      initialized = 1;
      childBelief = new childListClass[numActions];
      for( k = 0; k < numActions; ++k )
	childBelief[k].belief = new planningBeliefClass;
    }

  // check we have a PLANNING problem
  assert( ISPLANNING(PD.handle->problemType) );

  // set initial state (belief):
  //   We are using the special planningBeliefClass for
  //   representing single states.
  belief = theInitialBelief;

  // set the initial state value in the result
  result.runType = learning;
  result.initialValue = hashValue( belief )->value;

  // go for it!!!
  result.numMoves = 0;
  result.costToGoal = 0.0; 
  result.discountedCostToGoal = 0.0;
  result.startTimer();

  while( result.numMoves < maxMoves )
    {
      // check for goal state
      if( inGoal( belief ) )
	goto theEnd;

      // save belief
      savedBelief = belief;

      // simulated moves
      for( sim = 0; sim < 0; ++sim )
	{
	  for( depth = 0; depth < 2; ++depth )
	    {
	      // expand state
	      ++expansions;
	      children = planningPOMDPClass::expandState( belief );
	      for( child = children, k = 0; child->probability != -1; ++child, ++k )
		{
		  ++branching;
		  childBelief[k].belief->state = child->state;
		  childBelief[k].value = hashValue( childBelief[k].belief )->value;

		  // insert belief into hash to avoid recomputation of heuristic values
		  if( beliefHash->lookup( (void*)childBelief[k].belief ) == NULL )
		    beliefHash->insert( (void*)childBelief[k].belief, childBelief[k].value );
		}

	      bestChild = selectBest( childBelief, k, randomTies );
	      bestValue = childBelief[bestChild].value;
	      bestAction = children[bestChild].action;
	      actionCost = cost( childBelief[bestChild].belief, bestAction );

	      // update hash value
	      if( learning )
		beliefHash->updateValue( (void*)belief, bestValue + actionCost );

	      // get next state (belief)
	      if( (entry = beliefHash->lookup( (void*)childBelief[bestChild].belief )) == NULL )
		belief = (beliefClass*)beliefHash->insert( (void*)childBelief[bestChild].belief, bestValue )->getData();
	      else
		belief = (beliefClass*)entry->getData();
	    }

	  // restore belief
	  belief = savedBelief;
	}

      // expand state
      ++expansions;
      children = planningPOMDPClass::expandState( belief );
      for( child = children, k = 0; child->probability != -1; ++child, ++k )
	{
	  ++branching;
	  childBelief[k].belief->state = child->state;
	  childBelief[k].value = hashValue( childBelief[k].belief )->value;

	  // insert belief into hash to avoid recomputation of heuristic values
	  if( beliefHash->lookup( (void*)childBelief[k].belief ) == NULL )
	    beliefHash->insert( (void*)childBelief[k].belief, childBelief[k].value );
	}

      bestChild = selectBest( childBelief, k, randomTies );
      bestValue = childBelief[bestChild].value;
      bestAction = children[bestChild].action;
      actionCost = cost( childBelief[bestChild].belief, bestAction );

      // set result
      result.push_back( -1, bestAction, -1 );

      // update hash value
      if( learning )
	beliefHash->updateValue( (void*)belief, bestValue + actionCost );

      // get next state (belief)
      result.costToGoal += actionCost;
      result.discountedCostToGoal += actionCost;

      if( (entry = beliefHash->lookup( (void*)childBelief[bestChild].belief )) == NULL )
	belief = (beliefClass*)beliefHash->insert( (void*)childBelief[bestChild].belief, bestValue )->getData();
      else
	belief = (beliefClass*)entry->getData();
    }

  // maxMoves reached, return fail
  result.numMoves = -1;

  // the end
 theEnd:
  result.stopTimer();
}


///////////////////////
//
// serialization

void
planningPOMDPClass::checkIn( void )
{
  signRegistration( typeid( planningPOMDPClass ).name(),
		    (void (*)( istream&, POMDPClass * )) &planningPOMDPClass::fill,
		    (POMDPClass *(*)( void )) &planningPOMDPClass::constructor );
}

void
planningPOMDPClass::write( ostream& os ) const
{
  const char *id;
  unsigned len;

  // write registrationId
  id = typeid( *this ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  POMDPClass::write( os );
}

planningPOMDPClass *
planningPOMDPClass::read( istream& is )
{
  planningPOMDPClass *pomdp;
  
  pomdp = new planningPOMDPClass;
  fill( is, pomdp );

  return( pomdp );
}

planningPOMDPClass *
planningPOMDPClass::constructor( void )
{
  return( new planningPOMDPClass );
}

void
planningPOMDPClass::fill( istream& is, planningPOMDPClass *pomdp )
{
  POMDPClass::fill( is, pomdp );
}


///////////////////////
//
// static utils functions

static int
selectBest( childListClass *children, int num, bool randomTies )
{
  register int number, idx, j, k;
  register float min;

  // look for min value
  min = (float) INT_MAX;
  number = 0;
  for( k = 0; k < num; ++k )
    if( (children[k].belief->state->valid == 1) && (children[k].value <= min) )
      {
	if( children[k].value < min )
	  {
	    number = 0;
	    min = children[k].value;
	  }
	++number;
      }
  
  // selection
  assert( number > 0 );
  k = (randomTies ? lrand48() % number : 0);
  for( j = 0, idx = 0; j < num; ++j, ++idx )
    if( (children[j].belief->state->valid == 1) && (children[j].value == min) )
      {
	if( k == 0 )
	  break;
	else
	  --k;
      }

  // return
  return( idx );
}
