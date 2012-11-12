//  Theseus
//  theRtRegister.h -- RunTime Problem Register 
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theRtRegister_INCLUDE
#define _theRtRegister_INCLUDE

#include <theseus/theRtStandard.h>

problemHandleClass* getHandle( const char* problemFilename, const char* workingDir, const char* entryPoint );
void freeHandle( problemHandleClass *handle );

#endif // _theRtRegister_INCLUDE
