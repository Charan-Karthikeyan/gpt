#include <iostream.h>
#include <theseus/theRtStandard.h>
#include <assert.h>

int
_above( const stateClass&, int pos )
{
  static int map[] = { 10, 10, 1, 2, 10, 10, 10, 10, 10, 10, 10 };
  //assert( pos != 10 );
  return( map[pos] );
  //  return( (map[pos] == 10 ? pos : map[pos]) );
}

int
_below( const stateClass&, int pos )
{
  static int map[] = { 10, 2, 3, 10, 10, 10, 10, 10, 10, 10, 10 };
  //assert( pos != 10 ); 
  return( map[pos] );
  //  return( (map[pos] == 10 ? pos : map[pos]) );
}

int
_left( const stateClass&, int pos )
{
  static int map[] = { 10, 0, 10, 4, 10, 2, 5, 6, 7, 8, 10 };
  //assert( pos != 10 );
  return( map[pos] );
  //  return( (map[pos] == 10 ? pos : map[pos]) );
}

int
_right( const stateClass&, int pos )
{
  static int map[] = { 1, 10, 5, 10, 3, 6, 7, 8, 9, 10, 10 };
  //assert( pos != 10 );
  return( map[pos] );
  //  return( (map[pos] == 10 ? pos : map[pos]) );
}
