//  Theseus
//  theDynHeuristic.cc -- Dynamic Heuristic
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <values.h>
#include <iostream>
#include <typeinfo>
#include <map>

using namespace std;

#include <theseus/theDynHeuristic.h>
#include <theseus/theTopLevel.h>
#include <theseus/theRtStandard.h>
#include <theseus/thePOMDP.h>
#include <theseus/theModel.h>
#include <theseus/theUtils.h>

#define  MIN(x,y)    ((x)<(y)?(x):(y))
#define  MAX(x,y)    ((x)>(y)?(x):(y))


///////////////////////
//
// Class methods

float
dynHeuristicClass::value( int state )
{
  // allocate space if necessary
  allocateHTSpace( state );

  // compute value if not available
  if( !(solved[state/8] & (1<<(state%8))) )
    {
      HTable[state] = IDA( state );
      //HTable[state] = A( state );

      if( PD.verboseLevel >= 15 )
	*PD.outputFile << "heuristic for state " << state << " = " << HTable[state] << endl;

      if( HTable[state] == FLT_MAX )
	PD.model->printState( state, cout, 2 );
      solved[state/8] |= (1<<(state%8));
    }
  return( HTable[state] );
}

float
dynHeuristicClass::value( const beliefClass *belief )
{
  register float val, sum;
  const map<int,float> *m;
  map<int,float>::const_iterator it;

#if 0 // for SORTNETs
  sum = 10000.0;
#else
  sum = 0.0;
#endif
  m = belief->cast();
  for( it = m->begin(); it != m->end(); ++it )
    {
      val = value( (*it).first );
      switch( PD.handle->problemType )
	{
	case problemHandleClass::PROBLEM_ND_POMDP1:
	case problemHandleClass::PROBLEM_ND_POMDP2:
	case problemHandleClass::PROBLEM_CONFORMANT1:
#if 0 // for SORTNETs
	  sum = (sum == 10000.0 ? val : (val < sum ? val : sum));
#else
	  sum = (sum == 0.0 ? val : (val > sum ? val : sum));
#endif
	  break;
	default:
	  sum += ((*it).second * val);
	  break;
	}
    }

#if 0 // for SORTNETs
  sum += log(m->size())/log(2.0);
  sum = ceil( sum );
#endif

  if( PD.verboseLevel >= 10 )
    *PD.outputFile << "heuristic for belief " << *belief << " = " << sum << endl;

  return( sum );
}

void
dynHeuristicClass::allocateHTSpace( int state )
{
  while( state >= HTSize )
    {
      int oldsz = HTSize;
      HTSize = (HTSize == 0 ? sizeof( char ) : 2 * HTSize);
      solved = (char*)realloc( solved, HTSize/sizeof(char) );
      HTable = (float*)realloc( HTable, HTSize * sizeof(float) );
      memset( &solved[oldsz/sizeof(char)], 0, (oldsz == 0 ? HTSize/sizeof(char) : oldsz/sizeof(char)) );
      memset( &HTable[oldsz], 0, (oldsz == 0 ? HTSize:oldsz) * sizeof( float ) );
    }
}

void
dynHeuristicClass::allocateTTSpace( int state )
{
  while( state >= TTSize )
    {
      int oldsz = TTSize;
      TTSize = (TTSize == 0 ? sizeof( char ) : 2 * TTSize);
      TTable = (TTentry_t*)realloc( TTable, TTSize * sizeof( TTentry_t ) );
      memset( &TTable[oldsz], 0, (oldsz == 0 ? TTSize : oldsz) * sizeof( TTentry_t ) );
    }
}

float
dynHeuristicClass::IDA( int state )
{
  bool goal;
  float threshold;

  goal = false;
  threshold = 0.0;
  while( !goal && (threshold < FLT_MAX) )
    {
      if( PD.verboseLevel >= 18 )
	{
	  *PD.outputFile << "IDA* for state = " << state << ", with threshold = ";
	  *PD.outputFile << threshold << ", TTSize = " << TTSize << endl;
	}

      float newThreshold = contourDFS( state, 0.0, threshold, goal );
      threshold = newThreshold;
      memset( TTable, 0, TTSize * sizeof( TTentry_t ) );
    }
  return( threshold );
}

float
dynHeuristicClass::contourDFS( int state, float cost, float threshold, bool &goal )
{ 
  float rv, newThreshold, value;

#if 0
  // prune cycles
  if( (state < TTSize) && (TTable[state].g == -1.0) )
    return( FLT_MAX );
#endif

  // first of all, check if we already computed value for this state
  if( state < HTSize )
    {
      value = (HTable[state] == FLT_MAX ? FLT_MAX : cost + HTable[state]);
      if( solved[state/8] & (1<<(state%8)) )
	{
	  if( value <= threshold )
	    goal = true;
	  return( value );
	}
      else if( value > threshold )
	return( value );
    }

  // check if we have model for state
  model->checkModelFor( state );

  // check for threshold and goal
  if( cost > threshold )
    return( cost );
  else if( model->inGoal( state ) )
    {
      goal = true;
      return( cost );
    }

  // check transposition table
  if( (state < TTSize) && (TTable[state].g != 0.0) && (TTable[state].g <= cost) )
    return( TTable[state].t );

  // ok .. we need to do some work  .. first mark state to avoid cycles ..
  allocateTTSpace( state );
#if 0
  TTable[state].g = -1.0;
  //cerr << "contourDFS: state=" << state << ", threshold=" << threshold << endl;
#endif

  // expand it
  rv = newThreshold = FLT_MAX;
  for( int action = 0; action < model->numActions; ++action )
    if( model->applicable( state, action ) )
      {
	int newState = model->nextState( state, action );
	float newCost = cost + model->discountFactor * model->cost( state, action );
	rv = contourDFS( newState, newCost, threshold, goal );
	if( goal )
	  {
	    newThreshold = rv;
	    break;
	  }
	newThreshold = MIN(newThreshold,rv);
      }

  // fill transposition table
  TTable[state].g = cost;
  TTable[state].t = newThreshold;

  // fill heuristic table
  allocateHTSpace( state );
  value = (newThreshold == FLT_MAX ? FLT_MAX : newThreshold - cost);
  //  if( (newThreshold != FLT_MAX) && (HTable[state] < value) )
  if( HTable[state] < value )
    {
      HTable[state] = value;
      //cerr << "new lower bound for state " << state << " = " << HTable[state] << endl;
    }
  if( goal )
    {
      HTable[state] = value;
      solved[state/8] |= (1<<(state%8));
    }

  // return
  return( newThreshold );
}

void
dynHeuristicClass::allocateVTSpace( int state )
{
  while( state >= VTSize )
    {
      int oldsz = VTSize;
      VTSize = (VTSize == 0 ? sizeof( char ) : 2 * VTSize);
      VTable = (nodeInfo_t**)realloc( VTable, VTSize * sizeof( nodeInfo_t* ) );
      memset( &VTable[oldsz], 0, (oldsz == 0 ? VTSize : oldsz) * sizeof( nodeInfo_t* ) );
    }
}

float
dynHeuristicClass::A( int state )
{
  register float cost;
  register nodeInfo_t *node, *oldNode;

  node = allocNode();
  node->state = state;
  setInitialOPEN( node );
  allocateVTSpace( state );
  VTable[state] = node;
  while( true )
    {
      // get best node in OPEN
      if( (node = extractFirstOPEN()) == NULL )
	break;

      // be sure we have the model for this state
      model->checkModelFor( node->state );

      // check for goal
      if( model->inGoal( node->state ) )
	break;

      // expand node
      for( int action = 0; action < model->numActions; ++action )
	if( model->applicable( node->state, action ) )
	  {
	    // apply action
	    cost = model->cost( node->state, action );
	    state = model->nextState( node->state, action );
	    oldNode = (state < VTSize ? VTable[state] : NULL);

	    // check if we need to (re)open it
	    if( (oldNode == NULL) || (node->g + cost < oldNode->g) )
	      {
		if( oldNode != NULL )
		  {
		    if( oldNode->status == INCLOSE )
		      removeFromCLOSE( oldNode );
		    else
		      removeFromOPEN( oldNode );
		  }
		else
		  {
		    oldNode = allocNode();
		    allocateVTSpace( state );
		    VTable[state] = oldNode;
		    oldNode->state = state;
		    oldNode->h = (state < HTSize ? HTable[state] : 0.0);
		  }

		// insert it into OPEN
		assert( oldNode->parent != node );
		oldNode->g = node->g + cost;
		oldNode->action = action;
		oldNode->parent = node;
		insertIntoOPEN( oldNode );
	      }
	  }

      // thread OPEN list
      threadBuckets();
    }

  // fetch result
  assert( model->inGoal( node->state ) );
  cost = node->g;

  // cleanup
  cleanLists();

  // return value
  return( cost );
}


///////////////////////
//
// OPEN/CLOSE implementation

unsigned
dynHeuristicClass::bucketIndex( nodeInfo_t *node )
{
  return( (unsigned)floor( SCALE * (node->g + node->h) ) );
}

void
dynHeuristicClass::setInitialOPEN( nodeInfo_t *node )
{
  insertIntoOPEN( node );
  threadBuckets();
}

void
dynHeuristicClass::removeFromCLOSE( nodeInfo_t *node )
{
  assert( node->status == INCLOSE );

  node->status = 0;
  if( node->openPrev == NULL )
    {
      assert( CLOSE == node );
      CLOSE = node->openNext;
    }
  else
    node->openPrev->openNext = node->openNext;

  if( node->openNext != NULL )
    node->openNext->openPrev = node->openPrev;
}

void
dynHeuristicClass::removeFromOPEN( nodeInfo_t *node )
{
  unsigned index;

  assert( node->status == INOPEN );

  node->status = 0;
  index = bucketIndex( node );
  assert( index >= 0 );
  if( index >= MAXBUCKETS )
    {
      cerr << "Error: collapsing index " << index << " into bucket " << MAXBUCKETS - 1 << endl;
      index = MAXBUCKETS - 1;
    }

  // bucket removal
  if( node->bucketPrev == NULL )
    {
      assert( firstInBucket[index] == node );
      firstInBucket[index] = node->bucketNext;
    }
  else
    node->bucketPrev->bucketNext = node->bucketNext;

  if( node->bucketNext == NULL )
    {
      assert( lastInBucket[index] == node );
      lastInBucket[index] = node->bucketPrev;
    }
  else
    node->bucketNext->bucketPrev = node->bucketPrev;

  // clear bucket pointers
  node->bucketPrev = NULL;
  node->bucketNext = NULL;

  // open list removal
  if( node->openPrev == NULL )
    OPEN = node->openNext;
  else
    node->openPrev->openNext = node->openNext;

  if( node->openNext != NULL )
    node->openNext->openPrev = node->openPrev;

  // clear open pointers
  node->openPrev = NULL;
  node->openNext = NULL;
}

void
dynHeuristicClass::insertIntoCLOSE( nodeInfo_t *node )
{
  assert( (node->status != INOPEN) && (node->status != INCLOSE) );
  assert( (node->bucketPrev == NULL) && (node->bucketNext == NULL) );
  assert( (node->openPrev == NULL) && (node->openNext == NULL) );
  assert( (CLOSE == NULL) || (CLOSE->openPrev == NULL) );

  node->status = INCLOSE;
  if( CLOSE != NULL )
    {
      CLOSE->openPrev = node;
      node->openNext = CLOSE;
      CLOSE = node;
    }
  else
    {
      CLOSE = node;
    }
}

void
dynHeuristicClass::insertIntoOPEN( nodeInfo_t *node )
{
  unsigned index;
  nodeInfo_t *extra, *pExtra;

  assert( (node->status != INOPEN) && (node->status != INCLOSE) );
  assert( (node->bucketPrev == NULL) && (node->bucketNext == NULL) );
  assert( (node->openPrev == NULL) && (node->openNext == NULL) );

  node->status = INOPEN;
  index = bucketIndex( node );
  assert( (index >= 0) && (index < MAXBUCKETS) );

  extra = firstInBucket[index];
  if( extra == NULL )
    {
      firstInBucket[index] = node;
      lastInBucket[index] = node;
      minBucket = (index < minBucket ? index : minBucket);
      maxBucket = (index > maxBucket ? index : maxBucket);
    }
  else
    {
      pExtra = NULL;
      while( (extra != NULL) && (extra->g + extra->h < node->g + node->h) )
	{
	  pExtra = extra;
	  extra = extra->bucketNext;
	}

      // insert it into proper bucket
      if( pExtra == NULL )
	firstInBucket[index] = node;
      else
	{
	  pExtra->bucketNext = node;
	  pExtra->openNext = node;
	}

      if( extra == NULL )
	lastInBucket[index] = node;
      else
	{
	  extra->bucketPrev = node;
	  extra->openPrev = node;
	}

      // clear bucket pointers
      node->bucketPrev = pExtra;
      node->bucketNext = extra;
      node->openPrev = pExtra;
      node->openNext = extra;
    }

  // note: a bucket threading is needed but we'll make it after all insertions
}

dynHeuristicClass::nodeInfo_t*
dynHeuristicClass::extractFirstOPEN( void )
{
  nodeInfo_t *node;

  if( OPEN != NULL )
    {
      node = OPEN;
      removeFromOPEN( node );
      insertIntoCLOSE( node );
    }
  else
    {
      node = NULL;
    }
  return( node );
}

void
dynHeuristicClass::threadBuckets( void )
{
  register unsigned i, j;
  unsigned newMinBucket, newMaxBucket;

  newMaxBucket = 0; // to remove warning
  newMinBucket = MAXBUCKETS;
  i = minBucket;
  while( i <= maxBucket )
    if( firstInBucket[i] != NULL )
      {
	// new bucket limits
	if( newMinBucket == MAXBUCKETS )
	  {
	    newMinBucket = i;
	    firstInBucket[i]->openPrev = NULL;
	  }
	newMaxBucket = i;

	// threading
	for( j = i + 1; (j <= maxBucket) && (firstInBucket[j] == NULL); ++j );
	if( j <= maxBucket )
	  {
	    lastInBucket[i]->openNext = firstInBucket[j];
	    firstInBucket[j]->openPrev = lastInBucket[i];
	  }
	else
	  {
	    lastInBucket[i]->openNext = NULL;
	  }
	i = j;
      }
    else
      {
	++i;
      }

  // set new bucket limits
  minBucket = newMinBucket;
  maxBucket = newMaxBucket;

  // set OPEN
  OPEN = firstInBucket[minBucket];
}

void
dynHeuristicClass::cleanLists( void )
{
  // free space
  deallocSpace();
  VTSize = 0;
  if( VTable != NULL ) 
    {
      free( VTable );
      VTable = NULL;
    }

  // clean buckets and lists
  memset( firstInBucket, 0, MAXBUCKETS * sizeof( nodeInfo_t * ) );
  memset( lastInBucket, 0, MAXBUCKETS * sizeof( nodeInfo_t * ) );
  minBucket = MAXBUCKETS;
  maxBucket = 0;
  OPEN = NULL;
  CLOSE = NULL;
}

void
dynHeuristicClass::printLists( ostream &os )
{
  nodeInfo_t *extra;

  os << "max_bucket = " << maxBucket << endl;
  os << "min_bucket = " << minBucket << endl;
  for( unsigned i = minBucket; i <= maxBucket; ++i )
    if( (extra = firstInBucket[i]) )
      {
	os << "bucket[" << i << "] = ";
	while( extra )
	  {
	    os << "[" << extra << ":" << extra->state << ":";
	    os << extra->bucketPrev << ":" << extra->bucketNext << "] ";
	    extra = extra->bucketNext;
	  }
	os << endl;
      }
}

dynHeuristicClass::nodeInfo_t*
dynHeuristicClass::allocNode( void )
{
  if( (poolAreas == NULL) ||
      (sizePoolAreas[poolIndex] == &poolAreas[poolIndex][nodeIndex] - poolAreas[poolIndex]) )
    {
      if( poolIndex == poolAreasSize )
	{
	  poolAreasSize = (poolAreasSize == -1 ? 8 : 2*poolAreasSize);
	  poolAreas = (nodeInfo_t**)realloc( poolAreas, poolAreasSize * sizeof( nodeInfo_t* ) );
	  sizePoolAreas = (int*)realloc( sizePoolAreas, poolAreasSize * sizeof( int* ) );
	}
      ++poolIndex;
      sizePoolAreas[poolIndex] = (poolIndex == 0 ? 128 : 2 * sizePoolAreas[poolIndex-1]);
      poolAreas[poolIndex] = new nodeInfo_t[sizePoolAreas[poolIndex]];
      nodeIndex = 0;
    }
  return( &poolAreas[poolIndex][nodeIndex++] );
}

void
dynHeuristicClass::deallocSpace( void )
{
  // free space
  if( poolAreas != NULL )
    { 
      for( int i = 0; i <= poolIndex; ++i )
	delete[] poolAreas[i];
      free( poolAreas );
      free( sizePoolAreas );
    }

  // reset global vars
  poolIndex = -1;
  poolAreasSize = -1;
  poolAreas = NULL;
  sizePoolAreas = NULL;
}


///////////////////////
//
// serialization

void
dynHeuristicClass::checkIn( void )
{
  signRegistration( typeid( dynHeuristicClass ).name(),
		    (void (*)( istream&, heuristicClass * )) &dynHeuristicClass::fill,
		    (heuristicClass *(*)( void )) &dynHeuristicClass::constructor );
}

void
dynHeuristicClass::write( ostream& os )
{
  const char *id;
  unsigned len;

  // write registrationId
  id = typeid( *this ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  heuristicClass::write( os );

  // do it!
  safeWrite( &HTSize, sizeof( int ), 1, os );
  safeWrite( solved, sizeof( char ), HTSize/sizeof(char), os );
  safeWrite( HTable, sizeof( float ), HTSize, os );
}

dynHeuristicClass *
dynHeuristicClass::read( istream& is )
{
  dynHeuristicClass *heuristic;
  
  heuristic = new dynHeuristicClass;
  fill( is, heuristic );

  return( heuristic );
}

dynHeuristicClass *
dynHeuristicClass::constructor( void )
{
  return( new dynHeuristicClass );
}

void
dynHeuristicClass::fill( istream& is, dynHeuristicClass *heuristic )
{
  heuristicClass::fill( is, heuristic );

  safeRead( &heuristic->HTSize, sizeof( int ), 1, is );
  heuristic->solved = (char*)calloc( heuristic->HTSize/sizeof(char), sizeof(char) );
  heuristic->HTable = (float*)calloc( heuristic->HTSize, sizeof(float) );
  safeRead( heuristic->solved, sizeof(char), heuristic->HTSize, is );
  safeRead( heuristic->HTable, sizeof(float), heuristic->HTSize, is );
}
