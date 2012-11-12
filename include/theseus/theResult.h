//  Theseus
//  theResult.cc -- Result Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theResult_INCLUDE_
#define _theResult_INCLUDE_

#include <sys/time.h>
#include <list>

#include <theseus/theRtStandard.h>


///////////////////////////////////////////////////////////////////////////////
//
// Result Class
//

class resultClass
{
  // private methods
  resultClass( const resultClass& result ) { }

public:
  int   run;
  int   runType;
  int   numMoves;
  float costToGoal;
  float discountedCostToGoal;
  float initialValue;
  bool  solved;
  struct timeval startTime;
  struct timeval stopTime;
  unsigned long secs;
  unsigned long usecs;

  // constructor/destructor
  resultClass()
    {
      numMoves = -1;
      secs = (unsigned long)-1;
      usecs = (unsigned long)-1;
    }
  virtual ~resultClass( void ) = 0;

  // methods 
  void startTimer( void );
  void stopTimer( void );
  unsigned long getElapsedSeconds( void );
  unsigned long getElapsedUSeconds( void );

  // virtual methods
  virtual void clean( void ) = 0;
  virtual void print( ostream& os, int outputLevel, problemHandleClass *pHandle ) = 0;
  virtual void push_back( int state, int action, int observation ) = 0;
  virtual void push_front( int state, int action, int observation ) = 0;
  virtual void pop_back( void ) = 0;
  virtual void pop_front( void ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
// Standard Result Class
//

class standardResultClass : public resultClass
{
private:
  // private methods
  standardResultClass( const standardResultClass& result ) { }

protected:
  // members
  struct stepClass {
    int state;
    int action;
    int observation;

    stepClass( int s, int a, int o )
      { state = s; action = a; observation = o; }
  };

  list<stepClass> stepList;

public:
  // constructors/destructors
  standardResultClass() { }
  virtual ~standardResultClass() { clean(); }

  // virtual methods
  virtual void clean( void )
    {
      secs = (unsigned long)-1;
      usecs = (unsigned long)-1;
      stepList.clear();
    }
  virtual void print( ostream& os, int outputLevel, problemHandleClass *pHandle );
  virtual void push_back( int state, int action, int observation )
    {
      stepClass step( state, action, observation );
      stepList.push_back( step );
      ++numMoves;
    }
  virtual void pop_back( void )
    {
      stepList.pop_back();
      --numMoves;
    }
  virtual void push_front( int state, int action, int observation )
    {
      stepClass step( state, action, observation );
      stepList.push_front( step );
      ++numMoves;
    }
  virtual void pop_front( void )
    {
      stepList.pop_front();
      --numMoves;
    }
};


///////////////////////////////////////////////////////////////////////////////
//
// Search Result Class
//

class searchResultClass : public standardResultClass
{
public:
  // constructors
  searchResultClass() : standardResultClass() { }
  virtual ~searchResultClass() { clean(); }

  // virtual methods
  virtual void print( ostream& os, int outputLevel, problemHandleClass *pHandle );
};

#endif // _theResult_INCLUDE
