//  Theseus
//  theHash.h -- Hash Implementation
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#ifndef _theHash_INCLUDE_
#define _theHash_INCLUDE_


#include <assert.h>
#include <theseus/theRegister.h>


///////////////////////////////////////////////////////////////////////////////
//
// Hash Entry Class
//

class hashEntryClass
{
  // members
  float value;
  void  *data;
  void  *extra;
  hashEntryClass *next;

public:
  // constructors
  hashEntryClass();

  // methods
  float getValue( void ) const;
  void* getData( void ) const;
  void* getExtra( void ) const;
  void  setExtra( void *extraInfo );
  float updateValue( float newValue );
  void  setValue( float newValue );

  // friends
  friend class hashClass;
};

inline
hashEntryClass::hashEntryClass()
{
  value = 0.0;
  data = NULL;
  extra = NULL;
  next = NULL;
}

inline float
hashEntryClass::getValue( void ) const
{
  return( value );
};

inline void *
hashEntryClass::getData( void ) const
{
  return( data );
};

inline void *
hashEntryClass::getExtra( void ) const
{
  return( extra );
};

inline float
hashEntryClass::updateValue( float newValue )
{
  float oldValue;

  oldValue = value;
  if( newValue > value ) value = newValue;
  return( oldValue );
};

inline void
hashEntryClass::setExtra( void *extraInfo )
{
  extra = extraInfo;
};

inline void
hashEntryClass::setValue( float newValue )
{
  value = newValue;
};


///////////////////////////////////////////////////////////////////////////////
//
// Hash Class
//

class hashClass
{
  // members
  hashEntryClass **dataTable;

  unsigned hashSize;
  unsigned initialSize;

  unsigned *diameter;
  unsigned numElem;
  unsigned lookups;
  unsigned hits;

  // registeredHashes
  static registerClass *registeredElements;

  // default hash table has initially 27449 buckets (must be a prime)
  static const unsigned defaultInitialSize = 27449;

  // private methods 
  hashClass( const hashClass& hash ) { }

public:
  // constructors
  hashClass( unsigned isize = hashClass::defaultInitialSize );

  // destructor
  virtual ~hashClass();

  // methods
  unsigned          getNumElem( void ) { return( numElem ); }
  unsigned          getLookups( void ) { return( lookups ); }
  unsigned          getHits( void )    { return( hits ); }

  hashEntryClass*   lookup( const void *data );
  hashEntryClass*   insert( const void *data, const float value );
  hashEntryClass*   insert( const void *data, const float value, void *extra );
  hashEntryClass*   updateValue( const void *data, float value );
  void              setExtra( const void *data, void *extra );
  void              print( ostream& os );
  void              statistics( ostream& os );

  // virtual methods
  virtual void      clean( void );
  virtual unsigned  dataFunction( const void *data ) = 0;
  virtual void*     dataCopy( const void *data ) = 0;
  virtual bool      dataCompare( const void *newData, const void *dataInHash ) = 0;
  virtual void      dataDelete( void *data );
  virtual void      dataPrint( ostream& os, const void *data ) = 0;
  virtual void      dataWrite( ostream& os, const void *data ) const = 0;
  virtual void*     dataRead( istream& is ) = 0;

  // serialization
  static  void      signRegistration( const char *registrationId,
				      void (*fillFunction)( istream& is, hashClass * ), 
				      hashClass *(*constructor)( void ) );
  static  void      cleanup( void );  
  virtual void      write( ostream& is ) const;
  static hashClass* read( istream& is );
  static void       fill( istream& is, hashClass *hash );
};

inline hashEntryClass*
hashClass::insert( const void *data, const float value )
{
  hashEntryClass *entry;
  unsigned index;

  assert( lookup( data ) == NULL );

  index = dataFunction( data ) % hashSize;
  entry = new hashEntryClass;

  entry->data = dataCopy( data );
  entry->value = value;
  entry->next = dataTable[index];
  dataTable[index] = entry;

  // statistics
  ++numElem;
  ++diameter[index];

  // return
  return( entry );
}

inline hashEntryClass*
hashClass::insert( const void *data, const float value, void *extra )
{
  hashEntryClass *entry;

  entry = insert( data, value );
  entry->extra = extra;
  return( entry );
}

inline hashEntryClass*
hashClass::updateValue( const void *data, float newValue )
{
  hashEntryClass *entry;

  if( (entry = lookup( data )) == NULL )
    entry = insert( data, newValue );
  else
    entry->updateValue( newValue );
  return( entry );
}

inline void
hashClass::setExtra( const void *data, void *extra )
{
  hashEntryClass *entry;

  if( (entry = lookup( data )) != NULL )
    entry->setExtra( extra );
}


#endif // _theHash_INCLUDE
