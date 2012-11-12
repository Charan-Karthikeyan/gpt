//  Theseus
//  theLookAheadHeuristic.cc -- Look Ahead Heuristic in Belief Space
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <typeinfo>
#include <map>

using namespace std;

#include <theseus/theLookAheadHeuristic.h>
#include <theseus/theTopLevel.h>
#include <theseus/theBelief.h>
#include <theseus/theRtStandard.h>
#include <theseus/theException.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Look Ahead Heuristic Class
//

float
lookAheadHeuristicClass::value( int level, const beliefClass *belief )
{
  register float current, sum, hval;
  register bool noBest;
  register const beliefClass *belief_a, *belief_ao; 
  map<int,float> possibleObservations;
  map<int,float>::const_iterator it;

  // base case
  current = 0;
  if( level == 0 )
    current = H->value(belief);
  else if( !POMDP->inGoal(belief) ) {
    noBest = true;
    for( int action = 0; action < numActions; ++action ) {
      if( POMDP->applicable(belief,action) ) {
        // compute bel_a(s)
        belief_a = belief->updateByA(NULL,false,action);

        // compute possible observations
        belief_a->nextPossibleObservations(action,possibleObservations);
        assert( !possibleObservations.empty() );

        // make calculation for future costs
        sum = 0.0;
        for( it = possibleObservations.begin(); it != possibleObservations.end(); ++it ) {
          if( (*it).second > 0.0 ) {
            // compute bel_ao(s)
            belief_ao = belief_a->updateByAO(NULL,false,action,(*it).first);
            hval = value(level-1,belief_ao);
            delete belief_ao;

            // update value
            if( PD.pddlProblemType ) {
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
            else {
              sum += (*it).second*hval;
            }
          }
        }
        delete belief_a;
        sum = POMDP->cost(belief,action) + discountFactor*sum;

        // look for the best action
        if( noBest || POMDP->best(sum,current) ) {
          noBest = false;
          current = sum;
        }
      }
    }
  }
  return(current);
}


///////////////////////
//
// serialization

void
lookAheadHeuristicClass::checkIn()
{
  signRegistration( typeid(lookAheadHeuristicClass).name(),
		    (void (*)(istream&,heuristicClass*)) &lookAheadHeuristicClass::fill,
		    (heuristicClass *(*)()) &lookAheadHeuristicClass::constructor );
}

void
lookAheadHeuristicClass::write( ostream& os )
{
  // write registrationId
  const char *id = typeid(*this).name();
  unsigned len = strlen(id) + 1;
  safeWrite(&len,sizeof(unsigned),1,os);
  safeWrite((void*)id,sizeof(char),strlen(id)+1,os);

  // serialize base class
  heuristicClass::write(os);

  // do it!
  H->write(os);
  safeWrite(&lookahead,sizeof(int),1,os);
  safeWrite(&numActions,sizeof(int),1,os);
  safeWrite(&discountFactor,sizeof(float),1,os);
}

lookAheadHeuristicClass *
lookAheadHeuristicClass::read( istream& is )
{
  lookAheadHeuristicClass *heuristic = new lookAheadHeuristicClass;
  fill(is,heuristic);
  return(heuristic);
}

lookAheadHeuristicClass *
lookAheadHeuristicClass::constructor()
{
  return(new lookAheadHeuristicClass);
}

void
lookAheadHeuristicClass::fill( istream& is, lookAheadHeuristicClass *heuristic )
{
  heuristicClass::fill(is,heuristic);
  heuristic->H = heuristicClass::read(is);
  safeRead(&heuristic->lookahead,sizeof(int),1,is);
  safeRead(&heuristic->numActions,sizeof(int),1,is);
  safeRead(&heuristic->discountFactor,sizeof(float),1,is);
}

