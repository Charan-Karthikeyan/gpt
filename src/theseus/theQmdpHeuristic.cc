//  Theseus
//  theQmdpHeuristic.cc -- QMDP Heuristic for Beliefs
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <math.h>
#include <iostream>
#include <typeinfo>
#include <map>

using namespace std;

#include <theseus/theQmdpHeuristic.h>
#include <theseus/theTopLevel.h>
#include <theseus/theStandardPOMDP.h>
#include <theseus/theStandardModel.h>
#include <theseus/theException.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// QMDP Heuristic Class
//

void
QMDPHeuristicClass::compute()
{
  register int   state, action, noBest;
  register float number, value, sum, localError, error;
  register float *newVector, *oldVector, *tmpVector;
  std::map<int,std::map<int,float> > *transitionModel;
  std::map<int,std::map<int,float> >::const_iterator handle;
  std::map<int,float>::const_iterator it;

  // allocate memory
  oldVector = new float[numStates];
  newVector = new float[numStates];
  memset(oldVector,0,numStates*sizeof(float));
  memset(newVector,0,numStates*sizeof(float));

  transitionModel = ((standardModelClass*)pomdp->getModel())->transitionModel;

  do { // solve the MDP using Gauss-Seidel Value Iteration
    for( state = 0; state < numStates; ++state ) {
      if( !pomdp->inGoal(state) ) {
        if( PD.signal >= 0 ) goto end; // check signal
        noBest = 1;
        for( action = 0; action < numActions; ++action ) {
          if( pomdp->applicable(state,action) ) { // compute value for an action 
            sum = 0.0;
            number = 0.0;
            assert( (transitionModel[action].find(state) != transitionModel[action].end()) &&
                    !transitionModel[action][state].empty() );
            handle = transitionModel[action].find(state);
	    value = 0;
            for( it = (*handle).second.begin(); it != (*handle).second.end(); ++it ) {
              value = (*it).first < state ? newVector[(*it).first] : oldVector[(*it).first];
              if( PD.handle ) {
                switch( PD.handle->problemType ) {
                  case problemHandleClass::PROBLEM_ND_POMDP1:
                  case problemHandleClass::PROBLEM_ND_POMDP2:
                  case problemHandleClass::PROBLEM_CONFORMANT1:
                  case problemHandleClass::PROBLEM_CONFORMANT2:
                    sum = (sum == 0.0 ? value : (value > sum ? value : sum));
                    break;
                  default:
                    sum += (*it).second*value;
                    break;
                }
              }
              else
                sum += (*it).second*value;
            }
            value = pomdp->cost(state,action) + discountFactor * sum;
            if( noBest || (value < newVector[state]) ) {
              noBest = 0;
              newVector[state] = value;
            }
          }
        }
      }
    }

    // new error
    error = 0.0;
    for( state = 0; state < numStates; ++state ) {
      localError = fabs(newVector[state]-oldVector[state]);
      error = (localError > error ? localError : error);
    }

    // swap vectors
    tmpVector = oldVector;
    oldVector = newVector;
    newVector = tmpVector;

    if( PD.verboseLevel >= 100 ) *PD.outputFile << "QMDP residual = " << error << endl;
  } while( error > PD.epsilon );

  // result
 end:
  delete[] newVector;
  QMDPValue = oldVector;

  // check signal
  if( PD.signal >= 0 ) {
    int s = PD.signal;
    PD.signal = -1;
    throw signalExceptionClass(s);
  }
}

float
QMDPHeuristicClass::value( const beliefClass *belief )
{
  register int number;
  register float val, sum;
  const std::map<int,float> *m;
  std::map<int,float>::const_iterator it;

  sum = 0.0;
  number = 0;
  m = belief->cast();
  for( it = m->begin(); it != m->end(); ++it ) {
    val = value((*it).first);
    if( PD.handle ) {
      switch( PD.handle->problemType ) {
        case problemHandleClass::PROBLEM_ND_POMDP1:
        case problemHandleClass::PROBLEM_ND_POMDP2:
        case problemHandleClass::PROBLEM_CONFORMANT1:
          sum = (sum == 0.0 ? val : (val > sum ? val : sum));
          break;
        default:
          sum += (*it).second*val;
          break;
      }
    }
    else
      sum += (*it).second*val;
  }
  return(sum);
}


///////////////////////
//
// serialization

void
QMDPHeuristicClass::checkIn()
{
  signRegistration( typeid(QMDPHeuristicClass).name(),
		    (void (*)(std::istream&,heuristicClass*)) &QMDPHeuristicClass::fill,
		    (heuristicClass *(*)()) &QMDPHeuristicClass::constructor );
}

void
QMDPHeuristicClass::write( std::ostream& os )
{
  // write registrationId
  const char *id = typeid(*this).name();
  unsigned len = strlen(id) + 1;
  safeWrite(&len,sizeof(unsigned),1,os);
  safeWrite((void*)id,sizeof(char),strlen(id)+1,os);

  // serialize base class
  heuristicClass::write(os);

  // do it!
  safeWrite(&numStates,sizeof(int),1,os);
  safeWrite(&numActions,sizeof(int),1,os);
  safeWrite(&discountFactor,sizeof(float),1,os);
  safeWrite(QMDPValue,sizeof(float),numStates,os);
}

QMDPHeuristicClass*
QMDPHeuristicClass::read( std::istream& is )
{
  QMDPHeuristicClass *heuristic = new QMDPHeuristicClass;
  fill(is,heuristic);
  return(heuristic);
}

QMDPHeuristicClass*
QMDPHeuristicClass::constructor()
{
  return(new QMDPHeuristicClass);
}

void
QMDPHeuristicClass::fill( std::istream& is, QMDPHeuristicClass *heuristic )
{
  heuristicClass::fill(is,heuristic);
  safeRead(&heuristic->numStates,sizeof(int),1,is);
  safeRead(&heuristic->numActions,sizeof(int),1,is);
  safeRead(&heuristic->discountFactor,sizeof(float),1,is);
  heuristic->QMDPValue = new float[heuristic->numStates];
  safeRead(heuristic->QMDPValue,sizeof(float),heuristic->numStates,is);
}
