//  Theseus
//  theNonDetModel.h -- Non-Deterministic Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theNonDetModel_INCLUDE_
#define _theNonDetModel_INCLUDE_

#include <theseus/theStandardModel.h>


///////////////////////////////////////////////////////////////////////////////
//
// Non-Deterministic Model Class
//

class nonDetModelClass : public standardModelClass
{
private:
  // private methods
  nonDetModelClass( const nonDetModelClass& model ) { }

public:
  // constructors/destructors
  nonDetModelClass( void ) { }
  virtual ~nonDetModelClass( void ) { }

  // virtual functions
  virtual float               cost( const beliefClass *belief, int action ) const;
  virtual bool                applicable( const beliefClass* belief, int action ) const;
  virtual bool                inGoal( const beliefClass *belief ) const;

  // serialization
  static  void                checkIn( void );
  static  nonDetModelClass*   read( istream& is );
  static  nonDetModelClass*   constructor( void );
  static  void                fill( istream& is, nonDetModelClass *model );
};


#endif // _theNonDetModel_INCLUDE
