//  Theseus
//  theResult.cc -- Result Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>

using namespace std;

#include <theseus/theResult.h>
#include <theseus/theUtils.h>


///////////////////////////////////////////////////////////////////////////////
//
// Result Class
//

resultClass::~resultClass()
{
}

void
resultClass::startTimer()
{
  gettimeofday(&startTime,NULL);
}

void
resultClass::stopTimer()
{
  gettimeofday(&stopTime,NULL);
}

unsigned long
resultClass::getElapsedSeconds()
{
  if( secs == (unsigned long)-1 ) diffTime(secs,usecs,startTime,stopTime);
  return(secs);
}

unsigned long
resultClass::getElapsedUSeconds()
{
  if( usecs == (unsigned long)-1 ) diffTime(secs,usecs,startTime,stopTime);
  return(usecs);
}


///////////////////////////////////////////////////////////////////////////////
//
// Standard Result Class
//

void
standardResultClass::print( ostream& os, int outputLevel, problemHandleClass *pHandle )
{
  stepClass firstStep = stepList.front();
  stepClass lastStep = stepList.back();
  list<stepClass>::const_iterator it;

  os << "<e=" << (float)getElapsedSeconds() + ((float)getElapsedUSeconds()) / 1000000.0 << ">:";
  os << "<r=" << run << ">:";
  os << "<t=" << (runType == 1 ? 'l' : 'c') << ">:";
  os << "<v=" << initialValue << ">:";
  os << "<l=" << (solved ? 's' : 'u') << ">:";
  os << "<i=" << firstStep.state << ">:";
  os << "<f=" << lastStep.state << ">:";

  switch( outputLevel ) {
    case 0:
      os << "<s=" << numMoves << ">:<c=" << costToGoal << ">:<d=" << discountedCostToGoal << ">";
      break;
    case 1:
      os << "<s=" << numMoves << ">:<c=" << costToGoal << ">:<d=" << discountedCostToGoal << ">:";
      if( numMoves != -1 ) {
        os << "<p=";
        for( it = stepList.begin(); it != stepList.end(); ) {
          if( pHandle != NULL ) {
            os << '<';
            (*pHandle->printAction)(os,(*it).action);
            os << ",obs" << (*it).observation;
          }
          else {
            os << "<act" << (*it).action << ",obs" << (*it).action;
          }
          ++it;
          os << (it != stepList.end() ? ">," : ">");
        }
        os << ">";
      }
      else
        os << "<p=null>";
      break;
  }
  os << endl;
}


///////////////////////////////////////////////////////////////////////////////
//
// Search Result Class
//

void
searchResultClass::print( ostream& os, int outputLevel, problemHandleClass *pHandle )
{
  list<stepClass>::const_iterator it;

  os << "<e=" << (float)getElapsedSeconds() + ((float)getElapsedUSeconds()) / 1000000.0 << ">:";
  os << "<r=" << run << ">:";
  os << "<t=" << (runType == 1 ? 'l' : 'c') << ">:";
  os << "<v=" << initialValue << ">:";

  switch( outputLevel ) {
    case 0:
      os << "<s=" << numMoves << ">:<c=" << costToGoal << ">:<d=" << discountedCostToGoal << ">";
      break;
    case 1:
      os << "<s=" << numMoves << ">:<c=" << costToGoal << ">:<d=" << discountedCostToGoal << ">:";
      if( numMoves != -1 ) {
        os << "<p=<";
        assert( pHandle != NULL );
        for( it = stepList.begin(); it != stepList.end(); ) {
          (*pHandle->printAction)(os,(*it).action);
          ++it;
          os << (it != stepList.end() ? "," : "");
        }
        os << ">>";
      }
      else
        os << "<p=null>";
      break;
  }
  os << endl;

  // print sequence of actions plan-like
  os << "%plan-begin" << endl;
  for( it = stepList.begin(); it != stepList.end(); ++it ) {
    os << "%step ";
    (*pHandle->printAction)(os,(*it).action);
    os << endl;
  }
  os << "%plan-end" << endl;
}

