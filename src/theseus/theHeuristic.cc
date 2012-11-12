//  Theseus
//  theHeuristic.cc -- Abstarct Belief Heuristic Class
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <iostream>

using namespace std;

#include <theseus/theHeuristic.h>
#include <theseus/theRegister.h>


///////////////////////////////////////////////////////////////////////////////
//
// Heuristic Class
//

registerClass *heuristicClass::registeredElements = NULL;

void
heuristicClass::signRegistration( const char *registrationId,
				  void (*fillFunction)( istream&, heuristicClass * ), 
				  heuristicClass *(*constructor)( void ) )
{
  registerClass::insertRegistration( registeredElements,
				     registrationId,
				     (void (*)( istream&, void * )) fillFunction,
				     (void *(*)( void )) constructor );
}

void
heuristicClass::cleanup( void )
{
  registerClass::cleanup( registeredElements );
}

void
heuristicClass::write( ostream& os )
{
  // nothing to do
}

heuristicClass *
heuristicClass::read( istream& is )
{
  char *id;
  heuristicClass *heuristic;

  id = registerClass::getRegisteredId( is );
  heuristic = (heuristicClass*)registerClass::readElement( is, id, registeredElements );
  delete id;
  return( heuristic );
}

void
heuristicClass::fill( istream& is, heuristicClass *heuristic )
{
  // nothing to do
}
