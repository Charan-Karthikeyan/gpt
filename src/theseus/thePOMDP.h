//  Theseus
//  thePOMDP.h -- Abstract POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _thePOMDP_INCLUDE_
#define _thePOMDP_INCLUDE_

#include <iostream>
#include <deque>

#include <theseus/theBelief.h>

class registerClass;
class hashEntryClass;
class hashValueClass;
class resultClass;
class modelClass;


///////////////////////////////////////////////////////////////////////////////
//
// POMDP Class
//

class POMDPClass
{
  // static members
  static registerClass* registeredElements;

  // private methods
  POMDPClass( const POMDPClass& QValue );

  // private virtual methods
  virtual void       algorithm( bool learning, resultClass& result ) = 0;

protected:
  // protected members
  int                costPOMDP;
  int                maxMoves;
  bool               randomTies;
  float              discountFactor;
  int                numActions;
  int                actionCacheSize;
  int                actionClusterSize;
  int                observationCacheSize;
  int                observationClusterSize;
  modelClass*        model;
  const beliefClass* theInitialBelief;

  beliefHashClass*   beliefHash;
  beliefCacheClass*  actionCache;
  beliefCacheClass*  observationCache;

public:
  // hashValueClass
  struct hashValueClass
  {
    float value;
    bool  solved;
  };

public:
  // members
  unsigned           expansions;
  unsigned           branching;

  // constructors
  POMDPClass();

  // destructors
  virtual ~POMDPClass();

  // methods
  const beliefCacheClass* getActionCache( void ) const { return( actionCache ); }
  const beliefCacheClass* getObservationCache( void ) const { return( observationCache ); }
  int                     getActionCacheSize( void ) const { return( actionCacheSize ); }
  int                     getActionClusterSize( void ) const { return( actionClusterSize ); }
  int                     getObservationCacheSize( void ) const { return( observationCacheSize ); }
  int                     getObservationClusterSize( void ) const { return( observationClusterSize ); }

  void                    printHash( ostream& os );
  void                    statistics( ostream& os );
  void                    setHash( beliefHashClass *beliefHash );
  beliefHashClass*        removeHash( void );
  void                    setCache( int actionCacheSize, int actionClusterSize,
				    int observationCacheSize, int observationClusterSize,
				    beliefClass *(*beliefConstructor)( void ) );
  void                    setModel( modelClass *m ) { model = m; }
  modelClass*             getModel( void ) const { return( model ); }
  void                    setParameters( int numActions, int costPOMDP, int maxMoves,
					 bool randomTies, float discountFactor );
  void                    setMaxMoves( int mmoves ) { maxMoves = mmoves; }
  void                    setRandomTies( bool rties ) { randomTies = rties; }
  void                    setDiscountFactor( float dfactor ) { discountFactor = dfactor; }
  float                   getDiscountFactor( void ) const { return( discountFactor ); }

  int                     best( float val1, float val2 );
  void reset( void )
    {
      expansions = 0;
      branching = 0;
    }

  void cleanHash( void )
    {
      beliefHash->clean();
    }

  int getNumActions( void ) const
    {
      return( numActions );
    }

  bool emptyBeliefHash( void ) const
    {
      return( beliefHash->getNumElem() == 0 );
    }

  // algorithms
  void learnAlgorithm( resultClass& result )
    {
      algorithm( true, result );
    }

  void controlAlgorithm( resultClass& result )
    {
      algorithm( false, result );
    }

  // virtual methods
  virtual void                  setTheInitialBelief( const beliefClass *belief );
  virtual const hashEntryClass* getTheInitialBelief( void );
  virtual void                  initialize( void );
  virtual void                  finalize( void );
  virtual void                  initRun( int run );
  virtual void                  endRun( bool lastRun );
  virtual float                 cost( int state, int action ) const = 0;
  virtual float                 cost( const beliefClass *belief, int action ) const = 0;
  virtual bool                  inGoal( int state ) const = 0;
  virtual bool                  inGoal( const beliefClass* belief ) const = 0;
  virtual bool                  applicable( int state, int action ) const = 0;
  virtual bool                  applicable( const beliefClass* belief, int action ) const = 0;
  virtual int                   getInitialState( const beliefClass *belief ) const = 0;
  virtual int                   nextState( int state, int action ) const = 0;
  virtual int                   nextObservation( int state, int action ) const = 0;
  virtual const hashEntryClass* getHashEntry( const beliefClass *belief );
  virtual hashValueClass*       hashValue( const beliefClass* belief );
  virtual int                   getBestAction( const hashEntryClass *belief );
  virtual void                  expandBeliefWithAction( const hashEntryClass *belief, int action,
							deque<pair<pair<int,float>,const hashEntryClass*> >& );
  virtual void                  quantize( bool useCache, bool saveInHash, const beliefClass* belief,
					  hashEntryClass*& quantization );

  // serialization
  static  void                  signRegistration( const char *registrationId,
						  void (*fillFunction)( istream&, POMDPClass * ), 
						  POMDPClass *(*constructor)( void ) );
  static  void                  cleanup( void );
  virtual void                  write( ostream& os ) const;
  static POMDPClass*            read( istream& is );
  static void                   fill( istream& is, POMDPClass *pomdp );
};

inline int
POMDPClass::best( float val1, float val2 )
{
  if( costPOMDP )
    return( val1 < val2 );
  else
    return( val1 > val2 );
}


#endif // _thePOMDP_INCLUDE
