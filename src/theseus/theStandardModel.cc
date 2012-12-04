//  Theseus
//  theStandardModel.cc -- Standard Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <typeinfo>

using namespace std;

#include <theseus/theStandardModel.h>
#include <theseus/theTopLevel.h>
#include <theseus/theBelief.h>
#include <theseus/theRtStandard.h>
#include <theseus/theRtHash.h>
#include <theseus/theUtils.h>
#include <theseus/theException.h>

extern "C" {
#include <mdp/mdp.h>
extern int gNumGoals;
extern int gGoalList[];
}


///////////////////////////////////////////////////////////////////////////////
//
// Standard Model Class
//

standardModelClass::standardModelClass()
{
#ifndef ALTERNATE_HASH
  stateHash = new stateHashClass;
  invStateHash = new invStateHashClass;
#endif
  observationHash = new observationHashClass;
  invObservationHash = new invObservationHashClass;

  stateIndex = 0;
  observationIndex = 0;

  // simple models
  goalModel = NULL;
  goalModelSize = 0;
  availabilityModel = NULL;
  availabilityModelSize = 0;
  applicationModel = NULL;
  costModel = NULL;

  // complex models
  transitionModel = NULL;
  observationModel = NULL;

#ifdef ALTERNATE_HASH
  // set root of tree
  rootState = NULL;
  root = new stateClass::node_t;
#endif
}

standardModelClass::standardModelClass( const standardModelClass& model )
{
  theInitialBelief = NULL;
}

standardModelClass::~standardModelClass( void )
{
  delete theInitialBelief;

  // simple models
  free( goalModel );
  free( availabilityModel );
  delete[] applicationModel;
  delete[] costModel;

  // complex models
  delete[] transitionModel;
  delete[] observationModel;

#ifndef ALTERNATE_HASH
  delete invStateHash;
  delete stateHash;
#endif
  delete invObservationHash;
  delete observationHash;

#ifdef ALTERNATE_HASH
  // clean node tree
  delete rootState;
  delete root;
#endif
}

void
standardModelClass::setup( problemHandleClass *handle, beliefClass* (*belCtor)( void ) )
{
  register stateListClass *stateList, *state, *tmp;
  register hashEntryClass *entry;
  register beliefClass *belief;

  // fundamental vars
  numActions = handle->numActions;
  stateHash->setSize( handle->stateSize );
  observationHash->setSize( handle->observationSize );

  // simple models allocation
  applicationModel = new set<int>[numActions];
  costModel = new map<int,float>[numActions];

  // complex models
  transitionModel = new map<int,map<int,float> >[numActions];
  observationModel = new map<int,map<int,float> >[numActions];

  // bootstrap problem 
  stateList = (*handle->bootstrap)();

  // insert the nullObservation into the observationHash
  observationHash->insert( handle->theNullObservation, 0, (void*)observationIndex++ );

  // set initial states and initial belief
  belief = (*belCtor)();
  for( state = stateList; state != NULL; state = tmp )
    {
      if( state->state->valid )
	{
	  belief->insert( stateIndex, state->probability );
	  if( PD.verboseLevel >= 100 )
	    {
	      *PD.outputFile << "new state " << stateIndex << endl;
	      if( PD.verboseLevel >= 150 )
		*PD.outputFile << *state->state << endl;
	    }

#ifndef ALTERNATE_HASH
	  // insert state into standard state hash
	  assert( stateHash->lookup( state->state ) == NULL );
	  entry = stateHash->insert( state->state, 0, (void*)stateIndex );
	  invStateHash->insert( (void*)stateIndex, 0, entry->getData() );
#else
	  // insert state into alternate state hash
	  if( rootState == NULL )
	    rootState = state->state->clone();
	  const stateClass::node_t *node = state->state->insert( root, stateIndex );
	  shash[stateIndex] = node;
#endif

	  // increase state counter
	  ++stateIndex;
	}

      // next state, free resources
      tmp = state->next;
      delete state;
    }
  theInitialBelief = belief;
  numInitialStates = stateIndex;
  if( stateIndex == 0 )
    *PD.outputFile << "WARNING: zero initial states." << endl;
}

void
standardModelClass::setup( char *cassandraFilename, beliefClass* (*belCtor)() )
{
  register int action, state, statePrime, observation;
  register beliefClass *belief;
  register float p;
  int absorbing;

  // parse filename with Cassandra's MDP parser
  if( !readMDP(cassandraFilename) ) {
    cerr << "Fatal Error: when reading file: " << cassandraFilename << endl;
    exit(-1);
  }

  // fundamental vars
  numActions = gNumActions;
  discountFactor = 1;
  //discountFactor = gDiscount;

  // simple models allocation
  applicationModel = new set<int>[numActions];
  costModel = new map<int,float>[numActions];

  // complex models allocation
  transitionModel = new map<int,map<int,float> >[numActions];
  observationModel = new map<int,map<int,float> >[numActions];

  // compute maxReward
  float maxReward = 0.0;
  for( state = 0; state < gNumStates; ++state ) {
    for( action = 0; action < numActions; ++action ) {
      p = getEntryMatrix(Q,action,state);
      maxReward = max(maxReward,p);
    }
  }

  // compute model:
  for( state = 0; state < gNumStates; ++state ) {
    for( action = 0; action < numActions; ++action ) {
      if( PD.signal >= 0 ) goto end;

      // cost model: c( s, a )
      p = getEntryMatrix(Q,action,state);
      costModel[action][state] = -(p - (maxReward+1));
#if 0
      costModel[action][state] = -(p - (maxReward+0));
      if( p == maxReward ) {
        transitionModel[action][state].insert(make_pair(state,1));
        observationModel[action][state][gNumObservations] = 1.0;
        applicationModel[action].insert(state);
        setGoal(state);
      }
      else {
#endif

      // transition model: P( s' | s, a )
      for( statePrime = 0; statePrime < gNumStates; ++statePrime ) {
        if( (p=getEntryMatrix(P[action],state,statePrime)) > 0.0 ) {
          p *= gDiscount;
          map<int,float>::iterator it = transitionModel[action][state].find(statePrime);
          if( it == transitionModel[action][state].end() )
            transitionModel[action][state].insert(make_pair(statePrime,p));
          else
            (*it).second = p;
        }
      }

      // observation model: P( o | s, a )
      for( observation = 0; observation < gNumObservations; ++observation ) {
        if( (p=getEntryMatrix(R[action],state,observation)) > 0.0 )
          observationModel[action][state][observation] = p;
      }

      // application model
      applicationModel[action].insert(state);
      //}
    }

    // availability model
    setAvailability(state);
  }

  // set model for absorving state
#if 1
  absorbing = gNumStates;
  for( action = 0; action < numActions; ++action ) {
    transitionModel[action][absorbing].insert(make_pair(absorbing,1));
    for( state = 0; state < gNumStates; ++state ) {
      transitionModel[action][state].insert(make_pair(absorbing,1-gDiscount));
    }
    observationModel[action][absorbing][gNumObservations] = 1;
    applicationModel[action].insert(absorbing);
  }
  setAvailability(absorbing);
#endif

  // goal model
  setGoal(absorbing);
  //for( int i = 0; i < gNumGoals; ++i ) setGoal(gGoalList[i]);

  // set the initial belief
  numInitialStates = 0;
  belief = (*belCtor)();
  for( state = 0; state < gNumStates; ++state ) {
    if( gInitialBelief[state] > 0.0 ) {
      ++numInitialStates;
      belief->insert(state,(float)gInitialBelief[state]);
    }
  }
  theInitialBelief = belief;

  // set some model vars
  stateIndex = 1+gNumStates;
  observationIndex = 1+gNumObservations;

 end:
  // check for signals 
  if( PD.signal >= 0 ) {
    int s = PD.signal;
    PD.signal = -1;
    throw signalExceptionClass(s);
  }
}

void
standardModelClass::computeWholeModelFrom( const beliefClass *belief )
{
  const map<int,float> *m;
  map<int,float>::const_iterator it;

  m = belief->cast();
  for( it = m->begin(); it != m->end(); ++it )
    computeWholeModelFrom( (*it).first );
}

void
standardModelClass::computeWholeModelFrom( int sIndex )
{
  list<int> stateQueue, newStates;

  stateQueue.push_front( sIndex );
  while( !stateQueue.empty() )
    {
      sIndex = stateQueue.front();
      stateQueue.pop_front();

      if( !isAvailable( sIndex ) )
	computeModelFor( sIndex, newStates );

      while( !newStates.empty() )
	{
	  stateQueue.push_back( newStates.front() );
	  newStates.pop_front();
	}
    }
}

void
standardModelClass::printState( int state, ostream& os, int indent ) const
{
  hashEntryClass *entry = invStateHash->lookup( (void*)state );
  if( !entry )
    {
      for( int i = 0; i < indent; ++i )
	os << " ";
      os << "Invalid state." << endl;
    }
  else
    ((stateClass*)entry->getExtra())->print( os, indent );
}

void
standardModelClass::printObservation( int obs, ostream& os, int indent ) const
{
  hashEntryClass *entry = invObservationHash->lookup( (void*)obs );
  if( !entry )
    {
      for( int i = 0; i < indent; ++i )
	os << " ";
      os << "Invalid observation." << endl;
    }
  else
    ((observationClass*)entry->getExtra())->print( os, indent );
}

void
standardModelClass::printData( ostream& os, const char *prefix ) const
{
  os << (prefix?prefix:"%model ") << "numberActions " << numActions << endl;
  os << (prefix?prefix:"%model ") << "numberStates " << stateIndex << endl;
  os << (prefix?prefix:"%model ") << "numberObservations " << observationIndex << endl;
  os << (prefix?prefix:"%model ") << "numberInitialStates " << numInitialStates << endl;
}

void
standardModelClass::printModel( ostream &os ) const
{
  set<int>::const_iterator it1;
  map<int,float>::const_iterator it2;

#if 0
  // print states
  for( int state = 0; state < stateIndex; ++state )
    {
      os << "state " << state << " = " << endl;
      printState( state, os, 4 );
      os << endl;
    }
#endif

  // print application model
  for( int action = 0; action < numActions; ++action )
    {
      os << "applicationModel[" << action << "] = { ";
      for( it1 = applicationModel[action].begin(); it1 != applicationModel[action].end(); ++it1 )
	os << *it1 << " ";
      os << "}" << endl;
    }

#if 0
  // print goal model
  os << "goalModel = { ";
  for( it1 = goalModel.begin(); it1 != goalModel.end(); ++it1 )
    os << *it1 << " ";
  os << "}" << endl;
#endif

  // print cost model
  for( int action = 0; action < numActions; ++action )
    {
      os << "costModel" << action << "] = { ";
      for( it2 = costModel[action].begin(); it2 != costModel[action].end(); ++it2 )
	os << "(" << (*it2).first << "," << (*it2).second << ") ";
      os << "}" << endl;
    }

  // print transition model
  for( int action = 0; action < numActions; ++action )
    {
      os << "transitionModel for action (" << action << ") ";
      (*PD.handle->printAction)( os, action );
      os << endl;
      for( int state = 0; state < stateIndex; ++state )
	if( transitionModel[action].find( state ) != transitionModel[action].end() )
	  {
	    os << state << ": ";
	    for( it2=transitionModel[action][state].begin(); it2!=transitionModel[action][state].end(); ++it2 )
	      os << "(" << (*it2).first << "," << (*it2).second << ") ";
	    os << endl;
	  }
    }

  // print observation model
  for( int action = 0; action < numActions; ++action )
    {
      os << "observationModel[" << action << "]:" << endl;
      for( int state = 0; state < stateIndex; ++state )
	if( observationModel[action].find( state ) != transitionModel[action].end() )
	  {
	    os << state << ": ";
	    for( it2=transitionModel[action][state].begin(); it2!=transitionModel[action][state].end(); ++it2 )
	      os << "(" << (*it2).first << "," << (*it2).second << ") ";
	    os << endl;
	  }
    }
}

int
standardModelClass::newState( void )
{
  return( stateIndex++ );
}

void
standardModelClass::newTransition( int state, int action, int statePrime, float probability )
{
  map<int,float>::iterator it;
  applicationModel[action].insert( state );
  it = transitionModel[action][state].find( statePrime );
  if( it == transitionModel[action][state].end() )
    transitionModel[action][state].insert( make_pair(statePrime,probability) );
  else
    (*it).second = probability;
}

const stateClass*
standardModelClass::getState( int index ) const
{
  return( (const stateClass*)invStateHash->lookup( (void*)index )->getExtra() );
}

void
standardModelClass::checkModelFor( int sIndex )
{
  list<int> newStates;

  if( !isAvailable( sIndex ) )
    computeModelFor( sIndex, newStates );
}

void
standardModelClass::computeModelFor( int sIndex, list<int> &newStates )
{
  register int statePrimeIndex, observation;
  register const stateClass *state;
  register stateListClass *stateList, *sListPtr;
  register hashEntryClass *entry;
  register observationListClass *oListPtr;
  map<int,float>::iterator it;

#ifdef ALTERNATE_HASH
  map<int,const stateClass::node_t*>::const_iterator it2
  const stateClass::node_t *node;
#endif

#ifndef ALTERNATE_HASH
  // lookup state index in standard hash
  state = (stateClass*)invStateHash->lookup( (void*)sIndex )->getExtra();
#else
  // lookup state index in alternate hash
  it2 = shash.find( sIndex );
  assert( it2 != shash.end() );
  recover( (*it2).second, rootState );
  state = rootState;
#endif

  // fill goalModel
  if( state->goal() )
    {
      setGoal( sIndex );
    }

  // expand state
  stateList = state->applyAllActions();

  // initialize cost model for applicable actions
  for( sListPtr = stateList; sListPtr->probability != -1; ++sListPtr )
    if( sListPtr->state->valid )
      costModel[sListPtr->action].insert( make_pair( sIndex, (float)0.0 ) );

  // compute transition and observation models
  for( sListPtr = stateList; sListPtr->probability != -1; ++sListPtr )
    {
      if( sListPtr->state->valid && (sListPtr->probability > 0.0)  )
	{
	  // check signals
	  if( PD.signal >= 0 )
	    goto cleanup;

	  // lookup statePrimeIndex
#ifndef ALTERNATE_HASH
	  if( !(entry = stateHash->lookup( sListPtr->state )) )
#else
          if( !(node = sListPtr->state->lookup( root )) )
#endif
	    {
	      // we have a new state
	      statePrimeIndex = stateIndex++;
	      newStates.push_front( statePrimeIndex );
	      if( PD.verboseLevel >= 100 )
		{
		  *PD.outputFile << "new state " << statePrimeIndex << endl;
		  if( PD.verboseLevel >= 150 )
		    *PD.outputFile << *sListPtr->state << endl;
		}

#ifndef ALTERNATE_HASH
	      // insert state into standard state hash
	      entry = stateHash->insert( sListPtr->state, 0, (void*)statePrimeIndex );
	      invStateHash->insert( (void*)statePrimeIndex, 0, entry->getData() );
#else
	      // insert state into alternate state hash
	      node = sListPtr->state->insert( root, statePrimeIndex );
	      shash[statePrimeIndex] = node;
#endif
	    }
	  else
	    {
#ifndef ALTERNATE_HASH
	      statePrimeIndex = (intptr_t)entry->getExtra();
#else
	      statePrimeIndex = (int)node->children;
#endif
	    }

	  // insert transition into model: P( s' | s, a )
	  it = transitionModel[sListPtr->action][sIndex].find( statePrimeIndex );
	  if( it == transitionModel[sListPtr->action][sIndex].end() )
	    transitionModel[sListPtr->action][sIndex].insert(make_pair(statePrimeIndex,sListPtr->probability));
	  else
	    (*it).second += sListPtr->probability;

	  if( PD.verboseLevel >= 100 )
	    {
	      *PD.outputFile << "T(s'=" << statePrimeIndex << "|s=" << sIndex;
	      *PD.outputFile << ",a=" << sListPtr->action << ") = ";
	      *PD.outputFile << transitionModel[sListPtr->action][sIndex][statePrimeIndex] << endl;
	    }

	  // observation model: P( o | s, a )
	  if( observationModel[sListPtr->action].find(statePrimeIndex) == 
	      observationModel[sListPtr->action].end() )
	    {
	      if( (sListPtr->observations == NULL) || (sListPtr->observations->probability == -1) )
		{
		  // We have an action that returns no observation.
		  // Insert nullObservation with 1.0 probability.
		  observationModel[sListPtr->action][statePrimeIndex][0] = 1.0;
		}
	      else
		{
		  for( oListPtr = sListPtr->observations; oListPtr->probability != -1; ++oListPtr )
		    if( oListPtr->probability > 0.0 )
		      {
			// We need to go to the observationHash in order to get the observationIndex
			if( !(entry = observationHash->lookup( oListPtr->observation )) )
			  {
			    // we have a new observation
			    observation = observationIndex++;
			    entry = observationHash->insert( oListPtr->observation, 0, (void*)observation );
			    invObservationHash->insert( (void*)observation, 0, entry->getData() );
			    if( PD.verboseLevel >= 100 )
			      {
				*PD.outputFile << "new observation " << observation << " = ";
				oListPtr->observation->print( *PD.outputFile, 0 );
			      }
			  }
			else
			  {
			    observation = (intptr_t) entry->getExtra();
			  }

			// insert observation into model
			it = observationModel[sListPtr->action][statePrimeIndex].find( observation );
			if( it == observationModel[sListPtr->action][statePrimeIndex].end() )
			  observationModel[sListPtr->action][statePrimeIndex].insert(make_pair(observation,oListPtr->probability));
			else
			  (*it).second += oListPtr->probability;

			if( PD.verboseLevel >= 100 )
			  {
			    *PD.outputFile << "O(o=" << observation << "|s= "<<statePrimeIndex;
			    *PD.outputFile << ",a=" << sListPtr->action << ") = ";
			    *PD.outputFile << observationModel[sListPtr->action][statePrimeIndex][observation];
			    *PD.outputFile << endl;
			  }

			assert( (observationModel[sListPtr->action][statePrimeIndex][observation] >= 0.0) &&
				(observationModel[sListPtr->action][statePrimeIndex][observation] <= 1.0) );
		      }
		}
	    }

	  // cost model: c( s, a ) = \sum_{s'} c( s, a, s' ) * P( s' | s, a' )
	  costModel[sListPtr->action][sIndex] += (sListPtr->probability * sListPtr->cost);
	  if( PD.outputLevel >= 100 )
	    {
	      *PD.outputFile << "cost(s=" << sIndex << ",a=" << sListPtr->action << ") = ";
	      *PD.outputFile << costModel[sListPtr->action][sIndex] << endl;
	    }

	  // set application model and inversion transition model
	  applicationModel[sListPtr->action].insert( sIndex );
#if 0
	  invTransitionModel[sIndex].insert( sListPtr->action );
#endif
	}
    }

  // cleanup
 cleanup:
  for( sListPtr = stateList; sListPtr->probability != -1; ++sListPtr )
    delete[] sListPtr->observations;

#if 0
  {
    void memoryUsage(void);
    memoryUsage();
  }
#endif

  // check signals 
  if( PD.signal >= 0 )
    {
      int s = PD.signal;
      PD.signal = -1;
      throw signalExceptionClass( s );
    }

  // update availability model
  setAvailability( sIndex );
}

int
standardModelClass::getInitialState( const beliefClass *belief ) const
{
  return( belief->randomSampling() );
}

float
standardModelClass::cost( int state, int action ) const
{
  assert( action < numActions );
  if( costModel[action].find(state) == costModel[action].end() )
    return( 0.0 );
  else
    return( costModel[action][state] );
}

float
standardModelClass::cost( const beliefClass *belief, int action ) const
{
  register float sum;
  const map<int,float> *m;
  map<int,float>::const_iterator it;

  assert( belief != NULL );

  sum = 0.0;
  m = belief->cast();
  for( it = m->begin(); it != m->end(); ++it )
    sum += cost( (*it).first, action ) * (*it).second;
  return( sum );
}

bool
standardModelClass::applicable( int state, int action ) const
{
  assert( action < numActions );
  return( applicationModel[action].find(state) != applicationModel[action].end() );
}

bool
standardModelClass::applicable( const beliefClass* belief, int action ) const
{
  const map<int,float> *m;
  map<int,float>::const_iterator it;

  assert( belief != NULL );

  // check applicability 
  m = belief->cast();
  for( it = m->begin(); it != m->end(); ++it )
    if( !applicable( (*it).first, action ) )
      return( false );
  return( true );
}

bool
standardModelClass::inGoal( int state ) const
{
  return( isGoal( state ) );
}

bool
standardModelClass::inGoal( const beliefClass* belief ) const
{
  const map<int,float> *m;
  map<int,float>::const_iterator it;

  assert( belief != NULL );
  if( PD.pddlProblemType && (PD.handle->otherInfo & (1<<problemHandleClass::CERTAINTY_GOAL)) )
    {
      return( belief->supportSize() == 1 );
    }
  else
    {
      m = belief->cast();
      for( it = m->begin(); it != m->end(); ++it )
	if( !isGoal( (*it).first ) )
	  return( false );
      return( true );
    }
}

int
standardModelClass::nextState( int state, int action ) const
{
  assert( transitionModel[action].find(state) != transitionModel[action].end() );
  return( randomSampling( transitionModel[action][state] ) );
}

int
standardModelClass::nextObservation( int state, int action ) const
{
  // if there is no observation model, return the null observation
  assert( observationModel[action].find(state) != observationModel[action].end() );
  return( randomSampling( observationModel[action][state] ) );
}

///////////////////////
//
// serialization

void
standardModelClass::checkIn( void )
{
  signRegistration( typeid( standardModelClass ).name(),
		    (void (*)( istream&, modelClass * )) &standardModelClass::fill,
		    (modelClass* (*)( void )) &standardModelClass::constructor );
}

void
standardModelClass::write( ostream& os ) const
{
  const char *id;
  unsigned len;
  int action;

  // write registrationId
  id = typeid( standardModelClass ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  modelClass::write( os );

  // searialize basic variables
  safeWrite( &stateIndex, sizeof( int ), 1, os );
  safeWrite( &observationIndex, sizeof( int ), 1, os );

  //xxxx must write state & observation hashes
  //xxxx stateHashClass       stateHash;
  //xxxx observationHashClass observationHash;

  // serialize models
  //xxxx os << availabilityModel;
  //xxxx os << goalModel;
  for( action = 0; action < numActions; ++action )
    {
      os << applicationModel[action];
      os << costModel[action];
      os << transitionModel[action];
      os << observationModel[action];
    }
#if 0
  os << invTransitionModel;
#endif
}

standardModelClass*
standardModelClass::read( istream& is )
{
  standardModelClass *model;

  model = new standardModelClass;
  fill( is, model );

  return( model );
}

standardModelClass*
standardModelClass::constructor( void )
{
  return( new standardModelClass );
}

void
standardModelClass::fill( istream& is, standardModelClass *model )
{
  int action;

  // fill base class
  modelClass::fill( is, model );

  // fill basic variables
  safeRead( &model->stateIndex, sizeof( int ), 1, is );
  safeRead( &model->observationIndex, sizeof( int ), 1, is );

  //xxxx must read state & observation hahshes
  //xxxx stateHashClass       stateHash;
  //xxxx observationHashClass observationHash;

  // allocate space for filling models
  model->applicationModel = new set<int>[model->numActions];
  model->costModel = new map<int,float>[model->numActions];
  model->transitionModel = 
    new map<int,map<int,float> >[model->numActions];
  model->observationModel = 
    new map<int,map<int,float> >[model->numActions];

  // fill models
  //xxxx is >> model->availabilityModel;
  //xxxx is >> model->goalModel;
  for( action = 0; action < model->numActions; ++action )
    {
      is >> model->applicationModel[action];
      is >> model->costModel[action];
      is >> model->transitionModel[action];
      is >> model->observationModel[action];
    }
#if 0
  is >> model->invTransitionModel;
#endif
}
