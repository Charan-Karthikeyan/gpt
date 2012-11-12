//  Theseus
//  theMemoryMgmt.h -- Memory Management Implemetantion
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theMemoryMgmt_INCLUDE_
#define _theMemoryMgmt_INCLUDE_

#include <stdio.h>
#include <assert.h>
#include <set>
#include <memory>
#include <cstdlib>
#include <cstring>

#define MAXSZ    1024
//#define PRINTINFO
//#define MDEBUG


namespace std {


class memoryExceptionClass
{
 public:
  memoryExceptionClass() { }
};

template<int inst>
class __gpt_memory_control
{
private:
  class chunk_t
  {
  public:
    chunk_t *next;
    char    *ptr;
  };
  static chunk_t** free_list;
  static chunk_t*  chunk_free_list;
  static size_t heapUsage;

#ifdef MDEBUG
  static set<char*> memoryAllocations;
#endif

  static chunk_t* get_chunk_node( char *ptr, chunk_t *nchunk )
    {
      chunk_t *chu;
      if( !chunk_free_list )
	{
	  if( !(chu = (chunk_t*)malloc(sizeof(chunk_t))) )
	    throw memoryExceptionClass();
	}
      else
	{
	  chu = chunk_free_list;
	  chunk_free_list = chu->next;
	}
      chu->ptr = ptr;
      chu->next = nchunk;
      return( chu );
    }

  static void free_chunk_node( chunk_t *chunk )
    {
      chunk->ptr = NULL;
      chunk->next = chunk_free_list;
      chunk_free_list = chunk;
    }

public:
  static void *allocate( size_t n )
    {
      char *ptr;
      size_t sz = n + sizeof(size_t);

      // either get chunk from free list or using malloc
      if( (sz < MAXSZ) && free_list && free_list[sz] )
	{
	  assert( *(size_t*)free_list[sz]->ptr == sz );
	  ptr = free_list[sz]->ptr + sizeof(size_t);
	  chunk_t *chunk = free_list[sz];
	  free_list[sz] = chunk->next;
	  free_chunk_node( chunk );
#if defined(MDEBUG) && defined(PRINTINFO)
	  fprintf( stderr, "\n" );
#endif
	}
      else
	{
	  if( !(ptr = (char*)malloc(sz)) )
	    throw memoryExceptionClass();
	  ((size_t*)ptr)[0] = sz;
	  heapUsage += sz;
#ifdef MDEBUG
	  memoryAllocations.insert( ptr );
#  ifdef PRINTINFO
	  fprintf( stderr, "__gpt_memory_control: allocate for %p(%u).\n", ptr, sz );
#  endif
#endif
	  ptr += sizeof(size_t);
	}
      return( (void*)ptr );
    }

  static void deallocate( void *p )
    {
      char *ptr = (char*)p - sizeof(size_t);
      size_t sz = *(size_t*)ptr;

      // either insert chunk into free list or free it
      if( sz < MAXSZ )
	{
	  if( !free_list && !(free_list = (chunk_t**)calloc(MAXSZ,sizeof(chunk_t*))) )
	    throw memoryExceptionClass();
	  free_list[sz] = get_chunk_node( ptr, free_list[sz] );
	  assert( *(size_t*)free_list[sz]->ptr == sz );
#if defined(MDEBUG) && defined(PRINTINFO)
	  fprintf( stderr, "\n" );
#endif
	}
      else
	{
#ifdef MDEBUG
	  set<char*>::iterator it;
	  if( (it = memoryAllocations.find(ptr)) == memoryAllocations.end() )
	    fprintf( stderr, "Error: bad argument to __gpt_memory_control::deallocate: %p.\n", ptr );
	  else
	    {
#  ifdef PRINTINFO
	      fprintf( stderr, "__gpt_memory_control: deallocate for %p(%u).\n", ptr, sz );
#  endif
	      memoryAllocations.erase( it );
	    }
#endif
	  heapUsage -= sz;
	  free( ptr );
	}
    }

  static void *reallocate( void *p, size_t n )
    {
      void *ptr2 = 0;
      if( !p )
	{
	  ptr2 = allocate(n);
	}
      else if( n == 0 )
	{
	  deallocate(p);
	  ptr2 = NULL;
	}
      else
	{
	  // recover ptr and compute new size
	  char* ptr = (char*)p - sizeof(size_t);
	  size_t sz = *(size_t*)ptr;
	  size_t sz2 = n + sizeof(size_t);

	  if( sz == sz2 )
	    ptr2 = ptr;
	  else
	    {
	      assert( sz2 > sz );
	      if( sz2 < MAXSZ )
		{
		  ptr2 = allocate(n);
		  assert( ((size_t*)ptr2 - sizeof(size_t))[0] == sz2 );
		  memcpy( p, ptr2, sz - sizeof(size_t) );
		  deallocate(p);
		}
	      else
		{
		  if( !(ptr2 = (char*)realloc(ptr,sz2)) )
		    throw memoryExceptionClass();
		  ((size_t*)ptr2)[0] = sz2;
		  heapUsage += sz2 - sz;
#ifdef MDEBUG
		  if( (it = memoryAllocations.find(ptr)) == memoryAllocations.end() )
		    fprintf( stderr, "__gpt_memory_control: unallocated ptr %p in ::reallocate.\n",ptr);
		  else
		    {
#  ifdef PRINTINFO
		      fprintf( stderr, "__gpt_memory_control: reallocate for %p(%u) to %p(%u).\n", 
			       ptr, sz, ptr2, sz2 );
#  endif
		      memoryAllocations.erase( it );
		      memoryAllocations.insert( ptr2 );
		    }
#endif
		}
	    }
	}
      return( (void*)ptr2 );
    }

  static void memoryUsage( void )
    {
      fprintf( stderr, "__gpt_memory_control: current allocation = %u.\n", heapUsage );
    }

  static void memoryReport( bool flag )
    {
#if 0
      size_t sum = 0;
      set<char*>::const_iterator it;
      for( it = memoryAllocations.begin(); it != memoryAllocations.end(); ++it )
	{
	  size_t sz = *(size_t*)(*it);
	  sum += sz;
	  if( flag )
	    fprintf( stderr, "__gpt_memory_control: unallocated chunk %p of size %u.\n", *it, sz );
	}
      fprintf( stderr, "__gpt_memory_control: total unallocated bytes = %u.\n", sum );
#endif
    }
};

template<int inst>
typename __gpt_memory_control<inst>::chunk_t** __gpt_memory_control<inst>::free_list = NULL;
template<int inst>
typename __gpt_memory_control<inst>::chunk_t* __gpt_memory_control<inst>::chunk_free_list = NULL;
template<int inst> 
size_t __gpt_memory_control<inst>::heapUsage = 0;

#ifdef MDEBUG
template<int inst> 
set<char*> __gpt_memory_control<inst>::memoryAllocations;
#endif


// STL allocator class

template <class M>
class __gpt_alloc_template {
private:
  static void* oom_malloc(size_t);
  static void* oom_realloc(void *, size_t);
  static void (*__gpt_alloc_oom_handler)();

public:
  static void* allocate(size_t n)
    {
      void *result = M::allocate(n);
      if( !result )
	result = oom_malloc(n);
      return( result );
    }

  static void deallocate(void *p, size_t /* n */)
    {
      M::deallocate(p);
    }

  static void* reallocate(void *p, size_t /* old_sz */, size_t new_sz)
    {
      void * result = M::reallocate(p,new_sz);
      if( !result )
	result = oom_realloc(p,new_sz);
      return( result );
    }

  static void (*set_malloc_handler(void (*f)()))()
    {
      void (*old)() = __gpt_alloc_oom_handler;
      __gpt_alloc_oom_handler = f;
      return( old );
    }
};

template <class M>
void (*__gpt_alloc_template<M>::__gpt_alloc_oom_handler)() = 0;

template <class M>
void* __gpt_alloc_template<M>::oom_malloc( size_t n )
{
  void (* my_malloc_handler)();
  void *result;
  for(;;)
    {
      my_malloc_handler = __gpt_alloc_oom_handler;
      if( !my_malloc_handler )
	throw memoryExceptionClass();
      (*my_malloc_handler)();
      result = M::allocate(n);
      if( result )
	return( result );
    }
}

template <class M>
void* __gpt_alloc_template<M>::oom_realloc(void *p, size_t n)
{
  void (* my_malloc_handler)();
  void *result;
  for(;;)
    {
      my_malloc_handler = __gpt_alloc_oom_handler;
      if( !my_malloc_handler )
	throw memoryExceptionClass();
      (*my_malloc_handler)();
      result = M::reallocate(p,n);
      if( result )
	return( result );
    }
}

typedef __gpt_alloc_template<__gpt_memory_control<0> > gpt_alloc;

#if 0

//
// STL Alloc_traits
//

// Versions for the predefined SGI-style allocators.

template <class _Tp, class __M>
struct _Alloc_traits<_Tp, __gpt_alloc_template<__M> >
{
  static const bool _S_instanceless = true;
#ifdef GNU_CC
#  if __GNUC__ == 2
     typedef simple_alloc<_Tp, __gpt_alloc_template<__M> > _Alloc_type;
#  else
     typedef __simple_alloc<_Tp, __gpt_alloc_template<__M> > _Alloc_type;
#  endif
#else
   typedef simple_alloc<_Tp, __gpt_alloc_template<__M> > _Alloc_type;
#endif
  typedef __allocator<_Tp, __gpt_alloc_template<__M> > allocator_type;
};

// Versions for the __allocator adaptor used with the predefined
// SGI-style allocators.

template <class _Tp, class _Tp1, class __M>
struct _Alloc_traits<_Tp,
                     __allocator<_Tp1, __gpt_alloc_template<__M> > >
{
  static const bool _S_instanceless = true;
#ifdef GNU_CC
#  if __GNUC__ == 2
     typedef simple_alloc<_Tp, __gpt_alloc_template<__M> > _Alloc_type;
#  else
     typedef __simple_alloc<_Tp, __gpt_alloc_template<__M> > _Alloc_type;
#  endif
#else
   typedef simple_alloc<_Tp, __gpt_alloc_template<__M> > _Alloc_type;
#endif
  typedef __allocator<_Tp, __gpt_alloc_template<__M> > allocator_type;
};
#endif


} // namespace


#endif // _theMemoryMgmt_INCLUDE
