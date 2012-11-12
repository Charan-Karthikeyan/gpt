#include <iostream>

using namespace std;

#include <theseus/theRtStandard.h>
#include <pkg/standard.h>


boolean
_lessThan( const stateClass&, register int x, register int y )
{
  return( x < y );
}


boolean
_lessEqual( const stateClass&, register int x, register int y )
{
  return( x <= y );
}


boolean
_greaterEqual( const stateClass&, register int x, register int y )
{
  return( x >= y );
}


boolean
_greaterThan( const stateClass&, register int x, register int y )
{
  return( x > y );
}


int
_plus( const stateClass&, register int x, register int y, register int modulo )
{
  if( modulo > 0 )
    return( (x + y) % modulo );
  else
    return( x + y );
}


int
_minus( const stateClass&, register int x, register int y, register int modulo )
{
  if( modulo > 0 )
    return( (x - y + modulo) % modulo );
  else
    return( x - y );
}
