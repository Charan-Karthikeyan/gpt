//  Theseus
//  theShell.cc -- GPT main shell module
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>

#include <readline-4.2/readline.h>
#include <readline-4.2/history.h>

#include <iostream>
#include <fstream>

using namespace std;

#include <theseus/theTopLevel.h>
#include <theseus/thePOMDP.h>
#include <theseus/theBeliefCache.h>
#include <theseus/theRtRegister.h>
#include <theseus/theException.h>
#include <theseus/theUtils.h>

#define  WELCOME        "Welcome to GPT" 
#define  PROMPT         "gpt> "
#define  RLC_COMMANDS   0
#define  RLC_FILES      1

#define HISTORY_FILE    "~/.gpt_history"
#define HISTORY_SIZE    1024

static struct
{
  char* gpthome;
  char* cc;
  char* ccflags;
  char* ld;
  char* ldflags;
  char* include;
  char* lib;
  char* entry;
} sys = { 0, 0, 0, 0, 0, 0, 0 };

static void  fillDefaultValues( void );
static void  fillHelpDB( void );
static void  setDefaultValue( const char* var, bool silent );


///////////////////////////////////////////////////////////////////////////////
//
// Classes
//

class commandClass
{
public:
  const char*   name;
  const char*   pattern;
  int           subexprs;
  regex_t       regex;
  int           (*commandFunction)(int,char**);
  commandClass( const char* name, const char* pattern, int subexprs, int (*commandFunction)(int,char**) );
  ~commandClass();
};


///////////////////////////////////////////////////////////////////////////////
//
// Prototypes and Global Vars
//

static void            commandRegistration();
static void            commandCleanup();
static commandClass*   commandParse( char* input, int& nargs, char**& args );

static void            clearLineBuffer();
static void            initializeReadline();
static void            readlineCompletionMode( int mode );

// Globals
static char*           currentProblem = 0;
static bool            loadedObject = false;
static bool            loadedCore = false;
static ostream*        terminal = &cout;
static struct timeval  startTime, endTime;
static unsigned long   secs, usecs;


///////////////////////////////////////////////////////////////////////////////
//
// Main Module
//

static void
help( void )
{
  cerr << "(under construction)" << endl;
}

static void
stripWhite( char* str )
{
  // strip prefix
  char *p = str;
  while( whitespace(*p) ) {
    for( char *t = p; *t; *t = *(t+1), ++t );
  }

  // strip others
  for( p = str; *p; ++p ) {
    while( whitespace(*p) && whitespace(*(p+1)) ) {
      for( char *t = p; *t; *t = *(t+1), ++t );
    }
  }

  // strip tail
  char *t = str + strlen(str) - 1;
  while( (t > str) && whitespace(*t) ) t--;
  *++t = '\0';
}

static void
evalLoop( void )
{
  commandClass *cmdPtr;
  char *line, **args, *history_file;
  int nargs, arg, rv;

  // history initialization
  history_file = tilde_expand( HISTORY_FILE );
  using_history();
  read_history( history_file );
  stifle_history( HISTORY_SIZE );

  initializeReadline();
  readlineCompletionMode( RLC_COMMANDS );
  rv = 1;
  while( (line = readline( PROMPT )) != 0 )
    {
      stripWhite( line );
      if( line[0] != '\0' )
	{
	  add_history( line );
	  if( (cmdPtr = commandParse( line, nargs, args )) != 0 )
	    {
	      // execute command
	      try {
		rv = (*cmdPtr->commandFunction)( nargs, args );
	      }
	      catch( signalExceptionClass& e ) {
		e.print( *terminal );
	      }
	      catch( unsupportedModelExceptionClass& e ) {
		e.print( *terminal );
	      }
	      catch(...) {
		cerr << endl << "Error: unexpected exception." << endl;
	      }

	      // clean arguments
	      if( nargs > 0 )
		{
		  for( arg = 0; arg <= nargs; ++arg )
		    if( args[arg] )
		      delete[] args[arg];
		  delete[] args;
		}
	    }
	  else
	    {
	      *terminal << "Unrecognized command '" << line << "'." << endl;
	    }
	}
      free( line );

      // check if need to quit
      if( !rv )
	break;
    }

  // write history
  write_history( history_file );
  free( history_file );
}

void
setCurrentProblem( const char* str )
{
  char *buff;

  if( (!str && !currentProblem) || 
      (str && (!currentProblem || strcmp( str, currentProblem ))) )
    {
      if( !str && !currentProblem )
	{
	  readlineCompletionMode( RLC_FILES );
	  while( (buff = readline( "Input problem: " )) )
	    {
	      stripWhite( buff );
	      if( buff[0] != '\0' )
		break;
	      free( buff );
	    }
	  readlineCompletionMode( RLC_COMMANDS );
	}
      else
	{
	  // clean
	  if( loadedObject )
	    {
	      PD.clean( problemClass::CLEAN_OBJECT );
	      freeHandle( PD.handle );
	    }
	  buff = strdup( str );
	}

      // set new problem
      delete[] currentProblem;
      delete[] PD.problemFile;
      currentProblem = new char[1 + strlen( buff )];
      PD.problemFile = new char[3 + strlen( buff )];

      strcpy( currentProblem, buff );
      if( PD.pddlProblemType )
	sprintf( PD.problemFile, "%s.o", buff );
      else
	strcpy( PD.problemFile, buff );

      free( buff );

      loadedObject = false;
      loadedCore = false;
    }
}

static int
bootstrap( void )
{
  int rv;
  char *cwd;

#if 1
  cwd = getcwd( 0, 1024 );
#else
  cwd = getcwd( 0, 0 );
#endif
  *terminal << "Bootstraping..." << endl;
  terminal->flush();
  if( bootstrapProblem( cwd, sys.entry ) )
    {
      delete[] currentProblem;
      delete[] PD.problemFile;
      currentProblem = 0;
      PD.problemFile = 0;
      rv = 0;
    }
  else
    {
      loadedObject = true;
      loadedCore = false;
      rv = 1;
    }
  *terminal << "Bootstraping... done!" << endl;
  free( cwd );
  return( rv );
}

static int
readcore( const char* coreFilename )
{
  *terminal << "Reading core... ";
  terminal->flush();
  if( readCore( coreFilename ) )
    return( 0 );
  *terminal << "done!." << endl;
  loadedCore = true;
  return( 0 );
}

void
signalHandler( int s )
{
  if( RL_ISSTATE(RL_STATE_READCMD) )
    {
      rl_crlf();
      rl_on_new_line();
      clearLineBuffer();
      rl_redisplay();
    }
  PD.signal = s;
  signal( s, signalHandler );
}

int
main( int argc, char** argv )
{
  // get GPTHOME from env
  if( !(sys.gpthome = getenv("GPTHOME")) ) {
    cerr << "Fatal Error: need to set variable GPTHOME." << endl;
    exit(-1);
  }

  // parse arguments & initialization
  signal(SIGINT,signalHandler);
  fillDefaultValues();
  fillHelpDB();
  parseArguments(argc,argv,&help);
  setDefaultValue(0,true);
  internalInitialization();
  commandRegistration();
  *terminal << WELCOME << ", Version " << PD.softwareRevision << "." << endl;

  // bootstrap problem
  if( PD.problemFile ) {
    char *str = PD.problemFile;
    PD.problemFile = 0;
    setCurrentProblem(str);
    bootstrap();
  }

  // eval loop
  evalLoop();

  // cleanup
  delete[] sys.cc;
  delete[] sys.ccflags;
  delete[] sys.ld;
  delete[] sys.ldflags;
  delete[] sys.include;
  delete[] sys.lib;

  delete[] currentProblem;
  delete[] PD.problemFile;
  delete[] PD.linkmap;
  commandCleanup();
  internalFinalization();

  // return
#if 0
#ifndef NDEBUG
  memoryReport( false );
#endif
#endif
  exit(0);
}


///////////////////////////////////////////////////////////////////////////////
//
// Default Values & Help Messages
//

struct stringCompareFunction {
  bool operator()( const char* s1, const char* s2 ) const { return(strcmp(s1,s2)<0); }
};

struct stringCaseCompareFunction {
  bool operator()( const char* s1, const char* s2 ) const { return(strcasecmp(s1,s2)<0); }
};

static map<const char*,const char*,stringCaseCompareFunction> defaultValues;

static void
fillDefaultValues()
{
  defaultValues.insert( make_pair( (const char*)"version", (const char*)"grid-points" ) );
  defaultValues.insert( make_pair( (const char*)"stoprule", (const char*)"0.000000001" ) );
  defaultValues.insert( make_pair( (const char*)"epsilon", (const char*)"0.000001" ) );
  defaultValues.insert( make_pair( (const char*)"cutoff", (const char*)"off" ) );
  defaultValues.insert( make_pair( (const char*)"runs", (const char*)"1" ) );
  defaultValues.insert( make_pair( (const char*)"pims", (const char*)"[1,1,0]" ) );
  defaultValues.insert( make_pair( (const char*)"discount", (const char*)"1.0" ) );
  defaultValues.insert( make_pair( (const char*)"discretization-levels", (const char*)"20" ) );
  defaultValues.insert( make_pair( (const char*)"qmdp-discount", (const char*)"1.00" ) );
  defaultValues.insert( make_pair( (const char*)"incremental-mode", (const char*)"off" ) );
  defaultValues.insert( make_pair( (const char*)"heuristic-lookahead", (const char*)"0" ) );
  defaultValues.insert( make_pair( (const char*)"zero-heuristic", (const char*)"off" ) );
  defaultValues.insert( make_pair( (const char*)"random-ties", (const char*)"off" ) );
  defaultValues.insert( make_pair( (const char*)"core-level", (const char*)"0" ) );
  defaultValues.insert( make_pair( (const char*)"output-level", (const char*)"0" ) );
  defaultValues.insert( make_pair( (const char*)"verbose-level", (const char*)"0" ) );
  defaultValues.insert( make_pair( (const char*)"precision", (const char*)"6" ) );
  defaultValues.insert( make_pair( (const char*)"action-cache", (const char*)"[20240,80]" ) );
  defaultValues.insert( make_pair( (const char*)"obs-cache", (const char*)"[20240,80]" ) );
  defaultValues.insert( make_pair( (const char*)"other-cache", (const char*)"[10240,40]" ) );

  defaultValues.insert( make_pair( (const char*)"cc", (const char*)"g++" ) );
  defaultValues.insert( make_pair( (const char*)"ccflags", (const char*)"" ) );
  //defaultValues.insert( make_pair( (const char*)"ld", (const char*)"ld" ) );
  //defaultValues.insert( make_pair( (const char*)"ldflags", (const char*)"-G" ) );
  defaultValues.insert( make_pair( (const char*)"ld", (const char*)"g++" ) );
  defaultValues.insert( make_pair( (const char*)"ldflags", (const char*)"-shared" ) );
  defaultValues.insert( make_pair( (const char*)"include-dir", (const char*)"$GPTHOME/include:." ) );
  defaultValues.insert( make_pair( (const char*)"lib-dir", (const char*)"$GPTHOME/lib" ) );
  defaultValues.insert( make_pair( (const char*)"entry-point", (const char*)"problemRegister" ) );

#if 0
#if defined(GNU_CC) || defined(SUN_CC)
#  ifdef GNU_CC
#    if __GNUC__ == 2
       defaultValues.insert( make_pair( (const char*)"entry-point", (const char*)"problemRegister__Fv" ) );
#    else
       defaultValues.insert( make_pair( (const char*)"entry-point", (const char*)"_Z15problemRegisterv" ) );
#    endif
#  else
     defaultValues.insert( make_pair( (const char*)"entry-point", (const char*)"__1cPproblemRegister6F_pnSproblemHandleClass__" ) );
#  endif
#else
#  error "Must define compiler"
#endif
#endif
}

static map<const char*,const char*,stringCaseCompareFunction> helpDB;

static void
fillHelpDB()
{
#define HELP_MSG(command,msg)     helpDB[command] = msg;
#include "theHelpMsg.dat"
#undef HELP_MSG
}


///////////////////////////////////////////////////////////////////////////////
//
// Commands Functions
//

// about function
static const char* aboutPattern  = "about";
static int   aboutSubexprs = 0;
static int   aboutFunction( int nargs, char** args )
{
  *terminal << "(under construction)" << endl;
  return(1);
}

// help function
static const char* helpPattern  = "help( (.*)){0,1}";
static int   helpSubexprs = 2;
static int   helpFunction( int nargs, char** args )
{
  if( args[2] != 0 ) {
    const char *helpMsg = helpDB[args[2]];
    if( helpMsg )
      *terminal << helpMsg << endl;
    else
      *terminal << "Unrecognized help command." << endl;
  }
  else {
    *terminal << helpDB["help"] << endl;
  }
  return(1);
}

// clear screen function
static const char* clearPattern  = "clear";
static int   clearSubexprs = 0;
static int   clearFunction( int, char**  )
{
  rl_clear_screen(0,0);
  rl_crlf();
  rl_on_new_line();
  clearLineBuffer();
  rl_redisplay();
  return(1);
}

// change working directory
static const char* cdPattern  = "cd( ([[:print:]]+)){0,1}";
static int   cdSubexprs = 2;
static int   cdFunction( int nargs, char**  args )
{
  if( args[1] ) {
    if( chdir( args[2] ) )
      cerr << "Error: " << args[2] << ": " << strerror(errno) << "." << endl;
  }
  else {
    *terminal << getcwd(0,0) << endl;
  }
  return(1);
}


// quit/exit function
static const char* quitPattern  = "quit|exit";
static int   quitSubexprs = 0;
static int   quitFunction( int, char**  )
{
  PD.clean(problemClass::CLEAN_OBJECT);
  if( PD.handle != 0 ) freeHandle(PD.handle);
  *terminal << "Bye." << endl;
  return(0);
}

// parse function
static const char* parsePattern  = "parse( ([[:alnum:]_]+)){0,1}( ([[:print:]]+)){0,1}";
static int   parseSubexprs = 4;
static int   parseFunction( int nargs, char** args )
{
  if( !args[1] || !args[3] ) {
    *terminal << "Not enough arguments. Use 'parse <problem-name> <pddl-file>+'." << endl;
    return(1);
  }

  char *cmdline = new char[15+strlen(sys.gpthome)+strlen(args[2])+strlen(args[4])];
  sprintf(cmdline,"%s/bin/parser %s %s",sys.gpthome,args[2],args[4]);

  // register initial time
  gettimeofday(&startTime,0);

  // parse
  *terminal << "-------" << endl << "$GPTHOME/bin/parser " << args[2] << " " << args[4] << endl;
  int rv = system( cmdline );
  *terminal << "-------" << endl;
  if( !rv ) { // print elapsed time
    gettimeofday(&endTime,0);
    diffTime(secs,usecs,startTime,endTime);
    *terminal << (float)secs + ((float)usecs) / 1000000.0 << " seconds." << endl;
  }

  // clean & return
  delete[] cmdline;
  return(1);
}

// print function
static const char* printPattern  = "print( (action|action@|belief|observation|state)( (.+)){0,1}){0,1}";
static int   printSubexprs = 4;
static int   printFunction( int nargs, char** args )
{
  void *ptr;
  char *str, *tok;
  int action, obs, state;
  const hashEntryClass *entry;
  deque<pair<pair<int,float>,const hashEntryClass*> > expansion;
  deque<pair<pair<int,float>,const hashEntryClass*> >::const_iterator it;

  if( !PD.POMDP || !PD.model )
    {
      *terminal << "No model available." << endl;
      return( 1 );
    }

  if( !strcasecmp( args[2], "action" ) )
    {
      if( !args[4] )
	*terminal << "Not enough arguments. Use 'print action <action-id>'." << endl;
      else
	{
	  action = atoi( args[4] );
	  if( (action < 0) || (action >= PD.POMDP->getNumActions()) )
	    *terminal << "Invalid action." << endl;
	  else if( PD.pddlProblemType )
	    {
	      (*PD.handle->printAction)( *terminal, action );
	      *terminal << endl;
	    }
	  else
	    *terminal << "Name not available." << endl;
	}
    }
  else if( !strcasecmp( args[2], "action@" ) )
    {
      if( !args[4] )
	*terminal << "Not enough arguments. Use 'print action@ <belief-id>'." << endl;
      else if( ISCONFORMANT(PD.handle->problemType) || ISPLANNING(PD.handle->problemType) )
	{
	  *terminal << "Print request not supported in CONFORMANT/PLANNING models." << endl;
	}
      else
	{
	  sscanf( args[4], "0x%p", &ptr );
	  entry = (const hashEntryClass*)ptr;
	  if( !entry || (entry != PD.POMDP->getHashEntry( (const beliefClass*)entry->getData() )) )
	    *terminal << "Invalid belief." << endl;
	  else
	    {
	      action = PD.POMDP->getBestAction( entry );
	      if( PD.pddlProblemType )
		{
		  *terminal << "action " << action << " = ";
		  (*PD.handle->printAction)( *terminal, action );
		  *terminal << endl;
		}
	      else
		*terminal << "action " << action << endl;
	    }
	}
    }
  else if( !strcasecmp( args[2], "belief" ) )
    {
      if( !args[4] )
	{
	  *terminal << "Not enough arguments. ";
	  *terminal << "Use 'print belief [<belief-id>|initial|actions <belief-id>|afteraction <belief-id> <action-id>]'.";
	  *terminal << endl;
	}
      else if( ISCONFORMANT(PD.handle->problemType) || ISPLANNING(PD.handle->problemType) )
	{
	  *terminal << "Print request not supported in CONFORMANT/PLANNING models." << endl;
	}
      else if( !strcasecmp( args[4], "initial" ) )
	{
	  entry = PD.POMDP->getTheInitialBelief();
	  action = PD.POMDP->getBestAction( entry );
	  if( ISPOMDP(PD.handle->problemType) || ISNDPOMDP(PD.handle->problemType) )
	    ((const beliefClass*)entry->getData())->checkModelAvailability( PD.model );
	  float hvalue = (!PD.heuristic?0.0:PD.heuristic->value((const beliefClass*)entry->getData()));
	  *terminal << "belief-id = " << entry << endl;
	  *terminal << "value     = " << entry->getValue() << endl;
	  *terminal << "heuristic = " << hvalue << endl;
	  *terminal << "goal      = " << PD.POMDP->inGoal((const beliefClass*)entry->getData()) << endl;
	  *terminal << "solved    = " << (int)entry->getExtra() << endl;
	  *terminal << "action    = " << action;
	  if( PD.pddlProblemType )
	    {
	      *terminal << "=";
	      (*PD.handle->printAction)( *terminal, action );
	    }
	  *terminal << endl;
	  if( ISPOMDP(PD.handle->problemType) || ISNDPOMDP(PD.handle->problemType) )
	    *terminal << "belief    = " << *(const beliefClass*)entry->getData() << endl;
	  else if( ISMDP(PD.handle->problemType) || ISNDMDP(PD.handle->problemType) )
	    *terminal << "state     = " << (int)entry->getData() << endl;
	}
      else if( !strncasecmp( args[4], "actions", 7 ) )
	{
	  str = tok = strdup( &args[4][7] );
	  tok = strtok( tok, " " );
	  if( tok == 0 )
	    {
	      *terminal << "Not enough arguments. Use 'print belief actions <belief-id>'.";
	      *terminal << endl;
	    }
	  else
	    {
	      sscanf( tok, "0x%p", &ptr );
	      entry = (const hashEntryClass*)ptr;
	      if( !entry || (entry != PD.POMDP->getHashEntry( (const beliefClass*)entry->getData() )) )
		*terminal << "Invalid belief id." << endl;
	      else
		{
		  if( ISPOMDP(PD.handle->problemType) || ISNDPOMDP(PD.handle->problemType) )
		    ((const beliefClass*)entry->getData())->checkModelAvailability( PD.model );
		  *terminal << "actions = [ ";
		  for( action = 0; action < PD.POMDP->getNumActions(); ++action )
		    if( PD.POMDP->applicable( (const beliefClass*)entry->getData(), action ) )
		      {
			*terminal << action;
			if( PD.pddlProblemType )
			  {
			    *terminal << "=";
			    (*PD.handle->printAction)( *terminal, action );
			  }
			*terminal << ", " << endl;
		      }
		  *terminal << "]" << endl;
		}
	    }
	  free( str );
	}
      else if( !strncasecmp( args[4], "afteraction", 11 ) )
	{
	  str = tok = strdup( &args[4][11] );
	  tok = strtok( tok, " " );
	  if( tok == 0 )
	    {
	    err:
	      *terminal << "Not enough arguments. Use 'print belief afteraction <belief-id> <action-id>'.";
	      *terminal << endl;
	    }
	  else
	    {
	      sscanf( tok, "0x%p", &ptr );
	      tok = strtok( 0, " " );
	      if( tok == 0 )
		goto err;
	      sscanf( tok, "%d", &action );
	      entry = (const hashEntryClass*)ptr;
	      if( !entry || (entry != PD.POMDP->getHashEntry( (const beliefClass*)entry->getData() )) )
		*terminal << "Invalid belief." << endl;
	      else if( (action < 0) || (action >= PD.POMDP->getNumActions()) )
		*terminal << "Invalid action." << endl;
	      else if( !PD.POMDP->applicable( (const beliefClass*)entry->getData(), action ) )
		*terminal << "Action not applicable in belief." << endl;
	      else
		{
		  // we are ok ..
		  PD.POMDP->expandBeliefWithAction( entry, action, expansion );
		  *terminal << "cost = " <<PD.POMDP->cost((const beliefClass*)entry->getData(),action)<<endl;
		  for( it = expansion.begin(); it != expansion.end(); ++it )
		    {
		      *terminal << "obs-id = " << (*it).first.first;
		      *terminal << ", prob. = " << (*it).first.second;
		      *terminal << ", belief-id = " << (*it).second;
		      *terminal << endl;
		    }
		}
	    }
	  free( str );
	}
      else
	{
	  sscanf( args[4], "0x%p", &ptr );
	  entry = (const hashEntryClass*)ptr;
	  if( !entry || (entry != PD.POMDP->getHashEntry( (const beliefClass*)entry->getData() )) )
	    *terminal << "Invalid belief id." << endl;
	  else
	    {
	      action = PD.POMDP->getBestAction( entry );
	      if( ISPOMDP(PD.handle->problemType) || ISNDPOMDP(PD.handle->problemType) )
		((const beliefClass*)entry->getData())->checkModelAvailability( PD.model );
	      float hvalue = (!PD.heuristic?0.0:PD.heuristic->value((const beliefClass*)entry->getData()));
	      *terminal << "belief-id = " << entry << endl;
	      *terminal << "value     = " << entry->getValue() << endl;
	      *terminal << "heuristic = " << hvalue << endl;
	      *terminal << "goal      = " << PD.POMDP->inGoal((const beliefClass*)entry->getData()) << endl;
	      *terminal << "solved    = " << (int)entry->getExtra() << endl;
	      *terminal << "action    = " << action;
	      if( PD.pddlProblemType )
		{
		  *terminal << "=";
		  (*PD.handle->printAction)( *terminal, action );
		}
	      *terminal << endl;
	      if( ISPOMDP(PD.handle->problemType) || ISNDPOMDP(PD.handle->problemType) )
		*terminal << "belief    = " << *(const beliefClass*)entry->getData() << endl;
	      else if( ISMDP(PD.handle->problemType) || ISNDMDP(PD.handle->problemType) )
		*terminal << "state     = " << (int)entry->getData() << endl;
	    }
	}
    }
  else if( !strcasecmp( args[2], "observation" ) )
    {
      if( !args[4] )
	*terminal << "Not enough arguments. Use 'print observation <obs-id>'." << endl;
      else
	{
	  obs = atoi( args[4] );
	  PD.model->printObservation( obs, *terminal, 0 );
	}
    }
  else if( !strcasecmp( args[2], "state" ) )
    {
      if( !args[4] )
	{
	  *terminal << "Not enough arguments. ";
	  *terminal << "Use 'print state [<state-id>|actions <state-id>|afteraction <state-id> <action-id>]'.";
	  *terminal << endl;
	}
      else if( !strncasecmp( args[4], "actions", 7 ) )
	{
	  str = tok = strdup( &args[4][7] );
	  tok = strtok( tok, " " );
	  if( tok == 0 )
	    {
	      *terminal << "Not enough arguments. Use 'print state actions <state-id>'.";
	      *terminal << endl;
	    }
	  else
	    {
	      state = atoi( tok );
	      *terminal << "actions = [ ";
	      for( action = 0; action < PD.POMDP->getNumActions(); ++action )
		if( PD.POMDP->applicable( state, action ) )
		  {
		    *terminal << action;
		    if( PD.pddlProblemType )
		      {
			*terminal << "=";
			(*PD.handle->printAction)( *terminal, action );
		      }
		    *terminal << ", " << endl;
		  }
	      *terminal << "]" << endl;
	    }
	  free( str );
	}
      else if( !strncasecmp( args[4], "afteraction", 11 ) )
	{
	  str = tok = strdup( &args[4][11] );
	  tok = strtok( tok, " " );
	  if( tok == 0 )
	    {
	    err2:
	      *terminal << "Not enough arguments. Use 'print state afteraction <belief-id> <action-id>'.";
	      *terminal << endl;
	    }
	  else
	    {
	      state = atoi( tok );
	      tok = strtok( 0, " " );
	      if( tok == 0 )
		goto err2;
	      action = atoi( tok );
	      if( (action < 0) || (action >= PD.POMDP->getNumActions()) )
		*terminal << "Invalid action." << endl;
	      else if( !PD.POMDP->applicable( state, action ) )
		*terminal << "Action not applicable in state." << endl;
	      else
		{
		  // we are ok ..
		  state = PD.model->nextState( state, action );
		  PD.model->checkModelFor( state );
		  float hvalue = (!PD.heuristic?0.0:PD.heuristic->value(state));
		  *terminal << "state     = " << state << endl;
		  *terminal << "goal      = " << PD.model->inGoal( state ) << endl;
		  *terminal << "heuristic = " << hvalue << endl;
		  *terminal << "state:" << endl;
		  PD.model->printState( state, *terminal, 2 );
		}
	    }
	  free( str );
	}
      else
	{
	  state = atoi( args[4] );
	  PD.model->checkModelFor( state );
	  float hvalue = (!PD.heuristic?0.0:PD.heuristic->value(state));
	  *terminal << "goal      = " << PD.model->inGoal( state ) << endl;
	  *terminal << "heuristic = " << hvalue << endl;
	  *terminal << "state:" << endl;
	  PD.model->printState( state, *terminal, 2 );
	}
    }
  return( 1 );
}

// linkmap function
static const char* linkmapPattern  = "linkmap( (\\+|-)){0,1}( ([[:print:]]+)){0,1}";
static int   linkmapSubexprs = 4;
static int   linkmapFunction( int nargs, char** args )
{
  if( args[4] ) {
    if( !args[2] || (args[2][0] == '+') ) {
      char *tmp = new char[2+(PD.linkmap?strlen(PD.linkmap):0)+strlen(args[4])];
      sprintf(tmp,"%s%s%s",(PD.linkmap?PD.linkmap:""),(PD.linkmap?" ":""),args[4]);
      delete[] PD.linkmap;
      PD.linkmap = tmp;
    }
    else {
      char *tmp = strdup(PD.linkmap);
      delete[] PD.linkmap;
      PD.linkmap = 0;
      for( char *ptr = strtok(tmp," "); ptr; ptr = strtok(0," ") ) {
        if( strcmp(ptr,args[4]) ) {
          char *ptr2 = new char[2+(PD.linkmap?strlen(PD.linkmap):0)+strlen(ptr)];
          sprintf(ptr2,"%s%s%s",(PD.linkmap?PD.linkmap:""),(PD.linkmap?" ":""),ptr);
          delete[] PD.linkmap;
          PD.linkmap = ptr2;
        }
      }
      free(tmp);
    }
  }
  *terminal << "linkmap: " << PD.linkmap << endl;
  return(1);
}

// load function
static const char* loadPattern  = "load (core|problem)( ([[:print:]]+)){0,1}";
static int   loadSubexprs = 3;
static int   loadFunction( int nargs, char** args )
{
  if( !strcasecmp(args[1],"core") ) {
    // read problem name and core name (if necessary)
    char *coreFilename = 0;
    setCurrentProblem(0);

    // set corefile
    readlineCompletionMode(RLC_FILES);
    char *tmp = readline("Input corefile: ");
    while( tmp ) {
      stripWhite(tmp);
      if( tmp[0] != '\0' ) break;
      free(tmp);
    }
    readlineCompletionMode(RLC_COMMANDS );
    coreFilename = new char[1+strlen(tmp)];
    strcpy(coreFilename,tmp);
    free(tmp);

    // bootstrap problem
    if( !loadedObject && !bootstrap() ) {
      delete[] coreFilename;
      return(1);
    }

    // load core
    readcore(coreFilename);
    delete[] coreFilename;
  }
  else if( !strcasecmp(args[1],"problem") ) {
    setCurrentProblem(0);
    bootstrap();
  }
  return(1);
}

// compile function
#define COMPILE_CMD "make -f Makefile.%s CC=\"%s\" LD=\"%s\" CCFLAGS=\"%s\" LDFLAGS=\"%s\" INCLUDE=\"%s\" LIB=\"%s\" LINKMAP=\"%s\""
static const char* compilePattern  = "compile( ([[:alnum:]_]+)){0,1}";
static int   compileSubexprs = 2;
static int   compileFunction( int nargs, char** args )
{
  setCurrentProblem(args[2]);

  // generate cmdline
  char *include = 0;
  if( sys.include ) {
    char *tmp = strdup(sys.include);
    for( char *ptr = strtok(tmp,":"); ptr; ptr = strtok(0,":") ) {
      char *ptr2 = new char[4+(include?strlen(include):0)+strlen(ptr)];
      sprintf(ptr2,"%s%s-I%s",(include?include:""),(include?" ":""),ptr);
      delete[] include;
      include = ptr2;
    }
    free(tmp);
  }
  else
    include = (char*)"";

  char *cmdline = new char[1+strlen(COMPILE_CMD)+strlen(currentProblem)+strlen(sys.cc)+
		           strlen(sys.ld)+strlen(sys.ccflags)+strlen(sys.ldflags)+ 
		           strlen(include)+strlen(sys.lib)+(PD.linkmap?strlen(PD.linkmap):0)];
  sprintf(cmdline,COMPILE_CMD,currentProblem,sys.cc,sys.ld,sys.ccflags,sys.ldflags,include,sys.lib,(PD.linkmap?PD.linkmap:""));

  // register initial time
  gettimeofday(&startTime,0);

  // compile
  *terminal << "-------" << endl << cmdline << endl;
  int rv = system(cmdline);
  *terminal << "-------" << endl;
  if( !rv )
    *terminal << "Successful compilation." << endl;
  else
    cerr << "Error." << endl;

  // register final time & print elapsed time
  gettimeofday(&endTime,0);
  diffTime(secs,usecs,startTime,endTime);
  *terminal << (float)secs + ((float)usecs) / 1000000.0 << " seconds." << endl;

  // clean & return
  delete[] cmdline;
  if( include[0] != '\0' ) delete[] include;
  return(1);
}

// solve function
static const char* solvePattern  = "solve( ([^[:blank:]]+)){0,1}( ([^[:blank:]]+)){0,1}";
static int   solveSubexprs = 4;
static int   solveFunction( int nargs, char** args )
{
  register bool stdOutput;

  // reset signals and register initial time
  PD.signal = -1;

  // set problem
  if( !loadedObject || (args[2] && strcmp(args[2],currentProblem)) )
    setCurrentProblem(args[2]);

  // open outputFile
  stdOutput = false;
  if( args[4] ) {
    if( !strcasecmp(args[4],"-") )
      stdOutput = true;
    else {
      PD.outputFilename = new char[1+strlen(args[4])];
      strcpy(PD.outputFilename,args[4]);
    }
  }
  else {
    PD.outputFilename = new char[8+strlen(currentProblem)];
    sprintf(PD.outputFilename,"%s.output",currentProblem);
  }
  if( !stdOutput ) {
    PD.outputFile = new ofstream(PD.outputFilename);
    if( !(*PD.outputFile) ) {
      std::cerr << "Error: " << PD.outputFilename << ": " << strerror(errno) << "." << std::endl;
      return(1);
    }
  }
  else {
    PD.outputFile = terminal;
  }
  setOutputFormat(*PD.outputFile);

  // start timer
  gettimeofday(&startTime,0);

  // bootstrap it
  if( !loadedObject && !bootstrap() ) return(1);

  // check new settings
  if( (PD.POMDP != 0) && 
      (PD.POMDP->getActionCache() != 0) && 
      (PD.POMDP->getObservationCache() != 0) && 
      ((PD.cacheSize[0] != PD.POMDP->getActionCache()->getCacheSize()) ||
       (PD.clusterSize[0] != PD.POMDP->getActionCache()->getClusterSize()) ||
       (PD.cacheSize[1] != PD.POMDP->getObservationCache()->getCacheSize()) ||
       (PD.clusterSize[1] != PD.POMDP->getObservationCache()->getClusterSize())) ) {
    PD.POMDP->setCache( PD.cacheSize[0], PD.clusterSize[0], 
                        PD.cacheSize[1], PD.clusterSize[1], 
                        ((beliefClass*)PD.POMDP->getTheInitialBelief()->getData())->getConstructor() );
  }

  // print some data
#if 0
  if( !stdOutput ) printParameters(*PD.outputFile,"%call ");
#endif
  printParameters(*terminal, "%call ");
  *terminal << "Solver output redirected to ";
  if( stdOutput )
    *terminal << "<stdout>." << endl;
  else
    *terminal << "'" << PD.outputFilename << "'." << endl;
  *terminal << "Solving... ";
  terminal->flush();

  // solve problem
  loadedCore = false;
  try {
    solveProblem();
  } 
  catch( exceptionClass& ) {
    if( !stdOutput ) {
      ((ofstream*)PD.outputFile)->close();
      delete PD.outputFile;
      delete[] PD.outputFilename;
      PD.outputFile = 0;
      PD.outputFilename = 0;
    }
    PD.POMDP->cleanHash();
    throw;
  }

  // print the model data
  if( !stdOutput ) PD.model->printData(*PD.outputFile);
  PD.model->printData(*terminal);

  // register final time & print elapsed time
  gettimeofday(&endTime,0);
  diffTime(secs,usecs,startTime,endTime);
  *PD.outputFile << "%time " << (float)secs + ((float)usecs) / 1000000.0 << endl;
  *terminal << "done!." << endl;
  *terminal << (float)secs + ((float)usecs) / 1000000.0 << " seconds." << endl;

#if 0
  // close outputFile
  if( !stdOutput )
    {
      ((ofstream*)PD.outputFile)->close();
      delete PD.outputFile;
      delete[] PD.outputFilename;
      PD.outputFile = 0;
      PD.outputFilename = 0;
    }
#endif

  // check if we have a non-empty core
  if( !PD.POMDP->emptyBeliefHash() ) loadedCore = true;

  // return
  return(1);
}

// set function
static const char* setPattern  = "set( (defaults|problem|version|stoprule|epsilon|cutoff|runs|pims|discount|qmdp-discount|incremental-mode|heuristic-lookahead|discretization-levels|zero-heuristic|random-ties|random-seed|core-level|output-level|verbose-level|precision|action-cache|obs-cache|other-cache|cc|ccflags|ld|ldflags|include-dir|lib-dir|entry-point)( (.+)){0,1}){0,1}";
static int   setSubexprs = 4;
static void  setValue( const char* var, const char* value );
static void  invalidSetValue( const char* var, const char* value );
static void  setPimValue( char* value );
static void  setCacheValue( int cacheId, const char* value );
static int   setFunction( int nargs, char** args )
{
  if( !args[1] ) {
    terminal->precision(20);
    *terminal << "Problem:" << endl;
    *terminal << "  current               = " << (currentProblem?currentProblem:"(null)") << endl;
    *terminal << "  loaded obj            = " << (loadedObject?"true":"false") << endl;
    *terminal << "  loaded core           = " << (loadedCore?"true":"false") << endl;
    *terminal << "  linkmap               = " << (PD.linkmap?PD.linkmap:"(null)") << endl;

    *terminal << "Solve Options:" << endl;
    *terminal << "  version               = ";
    if( PD.version == problemClass::VERSION_GRID )
      *terminal << "grid-points" << endl;
    else if( PD.version == problemClass::VERSION_ORIGINAL )
      *terminal << "original" << endl;

    if( PD.useStopRule )
      *terminal << "  stoprule              = " << PD.SREpsilon << endl;
    else
      *terminal << "  stoprule              = off" << endl;

    *terminal << "  epsilon               = " << PD.epsilon << endl;
    *terminal << "  cutoff                = " << PD.cutoff << endl;
    *terminal << "  runs                  = " << PD.runs << endl;
    *terminal << "  pims                  = ";

    for( int pim = 0; PD.pimSteps[pim] != -1; ++pim ) {
      *terminal << "[" << PD.pimSteps[pim] << "," << PD.learningTrials[pim] << "," << PD.controlTrials[pim] << "] ";
    }

    *terminal << endl;
    *terminal << "  discount              = " << PD.discountFactor << endl;

    if( PD.discretizationLevels != -1 )
      *terminal << "  discretization-levels = " << PD.discretizationLevels << endl;
    else
      *terminal << "  discretization-levels = off" << endl;

    *terminal << "  qmdp-discount         = " << PD.MDPdiscountFactor << endl;
    *terminal << "  incremental-mode      = " << (PD.incrementalMode?"on":"off") << endl;
    *terminal << "  heuristic-lookahead   = " << PD.lookahead << endl;
    *terminal << "  zero-heuristic        = " << (PD.zeroHeuristic?"on":"off") << endl;
    *terminal << "  random-ties           = " << (PD.randomTies?"on":"off") << endl;
    *terminal << "  random-seed           = " << PD.randomSeed << endl;
    *terminal << "  core-level            = " << PD.coreLevel << endl;
    *terminal << "  output-level          = " << PD.outputLevel << endl;
    *terminal << "  verbose-level         = " << PD.verboseLevel << endl;
    *terminal << "  precision             = " << PD.precision << endl;
    *terminal << "  action-cache          = [size=" << PD.cacheSize[0] << ",cluster=" << PD.clusterSize[0] << "]" << endl;
    *terminal << "  obs-cache             = [size=" << PD.cacheSize[1] << ",cluster=" << PD.clusterSize[1] << "]" << endl;
    *terminal << "  other-cache           = [size=" << PD.cacheSize[2] << ",cluster=" << PD.clusterSize[2] << "]" << endl;

    *terminal << "System:" << endl;
    *terminal << "  cc                    = " << sys.cc << endl;
    *terminal << "  ccflags               = " << sys.ccflags << endl;
    *terminal << "  ld                    = " << sys.ld << endl;
    *terminal << "  ldflags               = " << sys.ldflags << endl;
    *terminal << "  include-dir           = " << sys.include << endl;
    *terminal << "  lib-dir               = " << sys.lib << endl;
    *terminal << "  entry-point           = " << sys.entry << endl;
    terminal->precision(6);
  }
  else if( !args[3] ) {
    if( !strcasecmp(args[2],"defaults") )
      setDefaultValue(0,false);
    else
      setDefaultValue(args[2],false);
  }
  else {
    setValue(args[2],args[4]);
  }
  return(1);
}

static void
setValue( const char* var, const char* value )
{
  int arg;
  float farg;

  if( !strcasecmp(var,"problem") ) {
    setCurrentProblem( value );
  }
  else if( !strcasecmp(var,"version") ) {
    if( !strcasecmp(value,"grid-points") )
      PD.version = problemClass::VERSION_GRID;
    else if( !strcasecmp(value,"original") )
      PD.version = problemClass::VERSION_ORIGINAL;
    else
      invalidSetValue(var,value);
  }
  else if( !strcasecmp(var,"stoprule") ) {
    if( !strcasecmp(value,"off") )
      PD.useStopRule = false;
    else {
      farg = atof(value);
      if( farg < 0.0 )
        invalidSetValue(var,value);
      else {
        PD.SREpsilon = farg;
        PD.useStopRule = true;
      }
    }
  }
  else if( !strcasecmp(var,"epsilon") ) {
    farg = atof(value);
    if( farg < 0.0 )
      invalidSetValue(var,value);
    else
      PD.epsilon = farg;
  }
  else if( !strcasecmp(var,"cutoff") ) {
    if( !strcasecmp(value,"off") )
      PD.cutoff = (int)(((unsigned)-1)>>1);
    else {
        arg = atoi(value);
        if( arg < 0 )
          invalidSetValue(var,value);
        else
          PD.cutoff = arg;
    }
    if( PD.POMDP ) PD.POMDP->setMaxMoves(PD.cutoff);
  }
  else if( !strcasecmp(var,"runs") ) {
    arg = atoi(value);
    if( arg < 0 )
      invalidSetValue(var,value);
    else
      PD.runs = arg;
  }
  else if( !strcasecmp(var,"pims") ) {
    PD.useStopRule = false;
    char *s = strdup(value);
    setPimValue(s);
    free(s);
  }
  else if( !strcasecmp(var,"discount") ) {
    farg = atof(value);
    if( (farg < 0.0) || (farg > 1.0) )
      invalidSetValue(var,value);
    else
      PD.discountFactor = farg;
    if( PD.POMDP ) PD.POMDP->setDiscountFactor(PD.discountFactor);
  }
  else if( !strcasecmp(var,"qmdp-discount") ) {
    farg = atof(value);
    if( (farg < 0.0) || (farg > 1.0) )
      invalidSetValue(var,value);
    else
      PD.MDPdiscountFactor = farg;
    }
  else if( !strcasecmp(var,"incremental-mode") ) {
    if( !strcasecmp(value,"on") || !strcasecmp(value,"1") )
      PD.incrementalMode = true;
    else if( !strcasecmp(value,"off") || !strcasecmp(value,"0") )
      PD.incrementalMode = false;
    else
      invalidSetValue(var,value);
  }
  else if( !strcasecmp(var,"heuristic-lookahead") ) {
    arg = atoi( value );
    if( arg < 0 )
      invalidSetValue(var,value);
    else
      PD.lookahead = arg;
  }
  else if( !strcasecmp(var,"discretization-levels") ) {
    if( !strcasecmp(value,"off") ) {
      if( (PD.discretizationLevels != -1) && loadedCore ) {
        loadedCore = false;
        PD.clean(problemClass::CLEAN_HASH);
        *terminal << "Hash cleared." << endl;
      }
      PD.discretizationLevels = -1;
    }
    else {
      if( (arg = atoi(value)) <= 0 )
        invalidSetValue(var,value);
      else {
        if( (arg != PD.discretizationLevels) && loadedCore ) {
          loadedCore = false;
          PD.clean(problemClass::CLEAN_HASH);
          *terminal << "Hash cleared." << endl;
        }
        PD.discretizationLevels = arg;
      }
    }
  }
  else if( !strcasecmp(var,"zero-heuristic") ) {
    if( !strcasecmp(value,"on") || !strcasecmp(value,"1") )
      PD.zeroHeuristic = true;
    else if( !strcasecmp(value,"off") || !strcasecmp(value,"0") )
      PD.zeroHeuristic = false;
    else
      invalidSetValue(var,value);
  }
  else if( !strcasecmp(var,"random-seed") ) {
    arg = atoi(value);
    if( arg < 0 )
      invalidSetValue(var,value);
    else
      PD.randomSeed = arg;
  }
  else if( !strcasecmp( var, "random-ties" ) )
    {
      if( !strcasecmp( value, "on" ) || !strcasecmp( value, "1" ) )
	PD.randomTies = true;
      else if( !strcasecmp( value, "off" ) || !strcasecmp( value, "0" ) )
	PD.randomTies = false;
      else
	invalidSetValue( var, value );

      if( PD.POMDP )
	PD.POMDP->setRandomTies( PD.randomTies );
    }
  else if( !strcasecmp(var,"core-level") ) {
    arg = atoi(value);
    if( arg < 0 )
      invalidSetValue(var,value);
    else
      PD.coreLevel = arg;
  }
  else if( !strcasecmp(var,"output-level") ) {
    arg = atoi(value);
    if( arg < 0 )
      invalidSetValue(var,value);
    else
      PD.outputLevel = arg;
  }
  else if( !strcasecmp(var,"verbose-level") ) {
    arg = atoi( value );
    if( arg < 0 )
      invalidSetValue(var,value);
    else
      PD.verboseLevel = arg;
  }
  else if( !strcasecmp(var,"precision") ) {
    arg = atoi(value);
    if( arg < 0 )
      invalidSetValue(var,value);
    else
      PD.precision = arg;
  }
  else if( !strcasecmp(var,"action-cache") ) {
    setCacheValue(0,value);
  }
  else if( !strcasecmp(var,"obs-cache") ) {
    setCacheValue(1,value);
  }
  else if( !strcasecmp(var,"other-cache") ) {
    setCacheValue(2,value);
  }
  else if( !strcasecmp(var,"cc") ) {
    delete[] sys.cc;
    sys.cc = new char[1+strlen(value)];
    strcpy(sys.cc,value);
  }
  else if( !strcasecmp(var,"ccflags") ) {
    delete[] sys.ccflags;
    sys.ccflags = new char[1+strlen(value)];
    strcpy(sys.ccflags,value);
  }
  else if( !strcasecmp(var,"ld") ) {
    delete[] sys.ld;
    sys.ld = new char[1+strlen(value)];
    strcpy(sys.ld,value);
  }
  else if( !strcasecmp(var,"ldflags") ) {
    delete[] sys.ldflags;
    sys.ldflags = new char[1+strlen(value)];
    strcpy(sys.ldflags,value);
  }
  else if( !strcasecmp(var,"include-dir") ) {
    delete[] sys.include;
    sys.include = new char[1+strlen(value)];
    strcpy(sys.include,value);
  }
  else if( !strcasecmp(var,"lib-dir") ) {
    delete[] sys.lib;
    sys.lib = new char[1+strlen(value)];
    strcpy(sys.lib,value);
  }
  else if( !strcasecmp(var,"entry-point") ) {
    delete[] sys.entry;
    sys.entry = new char[1+strlen(value)];
    strcpy(sys.entry,value);
  }
  else {
    *terminal << "Unrecognized variable '" << var << "'." << endl;
  }
}

static void
setDefaultValue( const char* var, bool silent )
{
  map<const char*,const char*,stringCaseCompareFunction>::const_iterator it;
  if( var != 0 ) {
    it = defaultValues.find(var);
    if( it == defaultValues.end() )
      *terminal << "No default value for " << var << "." << endl;
    else {
      setValue( var, (*it).second );
      if( !silent ) *terminal << var << " is now " << (*it).second << "." << endl;
    }
  }
  else {
    for( it = defaultValues.begin(); it != defaultValues.end(); ++it )
      setValue((*it).first,(*it).second);
    if( !silent ) *terminal << "default values restored for all variables." << endl;
  }
}

static void
invalidSetValue( const char* var, const char* value )
{
  *terminal << "Invalid value '" << value << "' for: " << var << "." << endl;
}

static void
setPimValue( char* value )
{
  int pim = 0;
  char *token = strtok(value," ");
  while( token ) {
    int s = -1, l = -1, c = -1;
    sscanf(token,"[%d,%d,%d]",&s,&l,&c);
    if( (s < 0) || (l < 0) || (c < 0) )
      *terminal << "Invalid pim value: " << token << "." << endl;
    else {
      PD.pimSteps[pim] = s;
      PD.learningTrials[pim] = l;
      PD.controlTrials[pim] = c;
      ++pim;
    }
    token = strtok(0," ");
  }
  PD.pimSteps[pim] = -1;
}

static void
setCacheValue( int cacheId, const char* value )
{
  int s = -1, c = -1;
  sscanf(value,"[%d,%d]",&s,&c);
  if( (s < 0) || (c < 0) || ((s > 0) && (c == 0)) || ((c > 0) && (s % c != 0)) )
    *terminal << "Invalid cache values: " << value << "." << endl;
  else {
    PD.cacheSize[cacheId] = s;
    PD.clusterSize[cacheId] = c;
  }
}

// clean function
static const char* cleanPattern  = "clean( (problem|object|hash|linkmap)){0,1}";
static int   cleanSubexprs = 2;
static int   cleanFunction( int nargs, char** args )
{
  if( args[1] )
    {
      if( !strcasecmp( args[2], "problem" ) )
	{
	  delete[] currentProblem;
	  currentProblem = 0;
	  delete[] PD.linkmap;
	  PD.linkmap = 0;

	  loadedObject = false;
	  loadedCore = false;
	  PD.clean( problemClass::CLEAN_OBJECT );
	}
      else if( !strcasecmp( args[2], "object" ) )
	{
	  loadedObject = false;
	  loadedCore = false;
	  PD.clean( problemClass::CLEAN_OBJECT );
	}
      else if( !strcasecmp( args[2], "hash" ) )
	{
	  loadedCore = false;
	  PD.clean( problemClass::CLEAN_HASH );
	}
      else if( !strcasecmp( args[2], "linkmap" ) )
	{
	  delete[] PD.linkmap;
	  PD.linkmap = 0;
	}
    }
  else
    {
      // default is 'clean hash'
      loadedCore = false;
      PD.clean( problemClass::CLEAN_HASH );
    }
  return( 1 );
}

// generate function
static const char* generatePattern  = "generate( (core|graph|hash|table)){0,1}( ([[:print:]]+)){0,1}";
static int   generateSubexprs = 4;
static int   generateFunction( int nargs, char** args )
{
  policyGraphClass graph;
  policyTableClass table;
  char *coreFilename, *tmp;
  ofstream *outputFile = 0;

  if( !args[1] )
    {
      *terminal << "Unrecognized command. Use 'generate (core|graph|table) [<file>]'." << endl;
      return( 1 );
    }

  if( !strcasecmp( args[2], "core" ) )
    {
      if( !loadedObject || !loadedCore )
	cerr << "Error: no current or empty core." << endl;
      else
	{
	  if( args[4] != 0 )
	    {
	      coreFilename = new char[1 + strlen( args[4] )];
	      strcpy( coreFilename, args[4] );
	    }
	  else
	    {
	      coreFilename = new char[6 + strlen( currentProblem )];
	      sprintf( coreFilename, "%s.core", currentProblem );
	    }
	  outputFile = new ofstream( coreFilename, ios::out /*| ios::noreplace*/ );
	  delete[] coreFilename;
	  if( !outputFile )
	    {
	      cerr << "Error: " << coreFilename << ": " << strerror( errno ) << "." << endl;
	      return( 1 );
	    }
	  writeCore( *outputFile );
	  outputFile->close();
	  delete outputFile;
	}
    }
  else if( !strcasecmp( args[2], "graph" ) ||
	   !strcasecmp( args[2], "hash" ) ||
	   !strcasecmp( args[2], "table" ) )
    {
      // read problem name and core name (if necessary)
      coreFilename = 0;
      setCurrentProblem( 0 );
      if( !loadedCore )
	{
	  readlineCompletionMode( RLC_FILES );
	  while( (tmp = readline( "Input corefile: " )) )
	    {
	      stripWhite( tmp );
	      if( tmp[0] != '\0' )
		break;
	      free( tmp );
	    }
	  readlineCompletionMode( RLC_COMMANDS );
	  coreFilename = new char[1 + strlen( tmp )];
	  strcpy( coreFilename, tmp );
	  free( tmp );
	}

      // check that the model support generation
      if( !strcasecmp( args[2], "graph" ) || !strcasecmp( args[2], "table" ) )
	{
	  if( PD.pddlProblemType && !ISMDP(PD.handle->problemType) && 
	      !ISNDMDP(PD.handle->problemType) && !ISPOMDP(PD.handle->problemType) &&
	      !ISNDPOMDP(PD.handle->problemType) )
	    {
	      cerr << "Error: this function is only available for MDP/POMDP models." << endl;
	      return( 1 );
	    }
	}

      // open output file
      if( args[3] != 0 )
	{
	  outputFile = new ofstream( args[4], ios::out /*| ios::noreplace*/ );
	  if( !outputFile )
	    {
	      cerr << "Error: " << args[4] << ": " << strerror( errno ) << "." << endl;
	      delete[] coreFilename;
	      delete outputFile;
	      return( 1 );
	    }
	}

      // bootstrap problem
      if( !loadedObject && !bootstrap() )
	{
	  delete[] coreFilename;
	  delete outputFile;
	  return( 1 );
	}

      // load core
      if( !loadedCore )
	{
	  if( readcore( coreFilename ) )
	    {
	      delete[] coreFilename;
	      delete outputFile;
	      return( 1 );
	    }
	  delete[] coreFilename;
	}

      // generate output
      if( !strcasecmp( args[2], "graph" ) )
	{
	  graph.generatePolicyGraph();
	  if( outputFile != 0 )
	    graph.outputPolicyGraph( *outputFile );
	  else
	    graph.outputPolicyGraph( cout );
	  *terminal << "Graph generated." << endl;
	}
      else if( !strcasecmp( args[2], "table" ) )
	{
	  table.generatePolicyTable();
	  if( outputFile != 0 )
	    table.outputPolicyTable( *outputFile );
	  else
	    table.outputPolicyTable( cout );
	  *terminal << "Policy generated." << endl;
	}
      else
	{
	  if( outputFile != 0 )
	    PD.POMDP->printHash( *outputFile );
	  else
	    PD.POMDP->printHash( cout );
	  *terminal << "Hash generated." << endl;
	}

      // close output file
      if( outputFile != 0 )
	{
	  outputFile->close();
	  delete outputFile;
	}
    }
  return( 1 );
}

// shell function
#define SHELL "ksh"
static const char* shellPattern  = "(shell |!)([[:print:]]+){0,1}";
static int   shellSubexprs = 2;
static int   shellFunction( int nargs, char** args )
{
  if( args[2] ) {
    char *cmdline = new char[1+strlen(args[2])];
    sprintf(cmdline,"%s",args[2]);
    system(cmdline);
    delete[] cmdline;
  }
  else {
    system(SHELL);
  }
  return(1);
}

// viewer function
static const char* viewerPattern  = "viewer( ([[:print:]]+)){0,1}";
static int   viewerSubexprs = 2;
static int   viewerFunction( int nargs, char** args )
{
  char *cmdline;

  cmdline = new char[14+strlen(sys.gpthome)+(args[2]?1+strlen(args[2]):0)];
  sprintf(cmdline,"%s/bin/viewer%s%s &",sys.gpthome,(args[2]?" ":""),(args[2]?args[2]:""));
  system(cmdline);
  delete[] cmdline;
  return(1);
}


///////////////////////////////////////////////////////////////////////////////
//
// Commands Definitions
//

static int registeredCommandsSize = 0;
static int registeredCommandsEntries = 0;
static commandClass** registeredCommands = 0;

static void
commandRegister( commandClass* command )
{
  if( registeredCommandsEntries == registeredCommandsSize ) {
    registeredCommandsSize = (!registeredCommandsSize ? 2 : 2*registeredCommandsSize);
    registeredCommands = (commandClass**)realloc(registeredCommands,registeredCommandsSize*sizeof(commandClass*));
  }
  registeredCommands[registeredCommandsEntries++] = command;
}

static void
commandRegistration()
{
  commandRegister( new commandClass( "about", aboutPattern, aboutSubexprs, &aboutFunction ) );
  commandRegister( new commandClass( "help", helpPattern, helpSubexprs, &helpFunction ) );
  commandRegister( new commandClass( "cd", cdPattern, cdSubexprs, &cdFunction ) );
  commandRegister( new commandClass( "clear", clearPattern, clearSubexprs, &clearFunction ) );
  commandRegister( new commandClass( "quit", quitPattern, quitSubexprs, &quitFunction ) );
  commandRegister( new commandClass( "exit", quitPattern, quitSubexprs, &quitFunction ) );
  commandRegister( new commandClass( "parse", parsePattern, parseSubexprs, &parseFunction ) );
  commandRegister( new commandClass( "print", printPattern, printSubexprs, &printFunction ) );
  commandRegister( new commandClass( "linkmap", linkmapPattern, linkmapSubexprs, &linkmapFunction ) );
  commandRegister( new commandClass( "load", loadPattern, loadSubexprs, &loadFunction ) );
  commandRegister( new commandClass( "compile", compilePattern, compileSubexprs, &compileFunction ) );
  commandRegister( new commandClass( "solve", solvePattern, solveSubexprs, &solveFunction ) );
  commandRegister( new commandClass( "set", setPattern, setSubexprs, &setFunction ) );
  commandRegister( new commandClass( "clean", cleanPattern, cleanSubexprs, &cleanFunction ) );
  commandRegister( new commandClass( "generate", generatePattern, generateSubexprs, &generateFunction ) );
  commandRegister( new commandClass( "shell", shellPattern, shellSubexprs, &shellFunction ) );
  commandRegister( new commandClass( "viewer", viewerPattern, viewerSubexprs, &viewerFunction ) );
}

static void
commandCleanup( void )
{
  for( int i = 0; i < registeredCommandsEntries; ++i )
    delete registeredCommands[i];
  free(registeredCommands);
}

static commandClass*
commandParse( char* input, int& nargs, char**& args )
{
  regmatch_t pmatch[32];
  nargs = 0;
  args = 0;
  for( commandClass **ptr = registeredCommands; ptr - registeredCommands < registeredCommandsEntries; ++ptr ) {
    if( !regexec(&(*ptr)->regex,input,1+(*ptr)->subexprs,pmatch,0) && !pmatch[0].rm_so && !input[pmatch[0].rm_eo] ) {
      // fill-in command arguments
      if( (nargs = (*ptr)->subexprs) ) {
        args = new char*[nargs+1];
        args[0] = 0;
        for( int arg = 1; arg <= nargs; ++arg )
          if( (pmatch[arg].rm_so != -1) && (pmatch[arg].rm_eo != -1) ) {
            args[arg] = new char[pmatch[arg].rm_eo-pmatch[arg].rm_so+1];
            strncpy(args[arg],&input[pmatch[arg].rm_so],pmatch[arg].rm_eo-pmatch[arg].rm_so);
            args[arg][pmatch[arg].rm_eo-pmatch[arg].rm_so] = '\0';
          }
          else
            args[arg] = 0;
      }
      return(*ptr);
    }
  }
  return(0);
}

commandClass::commandClass( const char* _name, const char* _pattern, 
			    int _subexprs, int (*_commandFunction)( int, char** ) )
{
  name = _name;
  pattern = _pattern;
  subexprs = _subexprs;
  commandFunction = _commandFunction;

  // compile the regular expression for command
  if( regcomp(&regex,pattern,REG_EXTENDED|REG_ICASE) )
    cerr << "Error: compiling pattern '" << pattern << "' for command '" << name << "'." << endl;
}

commandClass::~commandClass()
{
  regfree(&regex);
}


///////////////////////////////////////////////////////////////////////////////
//
// readline completion functions (taken from readline distribution examples)
//

static char* command_generator( const char*, int );
static char** fileman_completion( const char*, int, int );
static int  rlcMode;

typedef struct { const char* prefix; int pfxsz; const char** comp; } completionInfo_t;

static const char* fullList[] = { "about", "cd", "clean", "clear", "compile", "exit", "generate", 
				  "help", "linkmap", "load", "parse", "print", "quit", "set",
				  "shell", "solve", "viewer", 0 };
static const char* cleanList[] = { "problem", "object", "hash", "linkmap", 0 };
static const char* generateList[] = { "core", "graph", "hash", "table", 0 };
static const char* loadList[] = { "core", "problem", 0 };
static const char* linkmapList[] = { "+", "-", 0 };
static const char* setList[] = { "defaults", "problem", "stoprule", "epsilon", "cutoff",
				 "runs", "pims", "discount", "qmdp-discount", "incremental-mode",
				 "heuristic-lookahead", "discretization-levels", "zero-heuristic",
				 "random-ties", "random-seed", "core-level", "output-level",
				 "verbose-level", "precision", "action-cache", "obs-cache",
				 "other-cache", "cc", "ccflags", "ld", "ldflags", "include-dir",
				 "lib-dir", "entry-point", 0 };
#if 0
static const char* setVersionList[] = { "grid-points", "original", 0 };
#endif
static const char* printList[] = { "action", "action@", "belief", "observation", "state", 0 };
static const char* beliefList[] = { "actions","afteraction", "initial", 0 };
static const char* stateList[] = { "actions", "afteraction", 0 };
static const char* onOffList[] = { "on", "off", 0 };
static const char* emptyList[] = { 0 };

static const char** possibleCtxCompletions = 0;
static completionInfo_t completionInfo[] = { 
  { "about", 0, 0 },
  { "cd", 0, 0 },
  { "clean compile", 0, emptyList },
  { "clean object", 0, emptyList },
  { "clean hash", 0, emptyList },
  { "clean linkmap", 0, emptyList },
  { "clean", 0, cleanList },
  { "clear", 0, emptyList },
  { "compile", 0, 0 },
  { "exit", 0, emptyList },
  { "generate core", 0, 0 },
  { "generate graph", 0, 0 },
  { "generate hash", 0, 0 },
  { "generate table", 0, 0 },
  { "generate", 0, generateList },
  { "help clean", 0, emptyList },
  { "help clear", 0, emptyList },
  { "help compile", 0, emptyList },
  { "help exit", 0, emptyList },
  { "help generate", 0, emptyList },
  { "help help", 0, emptyList },
  { "help linkmap", 0, emptyList },
  { "help parse", 0, emptyList },
  { "help quit", 0, emptyList },
  { "help set", 0, emptyList },
  { "help shell", 0, emptyList },
  { "help solve", 0, emptyList },
  { "help", 0, fullList },
  { "linkmap +", 0, 0 },
  { "linkmap -", 0, 0 },
  { "linkmap", 0, linkmapList },
  { "load core", 0, 0 },
  { "load problem", 0, 0 },
  { "load", 0, loadList },
  { "parse", 0, 0 },
  { "print action", 0, emptyList },
  { "print action@", 0, emptyList },
  { "print belief initial", 0, emptyList },
  { "print belief", 0, beliefList },
  { "print observation", 0, emptyList },
  { "print state", 0, stateList },
  { "print", 0, printList },
  { "quit", 0, emptyList },
  { "set defaults", 0, emptyList },
  { "set problem", 0, 0 },
  { "set stoprule", 0, emptyList },
  { "set epsilon", 0, emptyList },
  { "set cutoff", 0, emptyList },
  { "set runs", 0, emptyList },
  { "set pims", 0, emptyList },
  { "set discount", 0, emptyList },
  { "set qmdp-discount", 0, emptyList },
  { "set incremental-mode", 0, emptyList },
  { "set heuristic-lookahead", 0, emptyList },
  { "set discretization-levels", 0, emptyList },
  { "set zero-heuristic", 0, emptyList },
  { "set random-ties", 0, onOffList },
  { "set random-seed", 0, emptyList },
  { "set core-level", 0, emptyList },
  { "set output-level", 0, emptyList },
  { "set verbose-level", 0, emptyList },
  { "set precision", 0, emptyList },
  { "set action-cache", 0, emptyList },
  { "set obs-cache", 0, emptyList },
  { "set other-cache", 0, emptyList },
  { "set cc", 0, emptyList },
  { "set ccflags", 0, emptyList },
  { "set ld", 0, emptyList },
  { "set ldflags", 0, emptyList },
  { "set include-dir", 0, 0 },
  { "set lib-dir", 0, 0 },
  { "set entry-point", 0, 0 },
  { "set", 0, setList },
  { "shell", 0, 0 },
  { "solve", 0, 0 },
  { "viewer", 0, 0 },
  { "set version", 0, emptyList },
  { "", 0, fullList }, 
  { 0, 0, 0 }
};

static void
readlineCompletionMode( int mode )
{
  rlcMode = mode;
}

static void
clearLineBuffer()
{
  rl_point = 0;
  rl_end = 0;
  rl_line_buffer[0] = '\0';
}

// Tell the GNU Readline library how to complete.  We want to try to complete
// on command names if this is the first word in the line, or on filenames
// if not.
static void
initializeReadline()
{
  completionInfo_t *info;
  
  // Allow conditional parsing of the ~/.inputrc file
  rl_readline_name = "GPT";

  // Tell the completer that we want a crack first
  rl_attempted_completion_function = fileman_completion;

  // fill completion info
  for( info = completionInfo; info->prefix; ++info )
    info->pfxsz = strlen(info->prefix);
}

// Attempt to complete on the contents of TEXT.  START and END bound the
// region of rl_line_buffer that contains the word to complete.  TEXT is
// the word to complete.  We can use the entire contents of rl_line_buffer
// in case we want to do some simple parsing.  Return the array of matches,
// or NULL if there aren't any.
static char** 
fileman_completion( const char* text, int start, int end )
{
  completionInfo_t *info;

  // Use appropriate mode.
  switch( rlcMode ) {
    case RLC_COMMANDS:
      // find context; if valid make completion on it, else go for a filename
      for( info = completionInfo; info->prefix != 0; ++info )
	if( !strncasecmp(info->prefix,rl_line_buffer,info->pfxsz) )
	  break;
      if( info->comp == 0 )
	return(0);
      else
	possibleCtxCompletions = info->comp;
      return( rl_completion_matches(text,command_generator) );
    case RLC_FILES:
      return(0);
  }
  return(0);
}

// Generator function for command completion.  STATE lets us know whether
// to start from scratch; without any state (i.e. STATE == 0), then we
// start at the top of the list.
static char*
command_generator( const char* text, int state )
{
  static const char **ptr;
  static int len;

  // If this is a new word to complete, initialize now.  This includes
  // saving the length of TEXT for efficiency, and initializing the index
  // variable to 0.
  if( !state ) {
    len = strlen(text);
    ptr = possibleCtxCompletions;
  }

  // look in command list.
  // it is NECESSARY to allocate memory here since readline will
  // attempt to free it.
  while( *ptr ) {
    if( !strncasecmp(*ptr++,text,len) )
      return( strdup(*(ptr-1)) );
  }
  return(0);
}
