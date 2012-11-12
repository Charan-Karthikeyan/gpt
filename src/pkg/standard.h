#ifndef _STANDARD_H
#define _STANDARD_H

#include <stdlib.h>
#include <string.h>

typedef int boolean;

boolean _lessThan( const stateClass&, register int, register int );
boolean _lessEqual( const stateClass&, register int, register int );
boolean _greaterEqual( const stateClass&, register int, register int );
boolean _greaterThan( const stateClass&, register int, register int );

int _plus( const stateClass&, register int, register int, register int );
int _minus( const stateClass&, register int, register int, register int );

#endif /* _STANDARD_H */
