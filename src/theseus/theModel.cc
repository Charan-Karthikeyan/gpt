//  Theseus
//  theModel.cc -- Abstract Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>

using namespace std;

#include <theseus/theModel.h>
#include <theseus/theBelief.h>
#include <theseus/theRegister.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Model Class
//

// static members
registerClass* modelClass::registeredElements = NULL;

modelClass::~modelClass()
{
}

beliefClass*
modelClass::getTheInitialBelief( void )
{
  return( theInitialBelief );
}

void
modelClass::signRegistration( const char *registrationId,
			      void (*fillFunction)( istream&, modelClass * ), 
			      modelClass *(*constructor)( void ) )
{
  registerClass::insertRegistration( registeredElements,
				     registrationId,
				     (void (*)( istream&, void * )) fillFunction,
				     (void *(*)( void )) constructor );
}

void
modelClass::cleanup( void )
{
  registerClass::cleanup( registeredElements );
}

void
modelClass::write( ostream& os ) const
{
  safeWrite( &numActions, sizeof( int ), 1, os );
  safeWrite( &discountFactor, sizeof( float ), 1, os );
  theInitialBelief->write( os );
}

modelClass *
modelClass::read( istream& is )
{
  char *id;
  modelClass *model;

  id = registerClass::getRegisteredId( is );
  model = (modelClass *) registerClass::readElement( is, id, registeredElements );
  delete id;
  return( model );
}

void
modelClass::fill( istream& is, modelClass *model )
{
  safeRead( &model->numActions, sizeof( int ), 1, is );
  safeRead( &model->discountFactor, sizeof( float ), 1, is );
  model->theInitialBelief = beliefClass::read( is );
}
