//  Theseus
//  theBelief.cc -- Abstract Belief Class
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <typeinfo>

using namespace std;
#include <theseus/theBelief.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Belief Class
//

// static members
registerClass* beliefClass::registeredElements = NULL;
heuristicClass* beliefClass::heuristic = NULL;

beliefClass::~beliefClass()
{
}

float
beliefClass::heuristicValue( void ) const
{
  return( heuristic == NULL ? 0.0 : heuristic->value( this ) );
}

unsigned
beliefClass::hashDataFunction( void ) const
{
  assert( 0 );
  return( 0 );
}

void*
beliefClass::hashDataCopy( void ) const
{
  assert( 0 );
  return( NULL );
}

bool
beliefClass::hashDataCompare( const void* data ) const
{
  assert( 0 );
  return( 0 );
}

void
beliefClass::hashDataWrite( ostream& os ) const
{
  assert( 0 );
}

void*
beliefClass::hashDataRead( istream& is ) const
{
  assert( 0 );
  return( NULL );
}


///////////////////////
//
// serialization

void
beliefClass::signRegistration( const char* registrationId,
			       void (*fillFunction)( istream&, beliefClass* ), 
			       beliefClass* (*constructor)( void ) )
{
  registerClass::insertRegistration( registeredElements,
				     registrationId,
				     (void (*)( istream&, void* )) fillFunction,
				     (void* (*)( void )) constructor );
}

void
beliefClass::cleanup( void )
{
  registerClass::cleanup( registeredElements );
}

void
beliefClass::write( ostream& os ) const
{
  // nothing to do
}

beliefClass*
beliefClass::read( istream& is )
{
  char *id;
  beliefClass *belief;

  id = registerClass::getRegisteredId( is );
  belief = (beliefClass*) registerClass::readElement( is, id, registeredElements );
  delete id;
  return( belief );
}

void
beliefClass::fill( istream& is, beliefClass* bel )
{
  // nothing to do
}

ostream&
operator<<( ostream& os, const beliefClass& belief )
{
  belief.print( os );
  return( os );
}


///////////////////////////////////////////////////////////////////////////////
//
// Belief Hash Class
//

beliefHashClass::~beliefHashClass()
{
}

unsigned
beliefHashClass::dataFunction( const void* data )
{
  return( ((beliefClass*)data)->hashDataFunction() );
}

void*
beliefHashClass::dataCopy( const void* data )
{
  return( ((beliefClass*)data)->hashDataCopy() );
}

bool
beliefHashClass::dataCompare( const void* newData, const void* dataInHash )
{
  return( ((beliefClass*)newData)->hashDataCompare( dataInHash ) );
}

void
beliefHashClass::statistics( ostream& os )
{
  hashClass::statistics( os );
}

void
beliefHashClass::write( ostream& os ) const
{
  hashClass::write( os );
}

beliefHashClass*
beliefHashClass::read( istream& is )
{
  return( (beliefHashClass*) hashClass::read( is ) );
}

void
beliefHashClass::fill( istream& is, beliefHashClass* hash )
{
  hashClass::fill( is, hash );
}


///////////////////////////////////////////////////////////////////////////////
//
// Hooked Belief Hash Class
//

hookedBeliefHashClass::~hookedBeliefHashClass()
{
}

void
hookedBeliefHashClass::dataDelete( void* data )
{
  delete (beliefClass*)data;
}

void
hookedBeliefHashClass::dataPrint( ostream& os, const void* data )
{
  os << *((beliefClass*)data);
} 

void
hookedBeliefHashClass::dataWrite( ostream& os, const void* data ) const
{
  ((beliefClass*)data)->hashDataWrite( os );
}

void*
hookedBeliefHashClass::dataRead( istream& is )
{
  cerr << "Error in hookedBeliefClass::dataRead()" << endl;
  return( beliefClass::read( is ) );
}

void
hookedBeliefHashClass::quantize( bool useCache, bool saveInHash, const beliefClass* belief, 
				 hashEntryClass*& quantization )
{
  hashEntryClass *entry = lookup( (void*)belief );
  if( entry == NULL )
    entry = insert( (void*)belief, belief->heuristicValue() );
  quantization = entry;
}

void
hookedBeliefHashClass::checkIn( void )
{
  signRegistration( typeid( hookedBeliefHashClass ).name(),
		    (void (*)( istream& is, hashClass* )) &hookedBeliefHashClass::fill,
		    (hashClass* (*)( void )) &hookedBeliefHashClass::constructor );
}

void
hookedBeliefHashClass::write( ostream& os ) const
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

hookedBeliefHashClass*
hookedBeliefHashClass::read( istream& is )
{
  hookedBeliefHashClass *result;

  result = new hookedBeliefHashClass;
  fill( is, result );
  return( result );
}

hookedBeliefHashClass*
hookedBeliefHashClass::constructor( void )
{
  return( new hookedBeliefHashClass );
}

void
hookedBeliefHashClass::fill( istream& is, hookedBeliefHashClass* hash )
{
  beliefHashClass::fill( is, hash );
}
