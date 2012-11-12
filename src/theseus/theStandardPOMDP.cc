//  Theseus
//  theStandardPOMDP.cc -- Standard POMDP Implementation
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

#include <theseus/theStandardPOMDP.h>
#include <theseus/theTopLevel.h>
#include <theseus/theBelief.h>
#include <theseus/theResult.h>
#include <theseus/theException.h>
#include <theseus/theUtils.h>

// static variables
static standardPOMDPClass::QValueClass *bestQValueStaticResult = 0;


///////////////////////////////////////////////////////////////////////////////
//
// Standard POMDP Class
//

// static members
int standardPOMDPClass::numberStates = 0;

void
standardPOMDPClass::finalize( void )
{
  delete bestQValueStaticResult;
  bestQValueStaticResult = 0;
}

void
standardPOMDPClass::initRun( int run )
{
  if( quantizationForInitialBelief == 0 ) {
    beliefHash->quantize(true,true,theInitialBelief,quantizationForInitialBelief);
  }
}

void
standardPOMDPClass::endRun( bool lastRun )
{
  if( !lastRun ) POMDPClass::endRun(lastRun);
}

const hashEntryClass*
standardPOMDPClass::getTheInitialBelief( void )
{
  if( quantizationForInitialBelief == 0 ) {
    beliefHash->quantize(true,true,theInitialBelief,quantizationForInitialBelief);
  }
  return(quantizationForInitialBelief);
}

POMDPClass::hashValueClass*
standardPOMDPClass::hashValue( const beliefClass* belief )
{
  hashEntryClass *hashEntry;
  static hashValueClass result;
  assert( belief != 0 );
  beliefHash->quantize(true,true,belief,hashEntry);
  result.value = hashEntry->getValue();
  result.solved = (bool)hashEntry->getExtra();
  return(&result);
}

int
standardPOMDPClass::getBestAction( const hashEntryClass *belief )
{
  int bestAction;
  QValueClass* QResult;

  QResult = bestQValue( (const beliefClass*)belief->getData(),true);
  if( QResult->numTies > 0 )
    bestAction = QResult->ties[0];
  else
    bestAction = -1;
  return(bestAction);
}

void
standardPOMDPClass::expandBeliefWithAction( const hashEntryClass *entry, int action, deque<pair<pair<int,float>,const hashEntryClass*> >&result )
{
  hashEntryClass *quantization;
  static std::map<int,float> possibleObservations;

  result.clear();
  const beliefClass *belief = (const beliefClass*)entry->getData();
  if( applicable(belief,action) ) {
    // compute belief_a and possible observations
    beliefClass *belief_a = belief->updateByA(actionCache,true,action);
    possibleObservations.clear();
    belief_a->nextPossibleObservations(action,possibleObservations);
    assert( !possibleObservations.empty() );

    for( std::map<int,float>::const_iterator it = possibleObservations.begin(); it != possibleObservations.end(); ++it ) {
      if( (*it).second > 0.0 ) {
        // compute bel_ao(s)
        beliefClass *belief_ao = belief_a->updateByAO(observationCache,true,action,(*it).first);
        beliefHash->quantize(true,true,belief_ao,quantization);
        result.push_back( make_pair(make_pair((*it).first,(*it).second),quantization) );
      }
    }
  }
}

void
standardPOMDPClass::algorithm( bool learning, resultClass& result )
{
  // stopping rule
  stack<hashEntryClass*> Stack;

  // check we have a POMDP problem
  assert( !PD.pddlProblemType || ISPOMDP(PD.handle->problemType) || ISNDPOMDP(PD.handle->problemType) );

  // initialize result
  result.runType = learning;
  result.initialValue = quantizationForInitialBelief->getValue();
  result.solved = (bool)quantizationForInitialBelief->getExtra();
  result.numMoves = 0;
  result.costToGoal = 0.0;
  result.discountedCostToGoal = 0.0;
  result.startTimer();

  // set initial belief and state
  beliefClass *belief = 0;
  hashEntryClass *quantization = 0;
  if( learning ) {
    quantization = quantizationForInitialBelief;
    if( PD.version == problemClass::VERSION_GRID )
      belief = ((beliefClass*)quantization->getData())->clone();
    else
      belief = theInitialBelief->clone();
  }
  else {
    quantization = 0;
    belief = theInitialBelief->clone();
  }
  int state = getInitialState(belief);

  // go for it!!!
  for( ; (PD.signal < 0) && (result.numMoves < maxMoves); ++result.numMoves ) {
    // check soundness of belief and simulated system state
    assert( belief->check(state) );
    if( PD.verboseLevel >= 30 ) {
      *PD.outputFile << "state = " << state << ", belief = " << *belief;
      if( PD.verboseLevel >= 40 ) *PD.outputFile << ", quantization = " << quantization;
      *PD.outputFile << ", HashValue = " << quantization->getValue() << endl;
    }

    // be sure we have the model for this belief
    belief->checkModelAvailability(model);

    // stopping rule
    if( learning && PD.useStopRule ) {
      // check for goal state (if in learning trial check also for solved)
      assert( quantization != 0 );
      if( inGoal(belief) || (learning && (bool)quantization->getExtra()) ) {
        result.push_back(state,-1,-1);
        quantization->setExtra((void*)true);
        goto theEnd;
      }
    }
    else {
      if( inGoal(belief) ) { // check for goal state
        result.push_back(state,-1,-1);
        goto theEnd;
      }
    }

    // compute the best QValues
    QValueClass *QResult = 0;
    if( learning && (PD.version == problemClass::VERSION_GRID) )
      QResult = bestQValue(quantization,true);
    else
      QResult = bestQValue(belief,true);

    // greedy selection of best action
    int bestAction = -1;
    if( QResult->numTies > 0 ) {
      int idx = (!randomTies ? 0 : lrand48() % QResult->numTies);
      bestAction = QResult->ties[idx];
    }
    else {
      // we have a dead-end
      if( learning ) {
        quantization->updateValue(FLT_MAX);
        if( PD.useStopRule ) quantization->setExtra((void*)true);
      }
      result.costToGoal = -1.0;
      result.discountedCostToGoal = -1.0;
      result.push_back(state,-1,-1);
      break;
    }

    // update hash value (if learning)
    if( learning ) {
      assert( !PD.useStopRule || !(bool)quantization->getExtra() ||
              (fabs((double)(quantization->getValue()-QResult->value)) < PD.SREpsilon) );
      quantization->updateValue(QResult->value);
      if( PD.useStopRule ) Stack.push(quantization);
    }

    // get next state and observation
    float realCost = cost(state,bestAction);
    result.costToGoal += realCost;
    result.discountedCostToGoal += realCost * powf(discountFactor,result.numMoves);
    int new_state = nextState(state,bestAction);
    int observation = nextObservation(new_state,bestAction);
    result.push_back(state,bestAction,observation);
    state = new_state;

    // get belief_ao and set new belief
    beliefClass *belief_a = belief->updateByA(actionCache,true,bestAction);
    beliefClass *belief_ao = belief_a->updateByAO(observationCache,true,bestAction,observation);
    if( learning ) {
      beliefHash->quantize(true,true,belief_ao,quantization);
      if( PD.version == problemClass::VERSION_GRID )
        *belief = *((beliefClass*)quantization->getData());
      else
        *belief = *belief_ao;
    }
    else {
      *belief = *belief_ao;
    }

    if( PD.verboseLevel >= 30 ) { // print info
      *PD.outputFile << "action = " << bestAction << endl;
      *PD.outputFile << "observation = " << observation << endl;
      *PD.outputFile << "belief_a = " << *belief_a << endl;
      *PD.outputFile << "belief_ao = " << *belief_ao << endl;
    }
  }

  // dead-end reached, return fail
  result.numMoves = -1;

  // this is the end, ...
 theEnd:
  delete belief;

  // check for abortion
  if( PD.signal >= 0 ) { // cleanup
    int s = PD.signal;
    PD.signal = -1;
    if( PD.useStopRule ) for( ; !Stack.empty(); Stack.pop() );
    throw( signalExceptionClass(s) );
  }

  // stopping rule
  if( PD.useStopRule && learning && (result.numMoves < maxMoves) ) {
    list<hashEntryClass*> closed;
    while( !Stack.empty() ) {
      hashEntryClass *entry = Stack.top();
      Stack.pop();
      if( !(bool)entry->getExtra() ) { // check if already solved
        // try labeling
        closed.clear();
        if( checkSolved(entry,closed) ) {
          for( std::list<hashEntryClass*>::iterator it = closed.begin(); it != closed.end(); ++it ) {
            (*it)->setExtra((void*)true);
            *PD.outputFile << "solved belief " << *(beliefClass*)(*it)->getData();
            *PD.outputFile << ", value = " << (*it)->getValue() << endl;
          }
        }
        else
	  break;
      }
    }
  }
  for( ; !Stack.empty(); Stack.pop() );
  result.stopTimer();
}

float
standardPOMDPClass::QValue( const beliefClass* belief, int action, bool useCache )
{
  static std::map<int,float> possibleObservations;

  if( applicable(belief,action) ) {
    // compute bel_a(s)
    beliefClass *belief_a = belief->updateByA(actionCache,useCache,action);

    // compute possible observations
    possibleObservations.clear();
    belief_a->nextPossibleObservations(action,possibleObservations);

    // check if we have observations in the model, if not, the belief doesn't change
    float sum = 0.0;
    if( !possibleObservations.empty() ) { // make the calculation for future costs
      for( std::map<int,float>::const_iterator it = possibleObservations.begin(); it != possibleObservations.end(); ++it ) { // compute bel_ao(s)
        beliefClass *belief_ao = belief_a->updateByAO(observationCache,useCache,action,(*it).first);
        hashValueClass *hValue = hashValue(belief_ao);
        if( PD.pddlProblemType ) { // update sum
          switch( PD.handle->problemType ) {
          case problemHandleClass::PROBLEM_POMDP1:
          case problemHandleClass::PROBLEM_POMDP2:
            sum += ((*it).second*hValue->value);
            break;
          case problemHandleClass::PROBLEM_ND_POMDP1:
          case problemHandleClass::PROBLEM_ND_POMDP2:
            sum = (sum == 0.0 ? hValue->value : (hValue->value > sum ? hValue->value : sum));
            break;
          default:
            throw( unsupportedModelExceptionClass(PD.handle->problemType) );
          }
        }
        else
          sum += (*it).second*hValue->value;
      }
    }
    else {
      sum = hashValue(belief_a)->value;
    }
    return(cost(belief,action)+discountFactor*sum);
  }
  else
    return(-1.0);
}

standardPOMDPClass::QValueClass*
standardPOMDPClass::bestQValue( const beliefClass* belief, bool useCache )
{
  static std::map<int,float> possibleObservations;
  hashEntryClass *quantization;

  // assertion and initialization
  assert( belief != 0 );
  if( bestQValueStaticResult == 0 ) {
    bestQValueStaticResult = new QValueClass(numActions);
  }

  bool noBest = true;
  ++expansions;
  bestQValueStaticResult->numTies = 0;
  for( int action = 0; action < numActions; ++action ) {
    if( applicable(belief,action)) { // update statistics
      ++branching;
      beliefClass *belief_a = belief->updateByA(actionCache,useCache,action);
      possibleObservations.clear();
      belief_a->nextPossibleObservations(action,possibleObservations);
      assert( !possibleObservations.empty() );
      float sum = 0.0;
      for( std::map<int,float>::const_iterator it = possibleObservations.begin(); it != possibleObservations.end(); ++it ) {
        if( (*it).second > 0.0 ) { // compute bel_ao(s)
          beliefClass *belief_ao = belief_a->updateByAO(observationCache,useCache,action,(*it).first);
          beliefHash->quantize(true,false,belief_ao,quantization);
          float hval = quantization->getValue();
          if( PD.pddlProblemType ) { // update value
            switch( PD.handle->problemType ) {
            case problemHandleClass::PROBLEM_POMDP1:
            case problemHandleClass::PROBLEM_POMDP2:
              sum += (*it).second*hval;
              break;
            case problemHandleClass::PROBLEM_ND_POMDP1:
            case problemHandleClass::PROBLEM_ND_POMDP2:
              sum = (sum == 0.0 ? hval : (hval > sum ? hval : sum));
              break;
            default:
              throw( unsupportedModelExceptionClass(PD.handle->problemType) );
            }
          }
          else
            sum += (*it).second*hval;
        }
      }
      sum = cost(belief,action) + discountFactor* sum;

      // look for the best action
      if( noBest || best(sum,bestQValueStaticResult->value) || (sum == bestQValueStaticResult->value) ) {
        if( noBest || (sum != bestQValueStaticResult->value) ) bestQValueStaticResult->numTies = 0;
        noBest = false;
        bestQValueStaticResult->value = sum;
        bestQValueStaticResult->ties[bestQValueStaticResult->numTies] = action;
        ++bestQValueStaticResult->numTies;
      }
    }
  }
  // if we have a dead-end, then result->numTies is zero.
  return(bestQValueStaticResult);
}

standardPOMDPClass::QValueClass*
standardPOMDPClass::bestQValue( const hashEntryClass* qBelief, bool useCache )
{
  static std::map<int,float> possibleObservations;
  hashEntryClass *quantization;

  // assertion and initialization
  assert( qBelief != 0 );
  if( bestQValueStaticResult == 0 ) {
    bestQValueStaticResult = new QValueClass(numActions);
  }

  bool noBest = true;
  ++expansions;
  bestQValueStaticResult->numTies = 0;
  beliefClass *belief = (beliefClass*)qBelief->getData();
  for( int action = 0; action < numActions; ++action ) {
    if( applicable(belief,action) ) { // update statistics
      ++branching;
      beliefClass *belief_a = belief->updateByA(actionCache,useCache,action);
      possibleObservations.clear();
      belief_a->nextPossibleObservations(action,possibleObservations);
      assert( !possibleObservations.empty() );
      if( possibleObservations.size() == 1 ) ;
      // make calculation for future costs
      float sum = 0.0;
      for( std::map<int,float>::const_iterator it = possibleObservations.begin(); it != possibleObservations.end(); ++it ) {
        if( (*it).second > 0.0 ) { // compute bel_ao(s)
          beliefClass *belief_ao = belief_a->updateByAO(observationCache,useCache,action,(*it).first);
          beliefHash->quantize(true,true,belief_ao,quantization);
          float hval = quantization->getValue();
          if( PD.pddlProblemType ) { // update value
            switch( PD.handle->problemType ) {
            case problemHandleClass::PROBLEM_POMDP1:
            case problemHandleClass::PROBLEM_POMDP2:
              sum += (*it).second*hval;
              break;
            case problemHandleClass::PROBLEM_ND_POMDP1:
            case problemHandleClass::PROBLEM_ND_POMDP2:
              sum = (sum == 0.0 ? hval : (hval > sum ? hval : sum));
              break;
            default:
              throw( unsupportedModelExceptionClass(PD.handle->problemType) );
            }
          }
          else
            sum += (*it).second*hval;
        }
      }
      sum = cost(belief,action) + discountFactor*sum;

      // look for the best action
      if( noBest || best( sum, bestQValueStaticResult->value ) || (sum == bestQValueStaticResult->value) ) {
        if( noBest || (sum != bestQValueStaticResult->value) ) bestQValueStaticResult->numTies = 0;
          noBest = false;
          bestQValueStaticResult->value = sum;
          bestQValueStaticResult->ties[bestQValueStaticResult->numTies] = action;
          ++bestQValueStaticResult->numTies;
      }
    }
  }

  // if we have a dead-end, then result->numTies is zero.
  return(bestQValueStaticResult);
}

bool
standardPOMDPClass::checkSolved( hashEntryClass *current, list<hashEntryClass*>& closed )
{
  int action;
  hashEntryClass *quantization;
  std::map<int,float> possibleObservations;
  list<hashEntryClass*> open;
  set<hashEntryClass*> aux;

  // initialization
  aux.clear();
  closed.clear();
  open.clear();
  if( !(bool)current->getExtra() ) open.push_front(current);

  // dfs
  bool rv = true;
  while( !open.empty() ) {
    // get first from queue
    current = open.front();
    open.pop_front();
    closed.push_front(current);

    // check epsilon condition 
    beliefClass *belief = (beliefClass*)current->getData();
    belief->checkModelAvailability(model);
    QValueClass *QResult = bestQValue(current,true);
    if( QResult->numTies == 0 ) { // dead-end state
      current->updateValue(FLT_MAX);
      current->setExtra((void*)true);
      rv = false;
      continue;
    }
    if( fabs(QResult->value-current->getValue()) > PD.SREpsilon ) {
      rv = false;
      continue;
    }
    action = QResult->ties[0];

    // unfold control
    possibleObservations.clear();
    beliefClass *belief_a = belief->updateByA(actionCache,true,action);
    belief_a->nextPossibleObservations(action,possibleObservations);
    if( !possibleObservations.empty() ) {
      for( std::map<int,float>::const_iterator it = possibleObservations.begin(); it != possibleObservations.end(); ++it ) {
        if( (*it).second > 0.0 ) { // compute next belief_ao
          beliefClass *belief_ao = belief_a->updateByAO(observationCache,true,action,(*it).first);
          belief_ao->checkModelAvailability(model);
          if( !inGoal(belief_ao) ) {
            beliefHash->quantize(true,true,belief_ao,quantization);
            if( !(bool)quantization->getExtra() && (aux.find(quantization) == aux.end()) ) {
              open.push_front(quantization);
              aux.insert(quantization);
            }
          }
        }
      }
    }
    else {
      belief_a->checkModelAvailability(model);
      if( !inGoal(belief_a) ) {
        beliefHash->quantize(true,true,belief_a,quantization);
        if( !(bool)quantization->getExtra() && (aux.find(quantization) == aux.end()) ) {
          open.push_front(quantization);
          aux.insert(quantization);
        }
      }
    }
  }

  // process nodes in dfs postorder
  if( !rv ) {
    while( !closed.empty() ) {
      current = closed.front();
      closed.pop_front();
      QValueClass *QResult = bestQValue(current,true);
      if( QResult->numTies > 0 ) current->updateValue(QResult->value);
    }
  }
  return(rv);
}


///////////////////////
//
// serialization

void
standardPOMDPClass::checkIn()
{
  signRegistration( typeid(standardPOMDPClass).name(),
		    (void (*)(istream&,POMDPClass*)) &standardPOMDPClass::fill,
		    (POMDPClass *(*)()) &standardPOMDPClass::constructor );
}

void
standardPOMDPClass::write( ostream& os ) const
{
  // write registrationId
  const char *id = typeid(*this).name();
  unsigned len = strlen(id) + 1;
  safeWrite(&len,sizeof(unsigned),1,os);
  safeWrite((void*)id,sizeof(char),strlen(id)+1,os);

  // serialize base class
  POMDPClass::write(os);
}

standardPOMDPClass *
standardPOMDPClass::read( istream& is )
{
  standardPOMDPClass *pomdp = new standardPOMDPClass;
  fill(is,pomdp);
  return(pomdp);
}

standardPOMDPClass *
standardPOMDPClass::constructor()
{
  return(new standardPOMDPClass);
}

void
standardPOMDPClass::fill( istream& is, standardPOMDPClass *pomdp )
{
  POMDPClass::fill(is,pomdp );
}

