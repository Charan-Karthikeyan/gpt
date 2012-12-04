//  Theseus
//  theMDPPOMDP.h -- MDP-POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theMDPPOMDP_INCLUDE_
#define _theMDPPOMDP_INCLUDE_

#include <list>

#include <theseus/thePOMDP.h>
#include <theseus/theModel.h>


///////////////////////////////////////////////////////////////////////////////
//
// MDP POMDP Class
//

class MDPPOMDPClass : public POMDPClass
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
    QValueClass( int size ) { value = 0.0; numTies = 0; ties = new int[size]; }
    ~QValueClass( void ) { delete[] ties; }
  };

private:
  // private methods
  MDPPOMDPClass( const MDPPOMDPClass& pomdp );

  float        QValue( const beliefClass* belief, int action, bool useCache );
  QValueClass* bestQValue( const beliefClass* belief, bool useCache );
  bool         checkSolved( hashEntryClass* current, list<hashEntryClass*>& closed );

  // private virtual methods
  virtual void algorithm( bool learning, resultClass& result );

public:
  // constructors
  MDPPOMDPClass()
    {
      beliefHash = NULL;
      actionCache = NULL;
      observationCache = NULL;
    }

  // destructors
  virtual ~MDPPOMDPClass( void ) { }

  // virtual methods
  virtual const hashEntryClass* getTheInitialBelief( void );
  virtual void                  initRun( int run ) { }
  virtual void                  endRun( bool lastRun ) { POMDPClass::endRun( lastRun ); }
  virtual float                 cost( int state, int action ) const
    {
      return( model->cost( state, action ) );
    }
  virtual float                 cost( const beliefClass *belief, int action ) const
    {
      return( model->cost( (intptr_t)belief, action ) );
    }
  virtual bool                  inGoal( int state ) const
    {
      return( model->inGoal( state ) );
    }
  virtual bool                  inGoal( const beliefClass* belief ) const
    {
      return( model->inGoal( (intptr_t)belief ) );
    }
  virtual bool                  applicable( int state, int action ) const
    {
      return( model->applicable( state, action ) );
    }
  virtual bool                  applicable( const beliefClass* belief, int action ) const
    {
      return( model->applicable( (intptr_t)belief, action ) );
    }
  virtual int                   getInitialState( const beliefClass* belief ) const
    {
      return( model->getInitialState( belief ) );
    }
  virtual int                   nextState( int state, int action ) const
    {
      return( model->nextState( state, action ) );
    }
  virtual int                   nextObservation( int state, int action ) const;

  virtual hashValueClass*       hashValue( const beliefClass* belief );
  virtual int                   getBestAction( const hashEntryClass *belief );
  virtual void                  expandBeliefWithAction( const hashEntryClass *belief, int action,
							deque<pair<pair<int,float>,const hashEntryClass*> >& );

  // serialization
  static  void                  checkIn( void );
  virtual void                  write( ostream& os ) const;
  static MDPPOMDPClass*         read( istream& is );
  static MDPPOMDPClass*         constructor( void );
  static void                   fill( istream& is, MDPPOMDPClass *pomdp );
};

#endif // _theMDPPOMDP_INCLUDE
