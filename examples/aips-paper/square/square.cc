#include <iostream.h>
#include <theseus/theRtStandard.h>

int
_above( const stateClass&, int size, int pos )
{
  return( (pos - size <= 0 ? pos : pos - size) );
}

int
_below( const stateClass&, int size, int pos )
{
  return( (pos + size > size * size ? pos : pos + size) );
}

int
_left( const stateClass&, int size, int pos )
{
  return( (pos % size == 1 ? pos : pos - 1) );
}

int
_right( const stateClass&, int size, int pos )
{
  return( (pos % size == 0 ? pos : pos + 1) );
}
