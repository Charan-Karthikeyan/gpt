//  Theseus
//  theRegister.cc -- Serialization Register Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

using namespace std;

#include <theseus/theRegister.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Register Class
//

registerClass::registerClass( const char *id,
			      void (*pFillFunction)( istream&, void * ), 
			      void *(*pConstructor)( void ) )
{
  registeredId = strdup( id );
  fillFunction = pFillFunction;
  constructor = pConstructor;
}

registerClass::~registerClass()
{
  free( registeredId );
}

void
registerClass::cleanup( registerClass *&registeredElements )
{
  registerClass *p1, *p2;

  p1 = registeredElements;
  while( p1 != NULL )
    {
      p2 = p1->next;
      delete p1;
      p1 = p2;
    }
}

void
registerClass::insertRegistration( registerClass *&registeredElements,
				   const char *id,
				   void (*fillFunction)( istream&, void * ), 
				   void *(*constructor)( void ) )
{
  registerClass *reg;

  reg = new registerClass( id, fillFunction, constructor );
  reg->next = registeredElements;
  registeredElements = reg;
}

char *
registerClass::getRegisteredId( istream& is )
{
  unsigned len;
  char *id;

  safeRead( &len, sizeof( unsigned ), 1, is );
  id = new char[len];
  safeRead( id, sizeof( char ), len, is );
  return( id );
}

void *
registerClass::readElement( istream& is, char *id, registerClass *registeredElements )
{
  void *result;
  registerClass *reg;

  result = NULL;
  for( reg = registeredElements; reg != NULL; reg = reg->next )
    if( !strcmp( reg->registeredId, id ) )
      {
	result = (*reg->constructor)();
	(*reg->fillFunction)( is, result );
	return( result );
	break;
      }
  cerr << "Error: unregistered class '" << id << "'" << endl;
  return( result );
}

void
(*registerClass::getFillFunction( char *id, registerClass *registeredElements ))( istream&, void * )
{
  registerClass *reg;
  void (*result)( istream&, void * );

  result = NULL;
  for( reg = registeredElements; reg != NULL; reg = reg->next )
    if( !strcmp( reg->registeredId, id ) )
      {
	result = reg->fillFunction;
	break;
      }
  return( result );
}

void *
(*registerClass::getConstructor( char *id, registerClass *registeredElements ))( void )
{
  registerClass *reg;
  void *(*result)( void );

  result = NULL;
  for( reg = registeredElements; reg != NULL; reg = reg->next )
    if( !strcmp( reg->registeredId, id ) )
      {
	result = reg->constructor;
	break;
      }
  return( result );
}
