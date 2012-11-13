#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

#include <daedalus/daeTypes.h>
#include <daedalus/daeTokens.h>
#include <daedalus/_daeErrors.h>


extern int lineno;
extern char *yyfile, *yyparser;
extern struct topList_s *top;

extern int  yyparse( void );
extern void initParser( void );


int  generatedProblems;
char *problemName, *fullProblemName;
char **fileNames;
char *prefix;
char *libPath;
char *HName, *CName, *MName;


static void
parseArguments( int argc, char **argv )
{
  char **file;
  static struct stat statBuff;

  // set default values
  prefix = NULL;
  generatedProblems = 0;
  yyparser = argv[0];
  libPath = (char*)"";

  // parse options
  while( (argc > 1) && (*(*++argv) == '-') )
    {
      switch( (*argv)[1] )
      {
      case 'P':
	prefix = *++argv;
	--argc;
	break;
      default:
	fatal( usage );
	break;
      }
      --argc;
    }
  --argv;
  if( argc < 3 )
    fatal( usage );

  // what remains in argv is the name of a problem and a sequence 
  // of pddl files to be parsed.
  problemName = *++argv;
  --argc;
  fileNames = file = new char*[argc];
  while( argc > 1 )
    {
      if( stat( *++argv, &statBuff ) )
	{
	  fprintf( stderr, "%s: %s\n", *argv, strerror( errno ) );
	  exit( -1 );
	}
      *file++ = *argv;
      --argc;
    }
  *file = NULL;
}

static void
generateCodeForProblem( problemClass *problem )
{
  FILE *HFile, *CFile, *MFile;
  extern void generateCode( FILE *, char *, FILE *, FILE *, char *, problemClass * );

  // set fullproblem name
  if( prefix == NULL )
    {
      fullProblemName = new char[strlen( problem->ident ) + strlen( problem->domain->ident )];
      sprintf( fullProblemName, "%s%s", &(problem->ident)[1], problem->domain->ident );
    }
  else
    {
      fullProblemName = new char[1 + strlen( prefix)];
      strcpy( fullProblemName, prefix );
    }

  // create .h output file
  HName = new char[3 + strlen( fullProblemName )];
  sprintf( HName, "%s.h", fullProblemName );
  if( (HFile = fopen( HName, "w" )) == NULL )
    {
      fprintf( stderr, "%s: %s\n", HName, strerror( errno ) );
      exit( -1 );
    }

  // create -state.cc output file
  CName = new char[10 + strlen( fullProblemName )];
  sprintf( CName, "%s-state.cc", fullProblemName );
  if( (CFile = fopen( CName, "w" )) == NULL )
    {
      fprintf( stderr, "%s: %s\n", CName, strerror( errno ) );
      exit( -1 );
    }

  // create Makefile output file
  MName = new char[10 + strlen( fullProblemName )];
  sprintf( MName, "Makefile.%s", fullProblemName );
  if( (MFile = fopen( MName, "w" )) == NULL )
    {
      fprintf( stderr, "%s: %s\n", MName, strerror( errno ) );
      exit( -1 );
    }

  // call genCode
  generateCode( HFile, HName, CFile, MFile, MName, problem );

  // free resources
  fclose( HFile );
  fclose( CFile );
  fclose( MFile );
}

void
generateCode( problemClass *problem )
{
  if( (numErrors == 0) && !strcmp( &(problem->ident)[1], problemName ) )
    {
      ++generatedProblems;
      generateCodeForProblem( problem );
    }
  delete problem;
}

int
main( int argc, char **argv )
{
  int rv, fd, file;
  extern void cleanDomains( void );

  // initialization
  parseArguments( argc, argv );
  initParser();

  // parse files
  rv = 0;
  fd = -1;
  file = 0;
  while( fileNames[file] != NULL )
    {
      if( (fd = open( fileNames[file], O_RDONLY )) == -1 )
	{
	  insertFmtError( "%s: %s\n", fileNames[file], strerror( errno ) );
	  exit( -1 );
	}
      else
	{
	  // redirection of fd to stdin
	  close( fileno( stdin ) );
	  dup( fd );
	  yyfile = fileNames[file];
	  lineno = 1;
	  rv += yyparse();
	  close( fd );
	}
      ++file;
    }
  close( fileno( stdin ) );
  close( fd );
  delete[] fileNames;

  // clean
  cleanDomains();

  // print error messages
  printErrors( stderr );

  // check if something was generated
  if( (rv == 0) && (generatedProblems == 0) )
    {
      if( numErrors == 0 )
	{
	  rv = -1;
	  fprintf( stderr, "problem \"%s\" not found in input file(s).\n", problemName );
	}
    }

  // exit
  if( (rv != 0) || (numErrors != 0) )
    {
      fprintf( stdout, "errors found. Nothing generated.\n" );
      exit( -1 );
    }
  else
    {
      fprintf( stdout, "Successful parse for \"%s\".\n", fullProblemName );
      fprintf( stdout, "Files generated: %s %s %s\n", MName, CName, HName );
      exit( rv );
    }
}
