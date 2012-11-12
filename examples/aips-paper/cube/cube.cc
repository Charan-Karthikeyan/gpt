#include <iostream.h>
#include <theseus/theRtStandard.h>

int
_above( const stateClass&, int size, int pos )
{
  return( (pos-size <= 0) || (((pos-1) - size) / (size*size) != (pos-1) / (size*size)) ? pos : pos-size );
}

int
_below( const stateClass&, int size, int pos )
{
  return( ((pos-1) + size) / (size*size) != (pos-1) / (size*size) ? pos : pos+size );
}

int
_left( const stateClass&, int size, int pos )
{
  return( pos % size == 1 ? pos : pos-1 );
}

int
_right( const stateClass&, int size, int pos )
{
  return( pos % size == 0 ? pos : pos+1 );
}

int
_in( const stateClass&, int size, int pos )
{
  return( pos + size*size > size*size*size ? pos : pos + size*size );
}

int
_out( const stateClass&, int size, int pos )
{
  return( pos - size*size <= 0 ? pos : pos - size*size );
}
