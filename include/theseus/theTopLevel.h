//  Theseus
//  theTopLevel.h -- Top Level Routines
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theTopLevel_INCLUDE_
#define _theTopLevel_INCLUDE_

#include <set>
#include <theseus/theLookAheadHeuristic.h>

class POMDPClass;
class modelClass;
class beliefClass;
class hashEntryClass;
class problemHandleClass;

#define MAX_PIMS          128
#define ISCONFORMANT1(id) ((id)==problemHandleClass::PROBLEM_CONFORMANT1)
#define ISCONFORMANT2(id) ((id)==problemHandleClass::PROBLEM_CONFORMANT2)
#define ISCONFORMANT(id)  (ISCONFORMANT1(id) || ISCONFORMANT2(id))
#define ISPOMDP1(id)      ((id)==problemHandleClass::PROBLEM_POMDP1)
#define ISPOMDP2(id)      ((id)==problemHandleClass::PROBLEM_POMDP2)
#define ISPOMDP(id)       (ISPOMDP1(id) || ISPOMDP2(id))
#define ISNDPOMDP1(id)    ((id)==problemHandleClass::PROBLEM_ND_POMDP1)
#define ISNDPOMDP2(id)    ((id)==problemHandleClass::PROBLEM_ND_POMDP2)
#define ISNDPOMDP(id)     (ISNDPOMDP1(id) || ISNDPOMDP2(id))
#define ISMDP(id)         ((id)==problemHandleClass::PROBLEM_MDP)
#define ISNDMDP(id)       ((id)==problemHandleClass::PROBLEM_ND_MDP)
#define ISPLANNING(id)    ((id)==problemHandleClass::PROBLEM_PLANNING)


class problemClass 
{
public:
  // members
  int                      signal;

  char*                    programName;
  char*                    problemFile;
  char*                    outputPrefix;
  char*                    outputFilename;
  char*                    coreFilename;
  char*                    linkmap;
  ostream*                 outputFile;

  const char*              softwareRevision;
  int                      version;
  int                      cutoff;
  int                      runs;
  int                      pimSteps[MAX_PIMS];
  int                      learningTrials[MAX_PIMS];
  int                      controlTrials[MAX_PIMS];
  int                      discretizationLevels;
  float                    discountFactor;
  float                    MDPdiscountFactor;
  int                      lookahead;
  bool                     incrementalMode;
  bool                     zeroHeuristic;
  int                      randomSeed;
  bool                     randomTies;
  bool                     pddlProblemType;
  int                      outputLevel;
  int                      verboseLevel;
  int                      precision;
  int                      coreLevel;
  int                      loadType;         // not yet implemented
  int                      cacheSize[3];
  int                      clusterSize[3];

  bool                     useStopRule;
  double                   SREpsilon;
  double                   epsilon;

  POMDPClass*              POMDP;
  modelClass*              model;
  beliefClass*             belief;
  lookAheadHeuristicClass* heuristic;
  heuristicClass*          baseHeuristic;
  problemHandleClass*      handle;

  // methods
  void clean( int level );

  // constants
  enum { CLEAN_OBJECT, CLEAN_CORE, CLEAN_HASH };
  enum { VERSION_GRID, VERSION_ORIGINAL };
};

// global data
extern problemClass          PD;

// global functions
void parseArguments( int argc, char **argv, void (*helpFunction)() );
void internalInitialization( void );
void internalFinalization( void );
void setOutputFormat( ostream& os );
void printParameters( ostream& os, const char *prefix );
int  bootstrapProblem( const char *workingDir, const char *entryPoint );
void writeCore( ostream& os );
int  readCore( const char *coreFilename );
void solveProblem( void );


///////////////////////////////////////////////////////////////////////////////
//
// Policy Generation
//

typedef pair<int,const hashEntryClass*> policyGraphNodeClass;
typedef pair<int,pair<policyGraphNodeClass,policyGraphNodeClass> > policyGraphEdgeClass;

class policyGraphClass
{
private:
  void generatePolicyGraphAux( policyGraphNodeClass& node );

public:
  set<policyGraphNodeClass> nodes;
  set<policyGraphEdgeClass> edges;
  set<policyGraphNodeClass> goals;
  set<policyGraphNodeClass> roots;

  void generatePolicyGraph( void );
  void outputPolicyGraph( ostream& out );
};

typedef pair<int,const hashEntryClass*> policyTableNodeClass;

class policyTableClass
{
private:
  void generatePolicyTableAux( policyTableNodeClass& node );

public:
  set<policyTableNodeClass> nodes;
  set<policyTableNodeClass> goals;
  set<int>                  states;

  void generatePolicyTable( void );
  void outputPolicyTable( ostream& out );
};

#endif // _theTopLevel_INCLUDE
