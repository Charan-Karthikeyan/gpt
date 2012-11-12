//  Theseus
//  theUtils.h -- Utilities Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theUtils_INCLUDE_
#define _theUtils_INCLUDE_

#include <sys/time.h>
#include <map>
#include <set>
#include <vector>

void     safeRead( void *buff, const unsigned size, const unsigned num, istream& is );
void     safeWrite( const void *buff, const unsigned size, const unsigned num, ostream& os );
void     diffTime( unsigned long& secs, unsigned long& usecs, struct timeval& t1, struct timeval& t2 );

void     fill( map<int,float>& m, const float *array, int dimension );


template<class T, class Comp, class Alloc>
T
randomSampling( const map<T,float,Comp,Alloc>& m )
{
  register float d;
  typename map<T,float,Comp,Alloc>::const_iterator it;

  // intended only for stochastic arrays
  d = (float)drand48();
  for( it = m.begin(); it != m.end(); ++it )
    if( (d -= (*it).second) <= 0.0 )
      return( (*it).first );

  // we shouldn't reach here
  assert( 0 );
  return( (*it).first );
}

template<class T, class Comp, class Alloc>
T
randomSampling( const set<T,Comp,Alloc>& s )
{
  register float d;
  typename set<T,Comp,Alloc>::const_iterator it;

  // intended only for stochastic arrays
  assert( !s.empty() );
  d = (float)drand48();
  for( it = s.begin(); it != s.end(); ++it )
    if( (d -= 1.0/(float)s.size()) <= 0.0 )
      return( *it );

  // we shouldn't reach here
  assert( 0 );
  return( *it );
}

template<class T, class Alloc>
T
randomSampling( const vector<pair<T,float>,Alloc>& v )
{
  register float d;
  typename vector<pair<T,float>,Alloc>::const_iterator it;

  // intended only for stochastic arrays
  d = (float)drand48();
  for( it = v.begin(); it != v.end(); ++it )
    if( (d -= (*it).second) <= 0.0 )
      return( (*it).first );

  // we shouldn't reach here
  assert( 0 );
  return( (*it).first );
}

template<class T, class Alloc>
T
randomSampling( const vector<T,Alloc>& v )
{
  register float d;
  typename vector<T,Alloc>::const_iterator it;

  // intended only for stochastic arrays
  assert( !v.empty() );
  d = (float)drand48();
  for( it = v.begin(); it != v.end(); ++it )
    if( (d -= 1.0/(float)v.size()) <= 0.0 )
      return( *it );

  // we shouldn't reach here
  assert( 0 );
  return( *it );
}

template<class T, class Comp, class Alloc>
ostream& 
operator<<( ostream& os, const set<T,Comp,Alloc>& s )
{
  T elem;
  register unsigned size;
  typename set<T,Comp,Alloc>::const_iterator it;

  size = s.size();
  safeWrite( &size, sizeof( int ), 1, os );
  for( it = s.begin(); it != s.end(); ++it )
    {
      elem = *it;
      safeWrite( &elem, sizeof( T ), 1, os );
    }
  return( os );
}

template <class T1,class T2, class Comp, class Alloc>
ostream& 
operator<<( ostream& os, const map<T1,T2,Comp,Alloc>& m )
{
  T1 index;
  T2 elem;
  register unsigned size;
  typename map<T1,T2,Comp,Alloc>::const_iterator it;

  size = m.size();
  safeWrite( &size, sizeof( int ), 1, os );
  for( it = m.begin(); it != m.end(); ++it )
    {
      index = (*it).first;
      elem = (*it).second;
      safeWrite( &index, sizeof( T1 ), 1, os );
      safeWrite( &elem, sizeof( T2 ), 1, os );
      // os << (*it).first << (*it).second;
    }
  return( os );
}

template<class T, class Alloc>
ostream& 
operator<<( ostream& os, const vector<T,Alloc>& v )
{
  T elem;
  register unsigned size;
  typename vector<T,Alloc>::const_iterator it;

  size = v.size();
  safeWrite( &size, sizeof( int ), 1, os );
  for( it = v.begin(); it != v.end(); ++it )
    {
      elem = *it;
      safeWrite( &elem, sizeof( T ), 1, os );
    }
  return( os );
}

template<class T, class Comp, class Alloc>
istream&
operator>>( istream& is, set<T,Comp,Alloc>& s )
{
  T elem;
  register unsigned i, n;

  safeRead( &n, sizeof( int ), 1, is );
  for( i = 0; i < n; ++i )
    {
      safeRead( &elem, sizeof( T ), 1, is );
      s.insert( elem );
    }
  return( is );
}

template <class T1,class T2, class Comp, class Alloc>
istream&
operator>>( istream& is, map<T1,T2,Comp,Alloc>& m )
{
  T1 index;
  T2 elem;
  register unsigned i, n;

  safeRead( &n, sizeof( int ), 1, is );
  for( i = 0; i < n; ++i )
    {
      safeRead( &index, sizeof( T1 ), 1, is );
      safeRead( &elem, sizeof( T2 ), 1, is );
      m.insert( make_pair( index, elem ) );
    }
  return( is );
}

template<class T, class Alloc>
istream&
operator>>( istream& is, vector<T,Alloc>& v )
{
  T elem;
  register unsigned i, n;

  safeRead( &n, sizeof( int ), 1, is );
  for( i = 0; i < n; ++i )
    {
      safeRead( &elem, sizeof( T ), 1, is );
      v.push_back( elem );
    }
  return( is );
}

template<class T, class Alloc>
typename vector<T,Alloc>::iterator
find_in_vector( vector<T,Alloc>&v, int i )
{
  typename vector<T,Alloc>::iterator it;
  for( it = v.begin(); it != v.end(); ++it )
    if( (*it).first == i )
      break;
  return( it );
}

template<class T, class Alloc>
typename vector<T,Alloc>::const_iterator
find_in_vector( const vector<T,Alloc>&v, int i )
{
  typename vector<T,Alloc>::const_iterator it;
  for( it = v.begin(); it != v.end(); ++it )
    if( (*it).first == i )
      break;
  return( it );
}

#endif // _theUtils_INCLUDE
