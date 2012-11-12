#include "race_racetrack.h"

bool
_valid( const stateClass& state, int old_x, int old_y, int x, int y )
{
  bool rv;

  if( (x < 0) || (y < 0) ||
      (x > 34) || (y > 11) ||
      ((y <= 4) && (x <= 31)) ||
      ((y == 9) && (x <= 3)) ||
      ((y == 10) && (x <= 7)) ||
      ((y == 11) && (x <= 11)) )
    rv = false;
  else
    rv = true;

  return( rv );
}
