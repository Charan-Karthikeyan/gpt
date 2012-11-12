#include "race_racetrack2.h"

bool
_valid( const stateClass& state, int s )
{
  const stateFor_raceClass& raceState = (const stateFor_raceClass&)state;
  int x, y;
  bool rv;

  x = raceState.function1[_x][raceState.object[s]];
  y = raceState.function1[_y][raceState.object[s]];

  if( (x < 0) || (y < 0) ||
      (x > 4) || (y > 2) )
    rv = false;
  else
    rv = true;

  return( rv );
}

  
