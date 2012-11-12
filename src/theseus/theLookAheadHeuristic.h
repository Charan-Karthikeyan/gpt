//  Theseus
//  theLookAheadHeuristic.h -- Look Ahead Heuristic in Belief Space
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theLookAheadHeuristic_INCLUDE_
#define _theLookAheadHeuristic_INCLUDE_

#include <iostream>

#include <theseus/theHeuristic.h>
#include <theseus/thePOMDP.h>


class POMDPClass;
class beliefClass;


///////////////////////////////////////////////////////////////////////////////
//
// Look Ahead Heuristic Class
//

class lookAheadHeuristicClass : public heuristicClass
{
  POMDPClass *POMDP;
  heuristicClass *H;
  int lookahead;
  int numActions;
  float discountFactor;

  lookAheadHeuristicClass()
    {
      POMDP = NULL;
      H = NULL;
      lookahead = 0;
      numActions = 0;
    }

public:
  // constructors/destructors
  lookAheadHeuristicClass( POMDPClass *p, heuristicClass *h )
    {
      POMDP = p;
      H = h;
      lookahead = 0;
      numActions = POMDP->getNumActions();
      discountFactor = POMDP->getDiscountFactor();
    }

  virtual ~lookAheadHeuristicClass()
    {
      delete H;
    }

  // methods
  void  setPOMDP( POMDPClass *P ) { POMDP = P; }
  void  setHeuristic( heuristicClass *h ) { H = h; }
  void  setLookahead( int l ) { lookahead = l; }
  int   getLookahead( void ) { return( lookahead ); }
  float value( int l, const beliefClass *belief );

  // virtual methods
  virtual float  value( int stateIndex ) { return( H->value( stateIndex ) ); }
  virtual float  value( const beliefClass *belief )
    {
      return( value( lookahead, belief ) );
    }

  // serialization
  static  void                     checkIn( void );
  virtual void                     write( ostream& os );
  static  lookAheadHeuristicClass* read( istream& is );
  static  lookAheadHeuristicClass* constructor( void );
  static  void                     fill( istream& is, lookAheadHeuristicClass *heuristic );
};


#endif // _theLookAheadHeuristic_INCLUDE
