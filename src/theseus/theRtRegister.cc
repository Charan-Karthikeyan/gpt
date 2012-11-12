//  Theseus
//  theRtRegister.cc -- RunTime Problem Register 
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>

using namespace std;

#include <theseus/theRtRegister.h>

problemHandleClass *
getHandle( const char* problemFilename, const char* workingDir, const char *entryPoint )
{
  void *dlHandle;
  char *objFilename;
  problemHandleClass *(*registerFunction)( void );
  problemHandleClass *handle;

  assert( problemFilename != NULL );

  // append the . path to objectFileName
  objFilename = new char[strlen( problemFilename ) + strlen( workingDir ) + 2];
  if( *problemFilename != '/' )
    sprintf( objFilename, "%s/%s", workingDir, problemFilename );
  else
    strcpy( objFilename, problemFilename );

  // open shared object with problem description
  if( !(dlHandle = dlopen( objFilename, RTLD_NOW )) )
    {
      cerr << dlerror() << endl;
      delete[] objFilename;
      return( NULL );
    }
  delete[] objFilename;

  // find the problem register function in the shared object
  if( !(registerFunction = (problemHandleClass*(*)( void )) dlsym( dlHandle, entryPoint )) )
    {
      cerr << dlerror() << endl;
      return( NULL );
    }

  // get handle
  handle = (*registerFunction)();
  handle->dlHandle = dlHandle;

  // return value
  return( handle );
}

void
freeHandle( problemHandleClass *handle )
{
  void *dlHandle;

  dlHandle = handle->dlHandle;
  (*(handle->cleanUp))();
  delete handle;

  // close handle
  if( dlclose( dlHandle ) )
    cerr << dlerror() << endl;
}
