//  Theseus
//  theSearchPOMDP.h -- Search POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theSearchPOMDP_INCLUDE_
#define _theSearchPOMDP_INCLUDE_

#include <iostream>

#include <theseus/thePOMDP.h>
#include <theseus/theModel.h>

#define  SCALE          100
#define  MAXBUCKETS     20480
#define  INOPEN         1
#define  INCLOSE        2


///////////////////////////////////////////////////////////////////////////////
//
// Search POMDP Class
//

class searchPOMDPClass : public POMDPClass
{
private:
  // private methods
  searchPOMDPClass( const searchPOMDPClass& pomdp );

  // constants
  enum { SEARCH_IDA, SEARCH_BFS };

  // private virtual methods
  virtual void algorithm( bool learning, resultClass& result );

  // A* implementation
  struct nodeInfo_t
  {
    int          status;
    int          action;
    float        g, h;
    nodeInfo_t*  parent;
    beliefClass* belief;
    nodeInfo_t*  bucketNext;
    nodeInfo_t*  bucketPrev;
    nodeInfo_t*  openNext;
    nodeInfo_t*  openPrev;
    nodeInfo_t()
      {
	status = 0;
	belief = NULL;
	g = h = 0.0;
	parent = NULL;
	bucketNext = bucketPrev = NULL;
	openNext = openPrev = NULL;
      }
  };

  nodeInfo_t*  firstInBucket[MAXBUCKETS];
  nodeInfo_t*  lastInBucket[MAXBUCKETS];
  unsigned     minBucket;
  unsigned     maxBucket;
  nodeInfo_t*  OPEN;
  nodeInfo_t*  CLOSE;

  unsigned     bucketIndex( nodeInfo_t *node );
  void         setInitialOPEN( nodeInfo_t *node );
  void         removeFromCLOSE( nodeInfo_t *node );
  void         removeFromOPEN( nodeInfo_t *node );
  void         insertIntoOPEN( nodeInfo_t *node );
  void         insertIntoCLOSE( nodeInfo_t *node );
  nodeInfo_t*  extractFirstOPEN( void );
  void         threadBuckets( void );
  void         cleanLists( void );
  nodeInfo_t*  allocNode( void );
  void         deallocNode( nodeInfo_t *ptr );
  void         setupResult( nodeInfo_t *node, resultClass& result, float discountFactor );

public:
  // constructors/destructors
  searchPOMDPClass()
    {
      beliefHash = NULL;
      actionCache = NULL;
      observationCache = NULL;
      minBucket = MAXBUCKETS;
      maxBucket = 0;
      OPEN = NULL;
      CLOSE = NULL;
    }
  virtual ~searchPOMDPClass();

  // virtual methods
  virtual const hashEntryClass* getTheInitialBelief( void );
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

  // serialization
  static  void             checkIn( void );
  virtual void             write( ostream& os ) const;
  static searchPOMDPClass* read( istream& is );
  static searchPOMDPClass* constructor( void );
  static void              fill( istream& is, searchPOMDPClass *pomdp );
};

#endif // _theSearchPOMDP_INCLUDE
