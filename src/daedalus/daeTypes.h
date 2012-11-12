#ifndef _daeTypes_INCLUDE_
#define _daeTypes_INCLUDE_

#include <daedalus/_daeErrors.h>

#include <list>
#include <map>
#include <set>


struct ltstr
{
  bool operator()(const char* s1, const char* s2) const { return strcmp( s1, s2 ) < 0; }
};


// forward references
class  functionClass;
class  termClass;
class  integerTermClass;
class  sumTermClass;
class  booleanTermClass;
class  termListClass;
class  conditionalTermClass;
class  formulaTermClass;
class  predicateClass;
class  specialFunctionClass;
class  formulaClass;
class  effectClass;
class  effectListClass;

class  typeClass;
class  genericIntegerTypeClass;
class  integerTypeClass;
class  booleanTypeClass;
class  simpleTypeClass;
class  typeListClass;
class  arrayTypeClass;
class  universalTypeClass;

class  varValuePairClass;
class  instantiationClass;
class  hookClass;

class  arrayClass;
class  actionClass;
class  namedTypeClass;
class  ramificationClass;
class  internalFunctionClass;
class  internalPredicateClass;

class  existsFormulaClass;
class  forallFormulaClass;
class  sumFormulaClass;

extern booleanTypeClass *theBooleanType;


class codeChunkClass
{
  // pure abstract
};


class domainClass : public codeChunkClass
{
public:
  // constants
  static const int MAX_ARITY = 48;

  // members
  const char *ident;
  const char *fileName;
  int model;
  int dynamics;
  int feedback;
  int numArrays;
  int maxArraySize;
  int numFunctions[MAX_ARITY];
  int numPredicates[MAX_ARITY];

  // static chains
  int existsNumber;
  int forallNumber;
  int sumNumber;
  existsFormulaClass *existsChain;
  forallFormulaClass *forallChain;
  sumTermClass* sumChain;

  // hashes
  map<const char*,const functionClass*,ltstr> functionHash;
  map<const char*,const predicateClass*,ltstr> predicateHash;
  map<const char*,const namedTypeClass*,ltstr> namedTypeHash;
  map<const char*,list<const char*>*,ltstr> objectHashByType;
  map<const char*,const typeClass*,ltstr> objectHashByName;
  map<const char*,const arrayClass*,ltstr> arrayHash;
  map<const char*,int,ltstr> varDenotation;

  // lists
  list<const actionClass*> actionList;
  list<const ramificationClass*> ramificationList;
  list<const internalFunctionClass*> internalFunctionList;
  list<const internalPredicateClass*> internalPredicateList;
  set<const char*,ltstr> externalProcedureList;

  // constructors
  domainClass( const char *fileName, const char *ident );
};


class problemClass : public codeChunkClass
{
public:
  // members
  const char *ident;
  const char *fileName;
  const domainClass *domain;
  int hookType;
  int numActions;
  int numRamifications;
  int numObjects;
  int numArrays;
  int maxArraySize;
  int numFunctions[domainClass::MAX_ARITY];
  int numPredicates[domainClass::MAX_ARITY];
  int numStaticFunctions[domainClass::MAX_ARITY];
  int numStaticPredicates[domainClass::MAX_ARITY];

  const effectListClass *initEffect;
  const formulaClass *goalFormula;
  const hookClass *modelHook;
  const hookClass *beliefHook;
  const hookClass *heuristicHook;

  // for domain/problem analysis
  set<const char*,ltstr> dirtyPredicates;
  set<const char*,ltstr> dirtyFunctions;

  // static chains
  int existsNumber;
  int forallNumber;
  int sumNumber;
  existsFormulaClass *existsChain;
  forallFormulaClass *forallChain;
  sumTermClass* sumChain;

  // hashes
  map<const char*,const namedTypeClass*,ltstr> namedTypeHash;
  map<const char*,list<const char*>*,ltstr> objectHashByType;
  map<const char*,const typeClass*,ltstr> objectHashByName;
  map<const char*,const arrayClass*,ltstr> arrayHash;
  map<const char*,int,ltstr> varDenotation;

  // constants for hooked mode
  static const int NO_HOOK = 0;
  static const int HEURISTIC_HOOK = 1;
  static const int BELIEF_HOOK = 2;
  static const int MODEL_HOOK = 4;

  // constants for belief mode
  static const int STANDARD_BELIEF = 0;
  static const int THREE_VALUED_BELIEF = 1;
  static const int UNIFORM_BELIEF = 2;

  // constructors
  problemClass( const char *fileName, const char *ident, const domainClass *domain );
  ~problemClass();

  // methods
  void getObjectsOfType( const typeClass *type, list<const char*> &oList ) const;

  // methods (code generation)
  int  observationSize( void ) const;
  void generateClasses( FILE *out );
  void generateMethodList( FILE *out );
  void generateBasicMethods( FILE *out );
  void generateStaticMethods( FILE *out );
  void generateVirtualMethods( FILE *out );
  void generateGoal( FILE *out );
  void generateStateHash( FILE *out );
  void generateRegistrationFunctions( FILE *out );
  void generateMakefile( FILE *out, char *MName );
};


class formulaClass
{
public:
  // ctor/dtor
  formulaClass() { }
  virtual ~formulaClass() { }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const = 0;
};


class andFormulaClass : public formulaClass
{
public:
  // members
  const list<const formulaClass*> *formulaList;

  // constructors
  andFormulaClass( const list<const formulaClass*> *fList ) { formulaList = fList; }
  virtual ~andFormulaClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class orFormulaClass : public formulaClass
{
public:
  // members
  const list<const formulaClass*> *formulaList;

  // constructors
  orFormulaClass( const list<const formulaClass*> *fList ) { formulaList = fList; }
  virtual ~orFormulaClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class notFormulaClass : public formulaClass
{
public:
  // members
  const formulaClass *formula;

  // constructors
  notFormulaClass( const formulaClass *form ) { formula = form; }
  virtual ~notFormulaClass()
    { delete formula; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class ifFormulaClass : public formulaClass
{
public:
  // members
  const formulaClass *formula1;
  const formulaClass *formula2;

  // constructors
  ifFormulaClass( const formulaClass *form1, const formulaClass *form2 )
    { formula1 = form1; formula2 = form2; }
  virtual ~ifFormulaClass()
    { delete formula1; delete formula2; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class iffFormulaClass : public formulaClass
{
public:
  // members
  const formulaClass *formula1;
  const formulaClass *formula2;

  // constructors
  iffFormulaClass( const formulaClass *form1, const formulaClass *form2 )
    { formula1 = form1; formula2 = form2; }
  virtual ~iffFormulaClass()
    { delete formula1; delete formula2; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class atomFormulaClass : public formulaClass
{
public:
  // members
  const predicateClass *predicate;
  const termListClass  *termList;

  // constructors
  atomFormulaClass( const char *ident, const termListClass *tList );
  virtual ~atomFormulaClass(); // dtor can't be inlined here

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class equalFormulaClass : public formulaClass
{
public:
  // members
  const termClass *lterm;
  const termClass *rterm;

  // constructors
  equalFormulaClass( const termClass *lt, const termClass *rt );
  virtual ~equalFormulaClass(); // dtor can't be inlined here

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class inclusionFormulaClass : public formulaClass
{
public:
  // members
  const termClass *term;
  const list<const termClass*> *termList;

  // constructors
  inclusionFormulaClass( const termClass *term, const list<const termClass*> *tList );
  virtual ~inclusionFormulaClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class existsFormulaClass : public formulaClass
{
public:
  // members
  int id;
  bool insideEffect;
  bool inlineFormula;
  const char *internal;
  const char *variable;
  const simpleTypeClass *varType;
  const formulaClass *formula;
  const list<map<const char*,const typeClass*,ltstr>*> *environment;
  existsFormulaClass *nextChain;

  // constructors
  existsFormulaClass( const char *var, const simpleTypeClass *type, const formulaClass *form,
		      list<map<const char*,const typeClass*,ltstr>*> *env, 
		      const char *intern, bool iEffect, bool inl );
  virtual ~existsFormulaClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &env ) const;
};


class forallFormulaClass : public formulaClass
{
public:
  // members
  int id;
  bool insideEffect;
  bool inlineFormula;
  const char *internal;
  const char *variable;
  const simpleTypeClass *varType;
  const formulaClass *formula;
  const list<map<const char*,const typeClass*,ltstr>*> *environment;
  forallFormulaClass *nextChain;

  // constructors
  forallFormulaClass( const char *var, const simpleTypeClass *type, const formulaClass *form,
		      list<map<const char*,const typeClass*,ltstr>*> *env, 
		      const char *intern, bool iEffect, bool inl );
  virtual ~forallFormulaClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &env ) const;
};


class certaintyGoalClass : public formulaClass
{
public:
  // constructors
  certaintyGoalClass() { }
  virtual ~certaintyGoalClass() { }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool neutral,
			     map<const char*,const char*,ltstr> &env ) const;
};


class typeClass
{
public:
  // members
  int selfValue;

  // constructors
  typeClass() { selfValue = 1; }
  virtual ~typeClass() { }

  // virtual methods
  virtual void getBaseObjects( list<const char*> &objectList ) const = 0;
  virtual typeClass *clone( void ) const = 0;

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    { return( 0 ); }
  virtual int validRValue( void ) const
    {  return( 1 ); }
  virtual int checkType( const termClass *term ) const = 0;
  virtual int checkType( const typeClass *type ) const = 0;
  virtual int checkType( const genericIntegerTypeClass *type ) const = 0;
  virtual int checkType( const integerTypeClass *type ) const = 0;
  virtual int checkType( const booleanTypeClass *type ) const = 0;
  virtual int checkType( const simpleTypeClass *type ) const = 0;
  virtual int checkType( const typeListClass *type ) const = 0;
  virtual int checkType( const arrayTypeClass *type ) const = 0;
  virtual int checkType( const universalTypeClass *type ) const = 0;
};


class genericIntegerTypeClass : public typeClass
{
public:
  // destructors
  virtual ~genericIntegerTypeClass() { }

  // virtual methods
  virtual void getBaseObjects( list<const char*> &objectList ) const { }
  virtual typeClass *clone( void ) const
    { return( new genericIntegerTypeClass ); }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const;
  virtual int checkType( const typeClass *type ) const;
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( 0 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }
};


class integerTypeClass : public typeClass
{
public:
  // members
  const char *min;
  const char *max;
  int minValue;
  int maxValue;

  // constructors
  integerTypeClass( const char *min, const char *max );
  integerTypeClass( const integerTypeClass &integer );
  virtual ~integerTypeClass()
    { free( (void*)min ); free( (void*)max ); }

  // virtual methods
  virtual void getBaseObjects( list<const char*> &objectList ) const;
  virtual typeClass *clone( void ) const
    { return( new integerTypeClass( *this ) ); }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const;
  virtual int checkType( const typeClass *type ) const;
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( (minValue == type->minValue) && (maxValue == type->maxValue) ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( 0 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }
};


class booleanTypeClass : public typeClass
{
public:
  // constructors
  booleanTypeClass() { }
  virtual ~booleanTypeClass() { }

  // virtual methods
  virtual void getBaseObjects( list<const char*> &objectList ) const;
  virtual typeClass *clone( void ) const
    { return( new booleanTypeClass( *this ) ); }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const;
  virtual int checkType( const typeClass *type ) const;
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( 0 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }
};


class simpleTypeClass : public typeClass
{
public:
  // members
  const char *ident;
  const namedTypeClass *namedType;

  // constructors
  simpleTypeClass( const char *ident );
  simpleTypeClass( const simpleTypeClass &simple );
  virtual ~simpleTypeClass()
    { free( (void*)ident ); }

  // virtual methods
  virtual void getBaseObjects( list<const char*> &objectList ) const { }
  virtual typeClass *clone( void ) const
    { return( new simpleTypeClass( *this ) ); }

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    { return( 1 ); }
  virtual int checkType( const termClass *term ) const;
  virtual int checkType( const typeClass *type ) const;
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const simpleTypeClass *type ) const;
  virtual int checkType( const typeListClass *type ) const
    { return( 0 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }
};


class typeListClass : public typeClass
{
public:
  // members
  const list<const typeClass*> *typeList;

  // constructors
  typeListClass( const list<const typeClass*> *tList );
  typeListClass( const typeListClass &tList );
  virtual ~typeListClass();

  // virtual methods
  virtual void getBaseObjects( list<const char*> &objectList ) const { }
  virtual typeClass *clone( void ) const
    { return( new typeListClass( *this ) ); }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const;
  virtual int checkType( const typeClass *type ) const;
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const;
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }
};


class arrayTypeClass : public typeClass
{
public:
  // members
  const char *size;
  const typeClass *base;

  // constructors
  arrayTypeClass( const char *s, const typeClass *b );
  arrayTypeClass( const arrayTypeClass &array );
  virtual ~arrayTypeClass()
    { free( (void*)size ); delete base; }

  // virtual methods
  virtual void getBaseObjects( list<const char*> &objectList ) const { }
  virtual typeClass *clone( void ) const
    { return( new arrayTypeClass( *this ) ); }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const;
  virtual int checkType( const typeClass *type ) const;
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( 0 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( !strcmp( size, type->size ) && base->checkType( type->base ) ); } // checktype ok
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }
};


class universalTypeClass : public typeClass
{
public:
  // destructors
  virtual ~universalTypeClass() { }

  // virtual methods
  virtual void getBaseObjects( list<const char*> &objectList ) const { }
  virtual typeClass *clone( void ) const
    { return( new universalTypeClass ); }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const
    { return( 1 ); }
  virtual int checkType( const typeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( 1 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }
};


class functionClass
{
public:
  // members
  const char *ident;
  const typeClass *domain;
  const typeClass *range;
  int external;
  int internal;
  int domainType;
  int numArguments;

  // constructors
  functionClass( const char *id, const typeClass *dom, const typeClass *ran, int dType );
  functionClass( const functionClass &function );
  virtual ~functionClass()
    { free( (void*)ident ); delete domain; delete range; }

  // methods
  functionClass* clone( void ) const
    { return( new functionClass( *this ) ); }
  bool staticFunction( const problemClass *problem ) const;

  // constants
  enum { SIMPLE_DOMAIN, LIST_DOMAIN, ARRAY_DOMAIN };
};


class predicateClass
{
public:
  // members
  const char *ident;
  const typeClass *domain;
  int external;
  int internal;
  int domainType;
  int numArguments;

  // constructors
  predicateClass( const char *id, const typeClass *dom, int dType, int ext = 0 );
  predicateClass( const predicateClass &predicate );
  ~predicateClass()
    { free( (void*)ident ); delete domain; }

  // methods
  predicateClass* clone( void ) const
    { return( new predicateClass( *this ) ); }
  bool staticPredicate( const problemClass *problem ) const;

  // constants
  enum { SIMPLE_DOMAIN, LIST_DOMAIN, ARRAY_DOMAIN };
};


class termClass
{
public:
  // destructor
  virtual ~termClass() { }

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    { return( 0 ); }
  virtual int validRValue( void ) const
    { return( 1 ); }

  virtual int checkType( const termClass *term ) const = 0;
  virtual int checkType( const integerTermClass *term ) const = 0;
  virtual int checkType( const sumTermClass *term ) const = 0;
  virtual int checkType( const booleanTermClass *term ) const = 0;
  virtual int checkType( const conditionalTermClass *term ) const = 0;
  virtual int checkType( const formulaTermClass *term ) const = 0;

  virtual int checkType( const typeClass *type ) const = 0;
  virtual int checkType( const genericIntegerTypeClass *type ) const = 0;
  virtual int checkType( const integerTypeClass *type ) const = 0;
  virtual int checkType( const booleanTypeClass *type ) const = 0;
  virtual int checkType( const simpleTypeClass *type ) const = 0;
  virtual int checkType( const typeListClass *type ) const = 0;
  virtual int checkType( const arrayTypeClass *type ) const = 0;
  virtual int checkType( const universalTypeClass *type ) const = 0;

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const = 0;
};


class conditionalTermClass : public termClass
{
public:
  // members
  const formulaClass *formula;
  const termClass *term1;
  const termClass *term2;

  // constructors
  conditionalTermClass( const formulaClass *form, const termClass *t1, const termClass *t2 );
  virtual ~conditionalTermClass()
    { delete formula; delete term1; delete term2; }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const
    { return( term1->checkType( term ) && term2->checkType( term ) ); } // checktype ok
  virtual int checkType( const integerTermClass *term ) const;
  virtual int checkType( const sumTermClass *term ) const;
  virtual int checkType( const booleanTermClass *term ) const;
  virtual int checkType( const conditionalTermClass *term ) const
    { return( term1->checkType( term ) && term2->checkType( term ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const;

  virtual int checkType( const typeClass *t ) const
    { return( t->checkType( term1 ) && t->checkType( term2 ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *t ) const
    { return( t->checkType( term1 ) && t->checkType( term2 ) ); } // checktype ok
  virtual int checkType( const integerTypeClass *t ) const
    { return( t->checkType( term1 ) && t->checkType( term2 ) ); } // checktype ok
  virtual int checkType( const booleanTypeClass *t ) const
    { return( t->checkType( term1 ) && t->checkType( term2 ) ); } // checktype ok
  virtual int checkType( const simpleTypeClass *t ) const
    { return( t->checkType( term1 ) && t->checkType( term2 ) ); } // checktype ok
  virtual int checkType( const typeListClass *t ) const
    { return( t->checkType( term1 ) && t->checkType( term2 ) ); } // checktype ok
  virtual int checkType( const arrayTypeClass *t ) const
    { return( t->checkType( term1 ) && t->checkType( term2 ) ); } // checktype ok
  virtual int checkType( const universalTypeClass *t ) const
    { return( t->checkType( term1 ) && t->checkType( term2 ) ); } // checktype ok

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class formulaTermClass : public termClass
{
public:
  // members
  const formulaClass *formula;

  // constructors
  formulaTermClass( const formulaClass *form )
    { formula = form; }
  virtual ~formulaTermClass()
    { delete formula; }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const
    { return( term->checkType( this ) ); } // checktype ok
  virtual int checkType( const integerTermClass *term ) const
    { return( 0 ); }
  virtual int checkType( const sumTermClass *term ) const
    { return( 0 ); }
  virtual int checkType( const booleanTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const conditionalTermClass *term ) const
    { return( term->checkType( this ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( 1 ); }

  virtual int checkType( const typeClass *type ) const
    { return( type->checkType( this ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( type->typeList->size()==1 ? checkType(type->typeList->front()) : 0 ); } //checktype ok
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};

inline int
conditionalTermClass::checkType( const formulaTermClass *term ) const
{
  return( term1->checkType( term ) && term2->checkType( term ) ); // checktype ok
}


class integerTermClass : public termClass
{
public:
  // members
  const char *integer;
  int value;

  // constructors
  integerTermClass( const char *i )
    { integer = i; value = atoi( i ); }
  virtual ~integerTermClass()
    { free( (void*)integer ); }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const
    { return( term->checkType( this ) ); } // checktype ok 
  virtual int checkType( const integerTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const sumTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const booleanTermClass *term ) const
    { return( 0 ); }
  virtual int checkType( const conditionalTermClass *term ) const
    { return( term->checkType( this ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( 0 ); }

  virtual int checkType( const typeClass *type ) const
    { return( type->checkType( this ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( (type->minValue <= value) && (type->maxValue >= value) ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( type->typeList->size()==1 ? checkType(type->typeList->front()) : 0 ); } //checktype ok
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};

inline int
conditionalTermClass::checkType( const integerTermClass *term ) const
{
  return( term1->checkType( term ) && term2->checkType( term ) ); // checktype ok
}


class sumTermClass : public termClass
{
public:
  // members
  int id;
  bool insideEffect;
  bool inlineTerm;
  const char *variable;
  const simpleTypeClass *varType;
  const termClass *term;
  const list<map<const char*,const typeClass*,ltstr>*> *environment;
  sumTermClass *nextChain;

  // constructors
  sumTermClass( const char *var, const simpleTypeClass *vtype, const termClass *t,
		list<map<const char*,const typeClass*,ltstr>*> *env, bool iEffect, bool iln );
  virtual ~sumTermClass();

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const
    { return( term->checkType( this ) ); } // checktype ok
  virtual int checkType( const integerTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const sumTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const booleanTermClass *term ) const
    { return( 0 ); }
  virtual int checkType( const conditionalTermClass *term ) const
    { return( term->checkType( this ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( 0 ); }

  virtual int checkType( const typeClass *type ) const
    { return( type->checkType( this ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { warning( unableCheckInteger ); return( 1 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( type->typeList->size()==1 ? checkType(type->typeList->front()) : 0 ); } //checktype ok
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &env ) const;
};

inline int
conditionalTermClass::checkType( const sumTermClass *term ) const
{
  return( term1->checkType( term ) && term2->checkType( term ) ); // checktype ok
}


class booleanTermClass : public termClass
{
public:
  // members
  const char *boolean;
  int value;

  // constructors
  booleanTermClass( const char *b )
    { boolean = b; value = (!strcmp( boolean, "true" ) ? 1 : 0 ); }
  virtual ~booleanTermClass()
    { free( (void*)boolean ); }

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const
    { return( term->checkType( this ) ); } // checktype ok
  virtual int checkType( const integerTermClass *term ) const
    { return( 0 ); }
  virtual int checkType( const sumTermClass *term ) const
    { return( 0 ); }
  virtual int checkType( const booleanTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const conditionalTermClass *term ) const
    { return( term->checkType( this ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( 1 ); }

  virtual int checkType( const typeClass *type ) const
    { return( type->checkType( this ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( type->typeList->size()==1 ? checkType(type->typeList->front()) : 0 ); } //checktype ok
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};

inline int
conditionalTermClass::checkType( const booleanTermClass *term ) const
{
  return( term1->checkType( term ) && term2->checkType( term ) ); // checktype ok
}


class variableTermClass : public termClass
{
public:
  // members
  int direct;
  const char *ident;
  const typeClass *type;

  // constructors
  variableTermClass( const char *i, const typeClass *t )
    { direct = 0; ident = i; type = t; }
  virtual ~variableTermClass()
    { free( (void*)ident ); delete type; }

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    { return( type->validLValue() ); }
  virtual int checkType( const termClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const integerTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const sumTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const booleanTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const conditionalTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok

  virtual int checkType( const typeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const integerTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const booleanTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const simpleTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const typeListClass *t ) const
    { return( t->typeList->size()==1 ? t->typeList->front()->checkType(type) : 0 ); } //checktype ok
  virtual int checkType( const arrayTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const universalTypeClass *t ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class objectTermClass : public termClass
{
public:
  // members
  const char *ident;
  const typeClass *type;

  // constructors
  objectTermClass( const char *id, const typeClass *t )
    { ident = id; type = t; }
  virtual ~objectTermClass()
    { free( (void*)ident ); delete type; }

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    { return( 1 ); }
  virtual int checkType( const termClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const integerTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const sumTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const booleanTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const conditionalTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( term->checkType( type ) ); } // checktype ok

  virtual int checkType( const typeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const integerTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const booleanTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const simpleTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const typeListClass *t ) const
    { return( t->typeList->size()==1 ? t->typeList->front()->checkType(type) : 0 ); } //checktype ok
  virtual int checkType( const arrayTypeClass *t ) const
    { return( t->checkType( type ) ); } // checktype ok
  virtual int checkType( const universalTypeClass *t ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class termListClass : public termClass
{
public:
  // members
  const list<const termClass*> *termList;

  // constructors
  termListClass( const list<const termClass*> *tList )
    { termList = tList; }
  virtual ~termListClass();

  // virtual methods (type checking)
  virtual int checkType( const termClass *term ) const;
  virtual int checkType( const integerTermClass *term ) const
    { return( termList->size()==1 ? termList->front()->checkType(term) : 0 ); } // checktype ok
  virtual int checkType( const sumTermClass *term ) const
    { return( termList->size()==1 ? termList->front()->checkType(term) : 0 ); } // checktype ok
  virtual int checkType( const booleanTermClass *term ) const
    { return( termList->size()==1 ? termList->front()->checkType(term) : 0 ); } // checktype ok
  virtual int checkType( const conditionalTermClass *term ) const
    { return( 0 ); } // not yet supported
  virtual int checkType( const formulaTermClass *term ) const
    { return( termList->size()==1 ? termList->front()->checkType(term) : 0 ); } // checktype ok

  virtual int checkType( const typeClass *type ) const
    { return( type->checkType( this ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( termList->size()==1 ? termList->front()->checkType(type) : 0 ); } // checktype ok
  virtual int checkType( const integerTypeClass *type ) const
    { return( termList->size()==1 ? termList->front()->checkType(type) : 0 ); } // checktype ok
  virtual int checkType( const booleanTypeClass *type ) const
    { return( termList->size()==1 ? termList->front()->checkType(type) : 0 ); } // checktype ok
  virtual int checkType( const simpleTypeClass *type ) const
    { return( termList->size()==1 ? termList->front()->checkType(type) : 0 ); } // checktype ok
  virtual int checkType( const typeListClass *type ) const;
  virtual int checkType( const arrayTypeClass *type ) const
    { return( termList->size()==1 ? termList->front()->checkType(type) : 0 ); } // checktype ok
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class functionTermClass : public termClass
{
public:
  // members
  const functionClass *function;
  const termClass *argument;
  int special;

  // constructors
  functionTermClass( const char *ident, const termClass *arg );
  functionTermClass( const specialFunctionClass *fun, const termClass *arg );
  virtual ~functionTermClass()
    { delete function; delete argument; }

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    { return( function->external == 0 ); }
  virtual int checkType( const termClass *term ) const
    { return( function->range->checkType( term ) ); } // checktype ok
  virtual int checkType( const integerTermClass *term ) const
    { return( function->range->checkType( term ) ); } // checktype ok
  virtual int checkType( const sumTermClass *term ) const
    { return( function->range->checkType( term ) ); } // checktype ok
  virtual int checkType( const booleanTermClass *term ) const
    { return( function->range->checkType( term ) ); } // checktype ok
  virtual int checkType( const conditionalTermClass *term ) const
    { return( function->range->checkType( term ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( function->range->checkType( term ) ); } // checktype ok

  virtual int checkType( const typeClass *type ) const
    { return( type->checkType( function->range ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( type->checkType( function->range ) ); } // checktype ok
  virtual int checkType( const integerTypeClass *type ) const
    { return( type->checkType( function->range ) ); } // checktype ok
  virtual int checkType( const booleanTypeClass *type ) const
    { return( type->checkType( function->range ) ); } // checktype ok
  virtual int checkType( const simpleTypeClass *type ) const
    { return( type->checkType( function->range ) ); } // checktype ok
  virtual int checkType( const typeListClass *type ) const
    { return( 0 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class predicateTermClass : public termClass
{
public:
  // members
  const predicateClass *predicate;
  const termClass *argument;

  // constructors
  predicateTermClass( const char *ident, const termClass *arg );
  virtual ~predicateTermClass()
    { delete predicate; delete argument; }

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    { return( predicate->external == 0 ); }
  virtual int checkType( const termClass *term ) const;
  virtual int checkType( const integerTermClass *term ) const
    { return( 0 ); }
  virtual int checkType( const sumTermClass *term ) const
    { return( 0 ); }
  virtual int checkType( const booleanTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const conditionalTermClass *term ) const
    { return( theBooleanType->checkType( term ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( 1 ); }

  virtual int checkType( const typeClass *type ) const;
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( 0 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 0 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class arrayTermClass : public termClass
{
public:
  // members
  const char *ident;
  const termClass *subscript;
  const arrayTypeClass *type;

  // constructors
  arrayTermClass( const char *id, const arrayTypeClass *t, const termClass *subs );
  virtual ~arrayTermClass()
    { free( (void*)ident ); delete type; delete subscript; }

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    {
      // in this version full arrays can't be manipulated
      return( subscript != NULL );
    }
  virtual int checkType( const termClass *term ) const
    { return( !subscript ? type->checkType(term) : type->base->checkType(term) ); } // checktype ok
  virtual int checkType( const integerTermClass *term ) const
    { return( !subscript ? 0 : type->base->checkType(term) ); } // checktype ok
  virtual int checkType( const sumTermClass *term ) const
    { return( !subscript ? 0 : type->base->checkType(term) ); } // checktype ok
  virtual int checkType( const booleanTermClass *term ) const
    { return( !subscript ? 0 : type->base->checkType(term) ); } // checktype ok
  virtual int checkType( const conditionalTermClass *term ) const
    { return( !subscript ? type->checkType(term) : type->base->checkType(term) ); } // checktype ok
  virtual int checkType( const formulaTermClass *term ) const
    { return( !subscript ? 0 : type->base->checkType(term) ); } // checktype ok

  virtual int checkType( const typeClass *t ) const
    { return( !subscript ? t->checkType(type) : t->checkType(type->base) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *t ) const
    { return( !subscript ? 0 : t->checkType(type->base) ); } // checktype ok
  virtual int checkType( const integerTypeClass *t ) const
    { return( !subscript ? 0 : t->checkType(type->base) ); } // checktype ok
  virtual int checkType( const booleanTypeClass *t ) const
    { return( !subscript ? 0 : t->checkType(type->base) ); } // checktype ok
  virtual int checkType( const simpleTypeClass *t ) const
    { return( !subscript ? 0 : t->checkType(type->base) ); } // checktype ok
  virtual int checkType( const typeListClass *t ) const
    { return( 0 ); }
  virtual int checkType( const arrayTypeClass *t ) const
    { return( !subscript ? t->checkType( type ) : t->checkType(type->base) ); } // checktype ok
  virtual int checkType( const universalTypeClass *t ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class starTermClass : public termClass
{
public:
  // members
  bool insideEffect;
  const termClass *term;

  // constructors
  starTermClass( const termClass *t, bool iEffect )
    { term = t; insideEffect = iEffect; }
  virtual ~starTermClass()
    { delete term; }

  // virtual methods (type checking)
  virtual int checkType( const termClass *t ) const
    { return( term->checkType( t ) ); } // checktype ok
  virtual int checkType( const integerTermClass *t ) const
    { return( term->checkType( t ) ); } // checktype ok
  virtual int checkType( const sumTermClass *t ) const
    { return( term->checkType( t ) ); } // checktype ok
  virtual int checkType( const booleanTermClass *t ) const
    { return( term->checkType( t ) ); } // checktype ok
  virtual int checkType( const conditionalTermClass *t ) const
    { return( term->checkType( t ) ); } // checktype ok
  virtual int checkType( const formulaTermClass *t ) const
    { return( term->checkType( t ) ); } // checktype ok

  virtual int checkType( const typeClass *type ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const integerTypeClass *type ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const booleanTypeClass *type ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const simpleTypeClass *type ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const typeListClass *type ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const arrayTypeClass *type ) const
    { return( term->checkType( type ) ); } // checktype ok
  virtual int checkType( const universalTypeClass *type ) const
    { return( term->checkType( type ) ); } // checktype ok

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class universalTermClass : public termClass
{
public:
  // constructors
  universalTermClass() { }
  virtual ~universalTermClass() { }

  // virtual methods (type checking)
  virtual int validLValue( void ) const
    { return( 1 ); }
  virtual int checkType( const termClass *term ) const
    { return( 1 ); }
  virtual int checkType( const integerTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const sumTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const booleanTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const conditionalTermClass *term ) const
    { return( 1 ); }
  virtual int checkType( const formulaTermClass *t ) const
    { return( 1 ); }

  virtual int checkType( const typeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const genericIntegerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const integerTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const booleanTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const simpleTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const typeListClass *type ) const
    { return( 1 ); }
  virtual int checkType( const arrayTypeClass *type ) const
    { return( 1 ); }
  virtual int checkType( const universalTypeClass *type ) const
    { return( 1 ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base, bool commSep, bool lvalue,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class effectClass
{
public:
  // members
  int dimension;

  // constructors
  effectClass() { dimension = 1; }
  virtual ~effectClass() { }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const = 0;
};


class effectListClass : public effectClass
{
public:
  // members
  const list<const effectClass*> *effectList;

  // constructors
  effectListClass( const list<const effectClass*> *eList )
    { effectList = eList; }
  virtual ~effectListClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class procedureEffectClass : public effectClass
{
public:
  // members
  const char* procedureIdent;

  // constructors
  procedureEffectClass( const char *ident )
    { procedureIdent = ident; }
  ~procedureEffectClass()
    { free( (void*)procedureIdent ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class whenEffectClass : public effectClass
{
public:
  // members
  const formulaClass *formula;
  const effectClass *effect;

  // constructors
  whenEffectClass( const formulaClass *form, const effectClass *eff )
    { formula = form; effect = eff; }
  virtual ~whenEffectClass()
    { delete formula; delete effect; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class forEachEffectClass : public effectClass
{
public:
  // members
  const char *variable;
  const simpleTypeClass *varType;
  const effectListClass *effectList;

  // constructors
  forEachEffectClass( const char *var, const simpleTypeClass *type, const effectListClass *eList )
    { variable = var; varType = type; effectList = eList; }
  virtual ~forEachEffectClass()
    { free( (void*)variable ); delete varType; delete effectList; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class termSetEffectClass : public effectClass
{
public:
  // members
  const termClass *lvalue;
  const termClass *rvalue;

  // constructors
  termSetEffectClass( const termClass *lv, const termClass *rv );
  virtual ~termSetEffectClass()
    { delete lvalue; delete rvalue; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class predicateSetEffectClass : public effectClass
{
public:
  // members
  const predicateTermClass *lvalue;
  const formulaClass *rvalue1;
  const termClass *rvalue2;

  // constructors
  predicateSetEffectClass( const char *ident, const termClass *argument, const formulaClass *form );
  predicateSetEffectClass( const char *ident, const termClass *argument, const termClass *term );
  virtual ~predicateSetEffectClass()
    { delete lvalue; delete rvalue1; delete rvalue2; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class printEffectClass : public effectClass
{
public:
  // members
  const formulaClass *formula;
  const termClass *term;

  // constructors
  printEffectClass( const formulaClass *form )
    { formula = form; term = NULL; }
  printEffectClass( const termClass *t )
    { formula = NULL; term = t; }
  virtual ~printEffectClass()
    { delete formula; delete term; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class printStateEffectClass : public effectClass
{
public:
  // constructors
  printStateEffectClass() { }
  virtual ~printStateEffectClass() { }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class initTermSetEffectClass : public effectClass
{
public:
  // members
  const termClass *lvalue;
  const list<const termClass*> *termList;
  const formulaClass *assertion;

  // constructors
  initTermSetEffectClass( const termClass *lv, const list<const termClass*> *tList, 
			  const formulaClass *ass );
  initTermSetEffectClass( const termClass *lv, const char *min, const char *max, 
			  const formulaClass *ass );
  virtual ~initTermSetEffectClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class initPredicateSetEffectClass : public effectClass
{
public:
  // members
  const termClass *lvalue;
  const list<const termClass*> *termList;
  const formulaClass *assertion;

  // constructors
  initPredicateSetEffectClass( const char *ident, const termClass *argument, 
			       const list<const termClass*> *tList, const formulaClass *ass );
  virtual ~initPredicateSetEffectClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class oneOfEffectClass : public effectClass
{
public:
  // members
  const list<const effectClass*> *effectList;

  // constructiors
  oneOfEffectClass( const list<const effectClass*> *eList )
    { effectList = eList; dimension = effectList->size(); }
  virtual ~oneOfEffectClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class formulaInitEffectClass : public effectClass
{
public:
  // members
  const formulaClass *formula;

  // constructiors
  formulaInitEffectClass( const formulaClass *form )
    { formula = form; }
  virtual ~formulaInitEffectClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class probabilisticEffectClass : public effectClass
{
public:
  // members
  const list<pair<const char*,const effectClass*>*> *effectList;

  // constructors
  probabilisticEffectClass( const list<pair<const char*,const effectClass*>*> *eList )
    { effectList = eList; }
  virtual ~probabilisticEffectClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class nonDeterministicEffectClass : public effectClass
{
public:
  // members
  const list<const effectClass*> *effectList;

  // constructors
  nonDeterministicEffectClass( const list<const effectClass*> *eList )
    { effectList = eList; }
  virtual ~nonDeterministicEffectClass();

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, const char *base1, const char *base2,
			     map<const char*,const char*,ltstr> &environment ) const;
};


class costClass
{
public:
  // destructor
  virtual ~costClass() { }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent,
			     map<const char*,const char*,ltstr> &environment ) const = 0;
};


#if 0
class constantCostClass : public costClass
{
public:
  // members
  const char *cost;

  // constructors
  constantCostClass( const char *c )
    { cost = c; }
  virtual ~constantCostClass()
    { free( (void*)cost ); }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, 
			     map<const char*,const char*,ltstr> &environment ) const;
};
#endif


class termCostClass : public costClass
{
public:
  // members
  const termClass *cost;

  // constructors
  termCostClass( const termClass *c );
  virtual ~termCostClass()
    { delete cost; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, 
			     map<const char*,const char*,ltstr> &environment ) const;
};


#if 0
class conditionalCostClass : public costClass
{
public:
  // members
  const formulaClass *formula;
  const costClass *cost1;
  const costClass *cost2;

  // constructors
  conditionalCostClass( const formulaClass *form, const costClass *c1, const costClass *c2 )
    { formula = form; cost1 = c1; cost2 = c2; }
  virtual ~conditionalCostClass()
    { delete formula; delete cost1; delete cost2; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out, int indent, 
			     map<const char*,const char*,ltstr> &environment ) const;
};
#endif


class observationClass
{
public:
  // constructors
  observationClass() { }
  virtual ~observationClass() { }

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const = 0;
  virtual int  number( const problemClass *problem ) const = 0;
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const = 0;
};


class observationListClass : public observationClass
{
public:
  // members
  const list<const observationClass*> *observationList;

  // constructors
  observationListClass( const list<const observationClass*> *oList )
    { observationList = oList; }
  virtual ~observationListClass();

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const;
  virtual int  number( const problemClass *problem ) const
    { return( 1 ); }
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const;
};


class formulaObservationClass : public observationClass
{
public:
  // members
  const formulaClass *formula;

  // constructors
  formulaObservationClass( const formulaClass *form )
    { formula = form; }
  virtual ~formulaObservationClass()
    { delete formula; }

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const
    { return( 1 ); }
  virtual int  number( const problemClass *problem ) const
    { return( 1 ); }
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const;
};


class termObservationClass : public observationClass
{
public:
  // members
  const termClass *term;

  // constructors
  termObservationClass( const termClass *t )
    { term = t; }
  virtual ~termObservationClass()
    { delete term; }

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const
    { return( 1 ); }
  virtual int  number( const problemClass *problem ) const
    { return( 1 ); }
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const;
};


class conditionalFormulaObservationClass : public observationClass
{
public:
  // members
  const formulaClass *formula;
  const formulaObservationClass *observation;

  // constructors
  conditionalFormulaObservationClass( const formulaClass *form, const formulaClass *obs )
    { formula = form; observation = new formulaObservationClass( obs ); }
  virtual ~conditionalFormulaObservationClass()
    { delete formula; delete observation; }

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const
    { return( observation->size( problem ) ); }
  virtual int  number( const problemClass *problem ) const
    { return( 1 ); }
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const;
};


class conditionalTermObservationClass : public observationClass
{
public:
  // members
  const formulaClass *formula;
  const termObservationClass *observation;

  // constructors
  conditionalTermObservationClass( const formulaClass *form, const termClass *obs )
    { formula = form; observation = new termObservationClass( obs ); }
  virtual ~conditionalTermObservationClass()
    { delete formula; delete observation; }

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const
    { return( observation->size( problem ) ); }
  virtual int  number( const problemClass *problem ) const
    { return( 1 ); }
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const;
};


class vectorObservationClass : public observationClass
{
public:
  // members
  const char *variable;
  const simpleTypeClass *varType;
  const termClass *term;
  const list<map<const char*,const typeClass*,ltstr>*> *environment;

  // constructors
  vectorObservationClass( const char *var, const simpleTypeClass *vtype, const termClass *t,
			  list<map<const char*,const typeClass*,ltstr>*> *env )
    { variable = var; varType = vtype; term = t; environment = env; }
  virtual ~vectorObservationClass()
    { free( (void*)variable ); delete varType; delete term; }

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const;
  virtual int  number( const problemClass *problem ) const
    { return( 1 ); }
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const;
};


class probabilisticObservationClass : public observationClass
{
public:
  // members
  const list<pair<const char*,const observationListClass*>*> *observationList;

  // constructors
  probabilisticObservationClass( const list<pair<const char*,const observationListClass*>*> *oList )
    { observationList = oList; }
  virtual ~probabilisticObservationClass();

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const;
  virtual int  number( const problemClass *problem ) const
    { return( observationList->size() ); }
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const;
};


class nonDeterministicObservationClass : public observationClass
{
public:
  // members
  const list<const observationListClass*> *observationList;

  // constructors
  nonDeterministicObservationClass( const list<const observationListClass*> *oList )
    { observationList = oList; }
  virtual ~nonDeterministicObservationClass();

  // virtual methods (code generation)
  virtual int  size( const problemClass *problem ) const;
  virtual int  number( const problemClass *problem ) const
    { return( observationList->size() ); }
  virtual void generateCode( FILE *out, int indent, const char *prefix ) const;
};


class namedTypeClass
{
public:
  // members
  const char *ident;
  const list<const char*> *ancestorList;

  // constructors
  namedTypeClass( const char *id, const list<const char*> *aList )
    { ident = id; ancestorList = aList; }
  namedTypeClass( const namedTypeClass &named );
  ~namedTypeClass();
};


class ramificationClass
{
public:
  // members
  const char *ident;
  list<pair<const char*,const typeClass*> > parameters;
  list<map<const char*,pair<const char*,const typeClass*>,ltstr> > instantiationList;

  // ctor/dtor
  ramificationClass() { }
  ramificationClass( const char *id, const list<pair<const char*,const typeClass*> > &par );
  virtual ~ramificationClass()
    { free( (void*)ident ); }

  // methods (code generation)
  void generateRamificationCode( FILE *out ) const;
  void generateInstantiations( FILE *out );

  // for domain/problem analysis
  set<const char*,ltstr> dirtyPredicates;
  set<const char*,ltstr> dirtyFunctions;

  // virtual methods (code generation)
  virtual void generateCode( FILE *out ) const = 0;
};


class formulaRamificationClass : public ramificationClass
{
public:
  // members
  const formulaClass *formula;

  // constructors
  formulaRamificationClass( const char *id, 
			    const list<pair<const char*,const typeClass*> > &par,
			    const formulaClass *form );
  virtual ~formulaRamificationClass()
    { delete formula; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out ) const;
};


class effectRamificationClass : public ramificationClass
{
public:
  // members
  const effectClass *effect;

  // constructors
  effectRamificationClass( const char *id,
			   const list<pair<const char*,const typeClass*> > &par,
			   const effectClass *form );
  virtual ~effectRamificationClass()
    { delete effect; }

  // virtual methods (code generation)
  virtual void generateCode( FILE *out ) const;
};


class internalFunctionClass
{
public:
  // members
  const char *ident;
  const termClass *term;
  list<pair<const char*,const typeClass*> > parameters;

  // ctor/dtor
  internalFunctionClass() { }
  internalFunctionClass( const char *id, 
			 const list<pair<const char*,const typeClass*> > &par, 
			 const termClass* t );
  ~internalFunctionClass();

  // methods (code generation)
  void generateCode( FILE *out ) const;
};


class internalPredicateClass
{
public:
  // members
  const char *ident;
  const formulaClass *formula;
  list<pair<const char*,const typeClass*> > parameters;

  // ctor/dtor
  internalPredicateClass() { }
  internalPredicateClass( const char *id, const list<pair<const char*,const typeClass*> > &par, 
			  const formulaClass* form );
  ~internalPredicateClass();

  // methods (code generation)
  void generateCode( FILE *out ) const;
};


class actionClass
{
public:
  // members
  const char *ident;
  int numResultingStates;
  const formulaClass *precondition;
  const effectClass  *effect;
  const costClass *cost;
  const observationClass *observation;
  list<pair<const char*,const typeClass*> > parameters;
  list<map<const char*,pair<const char*,const typeClass*>,ltstr> > instantiationList;

  // for domain/problem analysis
  set<const char*,ltstr> dirtyPredicates;
  set<const char*,ltstr> dirtyFunctions;

  // constructors
  actionClass( const char *ident );
  ~actionClass();

  // methods (code generation)
  void generatePreconditionCode( FILE *out, int indent ) const;
  void generateEffectCode( FILE *out, int indent ) const;
  void generateActionCode( FILE *out ) const;
  void generateCostCode( FILE *out ) const;
  void generateObservationCode( FILE *out ) const;
  void generateInstantiations( FILE *out );
};


class arrayClass
{
public:
  // members
  const char *ident;
  const char *size;
  const typeClass  *base;

  // constructors
  arrayClass( const char *ident, const char *size, const typeClass *base );
  arrayClass( const arrayClass& array );
  ~arrayClass()
    { free( (void*)ident ); free( (void*)size ); delete base; }
};


class specialFunctionClass : public functionClass
{
public:
  // members
  const termListClass *extraArgument;

  // constructors
  specialFunctionClass( const char *ident, const typeClass *domain, 
			const typeClass *range, const termListClass *extraArg );
  virtual ~specialFunctionClass()
    { delete extraArgument; }
};


class hookClass
{
public:
  // members
  int hookType;
  const char *ident;

  // constructors
  hookClass( int hType, const char *id );
};


typedef union
{
  int                                                    token;
  bool                                                   boolean;
  const codeChunkClass                                   *codeChunk;
  const char                                             *ident;
  const hookClass                                        *hook;
  const typeClass                                        *type;
  const termClass                                        *term;
  const formulaClass                                     *formula;
  const effectClass                                      *effect;
  const observationClass                                 *observation;
  const costClass                                        *cost;
  const specialFunctionClass                             *sfunction;

  struct { const char *min, *max; }                      integer;
  pair<const char*,const effectClass*>                   *probEffect;
  pair<const char*,const observationListClass*>          *probObs;

  list<const char*>                                      *identList;
  list<const formulaClass*>                              *formulaList;
  list<const typeClass*>                                 *typeList;
  list<const termClass*>                                 *termList;
  list<const effectClass*>                               *effectList;
  list<const observationClass*>                          *obsList;
  list<const observationListClass*>                      *obsListList;
  list<pair<const char*,const observationListClass*>*>   *probObsList;
  list<pair<const char*,const effectClass*>*>            *probEffectList;
  list<const codeChunkClass*>                            *codeChunkList;
} YYSTYPE_T;


#define YYSTYPE  YYSTYPE_T


extern functionClass*  newUniversalFunction( void );
extern predicateClass* newUniversalPredicate( void );


//
// rest of inlined functions
//

inline
atomFormulaClass::~atomFormulaClass()
{
  delete predicate; 
  delete termList;
}


inline equalFormulaClass::~equalFormulaClass()
{
  delete lterm;
  delete rterm;
}


#endif /* _daeTypes_INCLUDE_ */
