//  Theseus
//  theTopLevel.cc -- Top Level Routines
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#include <errno.h>

#include <iostream>
#include <fstream>

using namespace std;

#include <theseus/theTopLevel.h>
#include <theseus/theRtStandard.h>
#include <theseus/theRtRegister.h>
#include <theseus/theStandardPOMDP.h>
#include <theseus/theMDPPOMDP.h>
#include <theseus/thePlanningPOMDP.h>
#include <theseus/theSearchPOMDP.h>
#include <theseus/theStandardBelief.h>
#include <theseus/theSimpleBelief.h>
#include <theseus/thePlanningBelief.h>
#include <theseus/theSetBelief.h>
#include <theseus/theStandardModel.h>
#include <theseus/thePlanningModel.h>
#include <theseus/theNonDetModel.h>
#include <theseus/theQmdpHeuristic.h>
#include <theseus/theDynHeuristic.h>
#include <theseus/theLookAheadHeuristic.h>
#include <theseus/theQuantization.h>
#include <theseus/theException.h>
#include <theseus/theUtils.h>

#define  MAX_PIMS          128

// software revision/version (global data)
problemClass          PD;

void
problemClass::clean( int level )
{
  switch( level ) {
    case CLEAN_OBJECT:
      delete heuristic;
      delete belief;
      delete model;
      if( POMDP ) delete POMDP->removeHash();
      delete POMDP;
      quantizationClass::finalize();
      heuristic = NULL;
      belief = NULL;
      model = NULL;
      POMDP = NULL;
      break;
    case CLEAN_CORE:
      if( POMDP ) delete POMDP->removeHash();
      delete POMDP;
      POMDP = NULL;
      break;
    case CLEAN_HASH:
      if( POMDP ) POMDP->cleanHash();
      break;
  }
}

static char *
readArgument( char *str, char *ctx )
{
  char *result;
  if( str == NULL ) {
    std::cerr << "Fatal Error: argument expected after \"" << ctx << "\"" << std::endl;
    exit(-1);
  }
  else {
    result = str;
  }
  return(result);
}

void
parseArguments( int argc, char **argv, void (*helpFunction)() )
{
  int pimSet, k;
  short unsigned seedArray[3];
  struct timeval tp;
  char *ctx;

  // program name
  PD.programName = argv[0];

  // set default values
  PD.softwareRevision = REVISION;
  PD.version = problemClass::VERSION_GRID;
  PD.useStopRule = false;
  PD.SREpsilon = 0.0;
  PD.epsilon = 0.000001;
  PD.signal = -1;

  pimSet = -1;
  PD.runs = 1;
  PD.pimSteps[0] = 1;
  PD.pimSteps[1] = -1;
  memset(PD.learningTrials,0,MAX_PIMS*sizeof(int));
  memset(PD.controlTrials,0,MAX_PIMS*sizeof(int));

  PD.cutoff = (int)(((unsigned)-1)>>1);
  PD.discretizationLevels = -1;
  PD.discountFactor = 1.0;
  PD.MDPdiscountFactor = 1.0;

  PD.incrementalMode = false;
  PD.zeroHeuristic = false;
  PD.randomSeed = -1;
  PD.randomTies = true;
  PD.pddlProblemType = true;
  PD.coreLevel = 0;
  PD.loadType = 0;

  PD.outputLevel = 0;
  PD.outputFile = &std::cout;
  PD.outputPrefix = NULL;
  PD.coreFilename = NULL;

  // caches default sizes
  PD.cacheSize[0] = 16;
  PD.clusterSize[0] = 1;
  PD.cacheSize[1] = 16;
  PD.clusterSize[1] = 1;
  PD.cacheSize[2] = 16;
  PD.clusterSize[2] = 1;

  // parse options
  ++argv;
  while( (argc > 1) && (**argv == '-') ) {
    ctx = *argv;
    switch( (*argv)[1] ) {
      // trial control
      case 'c':
        --argc;
        if( pimSet < 0 ) {
          std::cerr << "Fatal Error: first specify a pim set with -p" << std::endl;
          exit(-1);
        }
        PD.controlTrials[pimSet] = atoi(readArgument(*++argv,ctx));
	std::cout << "hola" << endl;
        break;
      case 'l':
        --argc;
        if( pimSet < 0 ) {
          std::cerr << "Fatal Error: first specify a pim set with -p" << std::endl;
          exit(-1);
        }
        PD.learningTrials[pimSet] = atoi(readArgument(*++argv,ctx));
        break;
      case 'p':
        --argc;
        if( ++pimSet == 128 ) {
          std::cerr << "Fatal Error: max pim sets reached" << std::endl;
          exit(-1);
        }
        PD.pimSteps[pimSet] = atoi(readArgument(*++argv,ctx));
        PD.pimSteps[pimSet + 1] = -1;
        break;
      case 'r':
        --argc;
        PD.runs = atoi(readArgument(*++argv,ctx));
        break;

      // general parameters
      case 'C':
        --argc;
        PD.cutoff = atoi(readArgument(*++argv,ctx));
        break;
      case 'D':
        --argc;
        PD.discretizationLevels = atoi(readArgument(*++argv,ctx));
        break;
      case 'g':
        --argc;
        PD.MDPdiscountFactor = (float) atof(readArgument(*++argv,ctx));
        break;
      case 'G':
        --argc;
        PD.discountFactor = (float) atof(readArgument(*++argv,ctx));
        break;
      case 'S':
        --argc;
        PD.randomSeed = atoi(readArgument(*++argv,ctx));
        break;
      case 'O':
        --argc;
        PD.outputLevel = atoi(readArgument(*++argv,ctx));
        break;
      case 'P':
        --argc;
        PD.outputPrefix = readArgument(*++argv,ctx);
        break;
      
      // features
      case 'B':
        PD.randomTies = false;
        break;
      case 'I':
        PD.incrementalMode = true;
        PD.zeroHeuristic = true;
        break;
      case 'Z':
        PD.zeroHeuristic = true;
        break;
      case 'T':
        PD.pddlProblemType = false;
        break;

      // caching
      case 'a':
        if( (((*argv)[2] - '1') >= 0) && (((*argv)[2] - '1') <= 2) ) {
          argc -= 2;
          k = (*argv)[2] - '1';
          PD.cacheSize[k] = atoi(readArgument(*++argv,ctx));
          PD.clusterSize[k] = atoi(readArgument(*++argv,ctx));
          if( (PD.cacheSize[k] < 0) || (PD.clusterSize[k] < 1) || (PD.cacheSize[k] % PD.clusterSize[k] != 0) ) {
            std::cerr << "Fatal Error: bad cache parameters: " << PD.cacheSize[k];
            std::cerr << " " << PD.clusterSize[k] << std::endl;
            exit(-1);
          }
        }
        else {
          std::cerr << "Fatal Error: bad cache option." << std::endl;
          exit(-1);
        }
        break;

      // core images
      case 'u':
        if( (*argv)[2] == 'c' ) {
          --argc;
          PD.loadType = 1;
          PD.coreFilename = readArgument(*++argv,ctx);
        }
        else if( (*argv)[2] == 'l' ) {
          std::cerr << "Fatal Error: unsupported feature." << std::endl;
          exit(-1);
        }
        else {
          std::cerr << "Fatal Error: bad use core option." << std::endl;
          exit(-1);
        }
        break;
      case 'w':
        if( (*argv)[2] == 'c' ) {
          --argc;
          PD.coreLevel = atoi(readArgument(*++argv,ctx));
        }
        else if( (*argv)[2] == 'l' ) {
          std::cerr << "Fatal Error: unsupported feature." << std::endl;
          exit(-1);
        }
        else {
          std::cerr << "Fatal Error: bad write core option." << std::endl;
          exit(-1);
        }
        break;

      // version
      case 'v':
        --argc;
        PD.version = atoi(readArgument(*++argv,ctx));
        if( (PD.version != problemClass::VERSION_GRID) && (PD.version != problemClass::VERSION_ORIGINAL) ) {
          std::cerr << "Fatal Error: bad version option." << std::endl;
          exit(-1);
        }
        break;

      // help, undefined 
      case 'h':
        (*helpFunction)();
        exit(0);
        break;
      default:
        std::cerr << "usage: " << PD.programName << " <options>* <obj-file> <problem>" << std::endl;
        exit(-1);
        break;
    }
    --argc;
    ++argv;
  }

  // if more arguments, error
  if( argc > 2 ) {
    std::cerr << "usage: " << PD.programName << " <options>* [<obj-file>]" << std::endl;
    exit(-1);
  }

  // What remains in argv is the name of a problem file
  if( *argv ) {
    std::cout << argc << " " << *argv << std::endl;
    PD.problemFile = new char[1+strlen(*argv)];
    strcpy(PD.problemFile,*argv);
  }
  else {
    PD.problemFile = NULL;
  }

  // set random seed
  if( PD.randomSeed == -1 ) {
    // generate a random seed using gettimeofday
    gettimeofday(&tp,NULL);
    PD.randomSeed = (int) tp.tv_usec;
  }

  seedArray[0] = (short unsigned) PD.randomSeed;
  seedArray[1] = (short unsigned) PD.randomSeed;
  seedArray[2] = (short unsigned) PD.randomSeed;
  srand48((long)PD.randomSeed);
  seed48(seedArray);
}

static void
classRegistration()
{
  // registration of all serializable classes

  // for POMDP
  standardPOMDPClass::checkIn();
  standardBeliefHashClass::checkIn();
  standardBeliefClass::checkIn();
  standardModelClass::checkIn();
  QMDPHeuristicClass::checkIn();
  dynHeuristicClass::checkIn();
  lookAheadHeuristicClass::checkIn();
  hookedBeliefHashClass::checkIn();

  // for MDP
  MDPPOMDPClass::checkIn();
  simpleBeliefClass::checkIn();
  simpleBeliefHashClass::checkIn();

  // for PLANNING
  planningPOMDPClass::checkIn();
  planningBeliefHashClass::checkIn();
  planningBeliefClass::checkIn();
  planningModelClass::checkIn();

  // for CONFORMANT (search)
  searchPOMDPClass::checkIn();
  setBeliefHashClass::checkIn();
  setBeliefClass::checkIn();
  nonDetModelClass::checkIn();
}

static void
classCleanup()
{
  // registration of all serializable classes
  POMDPClass::cleanup();
  beliefClass::cleanup();
  modelClass::cleanup();
  heuristicClass::cleanup();
  hashClass::cleanup();
}

void
internalInitialization()
{
  // register all known classes
  classRegistration();

  // initialization
  //xxxx  memoryMgmt = new memoryMgmtClass;
  setOutputFormat(*PD.outputFile);
}

void
internalFinalization()
{
  classCleanup();
  //xxxx  delete memoryMgmt;
}

void
setOutputFormat( ostream& os )
{
  os.setf(ios::fixed|ios::showpoint);
  os.precision(PD.precision);
}

void
printParameters( ostream& os, const char *prefix )
{
  int pimSet;

  os << prefix << "softwareRevision " << PD.softwareRevision << std::endl;
  os << prefix << "version " << PD.version << std::endl;
  os << prefix << "problem \"" << PD.problemFile << "\"" << std::endl;
  os << prefix << "pddlProblemType " << PD.pddlProblemType << std::endl;
  os << prefix << "maxsteps " << PD.cutoff << std::endl;
  os << prefix << "discretizationLevels " << PD.discretizationLevels << std::endl;
  os << prefix << "discountFactor " << PD.discountFactor << std::endl;
  os << prefix << "MDPdiscountFactor " << PD.MDPdiscountFactor << std::endl;
  os << prefix << "runs " << PD.runs << std::endl;

  os << prefix << "pimSteps";
  for( pimSet = 0; PD.pimSteps[pimSet] != -1; ++pimSet )
    os << " " << PD.pimSteps[pimSet];
  os << std::endl;

  os << prefix << "learningTrials";
  for( pimSet = 0; PD.pimSteps[pimSet] != -1; ++pimSet )
    os << " " << PD.learningTrials[pimSet];
  os << std::endl;

  os << prefix << "controlTrials";
  for( pimSet = 0; PD.pimSteps[pimSet] != -1; ++pimSet )
    os << " " << PD.controlTrials[pimSet];
  os << std::endl;

  os << prefix << "incrementalMode " << PD.incrementalMode << std::endl;
  os << prefix << "zeroHeuristic " << PD.zeroHeuristic << std::endl;
  os << prefix << "randomSeed " << PD.randomSeed << std::endl;
  os << prefix << "outputLevel " << PD.outputLevel << std::endl;
}

static int
bootstrapPLANNING( problemHandleClass *handle )
{
  unsigned long secs;
  unsigned long usecs;
  struct timeval t1, t2;

  gettimeofday(&t1,NULL);
  if( (handle->beliefHook != NULL) || (handle->modelHook != NULL) ) {
    std::cerr << "Error: model/belief hook not yet supported" << std::endl;
    return(-1);
  }
  else {
    // model creation & setup
    PD.model = new planningModelClass;
    PD.belief = new standardBeliefClass;
    PD.model->setup(handle,PD.belief->getConstructor());
    PD.model->discountFactor = 1.0;
    PD.belief->setModel(PD.model);
    PD.baseHeuristic = handle->heuristicHook;
    PD.heuristic = NULL;

    // POMDP creation & setup
    PD.POMDP = new planningPOMDPClass;
    PD.POMDP->setModel(PD.model);
    PD.POMDP->setParameters(PD.model->numActions,1,PD.cutoff,PD.randomTies,PD.model->discountFactor);
    PD.POMDP->setTheInitialBelief(PD.model->getTheInitialBelief());
  }

  // heuristic setup
  if( !PD.zeroHeuristic && !PD.baseHeuristic ) {
    PD.heuristic = new lookAheadHeuristicClass(PD.POMDP,PD.baseHeuristic);
    PD.heuristic->setLookahead(PD.lookahead);
    PD.belief->setHeuristic(PD.heuristic);
  }

  // timing
  gettimeofday(&t2,NULL);
  diffTime(secs,usecs,t1,t2);
  *PD.outputFile << "%boot setupTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;
  return(0);
}

static int
bootstrapMDP( problemHandleClass *handle )
{
  unsigned long secs;
  unsigned long usecs;
  struct timeval t1, t2;
  standardBeliefClass *belief;

  int initialState;
  int action;
  map <int,float>::const_iterator it;
  const map <int,float> *initialBelief;

  gettimeofday(&t1,NULL);
  if( (handle->beliefHook != NULL) || (handle->modelHook != NULL) ) {
    std::cerr << "Error: model/belief hook not yet supported" << std::endl;
    return(-1);
  }
  else {
    // model creation
    PD.model = new standardModelClass;
    PD.belief = new simpleBeliefClass;
    PD.baseHeuristic = handle->heuristicHook;
    PD.heuristic = NULL;

    // POMDP creation
    PD.POMDP = new MDPPOMDPClass;
    PD.POMDP->setHash(new simpleBeliefHashClass);
  }

  // model setup
  belief = new standardBeliefClass;
  PD.model->setup(handle,belief->getConstructor());
  PD.model->discountFactor = PD.discountFactor;
  PD.belief->setModel(PD.model);
  delete belief;

  // generate extra single initial state leading to the true initial states, 
  // setup model for the extra state
  initialState = PD.model->newState();
  action = 0;                       // the INIT action geenrated by the parser
  initialBelief = PD.model->theInitialBelief->cast();
  for( it = initialBelief->begin(); it != initialBelief->end(); ++it ) {
    PD.model->newTransition(initialState,action,(*it).first,(*it).second);
  }
  ((standardModelClass*)PD.model)->setAvailability(initialState);

  // POMDP setup (no cache)
  PD.POMDP->setTheInitialBelief((beliefClass*)initialState);
  PD.POMDP->setModel(PD.model);
  PD.POMDP->setParameters(PD.model->numActions,1,PD.cutoff,PD.randomTies,PD.model->discountFactor);

  // heuristic
  if( PD.baseHeuristic != NULL ) {
    PD.heuristic = new lookAheadHeuristicClass(PD.POMDP,PD.baseHeuristic);
    PD.heuristic->setLookahead(PD.lookahead);
    PD.belief->setHeuristic(PD.heuristic);
  }

  // timing
  gettimeofday(&t2,NULL);
  diffTime(secs,usecs,t1,t2);
  *PD.outputFile << "%boot setupTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;
  return(0);
}

static int
bootstrapPOMDP( problemHandleClass *handle )
{
  unsigned long secs;
  unsigned long usecs;
  struct timeval t1, t2;

  gettimeofday(&t1,NULL);
  if( handle->modelHook != NULL ) {
    // model creation
    PD.model = handle->modelHook;
    PD.belief = handle->beliefHook;
    PD.baseHeuristic = handle->heuristicHook;
    PD.heuristic = NULL;

    // POMDP creation
    PD.POMDP = new standardPOMDPClass;
    PD.POMDP->setHash(new hookedBeliefHashClass);
  }
  else if( handle->beliefHook != NULL ) {
    std::cerr << "Error: belief hook not yet supported" << std::endl;
    return(-1);
  }
  else {
    if( ISPOMDP(handle->problemType) ) {
      PD.model = new standardModelClass;
      PD.baseHeuristic = handle->heuristicHook;
      PD.heuristic = NULL;
      PD.POMDP = new standardPOMDPClass;
      PD.belief = new standardBeliefClass;
      PD.POMDP->setHash(new standardBeliefHashClass);
    }
    else {
      PD.model = new nonDetModelClass;
      PD.baseHeuristic = handle->heuristicHook;
      PD.heuristic = NULL;
      PD.POMDP = new standardPOMDPClass;
      PD.belief = new setBeliefClass;
      PD.POMDP->setHash(new setBeliefHashClass);
    }
  }

  // model and belief setup
  PD.model->setup(handle,PD.belief->getConstructor());
  PD.model->discountFactor = PD.discountFactor;
  PD.belief->setModel(PD.model);

  // POMDP setup
  PD.POMDP->setTheInitialBelief(PD.model->getTheInitialBelief());
  if( ISPOMDP(handle->problemType) ) {
    PD.POMDP->setCache(PD.cacheSize[0],PD.clusterSize[0],PD.cacheSize[1],PD.clusterSize[1],
                        (beliefClass*(*)())&standardBeliefClass::constructor);
  }
  else {
    PD.POMDP->setCache(PD.cacheSize[0],PD.clusterSize[0],PD.cacheSize[1],PD.clusterSize[1],
                        (beliefClass*(*)())&setBeliefClass::constructor);
  }
  PD.POMDP->setModel(PD.model);
  PD.POMDP->setParameters(PD.model->numActions,1,PD.cutoff,PD.randomTies,PD.model->discountFactor);

  // timing
  gettimeofday(&t2,NULL);
  diffTime(secs,usecs,t1,t2);
  *PD.outputFile << "%boot setupTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;

  // compute model
  if( !PD.incrementalMode ) {
    try {
      ((standardModelClass*)PD.model)->computeWholeModelFrom(PD.model->getTheInitialBelief());
    } catch( exceptionClass& ) {
      PD.clean(problemClass::CLEAN_OBJECT);
      throw;
    }
  }

  // timing
  gettimeofday(&t1,NULL);
  diffTime(secs,usecs,t2,t1);
  *PD.outputFile << "%boot computeModelTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;

  // heuristic setup
  if( !PD.zeroHeuristic ) {
    if( !PD.baseHeuristic ) {
      if( !PD.incrementalMode ) {
        try {
          PD.baseHeuristic = new QMDPHeuristicClass(PD.MDPdiscountFactor,PD.model->numActions,
                                                     ((standardModelClass *)PD.model)->stateIndex,
                                                     (standardPOMDPClass*)PD.POMDP);
        } catch( exceptionClass& ) {
          PD.clean(problemClass::CLEAN_OBJECT);
          throw;
        }
      }
      else if( ISPOMDP1(handle->problemType) || ISNDPOMDP1(handle->problemType) ) {
        PD.baseHeuristic = new dynHeuristicClass(PD.model);
      }
    }
    if( PD.baseHeuristic ) {
      PD.heuristic = new lookAheadHeuristicClass(PD.POMDP,PD.baseHeuristic);
      PD.heuristic->setLookahead(PD.lookahead);
    }
  }

  // timing
  gettimeofday(&t2,NULL);
  diffTime(secs,usecs,t1,t2);
  *PD.outputFile << "%boot computeHeuristicTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;

  if( PD.heuristic ) PD.belief->setHeuristic(PD.heuristic);

  // others setup
  standardPOMDPClass::setup(((standardModelClass *)PD.model)->stateIndex);
  quantizationClass::initialize(PD.discretizationLevels,((standardModelClass *)PD.model)->stateIndex);
  if( ISPOMDP(handle->problemType) ) {
    quantizationClass::setCache(PD.cacheSize[2],PD.clusterSize[2],
                                 (beliefClass*(*)())&standardBeliefClass::constructor);
  }

  // check singal & end
  if( PD.signal >= 0 ) {
    int s = PD.signal;
    PD.signal = -1;
    throw(signalExceptionClass(s));
  }
  else
    return(0);
}

static int
bootstrapCONFORMANT( problemHandleClass *handle )
{
  unsigned long secs;
  unsigned long usecs;
  struct timeval t1, t2;

  gettimeofday( &t1, NULL );
  if( (handle->beliefHook != NULL) || (handle->modelHook != NULL) ) {
    std::cerr << "Error: model/belief hook not yet supported" << std::endl;
    return( -1 );
  }
  else {
    if( ISCONFORMANT2(handle->problemType) ) {
      PD.model = new standardModelClass;
      PD.baseHeuristic = handle->heuristicHook;
      PD.heuristic = NULL;
      PD.POMDP = new searchPOMDPClass;
      PD.belief = new standardBeliefClass;
      PD.POMDP->setHash( new standardBeliefHashClass );
    }
    else {
      PD.model = new nonDetModelClass;
      PD.baseHeuristic = handle->heuristicHook;
      PD.heuristic = NULL;
      PD.POMDP = new searchPOMDPClass;
      PD.belief = new setBeliefClass;
      PD.POMDP->setHash( new setBeliefHashClass );
    }
  }

  // model and belief setup
  PD.model->setup( handle, PD.belief->getConstructor() );
  PD.model->discountFactor = PD.discountFactor;
  PD.belief->setModel( PD.model );

  // POMDP setup
  PD.POMDP->setTheInitialBelief( PD.model->getTheInitialBelief() );
  if( ISCONFORMANT2(handle->problemType) ) {
    PD.POMDP->setCache( PD.cacheSize[0], PD.clusterSize[0], PD.cacheSize[1], PD.clusterSize[1],
                        (beliefClass*(*)())&standardBeliefClass::constructor );
  }
  else {
    PD.POMDP->setCache( PD.cacheSize[0], PD.clusterSize[0], PD.cacheSize[1], PD.clusterSize[1],
                        (beliefClass*(*)())&setBeliefClass::constructor );
  }
  PD.POMDP->setModel( PD.model );
  PD.POMDP->setParameters( PD.model->numActions, 1, PD.cutoff, PD.randomTies, PD.model->discountFactor );

  // timing
  gettimeofday( &t2, NULL );
  diffTime( secs, usecs, t1, t2 );
  *PD.outputFile << "%boot setupTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;

  // compute model
  if( !PD.incrementalMode ) {
    try {
      ((standardModelClass*)PD.model)->computeWholeModelFrom( PD.model->getTheInitialBelief() );
    } catch( exceptionClass& ) {
      PD.clean( problemClass::CLEAN_OBJECT );
      throw;
    }
  }

  // timing
  gettimeofday( &t1, NULL );
  diffTime( secs, usecs, t2, t1 );
  *PD.outputFile << "%boot computeModelTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;

  // heuristic setup
  if( !PD.zeroHeuristic ) {
    if( !PD.baseHeuristic ) {
      if( !PD.incrementalMode ) {
        try {
          PD.baseHeuristic = new QMDPHeuristicClass( PD.MDPdiscountFactor, PD.model->numActions,
                                                     ((standardModelClass *)PD.model)->stateIndex,
                                                     (standardPOMDPClass*)PD.POMDP );
        } catch( exceptionClass& ) {
          PD.clean( problemClass::CLEAN_OBJECT );
          throw;
        }
      }
      else {
        PD.baseHeuristic = new dynHeuristicClass( PD.model );
      }
    }

    if( PD.baseHeuristic ) {
      PD.heuristic = new lookAheadHeuristicClass( PD.POMDP, PD.baseHeuristic );
      PD.heuristic->setLookahead( PD.lookahead );
    }
  }

  // timing
  gettimeofday( &t2, NULL );
  diffTime( secs, usecs, t1, t2 );
  *PD.outputFile << "%boot computeHeuristicTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;

  if( !PD.zeroHeuristic && PD.heuristic ) PD.belief->setHeuristic( PD.heuristic );

  // check singal & end
  if( PD.signal >= 0 ) {
    int s = PD.signal;
    PD.signal = -1;
    throw( signalExceptionClass( s ) );
  }
  else
    return( 0 );
}

static int
bootstrapCassandra( void )
{
  unsigned long secs;
  unsigned long usecs;
  struct timeval t1, t2;

  // model creation: use setup for cassandra's format
  gettimeofday( &t1, NULL );
  PD.model = new standardModelClass;
  PD.belief = new standardBeliefClass;
  ((standardModelClass *)PD.model)->setup( PD.problemFile, PD.belief->getConstructor() );
  PD.discountFactor = PD.model->discountFactor;
  PD.belief->setModel( PD.model );

  // POMDP creation & setup
  PD.POMDP = new standardPOMDPClass;
  PD.POMDP->setTheInitialBelief( PD.model->getTheInitialBelief() );
  PD.POMDP->setHash( new standardBeliefHashClass );
  PD.POMDP->setCache( PD.cacheSize[0], PD.clusterSize[0],
		      PD.cacheSize[1], PD.clusterSize[1],
		      (beliefClass*(*)())&standardBeliefClass::constructor );
  PD.POMDP->setModel( PD.model );
  PD.POMDP->setParameters( PD.model->numActions, 1, PD.cutoff, PD.randomTies, PD.model->discountFactor );

  // timing
  gettimeofday( &t2, NULL );
  diffTime( secs, usecs, t1, t2 );
  *PD.outputFile << "%boot setupTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;

  // heuristic setup
  if( !PD.zeroHeuristic ) {
    PD.baseHeuristic = new QMDPHeuristicClass( PD.MDPdiscountFactor,
                                               PD.model->numActions,
                                               ((standardModelClass *)PD.model)->stateIndex,
                                               (standardPOMDPClass*)PD.POMDP );
    PD.heuristic = new lookAheadHeuristicClass( PD.POMDP, PD.baseHeuristic );
    PD.heuristic->setLookahead( PD.lookahead );
    PD.belief->setHeuristic( PD.heuristic );
  }

  // timing
  gettimeofday( &t1, NULL );
  diffTime( secs, usecs, t2, t1 );
  *PD.outputFile << "%boot computeHeuristicTime " << (float)secs + (float)usecs / 1000000.0 << std::endl;

  // others setup
  standardPOMDPClass::setup( ((standardModelClass *)PD.model)->stateIndex );
  quantizationClass::initialize( PD.discretizationLevels, 
				 ((standardModelClass *)PD.model)->stateIndex );
  quantizationClass::setCache( PD.cacheSize[2], PD.clusterSize[2],
			       (beliefClass*(*)())&standardBeliefClass::constructor );

  return( 0 );
}

int
bootstrapProblem( const char *workingDir, const char *entryPoint )
{
  int rv;

  PD.clean( problemClass::CLEAN_OBJECT );
  if( PD.pddlProblemType ) {
    // get problem handle and initialize problem
    if( !(PD.handle = getHandle( PD.problemFile, workingDir, entryPoint )) ) return( -1 );
    (*PD.handle->initializeFunction)();

    // bootstrap specific problem
    switch( PD.handle->problemType ) {
      case problemHandleClass::PROBLEM_PLANNING:
        planningBeliefClass::initialize( PD.handle );
        rv = bootstrapPLANNING( PD.handle );
        break;
      case problemHandleClass::PROBLEM_MDP:
      case problemHandleClass::PROBLEM_ND_MDP:
        rv = bootstrapMDP( PD.handle );
        break;
      case problemHandleClass::PROBLEM_POMDP1:
      case problemHandleClass::PROBLEM_POMDP2:
      case problemHandleClass::PROBLEM_ND_POMDP1:
      case problemHandleClass::PROBLEM_ND_POMDP2:
        rv = bootstrapPOMDP( PD.handle );
        break;
      case problemHandleClass::PROBLEM_CONFORMANT1:
      case problemHandleClass::PROBLEM_CONFORMANT2:
        rv = bootstrapCONFORMANT( PD.handle );
        break;
      default:
        std::cerr << std::endl << "Error: undefined problem model" << std::endl;
        return( -1 );
    }
  }
  else {
    rv = bootstrapCassandra();
  }
  return( rv );
}

void
writeCore( ostream& os )
{
  beliefHashClass *beliefHash;
  static char buff[1024];

  //xxxx In order to implement for MDP need to serialize the model; i.e.
  //xxxx the stateHash, invStateHash, ... For that need to generate code
  //xxxx for registering at bootstrap the specific state/obs classes.

  if( ISPOMDP(PD.handle->problemType) || ISNDPOMDP(PD.handle->problemType) ) {
    // write problemType and problemFile
    strcpy( buff, basename( PD.problemFile ) );
    safeWrite( buff, sizeof( char ), 1024, os );

    // write POMDP
    beliefHash = PD.POMDP->removeHash();
    beliefHash->write( os );
    PD.POMDP->setHash( beliefHash );
  }
  else {
    std::cerr << "Error: core generation is only supported for POMDP models." << std::endl;
  }
}

int
readCore( const char *coreFilename )
{
  beliefHashClass *beliefHash;
  static char buff[1024];

  // xxx: only supported for MDP/POMDP problems (yet?)

  // open core file
  ifstream coreFile( coreFilename, ios::in );
  if( !coreFile ) {
    *PD.outputFile << coreFilename << ": " << strerror( errno ) << std::endl;
    std::cerr << coreFilename << ": " << strerror( errno ) << std::endl;
    return( -1 );
  }

  // read problemFile
  safeRead( buff, sizeof( char ), 1024, coreFile );

  // check that core file correspond to problem
  if( strcmp( buff, basename( PD.problemFile ) ) ) {
    std::cerr << "Error: core does not correspond to problem \"" << basename( PD.problemFile ) << "\"" << std::endl;
    return( -1 );
  }

  // read hash table from core
  delete PD.POMDP->removeHash();
  beliefHash = (beliefHashClass *)hashClass::read( coreFile );
  PD.POMDP->setHash( beliefHash );

  // close coreFile
  coreFile.close();
  return( 0 );
}

static void
generateCore( int run, int pim, int trial, int pimSet )
{
  char *tmpStr;
  const char *prefix;
  static char buff[4096];

  // generate prefix
  tmpStr = strdup( basename( PD.problemFile ) );
  prefix = (PD.outputPrefix != NULL ? PD.outputPrefix : strtok( tmpStr, "." ));

  switch( PD.coreLevel ) {
    case 1:
      if( ((pim == PD.pimSteps[pimSet] - 1) && (trial == PD.learningTrials[pimSet] - 1)) ||
          (PD.useStopRule && (bool)PD.POMDP->getTheInitialBelief()->getExtra()) ) { // open core file
        sprintf( buff, "%s.core.run=%d", prefix, run + 1 );
        ofstream coreFile( buff, ios::out /*| ios::noreplace*/ );
        if( !coreFile ) {
          *PD.outputFile << buff << ": " << strerror( errno ) << std::endl;
          std::cerr << buff << ": " << strerror( errno ) << std::endl;
        }
        else { // write core and close it
          writeCore( coreFile );
          coreFile.close();
        }
      }
      break;
    case 2:
      if( trial == PD.learningTrials[pimSet] - 1 ) { // open core file
        sprintf( buff, "%s.core.run=%d.pim=%d", prefix, run + 1, pim + 1 );
        ofstream coreFile( buff, ios::out /*| ios::noreplace*/ );
        if( !coreFile ) {
          *PD.outputFile << buff << ": " << strerror( errno ) << std::endl;
          std::cerr << buff << ": " << strerror( errno ) << std::endl;
        }
        else { // write core and close it
          writeCore( coreFile );
          coreFile.close();
        }
      }
      break;
    case 3: // open core file
      sprintf( buff, "%s.core.run=%d.pim=%d.trial=%d", prefix, run + 1, pim + 1, trial + 1 );
      ofstream coreFile( buff, ios::out /*| ios::noreplace*/ );
      if( !coreFile ) {
        *PD.outputFile << buff << ": " << strerror( errno ) << std::endl;
        std::cerr << buff << ": " << strerror( errno ) << std::endl;
      }
      else { // write core and close it
        writeCore( coreFile );
        coreFile.close();
      }
      break;
    }
  free(tmpStr);
}

static void
solvePddlProblem( void )
{
  int run, pimSet, pim, trial;
  resultClass *result;

  // build result
  switch( PD.handle->problemType ) {
    case problemHandleClass::PROBLEM_CONFORMANT1:
    case problemHandleClass::PROBLEM_CONFORMANT2:
      result = new searchResultClass;
      break;
    case problemHandleClass::PROBLEM_MDP:
    case problemHandleClass::PROBLEM_ND_MDP:
    case problemHandleClass::PROBLEM_POMDP1:
    case problemHandleClass::PROBLEM_POMDP2:
    case problemHandleClass::PROBLEM_ND_POMDP1:
    case problemHandleClass::PROBLEM_ND_POMDP2:
      result = new standardResultClass;
      break;
    default:
      std::cerr << std::endl << "Error: unsupported problem type" << std::endl;
      return;
  }

  // solve it
  PD.POMDP->initialize();
  for( run = 0; run < PD.runs; ++run ) {
    result->run = run + 1;
    PD.POMDP->initRun( result->run );
    for( pimSet = 0; PD.pimSteps[pimSet] != -1; ++pimSet ) {
      bool solved = false;

      // check if problem is already solved (stopRule)
      if( !ISCONFORMANT(PD.handle->problemType) && PD.useStopRule ) {
        switch( PD.handle->problemType ) {
          case problemHandleClass::PROBLEM_POMDP1:
          case problemHandleClass::PROBLEM_POMDP2:
          case problemHandleClass::PROBLEM_ND_POMDP1:
          case problemHandleClass::PROBLEM_ND_POMDP2:
            if( PD.POMDP->getTheInitialBelief() && (bool)PD.POMDP->getTheInitialBelief()->getExtra() )
              solved = true;
            break;
          case problemHandleClass::PROBLEM_MDP:
          case problemHandleClass::PROBLEM_ND_MDP:
            map<int,float>::const_iterator it;
            int  state = (intptr_t)PD.POMDP->getTheInitialBelief()->getData();
            for( it = ((standardModelClass*)PD.model)->transitionModel[0][state].begin();
                 it != ((standardModelClass*)PD.model)->transitionModel[0][state].end();
                 ++it ) {
              if( !PD.POMDP->getHashEntry( (beliefClass*)(*it).first ) ||
                  !(bool)PD.POMDP->getHashEntry( (beliefClass*)(*it).first )->getExtra() )
                break;
            }
            if( it == ((standardModelClass*)PD.model)->transitionModel[0][state].end() ) solved = true;
            break;
        }
        if( solved ) break;
      }

      // perform pim step
      for( pim = 0; pim < PD.pimSteps[pimSet]; ++pim  ) { // learn trial
        for( trial = 0; (trial < PD.learningTrials[pimSet]) || (!ISCONFORMANT(PD.handle->problemType) && PD.useStopRule); ++trial ) {
          try {
            PD.POMDP->learnAlgorithm( *result );
          } catch( exceptionClass& ) {
            result->clean();
            delete result;
            PD.POMDP->endRun( true );
            PD.POMDP->finalize();
            throw;
          }
          result->print( *PD.outputFile, PD.outputLevel, PD.handle );
          result->clean();
          generateCore( run, pim, trial, pimSet );

          // check termination due to stopping rule
          if( PD.useStopRule && (bool)PD.POMDP->getTheInitialBelief()->getExtra() ) break;
        }
        for( trial = 0; trial < PD.controlTrials[pimSet]; ++trial ) { // control trial
          try {
            PD.POMDP->controlAlgorithm( *result );
          } 
          catch( exceptionClass& ) {
            result->clean();
            delete result;
            PD.POMDP->endRun( true );
            PD.POMDP->finalize();
            throw;
          }
          result->print( *PD.outputFile, PD.outputLevel, PD.handle );
          result->clean();
        }
      }
    }

    // check abortion
    if( PD.signal >= 0 ) {
      int s = PD.signal;
      PD.signal = -1;
      result->clean();
      delete result;
      PD.POMDP->endRun( true );
      PD.POMDP->finalize();
      throw( signalExceptionClass( s ) );
    }

    // statistics and clean
    PD.POMDP->statistics( *PD.outputFile );

    // finish this run
    PD.POMDP->endRun( (run == PD.runs-1) );
  }
  PD.POMDP->finalize();

  // free resources
  delete result;
}

static void
solveCassandraProblem()
{
  resultClass *result = new standardResultClass;
  PD.POMDP->initialize();
  for( int run = 0; run < PD.runs; ++run ) {
    result->run = run + 1;
    PD.POMDP->initRun(result->run);

    for( int pimSet = 0; PD.pimSteps[pimSet] != -1; ++pimSet ) { // perform pim step
      for( int pim = 0; pim < PD.pimSteps[pimSet]; ++pim  ) {
        for( int trial = 0; (trial < PD.learningTrials[pimSet]) || PD.useStopRule; ++trial ) { // learn trial
          try {
            PD.POMDP->learnAlgorithm(*result);
          }
          catch( exceptionClass& ) {
            result->clean();
            delete result;
            PD.POMDP->endRun(true);
            PD.POMDP->finalize();
            throw;
          }
          result->print(*PD.outputFile,PD.outputLevel,PD.handle);
          result->clean();
          generateCore(run,pim,trial,pimSet);
          if( PD.useStopRule && (bool)PD.POMDP->getTheInitialBelief()->getExtra() ) break;
        }
	float *costs = new float[PD.controlTrials[pimSet]];
        for( int trial = 0; trial < PD.controlTrials[pimSet]; ++trial ) { // control trial
          try {
            PD.POMDP->controlAlgorithm(*result);
          } 
          catch( exceptionClass& ) {
            result->clean();
            delete result;
            PD.POMDP->endRun(true);
            PD.POMDP->finalize();
            throw;
          }
          result->print(*PD.outputFile,PD.outputLevel,PD.handle);
	  costs[trial] = result->discountedCostToGoal;
          result->clean();
        }
        if( PD.controlTrials[pimSet] > 0 ) { // print control stats
          float avg = 0, dev = 0;
          for( int trial = 0; trial < PD.controlTrials[pimSet]; ++trial ) avg += costs[trial];
          avg /= PD.controlTrials[pimSet];
          for( int trial = 0; trial < PD.controlTrials[pimSet]; ++trial ) dev += (costs[trial]-avg)*(costs[trial]-avg);
          dev = sqrt(dev/PD.controlTrials[pimSet]);
          *PD.outputFile << "%control stats " << avg << " " << dev << std::endl;
        }
        delete[] costs;
      }
    }

    // check abortion
    if( PD.signal >= 0 ) {
      int s = PD.signal;
      PD.signal = -1;
      result->clean();
      delete result;
      PD.POMDP->endRun(true);
      PD.POMDP->finalize();
      throw(signalExceptionClass(s));
    }

    // statistics and clean
    PD.POMDP->statistics(*PD.outputFile);

    // finish this run
    PD.POMDP->endRun(run == PD.runs-1);
  }
  PD.POMDP->finalize();

  // free resources
  delete result;
}

void
solveProblem( void )
{
  if( PD.model->currentNumberStates() != 0 ) {
    if( PD.pddlProblemType )
      solvePddlProblem();
    else
      solveCassandraProblem();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Policy Generation
//

void
policyGraphClass::generatePolicyGraphAux( policyGraphNodeClass& node )
{
  policyGraphNodeClass child;
  pair<int,pair<policyGraphNodeClass,policyGraphNodeClass> > edge;
  deque<pair<pair<int,float>,const hashEntryClass*> >expansion;
  deque<pair<pair<int,float>,const hashEntryClass*> >::const_iterator it;

#if 0
  std::cout << "GRAPH: insert belief " << node.second << std::endl;
  std::cout << "GRAPH: with action " << node.first << std::endl;
#endif

  nodes.insert( node );
  if( PD.pddlProblemType && (ISMDP(PD.handle->problemType) || ISNDMDP(PD.handle->problemType)) ) {
    // check for goal node
    if( PD.POMDP->inGoal( (const beliefClass*)node.second ) ) {
      goals.insert(node);
      return;
    }
  }
  else {
    if( PD.POMDP->inGoal( (const beliefClass*)node.second->getData() ) ) {
      goals.insert(node);
      return;
    }
  }
  if( node.first == -1 ) return;

  // generate and expand belief_a
  PD.POMDP->expandBeliefWithAction(node.second,node.first,expansion);
  assert( expansion.size() >= 1 );

  // go over children
  for( it = expansion.begin(); it != expansion.end(); ++it ) {
    // create node and edge and recurse
    int obs = (*it).first.first;
    const hashEntryClass *belief_ao = (*it).second;
    child = make_pair(PD.POMDP->getBestAction(belief_ao),belief_ao);
    edge = make_pair(obs,make_pair(node,child));
    if( edges.find(edge) == edges.end() ) {
      edges.insert(edge);
      generatePolicyGraphAux(child);
    }
  }
}

void
policyGraphClass::generatePolicyGraph( void )
{
  policyTableNodeClass root;
  if( PD.pddlProblemType && (ISMDP(PD.handle->problemType) || ISNDMDP(PD.handle->problemType)) ) {
    map<int,float>::const_iterator it;
    int state = (intptr_t)PD.POMDP->getTheInitialBelief()->getData();
    for( it = ((standardModelClass*)PD.model)->transitionModel[0][state].begin();
         it != ((standardModelClass*)PD.model)->transitionModel[0][state].end();
         ++it ) {
      root.second = (const hashEntryClass*)(*it).first;
      root.first = PD.POMDP->getBestAction(root.second);
      nodes.insert(root);
      roots.insert(root);
      generatePolicyGraphAux(root);
    }
  }
  else {
    root.second = PD.POMDP->getTheInitialBelief();
    root.first = PD.POMDP->getBestAction(root.second);
    nodes.insert(root);
    roots.insert(root);
    generatePolicyGraphAux(root);
  }
}

void
policyGraphClass::outputPolicyGraph( ostream &out )
{
  int id;
  set<policyGraphEdgeClass>::const_iterator edge_it;
  set<policyGraphNodeClass>::const_iterator node_it;

  out << "Creator \"GPT " << PD.softwareRevision << "\"" << std::endl;
  out << "Version 1.7" << std::endl;
  out << "graph [ " << std::endl << std::endl;
  out << "  directed 1" << std::endl << std::endl;

  // nodes
  for( node_it = nodes.begin(), id = 0; node_it != nodes.end(); ++node_it, ++id ) {
    out << "  node [" << std::endl;
    out << "    id " << id << std::endl;
    if( goals.find( (*node_it) ) == goals.end() ) {
      out << "    label \"";
      if( PD.pddlProblemType )
        (*PD.handle->printAction)( out, (*node_it).first );
      else
        out << (*node_it).first;
      out << "\"" << std::endl;
    }
    else {
      out << "    label \"GOAL\"" << std::endl;
    }

    out << "      labelAnchor \"w\"" << std::endl;
    out << "    graphics [" << std::endl;
    out << "      type \"rectangle\"" << std::endl;
    out << "      x " << mrand48() % 1000 << ".0" << std::endl;
    out << "      y " << mrand48() % 1000 << ".0" << std::endl;
    out << "      w 30.0" << std::endl;
    out << "      h 30.0" << std::endl;

    if( roots.find( (*node_it) ) != roots.end() )
      out << "      fill \"#00FFFF\"" << std::endl;
    else if( goals.find( (*node_it) ) != goals.end() )
      out << "      fill \"#FFFF00\"" << std::endl;
    else
      out << "      fill \"#FA1340\"" << std::endl;

    out << "      width 1.0" << std::endl;
    out << "      outline \"#000000\"" << std::endl;
    out << "    ]" << std::endl;
    out << "    LabelGraphics [" << std::endl;
    out << "      type \"text\"" << std::endl;
    out << "      fill \"#000000\"" << std::endl;
    out << "      anchor \"w\"" << std::endl;
    out << "    ]" << std::endl;
    out << "  ]" << std::endl;
  }
  out << std::endl;

  // edges
  for( edge_it = edges.begin(); edge_it != edges.end(); edge_it++ ) {
    out << "  edge [" << std::endl;

    // find source id
    for( node_it = nodes.begin(), id = 0; node_it != nodes.end(); ++node_it, ++id ) {
      if( (*node_it) == (*edge_it).second.first ) {
        out << "    source " << id << std::endl;
        break;
      }
    }

    // find target id
    for( node_it = nodes.begin(), id = 0; node_it != nodes.end(); ++node_it, ++id ) {
      if( (*node_it) == (*edge_it).second.second ) {
        out << "    target " << id << std::endl;
        break;
      }
    }

    // edge label
    out << "    label \"obs:" << (*edge_it).first << "\"" << std::endl;
    out << "    graphics [" << std::endl;
    out << "      type \"line\"" << std::endl;
    out << "      arrow \"last\"" << std::endl;
    out << "    ]" << std::endl;
    out << "    labelGraphics [" << std::endl;
    out << "      type \"text\"" << std::endl;
    out << "      fill \"#000000\"" << std::endl;
    out << "      anchor \"e\"" << std::endl;
    out << "    ]" << std::endl;
    out << "  ]" << std::endl;
  }
  out << std::endl << "]" << std::endl;
}

void
policyTableClass::generatePolicyTableAux( policyTableNodeClass& node )
{
  const hashEntryClass *belief_ao;
  policyTableNodeClass child;
  deque<pair<pair<int,float>,const hashEntryClass*> > expansion;
  deque<pair<pair<int,float>,const hashEntryClass*> >::const_iterator it;

  nodes.insert( node );
  if( PD.pddlProblemType && (ISMDP(PD.handle->problemType) || ISNDMDP(PD.handle->problemType)) ) {
    states.insert( (intptr_t)node.second->getData() );
    if( PD.POMDP->inGoal( (const beliefClass*)node.second->getData() ) ) {
      goals.insert( node );
      return;
    }
  }
  else {
    // insert states
    const beliefClass *bel = (const beliefClass*)node.second->getData();
    const map<int,float> *m = bel->cast();
    map<int,float>::const_iterator it2;
    for( it2 = m->begin(); it2 != m->end(); ++it2 )
      states.insert( (*it2).first );

    // check for goal node
    if( PD.POMDP->inGoal( bel ) ) {
      goals.insert( node );
      return;
    }
  }

  // check for dead end
  if( node.first == -1 ) return;

  // generate and expand belief_a
  assert( PD.POMDP->applicable( (const beliefClass*)node.second->getData(), node.first ) );
  PD.POMDP->expandBeliefWithAction( node.second, node.first, expansion );
  assert( expansion.size() >= 1 );

  // go over children
  for( it = expansion.begin(); it != expansion.end(); ++it ) {
    // create new node
    belief_ao = (*it).second;
    child = make_pair( PD.POMDP->getBestAction( belief_ao ), belief_ao );
    if( nodes.find( child ) == nodes.end() ) generatePolicyTableAux( child );
  }
}

void
policyTableClass::generatePolicyTable( void )
{
  policyTableNodeClass root;
  if( PD.pddlProblemType && (ISMDP(PD.handle->problemType) || ISNDMDP(PD.handle->problemType)) ) {
    map<int,float>::const_iterator it;
    const hashEntryClass* entry = PD.POMDP->getTheInitialBelief();
    int state = (intptr_t)entry->getData();
    int action = PD.POMDP->getBestAction( entry );
    for( it = ((standardModelClass*)PD.model)->transitionModel[action][state].begin();
         it != ((standardModelClass*)PD.model)->transitionModel[action][state].end(); ++it ) {
      root.second = PD.POMDP->getHashEntry( (const beliefClass*)(*it).first );
      root.first = PD.POMDP->getBestAction( root.second );
      nodes.insert( root );
      generatePolicyTableAux( root );
    }
  }
  else {
    root.second = PD.POMDP->getTheInitialBelief();
    root.first = PD.POMDP->getBestAction( root.second );
    nodes.insert( root );
    generatePolicyTableAux( root );
  }
}

void
policyTableClass::outputPolicyTable( ostream &out )
{
  set<int>::const_iterator state_it;
  set<policyTableNodeClass>::const_iterator node_it;

  // states
  out << "*****  STATES  *****" << std::endl << std::endl;
  for( state_it = states.begin(); state_it != states.end(); ++state_it ) {
    out << "state-" << *state_it << ":" << std::endl;
    PD.model->printState( *state_it, out, 4 );
    out << std::endl;
  }

  // nodes
  out << "*****  POLICY  *****" << std::endl << std::endl;
  for( node_it = nodes.begin(); node_it != nodes.end(); ++node_it ) {
    if( PD.pddlProblemType && (ISMDP(PD.handle->problemType) || ISNDMDP(PD.handle->problemType)) )
      out << "(" << (intptr_t)(*node_it).second->getData() << ",";
    else
      out << "(" << *(const beliefClass*)(*node_it).second->getData() << ",";

    if( goals.find( (*node_it) ) == goals.end() ) {
      if( (*node_it).first != -1 ) {
        if( PD.pddlProblemType )
          (*PD.handle->printAction)( out, (*node_it).first );
        else
          out << (*node_it).first;
      }
      else {
        out << "DEAD-END";
      }
    }
    else
      out << "GOAL";
    out << ")" << std::endl;
  }
  out << std::endl;
}

