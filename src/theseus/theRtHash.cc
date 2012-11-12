//  Theseus
//  theRtHash.cc -- RunTime State and Observation Hash
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>

using namespace std;

#include <theseus/theRtHash.h>
#include <theseus/md4.h>


/////////////////////////////////////////////////////////////////////////////////////////
//
// stateHashClass
//

unsigned
stateHashClass::dataFunction( const void *data )
{
  register unsigned *ptr, result;
  unsigned char digest[16];
  MD4_CTX context;

  // copy belief into buffer
  memcpy( buff, data, size * sizeof( char ) );

  // compute MD4 digests
  MD4Init( &context );
  MD4Update( &context, (unsigned char*)buff, size );
  MD4Final( digest, &context );

  // compact digest into an unsigned and return it (this assumes that sizeof( unsigned ) = 4)
  ptr = (unsigned*)digest;
  result = (ptr[0] ^ ptr[1] ^ ptr[2] ^ ptr[3]);
  return( result );
}

void*
stateHashClass::dataRead( istream& is )
{
  // xxxxxxx
  return( NULL );
}

void
stateHashClass::dataWrite( ostream& os, const void *data ) const
{
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// invStateHashClass
//

void*
invStateHashClass::dataRead( istream& is )
{
  // xxxxxxx
  return( NULL );
}

void
invStateHashClass::dataWrite( ostream& os, const void *data ) const
{
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// observationHashClass
//

unsigned
observationHashClass::dataFunction( const void *data )
{
  register unsigned *ptr, result;
  unsigned char digest[16];
  MD4_CTX context;

  // copy belief into buffer
  memcpy( buff, data, size * sizeof( char ) );

  // compute MD4 digests
  MD4Init( &context );
  MD4Update( &context, (unsigned char*)buff, size );
  MD4Final( digest, &context );

  // compact digest into an unsigned and return it (this assumes that sizeof( unsigned ) = 4)
  ptr = (unsigned*)digest;
  result = (ptr[0] ^ ptr[1] ^ ptr[2] ^ ptr[3]);
  return( result );
}

void*
observationHashClass::dataRead( istream& is )
{
  // xxxxxxx
  return( NULL );
}

void
observationHashClass:: dataWrite( ostream& os, const void *data ) const
{
}


/////////////////////////////////////////////////////////////////////////////////////////
//
// invObservationHashClass
//

void*
invObservationHashClass::dataRead( istream& is )
{
  // xxxxxxx
  return( NULL );
}

void
invObservationHashClass::dataWrite( ostream& os, const void *data ) const
{
}
