//  Theseus
//  theUtils.cc -- Utilities Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <iostream>
#include <errno.h>
#include <cstring>

using namespace std;

#include <theseus/theUtils.h>

void
safeRead( void *buff, const unsigned size, const unsigned num, istream& is )
{
  is.read( (char*)buff, size * num );
  if( !is )
    {
      cerr << "Fatal Error: safeRead: " << strerror( errno ) << endl;
      exit( -1 );
    }
}

void
safeWrite( const void *buff, const unsigned size, const unsigned num, ostream& os )
{
  os.write( (char*)buff, size * num );
  if( !os )
    {
      cerr << "Fatal Error: safeWrite: " << strerror( errno ) << endl;
      exit( -1 );
    }
}

void
diffTime( unsigned long& secs, unsigned long& usecs, struct timeval& t1, struct timeval& t2 )
{
  if( t1.tv_sec == t2.tv_sec )
    {
      secs = 0;
      usecs = t2.tv_usec - t1.tv_usec;
    }
  else
    {
      secs = (t2.tv_sec - t1.tv_sec) - 1;
      usecs = (1000000 - t1.tv_usec) + t2.tv_usec;
      if( usecs > 1000000 )
	{
	  ++secs;
	  usecs = usecs % 1000000;
	}
    }
}

void
fill( map<int,float>& m, const float *array, int dimension )
{
  register int i;

  for( i = 0; i < dimension; ++i )
    {
      if( array[i] > 0.0 )
	m[i] = array[i];
      else if( m.find( i ) != m.end() )
	m.erase( i );
    }
}
