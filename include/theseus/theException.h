//  Theseus
//  theExceptions.h -- Exceptions 
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theExceptions_INCLUDE_
#define _theExceptions_INCLUDE_

#include <theseus/theModel.h>
#include <theseus/theResult.h>
#include <theseus/theBelief.h>
#include <theseus/theRegister.h>


///////////////////////////////////////////////////////////////////////////////
//
// Exceptions Class
//

class exceptionClass
{
 public:
  exceptionClass( void ) { }
  virtual void print( ostream& out ) = 0;
};

class signalExceptionClass : public exceptionClass
{
 public:
  int signal;
  signalExceptionClass( int s ) { signal = s; }
  virtual void print( ostream& out );
};

inline void
signalExceptionClass::print( ostream& out )
{
#ifdef SOLARIS
  char buff[SIG2STR_MAX];
  sig2str( signal, buff );
  out << endl << "Error: interrupted by signal SIG" << buff << "(" << signal << ")." << endl;
#else
  out << endl << "Error: interrupted by signal SIG" << signal << "." << endl;
#endif
}

class unsupportedModelExceptionClass : public exceptionClass
{
 public:
  int model;
  unsupportedModelExceptionClass( int m ) { model = m; }
  virtual void print( ostream& out );
};

inline void
unsupportedModelExceptionClass::print( ostream& out )
{
  out << endl << "Error: unsupported model ";
  switch( model )
    {
    case problemHandleClass::PROBLEM_PLANNING:
      out << "PLANNING";
      break;
    case problemHandleClass::PROBLEM_MDP:
      out << "MDP";
      break;
    case problemHandleClass::PROBLEM_ND_MDP:
      out << "NON-DETERMINISTIC MDP";
      break;
    case problemHandleClass::PROBLEM_POMDP1:
    case problemHandleClass::PROBLEM_POMDP2:
      out << "POMDP";
      break;
    case problemHandleClass::PROBLEM_ND_POMDP1:
    case problemHandleClass::PROBLEM_ND_POMDP2:
      out << "NON-DETERMINISTIC POMDP";
      break;
    case problemHandleClass::PROBLEM_CONFORMANT1:
    case problemHandleClass::PROBLEM_CONFORMANT2:
      out << "CONFORMANT";
      break;
    }
  out << endl;
}


#endif // _theExceptions_INCLUDE
