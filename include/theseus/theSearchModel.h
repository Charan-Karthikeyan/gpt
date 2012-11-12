//  Theseus
//  theSearchModel.h -- Search Model Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theSearchModel_INCLUDE_
#define _theSearchModel_INCLUDE_

#include <theseus/theStandardModel.h>


///////////////////////////////////////////////////////////////////////////////
//
// Standard Model Class
//

class searchModelClass : public standardModelClass
{
private:
  // private methods
  searchModelClass( const searchModelClass& model ) { }

public:
  // constructors/destructors
  searchModelClass( void ) { }
  virtual ~searchModelClass( void ) { }

  // virtual functions
  virtual float               cost( const beliefClass *belief, int action ) const;
  virtual bool                applicable( const beliefClass* belief, int action ) const;
  virtual bool                inGoal( const beliefClass *belief ) const;

  // serialization
  static  void                checkIn( void );
  static  searchModelClass*   read( istream& is );
  static  searchModelClass*   constructor( void );
  static  void                fill( istream& is, searchModelClass *model );
};


#endif // _theSearchModel_INCLUDE
