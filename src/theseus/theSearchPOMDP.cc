//  Theseus
//  theSearchPOMDP.cc -- Search POMDP Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <typeinfo>
#include <math.h>

using namespace std;

#include <theseus/theSearchPOMDP.h>
#include <theseus/theTopLevel.h>
#include <theseus/theResult.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Search POMDP Class
//

searchPOMDPClass::~searchPOMDPClass()
{
  delete theInitialBelief;
  cleanLists();
}

const hashEntryClass*
searchPOMDPClass::getTheInitialBelief( void )
{
  return( beliefHash->lookup( theInitialBelief ) );
}

POMDPClass::hashValueClass*
searchPOMDPClass::hashValue( const beliefClass* belief )
{
  hashEntryClass *entry;
  static hashValueClass result;

  if( !(entry = beliefHash->lookup( (const void*)belief )) )
    result.value = belief->heuristicValue();
  else
    result.value = entry->getValue();

  // return value
  return( &result );
}

void
searchPOMDPClass::algorithm( bool learning, resultClass& result )
{
  register int action;
  register float costAction;
  register beliefClass *belief_a;
  register nodeInfo_t *node, *oldNode;
  register hashEntryClass *entry;

  // check we have a CONFORMANT problem
  assert( ISCONFORMANT(PD.handle->problemType) );

  // clean everything before starting (just in case called multiple times)
  cleanLists();
  cleanHash();
  reset();

  // initialize result and initial state
  result.runType = learning;
  result.initialValue = hashValue( theInitialBelief )->value;
  result.numMoves = 0;
  result.costToGoal = 0.0;
  result.discountedCostToGoal = 0.0;
  result.startTimer();

  // initialize OPEN
  entry = beliefHash->insert( theInitialBelief, hashValue( theInitialBelief )->value );
  node = allocNode();
  node->action = -1;
  node->belief = (beliefClass*)entry->getData();
  node->parent = NULL;
  node->g = 0.0;
  node->h = entry->getValue();
  entry->setExtra( node );
  setInitialOPEN( node );

  // BFS loop
  while( true )
    {
      // get best node in OPEN
      if( (node = extractFirstOPEN()) == NULL )
	break;

      // be sure we have the model for this belief
      node->belief->checkModelAvailability( model );

      // check for goal
      if( inGoal( node->belief ) )
	break;

      // expand node
      ++expansions;
      for( action = 0; action < numActions; ++action )
	if( applicable( node->belief, action ) )
	  {
	    // apply action
	    ++branching;
	    belief_a = node->belief->updateByA( actionCache, 1, action );
	    if( (entry = beliefHash->lookup( belief_a )) == NULL )
	      entry = beliefHash->insert( belief_a, hashValue( belief_a )->value );
	    oldNode = (nodeInfo_t*)entry->getExtra();
	    costAction = cost( node->belief, action );

	    // check if we need to (re)open it
	    if( (oldNode == NULL) || 
		(node->g + costAction < oldNode->g) )
	      {
		if( oldNode != NULL )
		  {
		    if( oldNode->status == INCLOSE )
		      {
			removeFromCLOSE( oldNode );
		      }
		    else
		      {
			removeFromOPEN( oldNode );
		      }
		  }
		else
		  {
		    oldNode = allocNode();
		    oldNode->belief = (beliefClass*)entry->getData();
		    oldNode->h = entry->getValue();
		    entry->setExtra( oldNode );
		  }

		// insert it into OPEN
		assert( oldNode->parent != node );
		oldNode->g = node->g + costAction;
		oldNode->action = action;
		oldNode->parent = node;
		insertIntoOPEN( oldNode );
	      }
	  }

      // thread OPEN list
      threadBuckets();
    }
  result.stopTimer();

  // udpate result
  if( node == NULL )
    {
      // we have a dead-end
      result.costToGoal = -1.0;
      result.discountedCostToGoal = -1.0;
      result.numMoves = -1;
    }
  else
    {
      setupResult( node, result, discountFactor );
    }
}


///////////////////////
//
// serialization

void
searchPOMDPClass::checkIn( void )
{
  signRegistration( typeid( searchPOMDPClass ).name(),
		    (void (*)( istream&, POMDPClass * )) &searchPOMDPClass::fill,
		    (POMDPClass *(*)( void )) &searchPOMDPClass::constructor );
}

void
searchPOMDPClass::write( ostream& os ) const
{
  const char *id;
  unsigned len;

  // write registrationId
  id = typeid( *this ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  POMDPClass::write( os );
}

searchPOMDPClass*
searchPOMDPClass::read( istream& is )
{
  searchPOMDPClass *pomdp;
  
  pomdp = new searchPOMDPClass;
  fill( is, pomdp );

  return( pomdp );
}

searchPOMDPClass *
searchPOMDPClass::constructor( void )
{
  return( new searchPOMDPClass );
}

void
searchPOMDPClass::fill( istream& is, searchPOMDPClass *pomdp )
{
  POMDPClass::fill( is, pomdp );
}


///////////////////////
//
// OPEN/CLOSE implementation

unsigned
searchPOMDPClass::bucketIndex( nodeInfo_t *node )
{
  return( (unsigned)floor( SCALE * (node->g + node->h) ) );
}

void
searchPOMDPClass::setInitialOPEN( nodeInfo_t *node )
{
  insertIntoOPEN( node );
  threadBuckets();
}

void
searchPOMDPClass::removeFromCLOSE( nodeInfo_t *node )
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
searchPOMDPClass::removeFromOPEN( nodeInfo_t *node )
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
searchPOMDPClass::insertIntoCLOSE( nodeInfo_t *node )
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
searchPOMDPClass::insertIntoOPEN( nodeInfo_t *node )
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

searchPOMDPClass::nodeInfo_t*
searchPOMDPClass::extractFirstOPEN( void )
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
searchPOMDPClass::threadBuckets( void )
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
searchPOMDPClass::cleanLists( void )
{
  nodeInfo_t *node, *tmp;

  // clean OPEN
  for( node = OPEN; node != NULL; node = tmp )
    {
      tmp = node->openNext;
      deallocNode( node );
    }
  OPEN = NULL;

  // clean CLOSE
  for( node = CLOSE; node != NULL; node = tmp )
    {
      tmp = node->openNext;
      deallocNode( node );
    }
  CLOSE = NULL;

  // clean buckets
  memset( firstInBucket, 0, MAXBUCKETS * sizeof( nodeInfo_t * ) );
  memset( lastInBucket, 0, MAXBUCKETS * sizeof( nodeInfo_t * ) );
  minBucket = MAXBUCKETS;
  maxBucket = 0;
}

searchPOMDPClass::nodeInfo_t*
searchPOMDPClass::allocNode( void )
{
  return( new nodeInfo_t );
}

void
searchPOMDPClass::deallocNode( nodeInfo_t *ptr )
{
  delete ptr;
}

void
searchPOMDPClass::setupResult( nodeInfo_t *node, resultClass& result, float discountFactor )
{
  while( (node != NULL) && (node->parent != NULL) )
    {
      result.push_front( -1, node->action, -1 );
      float cost = node->g - node->parent->g;
      result.costToGoal += cost;
      result.discountedCostToGoal = cost + discountFactor * result.discountedCostToGoal;
      node = node->parent;
    }
}
