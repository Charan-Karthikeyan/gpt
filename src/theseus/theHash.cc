//  Theseus
//  theHash.cc -- Hash Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <iostream>

using namespace std;

#include <theseus/theHash.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Hash Class
//

// static members
registerClass *hashClass::registeredElements = NULL;

hashClass::hashClass( unsigned isize )
{
  dataTable = new hashEntryClass*[isize];
  memset( dataTable, 0, isize * sizeof( hashEntryClass * ) );
  
  diameter = new unsigned[isize];
  memset( diameter, 0, isize * sizeof( unsigned ) );

  initialSize = isize;
  hashSize = initialSize;

  numElem = 0;
  lookups = 0;
  hits = 0;
}

hashClass::~hashClass()
{
  clean();
  delete[] dataTable;
  delete[] diameter;
}

hashEntryClass*
hashClass::lookup( const void *data )
{
  register hashEntryClass *entry;
  unsigned index;

  ++lookups;
  index = dataFunction( data ) % hashSize;
  for( entry = dataTable[index]; entry != NULL; entry = entry->next )
    if( !dataCompare( data, entry->data ) )
      {
	++hits;
	return( entry );
      }
  return( NULL );
}

void
hashClass::clean( void )
{
  register unsigned i;
  register hashEntryClass *entry, *next;

  for( i = 0; i < hashSize; ++i )
    for( entry = dataTable[i]; entry != NULL; entry = next )
      {
	next = entry->next;
	dataDelete( entry->data );
	delete entry;
      }

  memset( dataTable, 0, hashSize * sizeof( hashEntryClass * ) );
  memset( diameter, 0, hashSize * sizeof( unsigned ) );
  
  numElem = 0;
  lookups = 0;
  hits = 0;
}

void
hashClass::print( ostream& os )
{
  register unsigned i;
  register hashEntryClass *entry;

  for( i = 0; i < hashSize; ++i )
    {
      if( dataTable[i] != NULL )
	{
	  os << "[SLOT " << i << "]" << endl;
	  for( entry = dataTable[i]; entry != NULL; entry = entry->next )
	    {
	      os << "\tVALUE = " << entry->value << ", DATA = { ";
	      dataPrint( os, entry->data );
	      os << "}, EXTRA = " << entry->extra << endl;
	    }
	}
    }
}

void
hashClass::statistics( ostream& os )
{
  register unsigned i, n, max, total, slots;

  max = 0;
  total = 0;
  slots = 0;
  for( i = 0; i < hashSize; ++i )
    {
      n = diameter[i];
      total += n;
      slots += (n ? 1 : 0);
      max = (n > max ? n : max);
    }

  os << "%hash numElements " << numElem << endl;
  os << "%hash usedSlots " << slots << endl;
  os << "%hash freeSlots " << (hashSize - slots) << endl;
  os << "%hash diameter " << max << endl;
  if( slots )
    os << "%hash avgLength " << ((float)total / (float)slots) << endl;
  os << "%hash hits " << hits << endl;
  os << "%hash missRatio " << ((float)(lookups - hits) / (float)lookups) << endl;
}


///////////////////////
//
// serialization

void
hashClass::signRegistration( const char *registrationId,
			     void (*fillFunction)( istream&, hashClass * ), 
			     hashClass *(*constructor)( void ) )
{
  registerClass::insertRegistration( registeredElements,
				     registrationId,
				     (void (*)( istream&, void * )) fillFunction,
				     (void *(*)( void )) constructor );
}

void
hashClass::cleanup( void )
{
  registerClass::cleanup( registeredElements );
}

void
hashClass::write( ostream& os ) const
{
  unsigned i;
  hashEntryClass *entry;

  // first write fixed size elements (same order for reading)
  safeWrite( &hashSize, sizeof( unsigned ), 1, os );
  safeWrite( &initialSize, sizeof( unsigned ), 1, os );
  safeWrite( &numElem, sizeof( unsigned ), 1, os );
  safeWrite( &lookups, sizeof( unsigned ), 1, os );
  safeWrite( &hits, sizeof( unsigned ), 1, os );
  safeWrite( diameter, sizeof( unsigned ), hashSize, os );

  // now, write the data
  for( i = 0; i < hashSize; ++i )
    for( entry = dataTable[i]; entry != NULL; entry = entry->next )
      {
	dataWrite( os, entry->data );
	safeWrite( &entry->value, sizeof( float ), 1, os );
	safeWrite( &entry->extra, sizeof( void* ), 1, os );
      }
}

hashClass *
hashClass::read( istream& is )
{
  char *id;
  hashClass *hash;

  id = registerClass::getRegisteredId( is );
  hash = (hashClass *) registerClass::readElement( is, id, registeredElements );
  delete id;
  return( hash );
}

void
hashClass::fill( istream& is, hashClass *hash )
{
  unsigned i, j;
  hashEntryClass *entry;

  // first read fixed size data
  safeRead( &hash->hashSize, sizeof( unsigned ), 1, is );
  safeRead( &hash->initialSize, sizeof( unsigned ), 1, is );
  safeRead( &hash->numElem, sizeof( unsigned ), 1, is );
  safeRead( &hash->lookups, sizeof( unsigned ), 1, is );
  safeRead( &hash->hits, sizeof( unsigned ), 1, is );

  // now, those that need space allocation
  delete[] hash->diameter;
  hash->diameter = new unsigned[hash->hashSize];
  safeRead( hash->diameter, sizeof( unsigned ), hash->hashSize, is );

  // we don't read the pool, just use what we have (an empt one)

  // read the data
  delete[] hash->dataTable;
  hash->dataTable = new hashEntryClass*[hash->hashSize];
  memset( hash->dataTable, 0, hash->hashSize * sizeof( hashEntryClass * ) );
  for( i = 0; i < hash->hashSize; ++i )
    for( j = 0; j < hash->diameter[i]; ++j )
      {
	entry = new hashEntryClass;
	entry->data = hash->dataRead( is );
	safeRead( &entry->value, sizeof( float ), 1, is );
	safeRead( &entry->extra, sizeof( void* ), 1, is );
	entry->next = hash->dataTable[i];
	hash->dataTable[i] = entry;
      }
}

void
hashClass::dataDelete( void *data )
{
  cerr << "Error: inside hashClass::dataDelete()" << endl;
}
