//  Theseus
//  thePlanningBelief.cc -- Planning Belief Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <typeinfo>

using namespace std;

#include <theseus/thePlanningBelief.h>
#include <theseus/theUtils.h>
#include <theseus/md4.h>


static stateClass **stateClassPool = NULL;
static int stateClassPoolSize = 0;
static planningBeliefClass *beliefClassPool = NULL;
static int beliefClassPoolSize = 0;
static stateClass ***stateControlAreaPool = NULL;
static int stateControlAreaPoolSize = 0;
static planningBeliefClass **beliefControlAreaPool = NULL;
static int beliefControlAreaPoolSize = 0;


static stateClass *getState( int number = 1 );
static stateClass ***getStateControlArea( void );
static planningBeliefClass *getBelief( int number = 1 );
static planningBeliefClass **getBeliefControlArea( void );
static void cleanStates( void );
static void cleanBeliefs( void );


///////////////////////////////////////////////////////////////////////////////
//
// Planning Belief Class
//

// static members
void (*planningBeliefClass::stateAllocFunction)( int, stateClass ** ) = NULL;
void (*planningBeliefClass::stateDeallocFunction)( stateClass * ) = NULL;

void
planningBeliefClass::initialize( problemHandleClass *handle )
{
  planningBeliefClass::stateAllocFunction = handle->stateAllocFunction;
  planningBeliefClass::stateDeallocFunction = handle->stateDeallocFunction;
}

void
planningBeliefClass::print( ostream& os ) const
{
  os << "[ ";
  state->print( os, 0 );
  os << " ]";
}

const map<int,float>*
planningBeliefClass::cast( void ) const
{
  cerr << "Warning: planningBeliefClass::fill() called" << endl;
  return( NULL );
}

void
planningBeliefClass::insert( int state, float probability )
{
  cerr << "Warning: planningBeliefClass::insert() called" << endl;
}

unsigned
planningBeliefClass::randomSampling( void ) const
{
  cerr << "Warning: planningBeliefClass::randomSampling() called" << endl;
  return( 0 );
}

void
planningBeliefClass::checkModelAvailability( modelClass* model ) const
{
  cerr << "Warning: planningBeliefClass::checkModelAvailability() called" << endl;
}

void
planningBeliefClass::nextPossibleObservations( int action, map<int,float> &result ) const
{
  cerr << "Warning: planningBeliefClass::nextPossibleObservations() called" << endl;
}

int
planningBeliefClass::plausibleState( int action, int observation ) const
{
  cerr << "Error: planningBeliefClass::plausibleState() called" << endl;
  return( -1 );
}

beliefClass*
planningBeliefClass::updateByA( beliefCacheClass *cache, bool useCache, int action ) const
{
  cerr << "Warning: planningBeliefClass::updateByA() called" << endl;
  return( NULL );
}

beliefClass*
planningBeliefClass::updateByAO( beliefCacheClass *cache, bool useCache, int action, 
				 int observation ) const
{
  cerr << "Warning: planningBeliefClass::updateByAO() called" << endl;
  return( NULL );
}


///////////////////////
//
// serialization

void
planningBeliefClass::checkIn( void )
{
  signRegistration( typeid( planningBeliefClass ).name(),
		    (void (*)( istream&, beliefClass * )) &planningBeliefClass::fill,
		    (beliefClass *(*)( void )) &planningBeliefClass::constructor );
}

void
planningBeliefClass::write( ostream& os ) const
{
  const char *id;
  unsigned len;

  // write registrationId
  id = typeid( *this ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  beliefClass::write( os );

  //xxxxxx
  // write elements
  // state->write( os );
}

planningBeliefClass *
planningBeliefClass::read( istream& is )
{
  planningBeliefClass *bel;

  bel = new planningBeliefClass;
  fill( is, bel );

  return( bel );
}

planningBeliefClass *
planningBeliefClass::constructor( void )
{
  return( new planningBeliefClass() );
}

void
planningBeliefClass::fill( istream& is, planningBeliefClass *bel )
{
  beliefClass::fill( is, bel );
  //xxxxxx
  // stateClass::fill( is, state );
}


///////////////////////
//
// utils

// esto hay que mejorarlo de modo que al pedir el espacio
// no se llame al constructor para cada elemento (lo mismo
// para la destruccion). Idea: usar low-level allocation + 
//  explicit calls to ctors/dtors

static stateClass *
getState( int number )
{
  stateClass *result, ***controlArea;
  static stateClass **next;

  if( (stateClassPool == NULL) ||
      (next + number >= &stateClassPool[stateClassPoolSize]) )
    {
      stateClassPoolSize = (stateClassPoolSize == 0 ? 1024 : 2 * stateClassPoolSize);
      stateClassPool = new stateClass*[stateClassPoolSize];
      planningBeliefClass::alloc( stateClassPoolSize, stateClassPool );
      controlArea = getStateControlArea();

      next = stateClassPool;
      *controlArea = next;
    }
  result = *next;
  next += number;
  return( result );
}

static stateClass ***
getStateControlArea( void )
{
  int oldSize;
  stateClass ***temporal;
  static stateClass ***next;

  if( (stateControlAreaPool == NULL) ||
      (next == &stateControlAreaPool[stateControlAreaPoolSize]) )
    {
      oldSize = stateControlAreaPoolSize;
      stateControlAreaPoolSize = (oldSize == 0 ? 16 : 2 * oldSize);
      temporal = new stateClass**[stateControlAreaPoolSize];
      memset( temporal, 0, sizeof( stateClass ** ) * stateControlAreaPoolSize );
      memcpy( temporal, stateControlAreaPool, sizeof( stateClass ** ) * oldSize );
      delete[] stateControlAreaPool;

      stateControlAreaPool = temporal;
      next = &temporal[oldSize];
    }
  return( next++ );
}

static void
cleanStates( void )
{
  register int i;
	
  // clean space
  for( i = 0; i < stateControlAreaPoolSize; ++i )
    if( stateControlAreaPool[i] != NULL )
      {
	planningBeliefClass::dealloc( *(stateControlAreaPool[i]) );
	delete[] stateControlAreaPool[i];
      }
  delete[] stateControlAreaPool;

  // restore initial state
  stateControlAreaPoolSize = 0;
  stateControlAreaPool = NULL;
  stateClassPoolSize = 0;
  stateClassPool = 0;
}

static planningBeliefClass *
getBelief( int number )
{
  planningBeliefClass *result, **controlArea;
  static planningBeliefClass *next;

  if( (beliefClassPool == NULL) ||
      (next + number >= &beliefClassPool[beliefClassPoolSize]) )
    {
      beliefClassPoolSize = (beliefClassPoolSize == 0 ? 1024 : 2 * beliefClassPoolSize);
      beliefClassPool = new planningBeliefClass[beliefClassPoolSize];
      controlArea = getBeliefControlArea();

      next = beliefClassPool;
      *controlArea = next;
    }
  result = next;
  next += number;
  return( result );
}

static planningBeliefClass **
getBeliefControlArea( void )
{
  int oldSize;
  planningBeliefClass **temporal;
  static planningBeliefClass **next;

  if( (beliefControlAreaPool == NULL) ||
      (next == &beliefControlAreaPool[beliefControlAreaPoolSize]) )
    {
      oldSize = beliefControlAreaPoolSize;
      beliefControlAreaPoolSize = (oldSize == 0 ? 16 : 2 * oldSize);
      temporal = new planningBeliefClass*[beliefControlAreaPoolSize];
      memset( temporal, 0, sizeof( planningBeliefClass * ) * beliefControlAreaPoolSize );
      memcpy( temporal, beliefControlAreaPool, sizeof( planningBeliefClass * ) * oldSize );
      delete[] beliefControlAreaPool;

      beliefControlAreaPool = temporal;
      next = &temporal[oldSize];
    }
  return( next++ );
}

static void
cleanBeliefs( void )
{
  register int i;

  // clean space
  for( i = 0; i < beliefControlAreaPoolSize; ++i )
    if( beliefControlAreaPool[i] != NULL )
      delete[] beliefControlAreaPool[i];
  delete[] beliefControlAreaPool;

  // restore initial state
  beliefControlAreaPoolSize = 0;
  beliefControlAreaPool = NULL;
  beliefClassPoolSize = 0;
  beliefClassPool = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Planning Belief Hash Class
//

void
planningBeliefHashClass::quantize( bool useCache, bool saveInHash, const beliefClass* belief, 
				   hashEntryClass*& quantization )
{
  cerr << "Warning: planningBeliefHashClass::quantize() called" << endl;
}

void
planningBeliefHashClass::clean( void )
{
  beliefHashClass::clean();
  cleanStates();
  cleanBeliefs();
}

unsigned 
planningBeliefHashClass::dataFunction( const void *data )
{
  register unsigned size, rest, *ptr, result;
  unsigned char digest[16];
  planningBeliefClass *belief;

  static MD4_CTX context;
  static unsigned buffSize = 0;
  static char *buff = NULL;

  // data casting
  belief = (planningBeliefClass*) data;
  size = belief->state->stateSize + (belief->state->stateSize + 8) % 8;
  rest = (belief->state->stateSize + 8) % 8;
  
  // space allocation (if needed)
  if( size > buffSize )
    {
      for( ; size > buffSize; buffSize = (buffSize == 0 ? 64 : 2 * buffSize) );
      buff = (char*) realloc( buff, size * sizeof( char ) );
    }

  // data copy
  memcpy( buff, (char*)belief->state, belief->state->stateSize * sizeof( char ) );
  memset( &buff[belief->state->stateSize], 0, rest * sizeof( char ) );

  // compute MD4 digests
  MD4Init( &context );
  MD4Update( &context, (unsigned char*)buff, size );
  MD4Final( digest, &context );

  // compact digest into an unsigned and return it (this assumes that sizeof( unsigned ) = 4)
  ptr = (unsigned*)digest;
  result = (ptr[0] ^ ptr[1] ^ ptr[2] ^ ptr[3]);
  return( result );
}

void *
planningBeliefHashClass::dataCopy( const void *data )
{
  planningBeliefClass *result;

  result = getBelief();
  result->state = getState();
  *(result->state) = *(((planningBeliefClass*)data)->state);
  return( result );
}

bool
planningBeliefHashClass::dataCompare( const void *newData, const void *dataInHash )
{
  return( !(*(planningBeliefClass*)newData == *(planningBeliefClass*)dataInHash) );
}

void
planningBeliefHashClass::dataDelete( void *data )
{
  cerr << "Warning: planningBeliefHashClass::dataDelete()" << endl;
}

void
planningBeliefHashClass::dataPrint( ostream& os, const void *data )
{
  os << *((planningBeliefClass*)data);
} 

void
planningBeliefHashClass::dataWrite( ostream& os, const void *data ) const
{
  ((planningBeliefClass*)data)->write( os );
}

void *
planningBeliefHashClass::dataRead( istream& is )
{
  return( beliefClass::read( is ) );
}


///////////////////////
//
// serialization

void
planningBeliefHashClass::checkIn( void )
{
  signRegistration( typeid( planningBeliefHashClass ).name(),
		    (void (*)( istream&, hashClass * )) &planningBeliefHashClass::fill,
		    (hashClass* (*)( void )) &planningBeliefHashClass::constructor );
}

void
planningBeliefHashClass::write( ostream& os ) const
{
  const char *id;
  unsigned len;

  // write registrationId
  id = typeid( *this ).name();
  len = strlen( id ) + 1;
  safeWrite( &len, sizeof( unsigned ), 1, os );
  safeWrite( (void*)id, sizeof( char ), strlen( id ) + 1, os );

  // serialize base class
  beliefHashClass::write( os );
}

planningBeliefHashClass *
planningBeliefHashClass::read( istream& is )
{
  planningBeliefHashClass *result;

  result = new planningBeliefHashClass;
  fill( is, result );
  return( result );
}

planningBeliefHashClass *
planningBeliefHashClass::constructor( void )
{
  return( new planningBeliefHashClass );
}

void
planningBeliefHashClass::fill( istream& is, planningBeliefHashClass *hash )
{
  beliefHashClass::fill( is, hash );
}
