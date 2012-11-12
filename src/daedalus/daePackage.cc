#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>

#include <daedalus/daePackage.h>
#include <daedalus/daeStack.h>
#include <daedalus/_daeErrors.h>


static stackClass *packageStack;
static int packageProcessingError;

extern FILE *yyin;
extern int  lineno;
extern char *yyfile;
extern char *libPath;


// prototypes
static char *findPkgFile( const char * );


packageClass::packageClass( const char *ident, FILE *oldInput, int oldLineno, const char *oldFile )
{
  packageClass::ident = (char*)ident;
  packageClass::oldInput = oldInput;
  packageClass::oldLineno = oldLineno;
  packageClass::oldFile = (char*)oldFile;
}


void
initPackages( void )
{
  packageStack = new stackClass();
}


void
startPackageParse( const char *ident )
{
  char *file;
  FILE *input;
  packageClass *package;
  static char buff[128];

  strcpy( buff, ident );
  strcat( buff, ".pkg" );
  
  /* save context */
  package = new packageClass( strdup( buff ), yyin, lineno, yyfile );
  packageStack->push( (void *) package );

  file = findPkgFile( &buff[1] );
  if( (input = fopen( file, "r" )) == NULL )
    {
      insertFmtError( "%s: %s\n", file, strerror( errno ) );
      packageProcessingError = 1;
    }
  else
    {
      /* change context */
      yyin = input;
      lineno = 1;
      yyfile = (char*)&package->ident[1];
      packageProcessingError = 0;
    }
  //  free( file );
}


void
finishPackageParse( void )
{
  packageClass *package;

  package = (packageClass *) packageStack->pop();
  
  if( !packageProcessingError )
    {
      fclose( yyin );
      yyin = package->oldInput;
    }
  
  /* restore context */
  lineno = package->oldLineno - 1;
  yyfile = (char*)package->oldFile;
  packageProcessingError = 0;

  //  free( package->ident );
  //  free( package );
}


void
startPackage( const char *ident )
{
  packageClass *package;
  char *s, *p;

  package = (packageClass *) packageStack->top();
  s = strdup( package->ident );
  p = strchr( s, '.' );
  *p = '\0';

  if( strcmp( ident, s ) )
    error( wrongPackageName, ident, s );

  //  free( s );
}


void
finishPackage( void )
{
}


static char *
findPkgFile( const char *file )
{
  char *tmp, *ptr;
  static char buff[1024];
  static struct stat fileStat; 

  tmp = ptr = strdup( libPath );
  for( ptr = strtok( ptr, ":" ); ptr != NULL; ptr = strtok( NULL, ":" ) )
    {
      sprintf( buff, "%s/%s", ptr, file );
      if( stat( buff, &fileStat ) == 0 )
	return( strdup( buff ) );
    }
  return( strdup( file ) );
}
