#ifndef _daePackage_INCLUDE_
#define _daePackage_INCLUDE_

#include <stdio.h>


class packageClass
{
public:
  // members
  char    *ident;
  char    *oldFile;
  FILE    *oldInput;
  int     oldLineno;

  // constructors
  packageClass( const char *ident, FILE *oldInput, int oldLineno, const char *oldFile );
};


extern void initPackages( void );
extern void startPackageParse( register const char * );
extern void finishPackageParse( void );
extern void startPackage( register const char * );
extern void finishPackage( void );


#endif  /* _daePackage_INCLUDE */
