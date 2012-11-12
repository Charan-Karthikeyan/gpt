#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <daedalus/_daeErrors.h>


#define FATAL_BASE      1000
#define ERROR_BASE      2000
#define WARNING_BASE    3000


static char message_buffer[1024];
struct message_t {
  char *msg;
  int  args;
};


#undef  FATALMSG
#undef  ERRORMSG
#undef  WARNINGMSG
#define FATALMSG(name,id,args,msg)    { msg, args },
#define ERRORMSG(name,id,args,msg)
#define WARNINGMSG(name,id,args,msg)
static const message_t fatalMessages[] = {
#include "daeErrors.msg"
};
#undef  FATALMSG
#undef  ERRORMSG
#undef  WARNINGMSG
#define FATALMSG(name,id,args,msg)
#define ERRORMSG(name,id,args,msg)    { msg, args },
#define WARNINGMSG(name,id,args,msg)
static const message_t errorMessages[] = {
#include "daeErrors.msg"
};
#undef  FATALMSG
#undef  ERRORMSG
#undef  WARNINGMSG
#define FATALMSG(name,id,args,msg)
#define ERRORMSG(name,id,args,msg)
#define WARNINGMSG(name,id,args,msg)  { msg, args },
static const message_t warningMessages[] = {
#include "daeErrors.msg"
};


int          numErrors = 0;
extern int   lineno;
extern char* yyfile;
extern char* yyparser;

const char** errorMsgs = NULL;
static int   errorMsgsSize = 0;
static int   errorMsgsIndex = 0;


//
// A fatal error has occured, probably there is some memory corrupted. 
// Parsing can't continue.
//
void
_fatal( int line, const char *file, int code )
{
  printErrors( stderr );

  if( code == usage )
    fprintf( stderr, fatalMessages[code - FATAL_BASE].msg, yyparser );
  else
    fprintf( stderr, "<%s:%d>fatal when parsing %s:%d:%s\n",
	     file, line, yyfile, lineno, fatalMessages[code - FATAL_BASE].msg );

  exit( code );
}


//
// A semantic error has been detected inside the file. Report it,
// and continue Parsing.
//
void
error( int code, ... )
{
  char *s1, *s2, *buff;
  va_list ap;

  ++numErrors;
  sprintf( message_buffer, "error %d in %s:%d:", code, yyfile, lineno );
  strcat( message_buffer, errorMessages[code - ERROR_BASE].msg );
  strcat( message_buffer, "\n" );
  
  va_start( ap, code );
  switch( errorMessages[code - ERROR_BASE].args )
    {
    case 0:
      insertError( strdup( message_buffer ) );
      break;
    case 1:
      s1 = va_arg( ap, char * );
      buff = new char[strlen( message_buffer ) + strlen( &s1[1] ) + 1];
      sprintf( buff, message_buffer, &s1[1] );
      insertError( buff );
      break;
    case 2:
      s1 = va_arg( ap, char * );
      s2 = va_arg( ap, char * );
      buff = new char[strlen( message_buffer ) + strlen( &s1[1] ) + strlen( &s1[2] ) + 1];
      sprintf( buff, message_buffer, &s1[1], &s2[1] );
      insertError( buff );
      break;
    }
  va_end( ap );
}


void
warning( int code, ... )
{
  va_list ap;

  sprintf( message_buffer, "warning %d in %s:%d:", code, yyfile, lineno );
  strcat( message_buffer, warningMessages[code - WARNING_BASE].msg );
  strcat( message_buffer, "\n" );

  va_start( ap, code );
  vfprintf( stderr, message_buffer, ap );
  va_end( ap );
}


void
insertError( const char *errorMsg )
{
  // check space in errorMsgs
  if( errorMsgsIndex == errorMsgsSize )
    {
      errorMsgsSize = (errorMsgsSize == 0 ? 16 : 2 * errorMsgsSize);
      errorMsgs = (const char **) realloc( errorMsgs, errorMsgsSize * sizeof( const char * ) );
    }
  errorMsgs[errorMsgsIndex++] = errorMsg;
}


void
insertFmtError( const char *errorFmt, ... )
{
  va_list ap;
  static char buff[1024];

  va_start( ap, errorFmt );
  vsprintf( buff, errorFmt, ap );
  insertError( strdup( buff ) );
  va_end( ap );
}


void
printErrors( FILE *file )
{
  int i;

  for( i = 0; i < errorMsgsIndex; ++i )
    fprintf( file, errorMsgs[i] );
}
