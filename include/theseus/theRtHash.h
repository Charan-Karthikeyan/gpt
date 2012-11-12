//  Theseus
//  theRtHash.h -- RunTime State and Observation Hash
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theRtHash_INCLUDE
#define _theRtHash_INCLUDE

#include <theseus/theHash.h>
#include <theseus/theRtStandard.h>

class stateHashClass : public hashClass
{
  // members
  int   size;
  char* buff;

  // private methods
  stateHashClass( const stateHashClass& hash )
    : hashClass( 1024 )
    {
      *this = hash;
    }

public:
  // constructors/destructors
  stateHashClass()
    : hashClass( 1024 )
    {
      size = 0;
      buff = NULL;
    }
  virtual ~stateHashClass()
    {
      clean();
      delete[] buff;
    }

  // methods
  void setSize( int sz )
    {
      size = sz;
      buff = new char[size];
    }

  // hash functions (since this is a hashClass derived class)
  virtual unsigned   dataFunction( const void *data );
  virtual void*      dataCopy( const void *data )
    {
      return( ((stateClass *)data)->clone() );
    }
  virtual bool       dataCompare( const void *data1, const void *data2 )
    {
      return( !(((stateClass *)data1)->operator==( *((stateClass *)data2) )) );
    }
  virtual void       dataDelete( void *data )
    {
      delete( (stateClass *)data );
    }
  virtual void       dataPrint( ostream& os, const void *data )
    {
      ((stateClass *)data)->print( os, 0 );
    }

  // serialization
  virtual hashClass* constructor( void )
    {
      return( new stateHashClass );
    }
  virtual void*      dataRead( istream& is );
  virtual void       dataWrite( ostream& os, const void *data ) const;
};


class invStateHashClass : public hashClass
{
  // private methods
  invStateHashClass( const invStateHashClass& hash )
    : hashClass( 1024 )
    {
      *this = hash;
    }

public:
  // constructors/destructors
  invStateHashClass() : hashClass( 1024 ) { }
  virtual ~invStateHashClass() { clean(); }

  // hash functions (since this is a hashClass derived class)
  virtual unsigned   dataFunction( const void *data )
    {
      return( (unsigned)data );
    }
  virtual void*      dataCopy( const void *data )
    {
      return( (void*)data );
    }
  virtual bool       dataCompare( const void *data1, const void *data2 )
    {
      return( !((unsigned)data1 == (unsigned)data2) );
    }
  virtual void       dataDelete( void *data ) { }
  virtual void       dataPrint( ostream& os, const void *data )
    {
      os << (unsigned)data;
    }

  // serialization
  virtual hashClass* constructor( void )
    {
      return( new invStateHashClass );
    }
  virtual void*      dataRead( istream& is );
  virtual void       dataWrite( ostream& os, const void *data ) const;
};


class observationHashClass : public hashClass
{
  // members
  int   size;
  char* buff;

  // private methods
  observationHashClass( const observationHashClass& hash )
    : hashClass( 1024 )
    {
      *this = hash;
    }

public:
  // constructors/destructors
  observationHashClass()
    : hashClass( 1024 )
    {
      size = 0;
      buff = NULL;
    }
  virtual ~observationHashClass()
    {
      clean();
      delete[] buff;
    }

  // methods
  void setSize( int sz )
    {
      size = sz;
      buff = new char[size];
    }

  // hash functions (since this is a hashClass derived class)
  virtual unsigned   dataFunction( const void *data );
  virtual void*      dataCopy( const void *data )
    {
      return( ((observationClass *)data)->clone() );
    }
  virtual bool       dataCompare( const void *data1, const void *data2 )
    {
      return( !(((observationClass *)data1)->operator==( *((observationClass *)data2) )) );
    }
  virtual void       dataDelete( void *data )
    {
      delete( (observationClass *)data );
    }
  virtual void       dataPrint( ostream& os, const void *data )
    {
      ((observationClass *)data)->print( os, 0 );
    }

  // serialization
  virtual hashClass* constructor( void )
    {
      return( new observationHashClass );
    }
  virtual void*      dataRead( istream& is );
  virtual void       dataWrite( ostream& os, const void *data ) const;
};


class invObservationHashClass : public hashClass
{
  // private methods
  invObservationHashClass( const invObservationHashClass& hash )
    : hashClass( 1024 )
    {
      *this = hash;
    }

public:
  // constructors/destructors
  invObservationHashClass() : hashClass( 1024 ) { }
  virtual ~invObservationHashClass() { clean(); }

  // hash functions (since this is a hashClass derived class)
  virtual unsigned   dataFunction( const void *data )
    {
      return( (unsigned)data );
    }
  virtual void*      dataCopy( const void *data )
    {
      return( (void*)data );
    }
  virtual bool       dataCompare( const void *data1, const void *data2 )
    {
      return( !((unsigned)data1 == (unsigned)data2) );
    }
  virtual void       dataDelete( void *data ) { }
  virtual void       dataPrint( ostream& os, const void *data )
    {
      os << (unsigned)data;
    }

  // serialization
  virtual hashClass* constructor( void )
    {
      return( new invObservationHashClass );
    }
  virtual void*      dataRead( istream& is );
  virtual void       dataWrite( ostream& os, const void *data ) const;
};

#endif   // _theRtHash_INCLUDE
