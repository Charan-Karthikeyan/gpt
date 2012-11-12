//  Theseus
//  theMDPPOMDP.cc -- MDP-POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifdef linux
#include <values.h>
#endif
#include <math.h>
#include <iostream>
#include <typeinfo>
#include <queue>
#include <deque>
#include <stack>

using namespace std;

#include <theseus/theMDPPOMDP.h>
#include <theseus/theTopLevel.h>
#include <theseus/theStandardModel.h>
#include <theseus/theResult.h>
#include <theseus/theException.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// MDP POMDP Class
//

const hashEntryClass*
MDPPOMDPClass::getTheInitialBelief( void )
{
  hashEntryClass *entry = beliefHash->lookup( (const void*)theInitialBelief );
  if( !entry )
    entry = beliefHash->insert((const void*)theInitialBelief,hashValue((beliefClass*)theInitialBelief)->value);
  return( entry );
}

int
MDPPOMDPClass::nextObservation( int state, int action ) const
{
  // should not be called
  assert( 0 );
  return( 0 );
}

POMDPClass::hashValueClass*
MDPPOMDPClass::hashValue( const beliefClass* belief )
{
  hashEntryClass *entry;
  static hashValueClass result;

  if( !(entry = beliefHash->lookup( (const void*)belief )) )
    {
      result.value = 0.0; //xxxxxxx MDPHeuristicClass::value( belief );
      result.solved = false;
    }
  else
    {
      result.value = entry->getValue();
      result.solved = (bool)entry->getExtra();
    }

  // return value
  return( &result );
}

int
MDPPOMDPClass::getBestAction( const hashEntryClass *entry )
{
  QValueClass* QResult = bestQValue( (beliefClass*)entry->getData(), true );
  if( QResult->numTies > 0 )
    return( QResult->ties[0] );
  else
    return( -1 );
}

void
MDPPOMDPClass::expandBeliefWithAction( const hashEntryClass *entry, int action,
				       deque<pair<pair<int,float>,const hashEntryClass*> > &result )
{
  result.clear();
  int state = (int)entry->getData();
  map<int,float>::const_iterator it;
  assert( ((standardModelClass*)model)->transitionModel[action].find( state ) !=
	  ((standardModelClass*)model)->transitionModel[action].end() );
  for( it = ((standardModelClass*)model)->transitionModel[action][state].begin();
       it != ((standardModelClass*)model)->transitionModel[action][state].end(); ++it )
    result.push_back( make_pair( make_pair(0,(*it).second),
				 getHashEntry( (beliefClass*)(*it).first ) ) );
}

void
MDPPOMDPClass::algorithm( bool learning, resultClass& result )
{
  register float              realCost; 
  register int                idx, bestAction, state;
  register QValueClass        *QResult;
  register hashEntryClass     *entry;
  hashValueClass              *hValue;

  // STOPPING RULE
  stack<hashEntryClass*> Stack;

  // check we have a POMDP problem
  assert( ISMDP(PD.handle->problemType) || ISNDMDP(PD.handle->problemType) );

  // initialize system
  state = (int)theInitialBelief;
  hValue = hashValue( (beliefClass*)state );
  if( !(entry = beliefHash->lookup( (const void*)state )) )
    entry = beliefHash->insert( (void*)state, hValue->value );

  // update value for INIT
  QResult = bestQValue( (beliefClass*)state, true );
  assert( QResult->numTies > 0 );
  if( learning )
    {
      entry->updateValue( QResult->value );
      if( PD.useStopRule )
	Stack.push( entry );
    }

  // jump to starting state
  state = nextState( state, 0 );
  hValue = hashValue( (beliefClass*)state );
  if( !(entry = beliefHash->lookup( (const void*)state )) )
    entry = beliefHash->insert( (void*)state, hValue->value );

  // initialize result
  result.runType = learning;
  result.initialValue = hValue->value;
  result.solved = hValue->solved;
  result.numMoves = 0;
  result.costToGoal = 0.0;
  result.discountedCostToGoal = 0.0;
  result.startTimer();

  // go for it!!!
  while( result.numMoves < maxMoves )
    {
      // check soundness of belief and simulated system state
      assert( entry != NULL );
      assert( state == (int)entry->getData() );

      //xxxx cout << "state = " << state << ", value = " << entry->getValue() << endl;
      //xxxx model->printState( state, cout, 4 );

      // be sure we have the model for this belief
      model->checkModelFor( state );

      // STOPPING RULE
      if( PD.useStopRule )
	{
	  // check for goal state (if in learning trial check also for solved)
	  if( inGoal( state ) ||
	      (learning && (entry != NULL) && (bool)entry->getExtra()) )
	    {
	      entry->setExtra( (void*)true );
	      goto theEnd;
	    }
	}
      else
	{
	  // check for goal state
	  if( inGoal( state ) )
	    goto theEnd;
	}

      // compute the best QValues
      QResult = bestQValue( (beliefClass*)state, true );

      // greedy selection of best action
      if( QResult->numTies > 0 )
	{
	  idx = (!randomTies ? 0 : lrand48() % QResult->numTies);
	  bestAction = QResult->ties[idx];
	}
      else
	{
	  // we have a dead-end
	  if( learning )
	    {
	      entry->updateValue( FLT_MAX );
	      entry->setExtra( (void*)true );
	    }
	  result.costToGoal = -1.0;
	  result.discountedCostToGoal = -1.0;
	  break;
	}
      //xxxx cout << "applying action = " << bestAction << endl;

      // update hash value (if learning)
      if( learning )
	{
	  assert( !PD.useStopRule || !(bool)entry->getExtra() ||
		  (fabs( (double)(entry->getValue()-QResult->value) ) < PD.SREpsilon) );
	  entry->updateValue( QResult->value );

	  // STOPPING RULE
	  if( PD.useStopRule )
	    Stack.push( entry );
	}

      // get next state
      realCost = cost( state, bestAction );
      result.costToGoal += realCost;
      result.discountedCostToGoal = realCost + discountFactor * result.discountedCostToGoal;

      // set result
      result.push_back( state, bestAction, -1 );

      // get next state
      state = nextState( state, bestAction );

      // set new belief
      if( !(entry = beliefHash->lookup( (const void*)state )) )
	entry = beliefHash->insert( (const void*)state, hashValue( (beliefClass*)state )->value );
    }

  // dead-end reached, return fail
  result.numMoves = -1;

  // this is the end, ...
 theEnd:
  result.stopTimer();

  // check for abortion
  if( PD.signal >= 0 )
    {
      // cleanup
      int s = PD.signal;
      PD.signal = -1;
      if( PD.useStopRule )
	for( ; !Stack.empty(); Stack.pop() );
      throw( signalExceptionClass( s ) );
    }

  // STOPPING RULE
  if( PD.useStopRule )
    {
      list<hashEntryClass*> closed;
      list<hashEntryClass*>::iterator it;

      while( !Stack.empty() )
	{
	  // get entry
	  entry = Stack.top();
	  Stack.pop();

	  // check if already solved
	  if( !(bool)entry->getExtra() )
	    {
	      // try labeling
	      closed.clear();
	      if( checkSolved( entry, closed ) )
		{
		  for( it = closed.begin(); it != closed.end(); ++it )
		    {
		      (*it)->setExtra( (void*)true );
		      *PD.outputFile << "solved state " << (int)(*it)->getData();
		      *PD.outputFile << ", value = " << (*it)->getValue() << endl;
		    }
		}
	      else
		break;
	    }
	}

      // clean stack
      for( ; !Stack.empty(); Stack.pop() );
    }
}

float
MDPPOMDPClass::QValue( const beliefClass* belief, int action, bool useCache )
{
  register int state;
  register float sum;
  register hashValueClass *hValue;
  map<int,float>::const_iterator it;

  state = (int)belief;
  if( applicable( state, action ) )
    {
      sum = 0.0;
      for( it = ((standardModelClass*)model)->transitionModel[action][state].begin();
	   it != ((standardModelClass*)model)->transitionModel[action][state].end(); ++it )
	{
	  // update sum
	  hValue = hashValue( (beliefClass*)(*it).first );
	  switch( PD.handle->problemType )
	    {
	    case problemHandleClass::PROBLEM_MDP:
	      sum += ((*it).second * hValue->value);
	      break;
	    case problemHandleClass::PROBLEM_ND_MDP:
	      sum = (sum == 0.0 ? hValue->value : (hValue->value > sum ? hValue->value : sum));
	      break;
	    default:
	      throw( unsupportedModelExceptionClass( PD.handle->problemType ) );
	    }
	}
      sum = model->cost( state, action ) + discountFactor * sum;
    }
  else
    {
      sum = -1.0;
    }
  return( sum );
}

MDPPOMDPClass::QValueClass*
MDPPOMDPClass::bestQValue( const beliefClass* belief, bool useCache )
{
  register int action, noBest, state;
  register float sum;
  hashValueClass *hValue;
  static int initialized = 0;
  static QValueClass *result;
  map<int,float>::const_iterator it;

  // assertion and initialization
  if( !initialized )
    {
      initialized = 1;
      result = new QValueClass( numActions );
    }

  noBest = 1;
  ++expansions;
  result->numTies = 0;
  state = (int)belief;
  for( action = 0; action < numActions; ++action )
    if( applicable( state, action ) )
      {
	// update statistics
	++branching;

	// compute QValue
	sum = 0.0;
	for( it = ((standardModelClass*)model)->transitionModel[action][state].begin();
	     it != ((standardModelClass*)model)->transitionModel[action][state].end(); ++it )
	  {
	    hValue = hashValue( (beliefClass*)(*it).first );

	    // update sum
	    switch( PD.handle->problemType )
	      {
	      case problemHandleClass::PROBLEM_MDP:
		sum += ((*it).second * hValue->value);
		break;
	      case problemHandleClass::PROBLEM_ND_MDP:
		sum = (sum == 0.0 ? hValue->value : (hValue->value > sum ? hValue->value : sum));
		break;
	      default:
		throw( unsupportedModelExceptionClass( PD.handle->problemType ) );
	      }
	  }
	sum = model->cost( (int)belief, action ) + discountFactor * sum;

	// look for the best action
	if( noBest || best( sum, result->value ) || (sum == result->value) )
	  {
	    if( noBest || (sum != result->value) )
	      result->numTies = 0;
	    noBest = 0;
	    result->value = sum;
	    result->ties[result->numTies] = action;
	    ++result->numTies;
	  }
      }

  // if we have a dead-end, then result->numTies is zero.
  return( result );
}

bool
MDPPOMDPClass::checkSolved( hashEntryClass *current, list<hashEntryClass*>& closed )
{
  register int state, action;
  register QValueClass  *QResult;
  register hashEntryClass *entry;
  map<int,float>::const_iterator it;
  list<hashEntryClass*> open;
  set<hashEntryClass*> aux;

  // initialization
  aux.clear();
  closed.clear();
  open.clear();
  if( !(bool)current->getExtra() )
    open.push_front( current );

  // bfs
  bool rv = true;
  while( !open.empty() )
    {
      // get first from queue
      current = open.front();
      open.pop_front();
      closed.push_front( current );

      // check epsilon condition 
      state = (int)current->getData();
      model->checkModelFor( state );
      QResult = bestQValue( (beliefClass*)state, true );
      if( QResult->numTies == 0 )
	{
	  // dead-end
	  current->updateValue( FLT_MAX );
	  current->setExtra( (void*)true );
	  rv = false;
	  continue;
	}
      if( fabs( QResult->value - current->getValue() ) > PD.SREpsilon )
	{
	  rv = false;
	  continue;
	}
      action = QResult->ties[0];

      // unfold control
      for( it = ((standardModelClass*)model)->transitionModel[action][state].begin();
	   it != ((standardModelClass*)model)->transitionModel[action][state].end(); ++it )
	if( (*it).second > 0.0 )
	  {
	    entry = beliefHash->lookup( (const void*)(*it).first );
	    model->checkModelFor( (*it).first );
	    if( entry == NULL )
	      {
		// should use heuristic but don't have one... :-(
		entry = beliefHash->insert( (const void*)(*it).first, 0.0 );
	      }

	    if( !inGoal( (*it).first ) )
	      {
		if( !(bool)entry->getExtra() && (aux.find(entry) == aux.end()) )
		  {
		    open.push_front( entry );
		    aux.insert( entry );
		  }
	      }
	  }
    }

  // process nodes in dfs postorder
  if( !rv )
    {
      while( !closed.empty() )
	{
	  current = closed.front();
	  closed.pop_front();
	  QResult = bestQValue( (beliefClass*)current->getData(), true );
	  if( QResult->numTies > 0 )
	    current->updateValue( QResult->value );
	}
    }
  return( true );
}


///////////////////////
//
// serialization

void
MDPPOMDPClass::checkIn( void )
{
  signRegistration( typeid( MDPPOMDPClass ).name(),
		    (void (*)( istream&, POMDPClass * )) &MDPPOMDPClass::fill,
		    (POMDPClass *(*)( void )) &MDPPOMDPClass::constructor );
}

void
MDPPOMDPClass::write( ostream& os ) const
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

MDPPOMDPClass *
MDPPOMDPClass::read( istream& is )
{
  MDPPOMDPClass *pomdp;
  
  pomdp = new MDPPOMDPClass;
  fill( is, pomdp );

  return( pomdp );
}

MDPPOMDPClass *
MDPPOMDPClass::constructor( void )
{
  return( new MDPPOMDPClass );
}

void
MDPPOMDPClass::fill( istream& is, MDPPOMDPClass *pomdp )
{
  POMDPClass::fill( is, pomdp );
}
