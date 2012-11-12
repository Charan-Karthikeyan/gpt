//  Theseus
//  theMemoryMgmt.cc -- Memory Management Implemetantion
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <theseus/theMemoryMgmt.h>
using namespace std;


void *
operator new( size_t size )
{
#if defined(MDEBUG) && defined(PRINTINFO)
  cerr << "operator new called: ";
#endif
  return( __gpt_memory_control<0>::allocate(size) );
}

void *
operator new[]( size_t size )
{
#if defined(MDEBUG) && defined(PRINTINFO)
  cerr << "operator new[] called: ";
#endif
  return( __gpt_memory_control<0>::allocate(size) );
}

void
operator delete( void *ptr )
{
  if( ptr )
    {
#if defined(MDEBUG) && defined(PRINTINFO)
      cerr << "operator delete called: ";
#endif
      __gpt_memory_control<0>::deallocate( ptr );
    }
}

void
operator delete[]( void *ptr )
{
  if( ptr )
    {
#if defined(MDEBUG) && defined(PRINTINFO)
      cerr << "operator delete[] called: ";
#endif
      __gpt_memory_control<0>::deallocate( ptr );
    }
}
