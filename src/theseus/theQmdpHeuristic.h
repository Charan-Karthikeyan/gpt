//  Theseus
//  theQmdpHeuristic.h -- QMDP Heuristic for Beliefs
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theQmdpHeuristic_INCLUDE_
#define _theQmdpHeuristic_INCLUDE_

#include <iostream>

#include <theseus/theHeuristic.h>

class beliefClass;
class standardPOMDPClass;


///////////////////////////////////////////////////////////////////////////////
//
// QMDP Heuristic Class
//

class QMDPHeuristicClass : public heuristicClass
{
  int numStates;
  int numActions;
  float *QMDPValue;
  float discountFactor;
  const standardPOMDPClass *pomdp;

  // constructors
  QMDPHeuristicClass() { }

  // private methods
  void  compute();

public:
  // constructors/destructors
  QMDPHeuristicClass( float dfactor,
		      int nactions,
		      int nstates,
		      const standardPOMDPClass *p )
    {
      discountFactor = dfactor;
      numActions = nactions;
      numStates = nstates;
      pomdp = p;
      compute();
    }
  virtual ~QMDPHeuristicClass()
    {
      delete[] QMDPValue;
    }

  // virtual methods
  virtual float value( int stateIndex ) { return( QMDPValue[stateIndex] ); }
  virtual float value( const beliefClass *belief );

  // serialization
  static  void                checkIn( void );
  virtual void                write( ostream& os );
  static  QMDPHeuristicClass* read( istream& is );
  static  QMDPHeuristicClass* constructor( void );
  static  void                fill( istream& is, QMDPHeuristicClass *heuristic );
};

#endif // _theQmdpHeuristic_INCLUDE
