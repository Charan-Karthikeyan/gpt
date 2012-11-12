/*
**  Daedalus
**  daeParser.y -- Parser Grammar and Main Module
**
**  Blai Bonet, Hector Geffner
**  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002
**
*/

%{
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <assert.h>

using namespace std;

#include <daedalus/daeTokens.h>
#include <daedalus/daeTypes.h>
#include <daedalus/daePackage.h>
#include <daedalus/_daeErrors.h>

#include <typeinfo>
#include <list>
#include <map>

#ifdef linux
extern int yylex( void );
#endif

#define MAX(x,y)             ((x)>(y)?(x):(y))


// globals
char*                        yyfile;
char*                        yyparser;
bool                         parsingPredicateId = false;
static const char*           currInternalPredicate = NULL;

static int                   domainType;
static bool                  insideEffect = false; 
static const char*           currentRamification = NULL;
static typeListClass*        plusFunctionDomain = NULL;
static typeClass*            plusFunctionRange = NULL;
static typeListClass*        minusFunctionDomain = NULL;
static typeClass*            minusFunctionRange = NULL;
static int                   maxNumResStates = 0;

list<const codeChunkClass*>* globalChunk = NULL;
predicateClass*              theEqualPredicate = NULL;
predicateClass*              theLessThanPredicate = NULL;
predicateClass*              theLessEqualPredicate = NULL;
predicateClass*              theGreaterEqualPredicate = NULL;
predicateClass*              theGreaterThanPredicate = NULL;

booleanTypeClass*            theBooleanType = NULL;
genericIntegerTypeClass*     theGenericIntegerType = NULL;

domainClass*                 declaredDomain = NULL;
problemClass*                declaredProblem = NULL;
actionClass*                 declaredAction = NULL;

map<const char*,const domainClass*,ltstr> domainHash;
map<const char*,const char*,ltstr> problemSet;

map<const char*,const functionClass*,ltstr>   globalFunctionHash;
map<const char*,const predicateClass*,ltstr>  globalPredicateHash;
map<const char*,const namedTypeClass*,ltstr>  globalNamedTypeHash;
map<const char*,const functionClass*,ltstr>*  functionHash = NULL;
map<const char*,const predicateClass*,ltstr>* predicateHash = NULL;
map<const char*,const namedTypeClass*,ltstr>* namedTypeHash = NULL;
map<const char*,list<const char*>*,ltstr>*    objectHashByType = NULL;
map<const char*,const typeClass*,ltstr>*      objectHashByName = NULL;
map<const char*,const arrayClass*,ltstr>*     arrayHash = NULL;
set<const char*,ltstr>*                       externalProcedureList = NULL;

set<const char*,ltstr>           actionSet;
set<const char*,ltstr>           ramificationSet;
set<const char*,ltstr>           internalPredicateSet;

list<const actionClass*>*            actionList = NULL;
list<const ramificationClass*>*      ramificationList = NULL;
list<const internalPredicateClass*>* internalPredicateList = NULL;

set<const char*,ltstr>           ramDirtyPredicates;
set<const char*,ltstr>           ramDirtyFunctions;

// environment for variables
static list<map<const char*,const typeClass*,ltstr>*> variableEnvironment;
static list<pair<const char*,const typeClass*> >          parameterList;

// prototypes
static void                  startDomain( const char* );
static const codeChunkClass* finishDomain( void );
static void                  declareModel( void );
static void                  declareDynamics( int );
static void                  declareFeedback( int );
static void                  declareExternal( const char* );
static void                  declareExternal( const list<const char*>* );
static void                  declareNamedType( const char*, const list<const char*>* );
static void                  declareFunction( const char*, const typeClass*, const typeClass* );
static void                  declarePredicate( const char*, const typeClass* );
static void                  declareProcedure( const char* );
static void                  declareSimpleObjects( const char*, const list<const char*> * );
static void                  declareIntegerObjects( const char*, const char*, const list<const char*> * );
static void                  declareBooleanObjects( const list<const char*>* );
static void                  declareArrayObjects( const char*, const typeClass*, const list<const char*>* );
static void                  declareVariable( const char*, const typeClass* );
static void                  startRamification( const char* );
static void                  declareRamification( const formulaClass* );
static void                  declareRamification( const effectClass* );
static void                  startLogicalDef( const char* );
static void                  declareLogicalDef( const char*, const formulaClass* );
static void                  startAction( const char* );
static void                  finishAction( void );
static void                  declareActionPrecondition( const formulaClass* );
static void                  declareActionEffect( const effectClass* );
static void                  declareActionCost( const costClass* );
static void                  declareActionObservation( const observationClass* );
static termClass *           fetchVariableTerm( const char*, termClass* );
static termClass *           fetchObjectTerm( const char*, const termClass* );
static void                  setVariableDenotationFor( const char* );
static bool                  checkProbabilities( const probabilisticEffectClass* );
static void                  checkProbabilities(const list<pair<const char*,const observationListClass*>*>*);
static void                  startProblem( const char*, const char* );
static const codeChunkClass* finishProblem( void );
static void                  declareInit( const effectListClass* );
static void                  declareGoal( const formulaClass* );
static void                  declareHook( const hookClass* );
static void                  cleanEnvironment( map<const char*,const typeClass*,ltstr>* );
static void                  printEnvironment( void ); 
static list<map<const char*,const typeClass*,ltstr>*>* environmentClone( void );
extern void                  generateCode( problemClass* );
%}

/* reserved words */
%token DEFINE
%token PDOMAIN
%token CDOMAIN
%token PROBLEM
%token REQUIREMENTS
%token TYPES
%token FUNCTION
%token FUNCTIONS
%token PREDICATE
%token PREDICATES
%token PROCEDURE
%token EXTERNAL
%token OBJECTS
%token ACTION
%token PARAMETERS
%token PRECONDITION
%token EFFECT 
%token COST
%token OBSERVATION
%token WHEN
%token INIT
%token GOAL
%token PACKAGE
%token RAMIFICATION
%token FORMULA
%token SET
%token IN
%token ONEOF
%token PRINT
%token PRINTSTATE
%token ASSERT
%token HOOKS
%token BELIEF
%token HEURISTIC

/* models */
%token MODEL
%token PLANNING
%token CONFORMANT1
%token CONFORMANT2
%token ND_MDP
%token MDP
%token ND_POMDP1
%token ND_POMDP2
%token POMDP1
%token POMDP2

%token DYNAMICS
%token DETERMINISTIC
%token NON_DETERMINISTIC
%token PROBABILISTIC
%token FEEDBACK
%token COMPLETE
%token PARTIAL
%token NULLF

/* primitive types */
%token INTEGER
%token BOOLEAN
%token ARRAY

/* parentesis and punctuation symbols */
%token LEFTPAR
%token RIGHTPAR
%token LEFTSQPAR
%token RIGHTSQPAR
%token LEFTCURLY
%token RIGHTCURLY
%token COLON
%token STAR
%token HYPHEN
%token COMMA

/* formula symbols */
%token NOT
%token AND
%token OR
%token IF
%token IFF

/* term symbols */
%token EQUAL
%token LT
%token LE
%token GE
%token GT

/* function symbols */
%token PLUS

/* quantifier symbols */
%token EXISTS
%token FORALL
%token VECTOR
%token SUM
%token FOREACH

/* others */
%token INLINE
%token CERTAINTY

/*
** proper tokens: tokens returned by the lexer with yylval values
** associated.
*/
%token TOK_IDENT
%token TOK_VIDENT
%token TOK_INTEGER
%token TOK_FLOAT
%token TOK_BOOLEAN
%token TOK_STRING
%token TOK_FUNCTION
%token TOK_PREDICATE
%token TOK_PROCEDURE

/* token types */
%type   <ident>           TOK_IDENT
%type   <ident>           TOK_VIDENT
%type   <ident>           TOK_INTEGER
%type   <ident>           TOK_FLOAT
%type   <ident>           TOK_BOOLEAN
%type   <ident>           TOK_STRING
%type   <ident>           TOK_FUNCTION
%type   <ident>           TOK_PREDICATE
%type   <ident>           TOK_PROCEDURE

%type   <ident>           LT
%type   <ident>           LE
%type   <ident>           GE
%type   <ident>           GT
%type   <ident>           IN

/* production types */
%type   <codeChunk>       domain
%type   <codeChunk>       problem
%type   <codeChunkList>   code_chunks

%type   <ident>           ancestor
%type   <identList>       ancestor_list

%type   <identList>       ident_list
%type   <identList>       variable_list

%type   <type>            compound_type_decl
%type   <type>            simple_type_decl
%type   <typeList>        simple_type_decl_list
%type   <integer>         integer_type
%type   <integer>         range

%type   <formula>         precondition_formula
%type   <formulaList>     formula_list
%type   <formula>         formula
%type   <boolean>         optional_inline
/*
%type   <formula>         literal_formula
*/
%type   <formula>         atomic_formula
%type   <formula>         optional_assert

%type   <ident>           action_name
%type   <ident>           predicate_symbol
%type   <ident>           primitive_predicate_symbol
%type   <ident>           optional_modulo

%type   <sfunction>       primitive_function_symbol

%type   <termList>        term_list
%type   <term>            term
%type   <term>            optional_subscript
%type   <term>            value

%type   <effect>          effect_list
%type   <effect>          atomic_effect
%type   <effect>          basic_effect
%type   <effect>          non_deterministic_effect
%type   <effect>          init_set_effect
%type   <effect>          atomic_init_effect
%type   <probEffect>      probabilistic_effect
%type   <effectList>      atomic_effect_list
%type   <effectList>      basic_effect_list
%type   <effectList>      non_deterministic_effect_list
%type   <effectList>      atomic_init_effect_list
%type   <probEffectList>  probabilistic_effect_list

%type   <cost>            cost

%type   <observation>     observation
%type   <obsList>         observation_list
%type   <observation>     atomic_observation
%type   <probObs>         probabilistic_observation
%type   <probObsList>     probabilistic_observation_list
%type   <observation>     non_deterministic_observation
%type   <obsListList>     non_deterministic_observation_list

%type   <hook>            hook

%token  BOGUS BOGUS2 BOGUS3 BOGUS4


%%


start             :  code_chunks
                       {
			 globalChunk = $1;
		       }
                  ;

code_chunks       :  code_chunks domain
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  code_chunks problem
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  /* empty */
                       {
			 $$ = new list<const codeChunkClass*>;
		       }
                  ;

domain            :  LEFTPAR DEFINE LEFTPAR PDOMAIN TOK_IDENT RIGHTPAR
                       {
			 startDomain( $5 );
		       }
                     LEFTPAR global_list definition_list ramification_list action_list RIGHTPAR
                       {
			 $$ = finishDomain();
		       }
                  ;

global_list       :  global_list global LEFTPAR
                  |  global LEFTPAR
                  ; 

global            :  MODEL model_def RIGHTPAR
                       {
			 declareModel();
		       }
                  |  REQUIREMENTS requirement_list RIGHTPAR
                  |  TYPES type_list RIGHTPAR
                  |  FUNCTIONS function_list RIGHTPAR
                  |  PREDICATES predicate_list RIGHTPAR
                  |  EXTERNAL external_list RIGHTPAR
                  |  OBJECTS object_list RIGHTPAR
                  ;

model_def         :  model_def dynamics_def
                  |  model_def feedback_def
                  |  /* empty */
                  ;

dynamics_def      :  LEFTPAR DYNAMICS DETERMINISTIC RIGHTPAR
                       {
			 declareDynamics( DETERMINISTIC );
		       }
                  |  LEFTPAR DYNAMICS NON_DETERMINISTIC RIGHTPAR
                       {
			 declareDynamics( NON_DETERMINISTIC );
		       }
                  |  LEFTPAR DYNAMICS PROBABILISTIC RIGHTPAR
                       {
			 declareDynamics( PROBABILISTIC );
		       }
                  ;

feedback_def      :  LEFTPAR FEEDBACK COMPLETE RIGHTPAR
                       {
			 declareFeedback( COMPLETE );
		       }
                  |  LEFTPAR FEEDBACK PARTIAL RIGHTPAR
                       {
			 declareFeedback( PARTIAL );
		       }
                  |  LEFTPAR FEEDBACK NULLF RIGHTPAR
                       {
			 declareFeedback( NULLF );
		       }
                  ;

requirement_list  :  requirement_list requirement
                  |  requirement
                  ;

requirement       :  LEFTPAR PACKAGE TOK_IDENT RIGHTPAR
                       {
			 startPackageParse( $3 );
		       }
                     rest_requirement
                  ;

rest_requirement  :  package
                       {
			 finishPackageParse();
		       }
                  ;

type_list         :  type_list type_def
                  |  type_def
                  ;

type_def          :  TOK_IDENT ancestor_list
                       {
			 $2->push_front( $1 );
			 declareNamedType( strdup( $1 ), $2 );
		       }
                  |  TOK_IDENT
                       {
			 list<const char*> *aList = new list<const char*>;
			 aList->push_front( $1 );
			 declareNamedType( strdup( $1 ), aList );
		       }
                  ;

ancestor_list     :  ancestor_list ancestor
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  ancestor
                       {
			 $$ = new list<const char*>;
			 $$->push_back( $1 );
		       }
                  ;

ancestor          :  COLON COLON TOK_IDENT
                       {
			 $$ = $3;
		       }
                  ;

function_list     :  function_list function
                  |  function
                  ;

function          :  LEFTPAR TOK_IDENT compound_type_decl simple_type_decl RIGHTPAR
                       {
			 declareFunction( $2, $3, $4 );
		       }
                  ;

predicate_list    :  predicate_list predicate
                  |  predicate
                  ;

predicate         :  LEFTPAR TOK_IDENT simple_type_decl_list RIGHTPAR
                       {
			 domainType = predicateClass::LIST_DOMAIN;
			 declarePredicate( $2, new typeListClass( $3 ) );
		       }
                  ;

compound_type_decl:  simple_type_decl
                       {
			 domainType = functionClass::SIMPLE_DOMAIN;
			 $$ = $1;
		       }
                  |  LEFTPAR simple_type_decl_list RIGHTPAR
                       {
			 domainType = functionClass::LIST_DOMAIN;
			 $$ = new typeListClass( $2 );
		       }
                  |  ARRAY LEFTSQPAR TOK_INTEGER RIGHTSQPAR simple_type_decl
                       {
			 // xxxxxx: array in function range 
			 // xxxxxx: not supported
			 domainType = functionClass::ARRAY_DOMAIN;
			 assert( 0 );
		       }
                  ;

simple_type_decl  :  TOK_IDENT
                       {
			 $$ = new simpleTypeClass( $1 );
		       }
                  |  integer_type
                       {
			 $$ = new integerTypeClass( $1.min, $1.max );
		       }
                  |  boolean_type
                       {
			 $$ = new booleanTypeClass();
		       }
                  ;

integer_type      :  INTEGER range
                       {
			 $$ = $2;
		       }
                  ;

range             :  LEFTSQPAR TOK_INTEGER COMMA TOK_INTEGER RIGHTSQPAR
                       {
			 $$.min = $2;
			 $$.max = $4;
		       }
                  ;

boolean_type      :  BOOLEAN
                  ;

simple_type_decl_list
                  :  simple_type_decl_list simple_type_decl
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  simple_type_decl
                       {
			 $$ = new list<const typeClass*>;
			 $$->push_back( $1 );
		       }
                  ;

external_list     :  external_list external_decl
                  |  external_decl
                  ;

external_decl     :  LEFTPAR FUNCTION TOK_IDENT compound_type_decl simple_type_decl RIGHTPAR
                       {
			 declareFunction( $3, $4, $5 );
			 declareExternal( strdup($3) );
		       }
                  |  LEFTPAR PREDICATE TOK_IDENT simple_type_decl_list RIGHTPAR
                       {
			 domainType = predicateClass::LIST_DOMAIN;
			 declarePredicate( $3, new typeListClass( $4 ) );
			 declareExternal( strdup($3) );
                       }
                  |  LEFTPAR PROCEDURE TOK_IDENT RIGHTPAR
                       {
			 declareProcedure( $3 );
		       }
                  ;

object_list       :  object_list object_def
                  |  object_def
                  ;

object_def        :  ident_list HYPHEN TOK_IDENT
                       {
			 declareSimpleObjects( $3, $1 );
		       }
                  |  ident_list HYPHEN integer_type
                       {
			 declareIntegerObjects( $3.min, $3.max, $1 );
                       }
                  |  ident_list HYPHEN boolean_type 
                       {
			 declareBooleanObjects( $1 );
                       }
                  |  ident_list HYPHEN ARRAY LEFTSQPAR TOK_INTEGER RIGHTSQPAR simple_type_decl
                       {
			 declareArrayObjects( $5, $7, $1 );
                       }
                  ;

ident_list        :  ident_list TOK_IDENT
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  TOK_IDENT
                       {
			 $$ = new list<const char*>;
			 $$->push_back( $1 );
		       }
                  ;

definition_list   :  definition_list definition LEFTPAR
                  |  /* empty */
                  ;

definition        :  logical_def
                  ;

logical_def       :  PREDICATE LEFTPAR TOK_IDENT
                       {
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
		       }
                     parameter_list
                       {
			 currInternalPredicate = $3;
			 startLogicalDef( $3 );
		       }
                     RIGHTPAR formula RIGHTPAR
                       {
			 currInternalPredicate = NULL;
			 declareLogicalDef( $3, $8 );
		       }
                  ;

ramification_list :  ramification_def LEFTPAR ramification_list
                  |  /* empty */
                  ;

ramification_def  :  RAMIFICATION TOK_IDENT 
                       {
			 startRamification( $2 );
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
		       }
                     optional_parameter ramification_def_rest
                  ;

ramification_def_rest
                  :  FORMULA formula RIGHTPAR 
                       {
			 declareRamification( $2 );
		       }
                  |  EFFECT atomic_effect_list RIGHTPAR 
                       {
			 declareRamification( new effectListClass( $2 ) );
		       }
                  ;

optional_parameter
                  :  PARAMETERS parameter_list
                  |  /* empty */
                  ;

parameter_list    :  parameter_list parameter
                  |  parameter
                  ;

parameter         :  variable_list HYPHEN simple_type_decl
                       {
			 list<const char*>::const_iterator it;
			 for( it = $1->begin(); it != $1->end(); ++it )
			   {
			     declareVariable( *it, $3->clone() );
			     parameterList.push_back( make_pair( strdup(*it), $3->clone() ) );
			   }
			 delete $1;
			 delete $3;
		       }
                  ;

variable_list     :  variable_list TOK_VIDENT
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  TOK_VIDENT
                       {
			 $$ = new list<const char*>;
			 $$->push_back( $1 );
		       }
                  ;

action_list       :  action_list LEFTPAR action_def
                  |  action_def
                  ;

action_def        :  ACTION action_name 
                       {
			 startAction( $2 );
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
		       }
                     action_body_list RIGHTPAR
                       {
			 finishAction();
		       }
                  ;

action_name       :  TOK_IDENT
                  ;

action_body_list  :  action_body_list action_body
                  |  /* empty */
                  ;

action_body       :  PARAMETERS parameter_list
                  |  PRECONDITION precondition_formula
                       {
			 declareActionPrecondition( $2 );
		       }
                  |  EFFECT
                       {
			 insideEffect = true;
		       }
                     effect_list
                       {
			 insideEffect = false;
			 declareActionEffect( $3 );
		       }
                  |  COST cost
                       {
			 declareActionCost( $2 );
			 if( declaredDomain->model == PLANNING )
			   warning( featureInModelWarning, ":cost" );
		       }
                  |  OBSERVATION observation
                       {
			 declareActionObservation( $2 );
			 if( (declaredDomain->model != ND_POMDP1) &&
			     (declaredDomain->model != ND_POMDP2) &&
			     (declaredDomain->model != POMDP1) &&
			     (declaredDomain->model != POMDP2) )
			   error( featureInModel, declaredDomain->model );
		       }
                  ;

precondition_formula
                  :  formula
                  ;

formula           :  atomic_formula
                  |  LEFTPAR NOT formula RIGHTPAR
                       {
			 $$ = new notFormulaClass( $3 );
		       }
                  |  LEFTPAR AND formula_list RIGHTPAR
                       {
			 $$ = new andFormulaClass( $3 );
		       }
                  |  LEFTPAR OR formula_list RIGHTPAR
                       {
			 $$ = new orFormulaClass( $3 );
		       }
                  |  LEFTPAR IF formula formula RIGHTPAR
                       {
			 $$ = new ifFormulaClass( $3, $4 );
		       }
                  |  LEFTPAR IFF formula formula RIGHTPAR
                       {
			 $$ = new iffFormulaClass( $3, $4 );
		       }
                  |  LEFTPAR EXISTS TOK_VIDENT HYPHEN TOK_IDENT optional_inline
                       {
			 // if variable already bounded, declare error
			 list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
			 for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
			   if( (*it)->find( $3 ) != (*it)->end() )
			     error( nameClash, $3 );

			 // extend current environment
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
			 declareVariable( strdup($3), new simpleTypeClass( strdup($5) ) );
		       } 
                     formula RIGHTPAR
                       {
			 $$ = new existsFormulaClass( $3, new simpleTypeClass($5), $8, environmentClone(), currInternalPredicate, insideEffect, $6 );
			 cleanEnvironment( variableEnvironment.front() );
			 variableEnvironment.pop_front();
		       }
                  |  LEFTPAR FORALL TOK_VIDENT HYPHEN TOK_IDENT optional_inline
                       {
			 // if variable already bounded, declare error
			 list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
			 for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
			   if( (*it)->find( $3 ) != (*it)->end() )
			     error( nameClash, $3 );

			 // extend current environment
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
			 declareVariable( strdup($3), new simpleTypeClass( strdup($5) ) );
		       } 
                     formula RIGHTPAR
                       {
			 $$ = new forallFormulaClass( $3, new simpleTypeClass($5), $8, environmentClone(), currInternalPredicate, insideEffect, $6 );
			 cleanEnvironment( variableEnvironment.front() );
			 variableEnvironment.pop_front();
		       }
                  ;

optional_inline   :  INLINE
                       {
			 $$ = true;
		       }
                  |
                       {
			 $$ = false;
		       }
                  ;

formula_list      :  formula_list formula
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  formula
                       {
			 $$ = new list<const formulaClass*>;
			 $$->push_back( $1 );
		       }
                  ;

/*
literal_formula   :  atomic_formula
                  |  LEFTPAR NOT atomic_formula RIGHTPAR
                       {
			 $$ = new notFormulaClass( $3 );
		       }
                  ;
*/

atomic_formula    :  LEFTPAR predicate_symbol term_list RIGHTPAR
                       {
			 $$ = new atomFormulaClass( $2, new termListClass( $3 ) );
		       }
                  |  LEFTPAR EQUAL term term RIGHTPAR
                       {
			 $$ = new equalFormulaClass( $3, $4 );
		       }
                  |  LEFTPAR IN term LEFTCURLY term_list RIGHTCURLY RIGHTPAR
                       {
			 $$ = new inclusionFormulaClass( $3, $5 );
		       }
                  ;

predicate_symbol  :  primitive_predicate_symbol
                  |  TOK_PREDICATE
                  ;

primitive_predicate_symbol
                  :  LT
                  |  LE
                  |  GE
                  |  GT
                  ;

term_list         :  term_list term
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  /* empty */
                       {
			 $$ = new list<const termClass*>;
		       }
                  ;

term              :  TOK_VIDENT
                       {
			 $$ = fetchVariableTerm( $1, NULL );
		       }
                  |  TOK_IDENT optional_subscript
                       {
			 $$ = fetchObjectTerm( $1, $2 );
		       }
                  |  value
                  |  LEFTPAR TOK_FUNCTION term_list RIGHTPAR
                       {
			 $$ = new functionTermClass( $2, new termListClass( $3 ) );
		       }
                  |  LEFTPAR primitive_function_symbol term_list RIGHTPAR
                       {
			 $$ = new functionTermClass( $2, new termListClass( $3 ) );
		       }
                  |  STAR term
                       {
			 if( !insideEffect )
			   error( badStarTerm );
			 $$ = new starTermClass( $2, insideEffect );
                       }
                  |  LEFTPAR SUM TOK_VIDENT HYPHEN TOK_IDENT optional_inline
                       {
			 // if variable already bounded, declare error
			 list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
			 for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
			   if( (*it)->find( $3 ) != (*it)->end() )
			     error( nameClash, $3 );

			 // extend current environment
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
			 declareVariable( strdup($3), new simpleTypeClass( strdup($5) ) );
		       } 
                     term RIGHTPAR
                       {
			 $$ = new sumTermClass($3,new simpleTypeClass($5),$8,environmentClone(),insideEffect,$6);
			 cleanEnvironment( variableEnvironment.front() );
			 variableEnvironment.pop_front();
		       }
                  |  LEFTPAR IF formula term term RIGHTPAR
                       {
			 $$ = new conditionalTermClass( $3, $4, $5 );
                       }
                  |  LEFTPAR FORMULA formula RIGHTPAR
                       {
			 $$ = new formulaTermClass( $3 );
                       }
                  ;

optional_subscript
                  :  LEFTSQPAR term RIGHTSQPAR
                       {
			 $$ = $2;
		       }
                  |  /* empty */
                       {
			 $$ = NULL;
		       }
                  ;

value             :  TOK_INTEGER
                       {
			 $$ = new integerTermClass( $1 );
		       }
                  |  TOK_BOOLEAN
                       {
			 $$ = new booleanTermClass( $1 );
		       }
                  ;

primitive_function_symbol
                  :  PLUS optional_modulo
                       {
			 list<const termClass*> *extra = new list<const termClass*>;
			 extra->push_back( new integerTermClass( $2 ) );
			 $$ = new specialFunctionClass( strdup("_plus"), 
                                                        plusFunctionDomain->clone(), 
                                                        plusFunctionRange->clone(),
							new termListClass( extra ) );
		       }
                  |  HYPHEN optional_modulo
                       {
			 list<const termClass*> *extra = new list<const termClass*>;
			 extra->push_back( new integerTermClass( $2 ) );
			 $$ = new specialFunctionClass( strdup("_minus"), 
                                                        minusFunctionDomain->clone(), 
                                                        minusFunctionRange->clone(),
							new termListClass( extra ) );
		       }
                  ;

optional_modulo   :  LT TOK_INTEGER GT
                       {
			 $$ = $2;
		       }
                  |  /* empty */
                       {
			 $$ = strdup("-1");
		       }
                  ;

effect_list       :  atomic_effect_list
                       {
			 $$ = new effectListClass( $1 );
		       }
                  |  LEFTPAR PROBABILISTIC probabilistic_effect_list RIGHTPAR
                       {
			 probabilisticEffectClass *result = new probabilisticEffectClass( $3 );
			 $$ = result;
			 declaredAction->numResultingStates = $3->size();
			 if( !checkProbabilities( result ) )
			   ++declaredAction->numResultingStates;
			 if( (declaredDomain->model != MDP) &&
			     (declaredDomain->model != POMDP2) &&
			     (declaredDomain->model != CONFORMANT2) )
			   error( featureInModel, declaredDomain->model );
		       }
                  |  LEFTPAR NON_DETERMINISTIC non_deterministic_effect_list RIGHTPAR
                       {
			 $$ = new nonDeterministicEffectClass( $3 );
			 declaredAction->numResultingStates = $3->size();
			 if( (declaredDomain->model != ND_MDP) &&
			     (declaredDomain->model != ND_POMDP2) &&
			     (declaredDomain->model != CONFORMANT1) )
			   error( featureInModel, declaredDomain->model );
		       }
                  ;

atomic_effect_list
                  :  atomic_effect_list atomic_effect
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  atomic_effect
                       {
			 $$ = new list<const effectClass*>;
			 $$->push_back( $1 );
		       }
                  ;

atomic_effect     :  basic_effect
                  |  LEFTPAR WHEN formula basic_effect_list RIGHTPAR
                       {
			 $$ = new whenEffectClass( $3, new effectListClass( $4 ) );
		       }
                  |  LEFTPAR FOREACH TOK_VIDENT HYPHEN TOK_IDENT
                       {
			 // if variable already bounded, declare error
			 list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
			 for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
			   if( (*it)->find( $3 ) != (*it)->end() )
			     error( nameClash, $3 );

			 // extend current environment
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
			 declareVariable( strdup($3), new simpleTypeClass( strdup($5) ) );
		       }
                     atomic_effect_list RIGHTPAR
                       {
			 $$ = new forEachEffectClass($3,new simpleTypeClass($5),new effectListClass($7));
			 cleanEnvironment( variableEnvironment.front() );
			 variableEnvironment.pop_front();
		       }
                  ;

basic_effect_list :  basic_effect_list basic_effect
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  basic_effect
                       {
			 $$ = new list<const effectClass*>;
			 $$->push_back( $1 );
		       }
                  ;

basic_effect      :  LEFTPAR SET term term RIGHTPAR
                       {
			 $$ = new termSetEffectClass( $3, $4 );

			 /*
			 ** to get information about dirty functions, we use RTTI.
			 ** I don't like that but now it seems a good reason for it ..
			 */
			 const functionTermClass *fun = dynamic_cast<const functionTermClass*>($3);
			 if( fun != NULL )
			   {
			     if( declaredAction )
			       declaredAction->dirtyFunctions.insert( strdup(fun->function->ident) );
			     if( currentRamification )
			       ramDirtyFunctions.insert( strdup(fun->function->ident) );
			   }

			 /*
			 ** similar for detecting objects with changing denotation
			 */
			 const objectTermClass *obj = dynamic_cast<const objectTermClass*>($3);
			 if( obj != NULL )
			   setVariableDenotationFor( obj->ident );
			 const variableTermClass *var = dynamic_cast<const variableTermClass*>($3);
			 if( var != NULL )
			   error( invalidLValue );
		       }
                  |  LEFTPAR SET LEFTPAR TOK_PREDICATE term_list RIGHTPAR term RIGHTPAR
                       {
			 if( declaredAction ) declaredAction->dirtyPredicates.insert( strdup($4) );
			 if( currentRamification ) ramDirtyPredicates.insert( strdup($4) );
			 $$ = new predicateSetEffectClass( $4, new termListClass( $5 ), $7 );
		       }
                  |  LEFTPAR SET LEFTPAR TOK_PREDICATE term_list RIGHTPAR formula RIGHTPAR
                       {
			 if( declaredAction ) declaredAction->dirtyPredicates.insert( strdup($4) );
			 if( currentRamification ) ramDirtyPredicates.insert( strdup($4) );
			 $$ = new predicateSetEffectClass( $4, new termListClass( $5 ), $7 );
		       }
                  |  LEFTPAR PRINT term RIGHTPAR
                       {
			 $$ = new printEffectClass( $3 );
		       }
                  |  LEFTPAR PRINT formula RIGHTPAR
                       {
			 $$ = new printEffectClass( $3 );
		       }
                  |  LEFTPAR PRINTSTATE RIGHTPAR
                       {
			 $$ = new printStateEffectClass();
		       }
                  |  LEFTPAR TOK_PROCEDURE RIGHTPAR
                       {
			 $$ = new procedureEffectClass( $2 );
		       }
                  ;

init_set_effect   :  LEFTPAR SET term IN LEFTCURLY term_list RIGHTCURLY optional_assert RIGHTPAR
                       {
			 const functionTermClass *f = dynamic_cast<const functionTermClass*>( $3 );
			 if( f != NULL ) declaredProblem->dirtyFunctions.insert( strdup(f->function->ident) );
			 $$ = new initTermSetEffectClass( $3, $6, $8 );

			 const objectTermClass *obj = dynamic_cast<const objectTermClass*>($3);
			 if( obj != NULL )
			   setVariableDenotationFor( obj->ident );
		       }
                  |  LEFTPAR SET term IN integer_type optional_assert RIGHTPAR
                       {
			 const functionTermClass *f = dynamic_cast<const functionTermClass*>( $3 );
			 if( f != NULL ) declaredProblem->dirtyFunctions.insert( strdup(f->function->ident) );
			 $$ = new initTermSetEffectClass( $3, $5.min, $5.max, $6 );

			 const objectTermClass *obj = dynamic_cast<const objectTermClass*>($3);
			 if( obj != NULL )
			   setVariableDenotationFor( obj->ident );
		       }
                  |  LEFTPAR SET LEFTPAR TOK_PREDICATE term_list RIGHTPAR IN 
                                 LEFTCURLY term_list RIGHTCURLY optional_assert RIGHTPAR
                       {
			 $$ = new initPredicateSetEffectClass( $4, new termListClass( $5 ), $9, $11 );
			 declaredProblem->dirtyPredicates.insert( strdup($4) );
		       }
                  |  LEFTPAR ONEOF atomic_init_effect_list RIGHTPAR
                       {
			 $$ = new oneOfEffectClass( $3 );
		       }
                  |  formula
                       {
			 $$ = new formulaInitEffectClass( $1 );
		       }
                  ;

optional_assert   :  ASSERT formula
                       {
			 $$ = $2;
		       }
                  |  /* empty */
                       {
			 $$ = NULL;
		       }
                  ;

probabilistic_effect_list
                  :  probabilistic_effect_list probabilistic_effect
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  probabilistic_effect
                       {
			 $$ = new list<pair<const char*,const effectClass*>*>;
			 $$->push_back( $1 );
		       }
                  ;

probabilistic_effect
                  :  LEFTPAR TOK_FLOAT atomic_effect_list RIGHTPAR
                       {
			 $$ = new pair<const char*,const effectClass*>($2,new effectListClass( $3 ));
		       }
                  ;

non_deterministic_effect_list
                  :  non_deterministic_effect_list non_deterministic_effect
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  non_deterministic_effect
                       {
			 $$ = new list<const effectClass*>;
			 $$->push_back( $1 );
		       }
                  ;

non_deterministic_effect
                  :  LEFTPAR atomic_effect_list RIGHTPAR
                       {
			 $$ = new effectListClass( $2 );
		       }
                  ;

cost              :  term
                       {
			 $$ = new termCostClass( $1 );
		       }
/*
                  |  LEFTPAR IF formula cost cost RIGHTPAR
                       {
			 $$ = new conditionalCostClass( $3, $4, $5 );
                       }
                  |  LEFTPAR IF formula cost RIGHTPAR
                       {
			 $$ = new conditionalCostClass( $3, $4, new constantCostClass( strdup("1.0") ) );
                       }
                  |  LEFTPAR SUM TOK_VIDENT HYPHEN TOK_IDENT
                       {
			 // if variable already bounded, declare error
			 list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
			 for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
			   if( (*it)->find( $3 ) != (*it)->end() )
			     error( nameClash, $3 );

			 // extend current environment
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
			 declareVariable( strdup($3), new simpleTypeClass( strdup($5) ) );
		       } 
                     cost RIGHTPAR
                       {
			 $$ = new sumCostClass( $3, new simpleTypeClass( $5 ), $7 );
			 cleanEnvironment( variableEnvironment.front() );
			 variableEnvironment.pop_front();
		       }
*/
                  ;

observation
                  :  observation_list
                       {
			 $$ = new observationListClass( $1 );
		       }
                  |  LEFTPAR PROBABILISTIC probabilistic_observation_list RIGHTPAR
                       {
			 checkProbabilities( $3 );
			 if( declaredDomain->model != POMDP2 )
			   error( featureInModel, declaredDomain->model );
			 $$ = new probabilisticObservationClass( $3 );
		       }
                  |  LEFTPAR NON_DETERMINISTIC non_deterministic_observation_list RIGHTPAR
                       {
			 if( (declaredDomain->model != ND_POMDP1) ||
			     (declaredDomain->model != ND_POMDP2) )
			   error( featureInModel, declaredDomain->model );
			 $$ = new nonDeterministicObservationClass( $3 );
		       }
                  ;

observation_list  :  observation_list atomic_observation
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  atomic_observation
                       {
			 $$ = new list<const observationClass*>;
			 $$->push_back( $1 );
		       }
                  ;

atomic_observation
                  :  formula
                       {
			 $$ = new formulaObservationClass( $1 );
		       }
                  |  term
                       {
			 $$ = new termObservationClass( $1 );
		       }
                  |  LEFTPAR WHEN formula formula RIGHTPAR
                       {
			 $$ = new conditionalFormulaObservationClass( $3, $4 );
		       }
                  |  LEFTPAR WHEN formula term RIGHTPAR
                       {
			 $$ = new conditionalTermObservationClass( $3, $4 );
		       }
                  |  LEFTPAR VECTOR TOK_VIDENT HYPHEN TOK_IDENT
                       {
			 // if variable already bounded, declare error
			 list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
			 for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
			   if( (*it)->find( $3 ) != (*it)->end() )
			     error( nameClash, $3 );

			 // extend current environment
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
			 declareVariable( strdup($3), new simpleTypeClass( strdup($5) ) );
		       } 
                     term RIGHTPAR
                       {
			 $$ = new vectorObservationClass( $3, new simpleTypeClass( $5 ), $7, environmentClone() );
			 cleanEnvironment( variableEnvironment.front() );
			 variableEnvironment.pop_front();
		       }
                  ;

probabilistic_observation_list
                  :  probabilistic_observation_list probabilistic_observation
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  probabilistic_observation
                       {
			 $$ = new list<pair<const char*,const observationListClass*>*>;
			 $$->push_back( $1 );
		       }
                  ;

probabilistic_observation
                  :  LEFTPAR TOK_FLOAT observation_list RIGHTPAR
                       {
			 $$ = new pair<const char*,const observationListClass*>($2,new observationListClass($3));
		       }
                  ;

non_deterministic_observation_list
                  :  non_deterministic_observation_list non_deterministic_observation
                       {
			 $$ = $1;
			 $$->push_back( (const observationListClass*)$2 );
		       }
                  |  non_deterministic_observation
                       {
			 $$ = new list<const observationListClass*>;
			 $$->push_back( (const observationListClass*)$1 );
		       }
                  ;

non_deterministic_observation
                  :  LEFTPAR observation_list RIGHTPAR
                       {
			 $$ = new observationListClass( $2 );
		       }
                  ;


/*********************************************************************************
** Problem Definition
**/


problem           :  LEFTPAR DEFINE LEFTPAR PROBLEM TOK_IDENT RIGHTPAR 
                     LEFTPAR CDOMAIN TOK_IDENT RIGHTPAR
                       {
			 startProblem( $5, $9 );
		       }
                     problem_def_list RIGHTPAR
                       {
			 $$ = finishProblem();
			 
			 /* can't wait to the end for code generation */
			 generateCode( (problemClass*)$$ );
		       }
                  ;

problem_def_list  :  problem_def_list problem_def
                  |  /* empty */
                  ; 

problem_def       :  LEFTPAR INIT init_def RIGHTPAR
                  |  LEFTPAR GOAL goal_def RIGHTPAR
                  |  LEFTPAR OBJECTS object_list RIGHTPAR
                  |  LEFTPAR HOOKS hook_list RIGHTPAR
                  ;

init_def          :  atomic_init_effect_list
                       {
			 declareInit( new effectListClass( $1 ) );
		       }
                  ;

goal_def          :  formula
                       {
			 declareGoal( $1 );
		       }
                  |  CERTAINTY
                       {
			 declareGoal( new certaintyGoalClass );
		       }
                  ;

atomic_init_effect_list
                  :  atomic_init_effect_list atomic_init_effect
                       {
			 $$ = $1;
			 $$->push_back( $2 );
		       }
                  |  atomic_init_effect
                       {
			 $$ = new list<const effectClass*>;
			 $$->push_back( $1 );
		       }
                  ;

atomic_init_effect
                  :  basic_effect
                  |  init_set_effect
                  |  LEFTPAR WHEN formula atomic_init_effect_list RIGHTPAR
                       {
			 $$ = new whenEffectClass( $3, new effectListClass( $4 ) );
		       }
                  |  LEFTPAR FOREACH TOK_VIDENT HYPHEN TOK_IDENT
                       {
			 // if variable already bounded, declare error
			 list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
			 for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
			   if( (*it)->find( $3 ) != (*it)->end() )
			     error( nameClash, $3 );

			 // extend current environment
			 variableEnvironment.push_front( new map<const char*,const typeClass*,ltstr> );
			 declareVariable( strdup($3), new simpleTypeClass( strdup($5) ) );
		       }
                     atomic_init_effect_list RIGHTPAR
                       {
			 $$ = new forEachEffectClass($3,new simpleTypeClass($5),new effectListClass($7));
			 cleanEnvironment( variableEnvironment.front() );
			 variableEnvironment.pop_front();
		       }
                  ;

hook_list         :  hook_list hook
                       {
			 declareHook( $2 );
		       }
                  |  hook
                       {
			 declareHook( $1 );
		       }
                  ;

hook              :  LEFTPAR MODEL TOK_STRING RIGHTPAR
                       {
			 $$ = new hookClass( problemClass::MODEL_HOOK, $3 );
		       }
                  |  LEFTPAR BELIEF TOK_STRING RIGHTPAR
                       {
			 $$ = new hookClass( problemClass::BELIEF_HOOK, $3 );
		       }
                  |  LEFTPAR HEURISTIC TOK_STRING RIGHTPAR
                       {
			 $$ = new hookClass( problemClass::HEURISTIC_HOOK, $3 );
		       }
                  ;


/*********************************************************************************
** Package Processing
**/


package           :  LEFTPAR PACKAGE TOK_IDENT
                       {
			 startPackage( $3 );
		       }
                     rest_package
                       {
			 finishPackage();
		       }
                  ;

rest_package      :  RIGHTPAR


%%


//
// Function used by YACC to print syntax errors.
//
void
yyerror( const char *s )
{
  extern int lineno;
#ifdef sparc
  extern char yytext[];      // needed in my g++/sparc
#endif
#ifdef linux
  extern char *yytext;       // needed in my g++/linux
#endif

  insertFmtError( "%s:%d: %s before \"%s\"\n", basename( yyfile ), lineno, s, yytext );
}

//
// Initialization of parsing process. We allocate primitive types and 
// predicates, create basic hash tables, and initialize the package 
// processing facility. This function must be called just once.
//
void
initParser( void )
{
  list<const typeClass*> *domain;

  // declared objects
  declaredDomain = NULL;
  declaredProblem = NULL;
  declaredAction = NULL;
  variableEnvironment.clear();
  parameterList.clear();

  actionSet.clear();
  ramificationSet.clear();
  internalPredicateSet.clear();

  // constant types
  theBooleanType = new booleanTypeClass();
  theGenericIntegerType = new genericIntegerTypeClass();

  // hashs
  domainHash.clear();
  problemSet.clear();
  globalFunctionHash.clear();
  globalPredicateHash.clear();
  globalNamedTypeHash.clear();

  // primitive functions
  domain = new list<const typeClass*>;
  domain->push_back( new genericIntegerTypeClass );
  domain->push_back( new genericIntegerTypeClass );
  plusFunctionDomain = new typeListClass( domain );
  plusFunctionRange = new genericIntegerTypeClass;
  minusFunctionDomain = (typeListClass*)plusFunctionDomain->clone();
  minusFunctionRange = new genericIntegerTypeClass;

  // primitive predicates
  theLessThanPredicate = new predicateClass( strdup("_lessThan"), plusFunctionDomain->clone(), 
					     predicateClass::SIMPLE_DOMAIN, 1 );
  theLessEqualPredicate = new predicateClass( strdup("_lessEqual"), plusFunctionDomain->clone(), 
					      predicateClass::SIMPLE_DOMAIN, 1 );
  theGreaterEqualPredicate = new predicateClass( strdup("_greaterEqual"), plusFunctionDomain->clone(),
						 predicateClass::SIMPLE_DOMAIN, 1 );
  theGreaterThanPredicate = new predicateClass( strdup("_greaterThan"), plusFunctionDomain->clone(),
						predicateClass::SIMPLE_DOMAIN, 1 );
  globalPredicateHash.insert( make_pair( (const char*)"_lessThan", theLessThanPredicate ) );
  globalPredicateHash.insert( make_pair( (const char*)"_lessEqual", theLessEqualPredicate ) );
  globalPredicateHash.insert( make_pair( (const char*)"_greaterEqual", theGreaterEqualPredicate ) );
  globalPredicateHash.insert( make_pair( (const char*)"_greaterThan", theGreaterThanPredicate ) );

  domain = new list<const typeClass*>;
  domain->push_back( new universalTypeClass() );
  domain->push_back( new universalTypeClass() );
  theEqualPredicate = new predicateClass( strdup("="), new typeListClass( domain ), 
					  predicateClass::SIMPLE_DOMAIN );
  globalPredicateHash.insert( make_pair( (const char*)"=", theEqualPredicate ) );

  // primitive types
  globalNamedTypeHash.insert( make_pair( (const char*)":integer", (const namedTypeClass*)NULL ) );
  globalNamedTypeHash.insert( make_pair( (const char*)":boolean", (const namedTypeClass*)NULL ) );

  // packages
  initPackages();
}

static void
startDomain( const char *ident )
{
  map<const char*,const functionClass*,ltstr>::const_iterator it1;
  map<const char*,const predicateClass*,ltstr>::const_iterator it2;
  map<const char*,const namedTypeClass*,ltstr>::const_iterator it3;

  if( domainHash.find( ident ) != domainHash.end() )
    {
      error( nameClash, ident );
      return;
    }

  // allocate domain
  declaredDomain = new domainClass( strdup( yyfile ), ident );

  // default hashes
  for( it1 = globalFunctionHash.begin(); it1 != globalFunctionHash.end(); ++it1 )
    declaredDomain->functionHash.insert( make_pair( (*it1).first, new functionClass( *(*it1).second ) ) );

  for( it2 = globalPredicateHash.begin(); it2 != globalPredicateHash.end(); ++it2 )
    declaredDomain->predicateHash.insert( make_pair( (*it2).first, new predicateClass( *(*it2).second ) ) );

  for( it3 = globalNamedTypeHash.begin(); it3 != globalNamedTypeHash.end(); ++it3 )
    if( (*it3).second )
      declaredDomain->namedTypeHash.insert( make_pair((*it3).first,new namedTypeClass(*(*it3).second)) );
    else
      declaredDomain->namedTypeHash.insert( make_pair( (*it3).first, (const namedTypeClass*)NULL ) );

  // setup global hashes
  namedTypeHash = &declaredDomain->namedTypeHash;
  objectHashByType = &declaredDomain->objectHashByType;
  objectHashByName = &declaredDomain->objectHashByName;
  ramificationList = &declaredDomain->ramificationList;
  internalPredicateList = &declaredDomain->internalPredicateList;
  actionList = &declaredDomain->actionList;
  functionHash = &declaredDomain->functionHash;
  predicateHash = &declaredDomain->predicateHash;
  arrayHash = &declaredDomain->arrayHash;
  externalProcedureList = &declaredDomain->externalProcedureList;
}

static const codeChunkClass *
finishDomain( void )
{
  // update model to appropriate
  if( (declaredDomain->model == ND_POMDP2) && (maxNumResStates == 1) )
    declaredDomain->model = ND_POMDP1;

  // save domain in global hash table
  domainHash.insert( make_pair( declaredDomain->ident, declaredDomain ) );

  actionSet.clear();
  ramificationSet.clear();
  internalPredicateSet.clear();

  // cleanning
  namedTypeHash = NULL;
  objectHashByType = NULL;
  objectHashByName = NULL;
  ramificationList = NULL;
  internalPredicateList = NULL;
  actionList = NULL;
  functionHash = NULL;
  predicateHash = NULL;
  arrayHash = NULL;
  externalProcedureList = NULL;
  domainClass *rv = declaredDomain;
  declaredDomain = NULL;
  return( rv );
}

void
cleanDomains( void )
{
  map<const char*,const domainClass*,ltstr>::const_iterator it;

  map<const char*,const functionClass*,ltstr>::const_iterator it1;
  map<const char*,const predicateClass*,ltstr>::const_iterator it2;
  map<const char*,const namedTypeClass*,ltstr>::const_iterator it3;
  map<const char*,list<const char*>*,ltstr>::const_iterator it4;
  map<const char*,const typeClass*,ltstr>::const_iterator it5;
  map<const char*,const arrayClass*,ltstr>::const_iterator it6;
  list<const ramificationClass*>::const_iterator it7;
  list<const internalFunctionClass*>::const_iterator it8;
  list<const internalPredicateClass*>::const_iterator it9;
  list<const actionClass*>::const_iterator it10;

  for( it = domainHash.begin(); it != domainHash.end(); ++it )
    {
      //      fprintf( stderr, "free of [%p]%s\n", ((*it).second->ident), ((*it).second->ident) );
      //      fprintf( stderr, "free of [%p]%s\n", ((*it).second->fileName), ((*it).second->fileName) );
      free( (void*)((*it).second->ident) );
      free( (void*)((*it).second->fileName) );

      for( it1 = (*it).second->functionHash.begin(); it1 != (*it).second->functionHash.end(); ++it1 );

      // clean predicateHash
      for( it2 = (*it).second->predicateHash.begin(); it2 != (*it).second->predicateHash.end(); ++it2 )
	delete (*it2).second;

      // clean namedTypeHash
      for( it3 = (*it).second->namedTypeHash.begin(); it3 != (*it).second->namedTypeHash.end(); ++it3 )
	delete (*it3).second;

      // clean objectHashByType
      for( it4 = (*it).second->objectHashByType.begin(); it4 != (*it).second->objectHashByType.end(); ++it4 )
	{
	  list<const char*>::const_iterator it;
	  //	  fprintf( stderr, "free of [%p]%s\n", (*it4).first, (*it4).first );
	  free( (void*)((*it4).first) );
	  for( it = (*it4).second->begin(); it != (*it4).second->end(); ++it )
	    {
	      //	      fprintf( stderr, "free of [%p]%s\n", (*it), (*it) );
	      free( (void*)(*it) );
	    }
	  delete (*it4).second;
	}

      // clean objectHashByName
      for( it5 = (*it).second->objectHashByName.begin(); it5 != (*it).second->objectHashByName.end(); ++it5 )
	{
	  //	  fprintf( stderr, "free of [%p]%s\n", (*it5).first, (*it5).first );
	  free( (void*)(*it5).first );
	  delete (*it5).second;
	}

      // clean arrayHash
      for( it6 = (*it).second->arrayHash.begin(); it6 != (*it).second->arrayHash.end(); ++it6 )
	{
	  //	  fprintf( stderr, "free of [%p]%s\n", (*it6).first, (*it6).first );
	  free( (void*)(*it6).first );
	  delete (*it6).second;
	}

      // clean ramification
      for( it7 = (*it).second->ramificationList.begin(); it7 != (*it).second->ramificationList.end(); ++it7 )
	delete (*it7);

      // clean internalFunctions
      for( it8 = (*it).second->internalFunctionList.begin(); it8 != (*it).second->internalFunctionList.end(); ++it8 )
	delete (*it8);

      // clean internalPredicates
      for( it9 = (*it).second->internalPredicateList.begin(); it9 != (*it).second->internalPredicateList.end(); ++it9 )
	delete (*it9);

      // clean actions
      for( it10 = (*it).second->actionList.begin(); it10 != (*it).second->actionList.end(); ++it10 )
	delete (*it10);
    }
}

static void
declareModel( void )
{
  assert( declaredDomain != NULL );
  switch( declaredDomain->feedback )
    {
    case COMPLETE:
      switch( declaredDomain->dynamics )
	{
	case DETERMINISTIC:
	case NON_DETERMINISTIC:
	  declaredDomain->model = ND_MDP;
	  break;
	case PROBABILISTIC:
	  declaredDomain->model = MDP;
	  break;
	}
      break;
    case PARTIAL:
      switch( declaredDomain->dynamics )
	{
	case DETERMINISTIC:
	  declaredDomain->model = POMDP1;
	  break;
	case NON_DETERMINISTIC:
	  declaredDomain->model = ND_POMDP2;
	  break;
	case PROBABILISTIC:
	  declaredDomain->model = POMDP2;
	  break;
	}
      break;
    case NULLF:
      switch( declaredDomain->dynamics )
	{
	case DETERMINISTIC:
	case PROBABILISTIC:
	  declaredDomain->model = CONFORMANT2;
	  break;
	case NON_DETERMINISTIC:
	  declaredDomain->model = CONFORMANT1;
	  break;
	}
      break;
    }
}

static void
declareDynamics( int dynamics )
{
  assert( declaredDomain != NULL );
  if( (dynamics != PROBABILISTIC) && (dynamics != NON_DETERMINISTIC) && (dynamics != DETERMINISTIC) )
    error( undefinedModel );
  else
    declaredDomain->dynamics = dynamics;
}

static void
declareFeedback( int feedback )
{
  assert( declaredDomain != NULL );
  if( (feedback != COMPLETE) && (feedback != PARTIAL) && (feedback != NULLF) )
    error( undefinedModel );
  else
    declaredDomain->feedback = feedback;
}

static void
declareExternal( const char* ident )
{
  map<const char*,const functionClass*,ltstr>::iterator it1;
  map<const char*,const predicateClass*,ltstr>::iterator it2;
  it1 = declaredDomain->functionHash.find( ident );
  it2 = declaredDomain->predicateHash.find( ident );

  if( (it1==declaredDomain->functionHash.end()) && (it2==declaredDomain->predicateHash.end()) )
    error( undefinedFunPred, ident );

  if( it1 != declaredDomain->functionHash.end() )
    {
      if( (*it1).second->external == 0 )
	{
	  ((functionClass*)(*it1).second)->external = 1;
	  --declaredDomain->numFunctions[(*it1).second->numArguments];
	}
    }

  if( it2 != declaredDomain->predicateHash.end() )
    {
      if( (*it2).second->external == 0 )
	{
	  ((predicateClass*)(*it2).second)->external = 1;
	  --declaredDomain->numPredicates[(*it2).second->numArguments];
	}
    }

  free( (void*)ident );
}

static void
declareExternal( const list<const char*> *eList )
{
  assert( declaredDomain != NULL );
  list<const char*>::const_iterator it;
  for( it = eList->begin(); it != eList->end(); ++it )
    declareExternal( *it );
  delete eList;
}

static void
declareNamedType( const char *ident, const list<const char*> *aList )
{
  assert( namedTypeHash != NULL );
  if( namedTypeHash->find( ident ) != namedTypeHash->end() )
    {
      error( nameClash, ident );
    }
  else
    {
      namedTypeClass *type = new namedTypeClass( ident, aList );
      namedTypeHash->insert( make_pair( ident, type ) );
    }
}

static void
declareFunction( const char *ident, const typeClass *domain, const typeClass *range )
{
  functionClass *function;

  assert( functionHash != NULL );
  assert( declaredDomain != NULL );

  if( functionHash->find( ident ) != functionHash->end() )
    {
      error( nameClash, ident );
    }
  else
    {
      function = new functionClass( ident, domain, range, domainType );
      functionHash->insert( make_pair( ident, function ) );
      ++declaredDomain->numFunctions[function->numArguments];
    }
}

static void
declarePredicate( const char *ident, const typeClass *domain )
{
  predicateClass *predicate;

  assert( predicateHash != NULL );
  assert( declaredDomain != NULL );

  if( predicateHash->find( ident ) != predicateHash->end() )
    {
      error( nameClash, ident );
    }
  else
    {
      predicate = new predicateClass( ident, domain, domainType );
      predicateHash->insert( make_pair( ident, predicate ) );
      ++declaredDomain->numPredicates[predicate->numArguments];
    }
}

static void
declareProcedure( const char *ident )
{
  assert( externalProcedureList != NULL );
  if( externalProcedureList->find( ident ) != externalProcedureList->end() )
    error( nameClash, ident );
  else
    externalProcedureList->insert( ident );
}

static void
declareSimpleObjects( const char *ident, const list<const char*> *oList )
{
  typeClass *type;
  list<const char*>::const_iterator it1;
  map<const char*,list<const char*>*,ltstr>::const_iterator it;

  assert( objectHashByType != NULL );
  assert( objectHashByName != NULL );

  // classified insertion of objects into domain objects
  it = objectHashByType->find( ident );
  if( it == objectHashByType->end() )
    objectHashByType->insert( make_pair( strdup( ident ), new list<const char*>(*oList) ) );
  else
    {
      for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
	(*it).second->push_back( *it1 );
    }

  // insert objects into hashByName
  type = new simpleTypeClass( ident );
  for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
    {
      if( objectHashByName->find( (*it1) ) != objectHashByName->end() )
	error( nameClash, (*it1) );
      else
	objectHashByName->insert( make_pair( strdup( *it1 ), type->clone() ) );
    }

  // clean 
  delete type;
  delete oList;
}

static void
declareIntegerObjects( const char *min, const char *max, const list<const char*> *oList )
{
  typeClass *type;
  list<const char*>::const_iterator it1;
  map<const char*,list<const char*>*,ltstr>::const_iterator it;

  assert( objectHashByType != NULL );
  assert( objectHashByName != NULL );

  // classified insertion of objects into domain objects
  it = objectHashByType->find( (const char*)":integer" );
  if( it == objectHashByType->end() )
    objectHashByType->insert( make_pair( strdup(":integer"), new list<const char*>(*oList) ) );
  else
    {
      for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
	(*it).second->push_back( *it1 );
    }

  // insert objects into hashByName
  type = new integerTypeClass( min, max );
  for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
    {
      if( objectHashByName->find( (*it1) ) != objectHashByName->end() )
	error( nameClash, (*it1) );
      else
	objectHashByName->insert( make_pair( strdup( *it1 ), type->clone() ) );
    }

  // clean 
  delete type;
  delete oList;
}

static void
declareBooleanObjects( const list<const char*> *oList )
{
  list<const char*>::const_iterator it1;
  map<const char*,list<const char*>*,ltstr>::const_iterator it;

  assert( objectHashByType != NULL );
  assert( objectHashByName != NULL );

  // classified insertion of objects into domain objects
  it = objectHashByType->find( (const char*)":boolean" );
  if( it == objectHashByType->end() )
    objectHashByType->insert( make_pair( strdup(":boolean"), new list<const char*>(*oList) ) );
  else
    {
      for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
	(*it).second->push_back( *it1 );
    }

  // insert objects into hashByName
  for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
    {
      if( objectHashByName->find( (*it1) ) != objectHashByName->end() )
	error( nameClash, (*it1) );
      else
	objectHashByName->insert( make_pair( strdup( *it1 ), theBooleanType->clone() ) );
    }

  // clean 
  delete oList;
}

static void
declareArrayObjects( const char *size, const typeClass *base, const list<const char*> *oList )
{
  int dim;
  typeClass *type;
  arrayClass *array;
  map<const char*,list<const char*>*,ltstr>::const_iterator it;
  list<const char*>::const_iterator it1;
  map<const char*,const arrayClass*,ltstr>::const_iterator it2;

  assert( arrayHash != NULL );
  assert( declaredDomain != NULL );
  assert( objectHashByType != NULL );
  assert( objectHashByName != NULL );

  // classified insertion of objects into domain objects
  it = objectHashByType->find( (const char*)":array" );
  if( it == objectHashByType->end() )
    objectHashByType->insert( make_pair( strdup(":array"), new list<const char*>(*oList) ) );
  else
    {
      for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
	(*it).second->push_back( *it1 );
    }

  // insert objects into hashByName
  type = new arrayTypeClass( size, base );
  for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
    {
      if( objectHashByName->find( (*it1) ) != objectHashByName->end() )
	error( nameClash, (*it1) );
      else
	objectHashByName->insert( make_pair( strdup( *it1 ), type->clone() ) );
    }

  // declare and insert new array into arrayHash
  for( it1 = oList->begin(); it1 != oList->end(); ++it1 )
    {
      it2 = arrayHash->find( (*it1) );
      if( it2 == arrayHash->end() )
	{
	  array = new arrayClass( strdup( (*it1) ), strdup(size), base->clone() );
	  arrayHash->insert( make_pair( strdup( (*it1) ), array ) );
	  if( declaredProblem != NULL )
	    declaredProblem->numArrays++;
	  else
	    declaredDomain->numArrays++;
	}
    }

  // update maxArraySize
  dim = atoi( size );
  if( declaredProblem != NULL )
    declaredProblem->maxArraySize = (declaredProblem->maxArraySize>dim?declaredProblem->maxArraySize:dim);
  else
    declaredDomain->maxArraySize = (declaredDomain->maxArraySize>dim?declaredDomain->maxArraySize:dim);

  // clean 
  delete type;
  delete oList;
}

static void
declareVariable( const char *ident, const typeClass *type )
{
  if( variableEnvironment.front()->find( ident ) == variableEnvironment.front()->end() )
    variableEnvironment.front()->insert( make_pair( ident, type ) );
  else
    error( nameClash, ident );
}

static void
startRamification( const char *ident )
{
  currentRamification = ident;
  if( ramificationSet.find( ident ) != ramificationSet.end() )
    error( nameClash, ident );
}

static void
declareRamification( const formulaClass *formula )
{
  ramificationClass *ram = new formulaRamificationClass( currentRamification, parameterList, formula );
  ramificationSet.insert( currentRamification );
  ramificationList->push_back( ram );
  cleanEnvironment( variableEnvironment.front() );
  variableEnvironment.pop_front();
  parameterList.clear();
  currentRamification = NULL;
}

static void
declareRamification( const effectClass *effect )
{
  ramificationClass *ram = new effectRamificationClass( currentRamification, parameterList, effect );
  ramificationSet.insert( currentRamification );
  ramificationList->push_back( ram );
  cleanEnvironment( variableEnvironment.front() );
  variableEnvironment.pop_front();
  parameterList.clear();
  currentRamification = NULL;

  ram->dirtyPredicates.insert( ramDirtyPredicates.begin(), ramDirtyPredicates.end() );
  ramDirtyPredicates.clear();
  ram->dirtyFunctions.insert( ramDirtyFunctions.begin(), ramDirtyFunctions.end() );
  ramDirtyFunctions.clear();
}

static void
startLogicalDef( const char *ident )
{
  if( internalPredicateSet.find( ident ) != internalPredicateSet.end() )
    error( nameClash, ident );
  else
    {
      list<pair<const char*,const typeClass*> >::const_iterator it;
      list<const typeClass*> *domain = new list<const typeClass*>;

      // create the type for the domain of the new predicate
      for( it = parameterList.begin(); it != parameterList.end(); ++it )
	domain->push_back( (*it).second->clone() );

      // create the entry in the corresponding hash table
      predicateClass *predicate = new predicateClass( strdup(ident), new typeListClass( domain ), 
						      predicateClass::LIST_DOMAIN );
      predicate->internal = 1;
      predicateHash->insert( make_pair( ident, predicate ) );
    }
}

static void
declareLogicalDef( const char *ident, const formulaClass *formula )
{
  internalPredicateClass *pred = new internalPredicateClass( ident, parameterList, formula );
  internalPredicateSet.insert( ident );
  internalPredicateList->push_back( pred );
  cleanEnvironment( variableEnvironment.front() );
  variableEnvironment.pop_front();
  parameterList.clear();
}

static void
startAction( const char *ident )
{
  actionClass *action;

  if( actionSet.find( ident ) != actionSet.end() )
    error( nameClash, ident );
  else
    {
      assert( actionList != NULL );
      action = new actionClass( ident );
      actionList->push_back( action );
      declaredAction = action;
    }
}

static void
finishAction( void )
{
  //xxxx
  maxNumResStates = MAX(maxNumResStates,declaredAction->numResultingStates);
  declaredAction->parameters.insert( declaredAction->parameters.end(), 
				     parameterList.begin(), parameterList.end() );
  cleanEnvironment( variableEnvironment.front() );
  variableEnvironment.pop_front();
  assert( variableEnvironment.empty() );
  parameterList.clear();
  declaredAction = NULL;
}

static void
declareActionPrecondition( const formulaClass *precondition )
{
  assert( declaredAction != NULL );
  declaredAction->precondition = precondition;
}

static void
declareActionEffect( const effectClass *effect )
{
  assert( declaredAction != NULL );
  declaredAction->effect = effect;
}

static void
declareActionCost( const costClass *cost )
{
  assert( declaredAction != NULL );
  declaredAction->cost = cost;
}

static void
declareActionObservation( const observationClass *observation )
{
  assert( declaredDomain != NULL );
  assert( declaredAction != NULL );
  declaredAction->observation = observation;
}

static termClass *
fetchVariableTerm( const char *ident, termClass *subscript )
{
  termClass *result = NULL;
  list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
  map<const char*,const typeClass*,ltstr>::const_iterator it2;

  // in this implementation subscript is ALWAYS NULL
  assert( subscript == NULL );

  // fetch variable
  for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
    {
      it2 = (*it)->find( ident );
      if( it2 != (*it)->end() )
	{
	  result = new variableTermClass( ident, (*it2).second->clone() );
	  break;
	}
    }

  // check for error
  if( it == variableEnvironment.end() )
    {
      error( undefinedVariable, ident );
      result = new universalTermClass();
    }

  // return value
  return( result );
}

static termClass *
fetchObjectTerm( const char *ident, const termClass *subscript )
{
  termClass *rv;
  list<const char*>::const_iterator it;
  map<const char*,const typeClass*,ltstr>::const_iterator it1;
  map<const char*,list<const char*>*,ltstr>::const_iterator it2;

  assert( objectHashByName != NULL );

  // get object type
  it1 = objectHashByName->find( ident );
  if( it1 == objectHashByName->end() )
    {
      error( undefinedObject, ident );
      rv = new universalTermClass();
      delete subscript;
    }
  else
    {
      // check if it is an array
      it2 = objectHashByType->find( (const char*)":array" );
      if( it2 != objectHashByType->end() )
	for( it = (*it2).second->begin(); it != (*it2).second->end(); ++it )
	  if( !strcmp( ident, (*it) ) )
	    break;

      // check for invalid susbcript
      if( (it2 == objectHashByType->end()) || (it == (*it2).second->end()) )
	{
	  if( subscript != NULL )
	    {
	      error( invalidSubscript, ident );
	      rv = new universalTermClass();
	    }
	  else
	    {
	      // simple object 
	      rv = new objectTermClass( ident, (*it1).second->clone() );
	    }
	  delete subscript;
	}
      else
	{
	  // we have an array either without subscript or with subscript
	  rv = new arrayTermClass( ident, (const arrayTypeClass*)(*it1).second->clone(), subscript );
	}
    }

  // return value
  return( rv );
}

static void
setVariableDenotationFor( const char *ident )
{
  if( declaredDomain && 
      (declaredDomain->varDenotation.find( ident ) == declaredDomain->varDenotation.end()) )
    {
      pair<const char*,int> p( strdup( ident ), declaredDomain->varDenotation.size() );
      declaredDomain->varDenotation.insert( p );
    }
  if( declaredProblem && 
      (declaredProblem->varDenotation.find( ident ) == declaredProblem->varDenotation.end()) )
    {
      pair<const char*,int> p( strdup( ident ), declaredProblem->varDenotation.size() );
      declaredProblem->varDenotation.insert( p );
    }
}

static bool
checkProbabilities( const probabilisticEffectClass *effect )
{
  list<pair<const char*,const effectClass*>*>::const_iterator it;

  float prob = 0.0;
  for( it = effect->effectList->begin(); it != effect->effectList->end(); ++it )
    prob += atof( (*it)->first );

#if linux
  prob = (float)((unsigned)(1000.0*prob+0.5))/1000.0;
#endif

  if( prob > 1.0 ) error( probabilitySum );
  if( prob != 1.0 ) warning( probabilitySumWarning, prob );
  return( prob == 1.0 );
}

static void
checkProbabilities( const list<pair<const char*,const observationListClass*>*> *obsList )
{
  list<pair<const char*,const observationListClass*>*>::const_iterator it;

  float prob = 0.0;
  for( it = obsList->begin(); it != obsList->end(); ++it )
    prob += atof( (*it)->first );
  if( prob != 1.0 ) error( probabilitySum );
}

static void
startProblem( const char *problemIdent, const char *domainIdent )
{
  map<const char*,const domainClass*,ltstr>::const_iterator it;

  it = domainHash.find( domainIdent );
  if( it == domainHash.end() )
    {
      // this is a serious error, parsing can't continue.
      // Report it, printErrors on stderr, and exit.
      // this should be improved.
      error( undefinedDomain, domainIdent );
      printErrors( stderr );
      exit( -1 );
    }
  free( (void*)domainIdent );

  if( problemSet.find( problemIdent ) != problemSet.end() )
    error( nameClash, problemIdent );

  // allocate problem
  declaredProblem = new problemClass( strdup( yyfile ), problemIdent, (*it).second );

  // setup global hashes
  namedTypeHash = &declaredProblem->namedTypeHash;
  objectHashByType = &declaredProblem->objectHashByType;
  objectHashByName = &declaredProblem->objectHashByName;
  ramificationList = (list<const ramificationClass*>*)&declaredProblem->domain->ramificationList;
  internalPredicateList = (list<const internalPredicateClass*>*)&declaredProblem->domain->internalPredicateList;
  actionList = (list<const actionClass*>*)&declaredProblem->domain->actionList;
  functionHash = (map<const char*,const functionClass*,ltstr>*)&declaredProblem->domain->functionHash;
  predicateHash = (map<const char*,const predicateClass*,ltstr>*)&declaredProblem->domain->predicateHash;
  arrayHash = &declaredProblem->arrayHash;
}

static const codeChunkClass *
finishProblem( void )
{
  // save domain in global hash table
  problemSet.insert( make_pair( strdup( declaredProblem->ident ), 
				strdup( declaredProblem->domain->ident ) ) );

  actionSet.clear();
  ramificationSet.clear();
  internalPredicateSet.clear();

  // cleanning
  namedTypeHash = NULL;
  objectHashByType = NULL;
  objectHashByName = NULL;
  ramificationList = NULL;
  internalPredicateList = NULL;
  actionList = NULL;
  functionHash = NULL;
  predicateHash = NULL;
  arrayHash = NULL;
  problemClass *rv = declaredProblem;
  declaredProblem = NULL;
  return( rv );
}

static void
declareInit( const effectListClass *effect )
{
  assert( declaredProblem != NULL );
  declaredProblem->initEffect = effect;
}

static void
declareGoal( const formulaClass *formula )
{
  assert( declaredProblem != NULL );
  declaredProblem->goalFormula = formula;
}

static void
declareHook( const hookClass *hook )
{
  assert( declaredProblem != NULL );
  declaredProblem->hookType |= hook->hookType;
  switch( hook->hookType )
    {
    case problemClass::MODEL_HOOK:
      declaredProblem->modelHook = hook;
      break;
    case problemClass::BELIEF_HOOK:
      declaredProblem->beliefHook = hook;
      break;
    case problemClass::HEURISTIC_HOOK:
      declaredProblem->heuristicHook = hook;
      break;
    }
}

static void
cleanEnvironment( map<const char*,const typeClass*,ltstr> *env )
{
  map<const char*,const typeClass*,ltstr>::iterator it;

  for( it = env->begin(); it != env->end(); ++it )
    {
      //      fprintf( stderr, "free of [%p]%s\n", (*it).first, (*it).first );
      free( (void*)(*it).first );
      delete (*it).second;
    }
  delete env;
}

static void
printEnvironment( void )
{
  list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
  map<const char*,const typeClass*,ltstr>::const_iterator it2;
  for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
    {
      for( it2 = (*it)->begin(); it2 != (*it)->end(); ++it2 )
	cout << "variable = " << (*it2).first << ", type = " << typeid( (*it2).second ).name() << endl;
    }
}

static list<map<const char*,const typeClass*,ltstr>*> *
environmentClone( void )
{
  list<map<const char*,const typeClass*,ltstr>*> *result;
  list<map<const char*,const typeClass*,ltstr>*>::const_iterator it;
  map<const char*,const typeClass*,ltstr> *m;
  map<const char*,const typeClass*,ltstr>::const_iterator it2;

  result = new list<map<const char*,const typeClass*,ltstr>*>;
  for( it = variableEnvironment.begin(); it != variableEnvironment.end(); ++it )
    if( (*it)->size() > 0 )
      {
	m = new map<const char*,const typeClass*,ltstr>;
	for( it2 = (*it)->begin(); it2 != (*it)->end(); ++it2 )
	  m->insert( make_pair( strdup( (*it2).first ), (*it2).second->clone() ) );
	result->push_back( m );
      }

  return( result );
}
