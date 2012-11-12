//  Theseus
//  theUtils.h -- Utilities Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theUtils_INCLUDE_
#define _theUtils_INCLUDE_

#include <assert.h>
#include <sys/time.h>
#include <map>
#include <set>
#include <vector>
#include <cstdlib>

void     safeRead( void *buff, const unsigned size, const unsigned num, istream& is );
void     safeWrite( const void *buff, const unsigned size, const unsigned num, std::ostream& os );
void     diffTime( unsigned long& secs, unsigned long& usecs, struct timeval& t1, struct timeval& t2 );

void     fill( std::map<int,float>& m, const float *array, int dimension );

template<class T, class Comp, class Alloc> T
randomSampling( const std::map<T,float,Comp,Alloc>& m )
{
  // intended only for stochastic arrays
  register float d = (float)drand48();
  for( typename std::map<T,float,Comp,Alloc>::const_iterator it = m.begin(); it != m.end(); ++it ) {
    if( (d -= (*it).second) <= 0.0 ) return((*it).first);
  }
  assert(0);
}

template<class T, class Comp, class Alloc> T
randomSampling( const std::set<T,Comp,Alloc>& s )
{
  // intended only for stochastic arrays
  assert(!s.empty());
  register float d = (float)drand48();
  for( typename std::set<T,Comp,Alloc>::const_iterator it = s.begin(); it != s.end(); ++it ) {
    if( (d -= 1.0/(float)s.size()) <= 0.0 ) return(*it);
  }
  assert(0);
}

template<class T, class Alloc> T
randomSampling( const std::vector<pair<T,float>,Alloc>& v )
{
  // intended only for stochastic arrays
  register float d = (float)drand48();
  for( typename std::vector<pair<T,float>,Alloc>::const_iterator it = v.begin(); it != v.end(); ++it ) {
    if( (d -= (*it).second) <= 0.0 ) return((*it).first);
  }
  assert(0);
}

template<class T, class Alloc> T
randomSampling( const std::vector<T,Alloc>& v )
{
  // intended only for stochastic arrays
  assert(!v.empty());
  register float d = (float)drand48();
  for( typename std::vector<T,Alloc>::const_iterator it = v.begin(); it != v.end(); ++it ) {
    if( (d -= 1.0/(float)v.size()) <= 0.0 ) return(*it);
  }
  assert(0);
}

template<class T, class Comp, class Alloc> std::ostream& 
operator<<( std::ostream& os, const std::set<T,Comp,Alloc>& s )
{
  register unsigned size = s.size();
  safeWrite(&size,sizeof(int),1,os);
  for( typename std::set<T,Comp,Alloc>::const_iterator it = s.begin(); it != s.end(); ++it ) {
    T elem = *it;
    safeWrite(&elem,sizeof(T),1,os);
  }
  return(os);
}

template <class T1,class T2, class Comp, class Alloc> std::ostream& 
operator<<( std::ostream& os, const std::map<T1,T2,Comp,Alloc>& m )
{
  register unsigned size = m.size();
  safeWrite(&size,sizeof(int),1,os);
  for( typename std::map<T1,T2,Comp,Alloc>::const_iterator it = m.begin(); it != m.end(); ++it ) {
      T1 index = (*it).first;
      T2 elem = (*it).second;
      safeWrite(&index,sizeof(T1),1,os);
      safeWrite(&elem,sizeof(T2),1,os);
  }
  return(os);
}

template<class T, class Alloc> std::ostream& 
operator<<( std::ostream& os, const std::vector<T,Alloc>& v )
{
  register unsigned size = v.size();
  safeWrite( &size, sizeof( int ), 1, os );
  for( typename std::vector<T,Alloc>::const_iterator it = v.begin(); it != v.end(); ++it ) {
    T elem = *it;
    safeWrite(&elem,sizeof(T),1,os);
  }
  return(os);
}

template<class T, class Comp, class Alloc> std::istream&
operator>>( std::istream& is, std::set<T,Comp,Alloc>& s )
{
  T elem;
  unsigned n;
  safeRead(&n,sizeof(int),1,is);
  for( unsigned i = 0; i < n; ++i ) {
    safeRead(&elem,sizeof(T),1,is);
    s.insert(elem);
  }
  return(is);
}

template <class T1,class T2, class Comp, class Alloc> std::istream&
operator>>( std::istream& is, std::map<T1,T2,Comp,Alloc>& m )
{
  T1 index;
  T2 elem;
  unsigned n;
  safeRead(&n,sizeof(int),1,is);
  for( unsigned i = 0; i < n; ++i ) {
      safeRead(&index, sizeof(T1),1,is);
      safeRead(&elem, sizeof(T2),1,is);
      m.insert(make_pair(index,elem));
  }
  return(is);
}

template<class T, class Alloc> std::istream&
operator>>( std::istream& is, std::vector<T,Alloc>& v )
{
  T elem;
  unsigned n;
  safeRead(&n,sizeof(int),1,is);
  for( unsigned i = 0; i < n; ++i ) {
    safeRead(&elem,sizeof(T),1,is);
    v.push_back(elem);
  }
  return(is);
}

template<class T, class Alloc> typename std::vector<T,Alloc>::iterator
find_in_vector( std::vector<T,Alloc>&v, int i )
{
  for( typename std::vector<T,Alloc>::iterator it = v.begin(); it != v.end(); ++it ) {
    if( (*it).first == i ) return(it);
  }
  return(v.end());
}

template<class T, class Alloc> typename std::vector<T,Alloc>::const_iterator
find_in_vector( const std::vector<T,Alloc>&v, int i )
{
  for( typename std::vector<T,Alloc>::const_iterator it = v.begin(); it != v.end(); ++it ) {
    if( (*it).first == i ) return(it);;
  }
  return(v.end());
}

#endif // _theUtils_INCLUDE

