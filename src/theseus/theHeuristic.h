//  Theseus
//  theHeuristic.h -- Abstarct Belief Heuristic Class
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theHeuristic_INCLUDE_
#define _theHeuristic_INCLUDE_


class POMDPClass;
class beliefClass;
class registerClass;


///////////////////////////////////////////////////////////////////////////////
//
// Heuristic Class
//

class heuristicClass
{
  // static members
  static registerClass *registeredElements;

public:
  // constructors
  heuristicClass() { }

  // destructor
  virtual ~heuristicClass() { }

  // virtual methods
  virtual float  value( int stateIndex ) = 0;
  virtual float  value( const beliefClass *belief ) = 0;

  // serialization
  static  void signRegistration( const char *registrationId,
				 void (*fillFunction)( istream&, heuristicClass * ), 
				 heuristicClass *(*constructor)( void ) );
  static  void            cleanup( void );
  virtual void            write( ostream& os );
  static  heuristicClass* read( istream& is );
  static  void            fill( istream& is, heuristicClass *heuristic );
};


#endif // _theHeuristic_INCLUDE
