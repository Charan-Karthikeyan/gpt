//  Theseus
//  theDynHeuristic.h -- Dynamic Heuristic
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theDynHeuristic_INCLUDE_
#define _theDynHeuristic_INCLUDE_

#include <stdlib.h>
#include <iostream>
#include <cstring>

#include <theseus/theHeuristic.h>

#define  SCALE          1
#define  MAXBUCKETS     20480
#define  INOPEN         1
#define  INCLOSE        2

class modelClass;
class beliefClass;


///////////////////////////////////////////////////////////////////////////////
//
// Dynamic Heuristic Class
//

class dynHeuristicClass : public heuristicClass
{
  // common to all algorithms
  modelClass *model;

  // IDA* implementation
  struct TTentry_t { float g, t; };
  int HTSize;
  char *solved;
  float *HTable;
  int TTSize;
  TTentry_t *TTable;

  void  allocateHTSpace( int state );
  void  allocateTTSpace( int state );
  float IDA( int state );
  float contourDFS( int state, float cost, float threshold, bool &goal );

  // A* implementation
  struct nodeInfo_t
  {
    int          status;
    int          action;
    float        g, h;
    nodeInfo_t*  parent;
    int          state;
    nodeInfo_t*  bucketNext;
    nodeInfo_t*  bucketPrev;
    nodeInfo_t*  openNext;
    nodeInfo_t*  openPrev;
    nodeInfo_t()
      {
	status = 0;
	state = 0;
	g = h = 0.0;
	parent = NULL;
	bucketNext = bucketPrev = NULL;
	openNext = openPrev = NULL;
      }
  };

  nodeInfo_t** VTable;
  int          VTSize;

  nodeInfo_t*  firstInBucket[MAXBUCKETS];
  nodeInfo_t*  lastInBucket[MAXBUCKETS];
  unsigned     minBucket;
  unsigned     maxBucket;
  nodeInfo_t*  OPEN;
  nodeInfo_t*  CLOSE;

  int          nodeIndex;
  int          poolIndex;
  int          poolAreasSize;
  nodeInfo_t** poolAreas;
  int*         sizePoolAreas;

  void         allocateVTSpace( int state );
  float        A( int state );
  unsigned     bucketIndex( nodeInfo_t *node );
  void         setInitialOPEN( nodeInfo_t *node );
  void         removeFromCLOSE( nodeInfo_t *node );
  void         removeFromOPEN( nodeInfo_t *node );
  void         insertIntoOPEN( nodeInfo_t *node );
  void         insertIntoCLOSE( nodeInfo_t *node );
  nodeInfo_t*  extractFirstOPEN( void );
  void         threadBuckets( void );
  void         cleanLists( void );
  void         printLists( ostream &os );
  nodeInfo_t*  allocNode( void );
  void         deallocSpace( void );

public:
  // constructors/destructors
  dynHeuristicClass( modelClass *m = NULL )
    {
      model = m;
      HTSize = 0;
      solved = NULL;
      HTable = NULL;
      TTSize = 0;
      TTable = NULL; 
      VTSize = 0;
      VTable = NULL;
      poolIndex = -1;
      poolAreasSize = -1;
      poolAreas = NULL;
      sizePoolAreas = NULL;
      minBucket = MAXBUCKETS;
      maxBucket = 0;
      OPEN = NULL;
      CLOSE = NULL;
      memset( firstInBucket, 0, MAXBUCKETS * sizeof( nodeInfo_t* ) );
      memset( lastInBucket, 0, MAXBUCKETS * sizeof( nodeInfo_t* ) );
    }
  virtual ~dynHeuristicClass()
    {
      free( solved );
      free( HTable );
      free( TTable );
    }

  // virtual methods
  virtual float value( int stateIndex );
  virtual float value( const beliefClass *belief );

  // serialization
  static  void                checkIn( void );
  virtual void                write( ostream& os );
  static  dynHeuristicClass*  read( istream& is );
  static  dynHeuristicClass*  constructor( void );
  static  void                fill( istream& is, dynHeuristicClass *heuristic );
};

#endif // _theDynHeuristic_INCLUDE
