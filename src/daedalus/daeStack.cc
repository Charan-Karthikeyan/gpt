#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <daedalus/daeStack.h>


stackClass::stackClass( void )
{
  stack = NULL;
  size = 0;
  topPtr = 0;
}


stackClass::~stackClass( void )
{
  delete[] stack;
}


void
stackClass::push( void *data )
{
  unsigned newSize;
  void     **newStack;

  if( topPtr == size )
    {
      newSize = (size == 0 ? 128 : 2 * size);
      newStack =  new void*[newSize];
      memcpy( newStack, stack, size * sizeof( void * ) );
      if( stack != NULL )
	delete[] stack;
      stack = newStack;
    }
  stack[topPtr++] = data;
}


void *
stackClass::pop( void )
{
  if( topPtr == 0 )
    return( NULL );
  else
    return( stack[--topPtr] );
}


void *
stackClass::top( void )
{
  if( topPtr == 0 )
    return( NULL );
  else
    return( stack[topPtr - 1] );
}
