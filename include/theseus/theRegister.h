//  Theseus
//  theRegister.h -- Serialization Register Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theRegister_INCLUDE_
#define _theRegister_INCLUDE_


///////////////////////////////////////////////////////////////////////////////
//
// Register Class
//

class registerClass
{
public:
  // members
  char *registeredId;
  void (*fillFunction)( istream&, void * );
  void *(*constructor)( void );
  registerClass *next;

  // constructors
  registerClass( const char *id,
		 void (*fillFunction)( istream&, void * ), 
		 void *(*constructor)( void ) );
  ~registerClass();

  // methods
  void*         read( char *id, istream& is );

  // static methods
  static void   cleanup( registerClass *&registeredElements );
  static void   insertRegistration( registerClass *&registeredElements,
				    const char *id,
				    void (*fillFunction)( istream&, void * ), 
				    void *(*constructor)( void ) );
  static char*  getRegisteredId( istream& is );
  static void*  readElement( istream& is, char *id, registerClass *registeredElements );
  static void   (*getFillFunction( char *id, registerClass *registeredElements ))( istream&, void * );
  static void*  (*getConstructor( char *id, registerClass *registeredElements ))( void ); 
};


#endif // _theRegister_INCLUDE
