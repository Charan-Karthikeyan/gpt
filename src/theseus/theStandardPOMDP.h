//  Theseus
//  theStandardPOMDP.h -- Standard POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theStandardPOMDP_INCLUDE_
#define _theStandardPOMDP_INCLUDE_

#include <iostream>
#include <map>
#include <list>

#include <theseus/thePOMDP.h>
#include <theseus/theModel.h>

class beliefClass;


///////////////////////////////////////////////////////////////////////////////
//
// Standard POMDP Class
//

class standardPOMDPClass : public POMDPClass
{
public:
  // subclass
  class QValueClass
  {
    // private methods
    QValueClass( const QValueClass& QValue );

  public:
    int   numTies;
    float value;
    int   *ties;

    // constructor/destructor
    QValueClass( int size )
      { value = 0.0; numTies = 0; ties = new int[size]; }
    ~QValueClass( void )
      { delete[] ties; }
  };

private:
  // member
  hashEntryClass* quantizationForInitialBelief;
  
  // private methods
  standardPOMDPClass( const standardPOMDPClass& pomdp );

  float        QValue( const beliefClass* belief, int action, bool useCache );
  QValueClass* bestQValue( const beliefClass* belief, bool useCache );
  QValueClass* bestQValue( const hashEntryClass* qBelief, bool useCache );
  bool         checkSolved( hashEntryClass* current, list<hashEntryClass*>& closed );

  // private virtual methods
  virtual void algorithm( bool learning, resultClass& result );

  // transition graph
  map<const hashEntryClass*,map<pair<int,int>,hashEntryClass*>*> trCache;

public:
  // static members
  static int numberStates;

  // constructors
  standardPOMDPClass()
    {
      beliefHash = NULL;
      actionCache = NULL;
      observationCache = NULL;
      quantizationForInitialBelief = NULL;
    }

  // destructors
  virtual ~standardPOMDPClass() { }

  // static methods
  static void setup( int nstates )
    {
      numberStates = nstates;
    }

  // virtual methods
  virtual const hashEntryClass* getTheInitialBelief( void );
  virtual void                  initialize( void ) { }
  virtual void                  finalize( void );
  virtual void                  initRun( int run );
  virtual void                  endRun( bool lastRun );

  virtual float cost( int state, int action ) const
    {
      return( model->cost( state, action ) );
    }

  virtual float cost( const beliefClass *belief, int action ) const
    {
      return( model->cost( belief, action ) );
    }

  virtual bool inGoal( int state ) const
    {
      return( model->inGoal( state ) );
    }

  virtual bool inGoal( const beliefClass* belief ) const
    {
      return( model->inGoal( belief ) );
    }

  virtual bool applicable( int state, int action ) const
    {
      return( model->applicable( state, action ) );
    }

  virtual bool applicable( const beliefClass* belief, int action ) const
    {
      return( model->applicable( belief, action ) );
    }

  virtual int getInitialState( const beliefClass* belief ) const
    {
      return( model->getInitialState( belief ) );
    }

  virtual int nextState( int state, int action ) const
    {
      return( model->nextState( state, action ) );
    }

  virtual int nextObservation( int state, int action ) const
    {
      return( model->nextObservation( state, action ) );
    }

  virtual hashValueClass* hashValue( const beliefClass* belief );
  virtual int getBestAction( const hashEntryClass *belief );
  virtual void expandBeliefWithAction( const hashEntryClass *belief, int action,
				       deque<pair<pair<int,float>,const hashEntryClass*> >& );

  // serialization
  static  void               checkIn( void );
  virtual void               write( ostream& os ) const;
  static standardPOMDPClass* read( istream& is );
  static standardPOMDPClass* constructor( void );
  static void                fill( istream& is, standardPOMDPClass *pomdp );
};

#endif // _theStandardPOMDP_INCLUDE


