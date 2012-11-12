#ifndef _theArray_INCLUDE_
#define _theArray_INCLUDE_

#include <stdio.h>

class poolClass;
class floatArrayClass;


///////////////////////////////////////////////////////////////////////////////
//
// Iterator Classes
//

class intArrayIteratorClass
{
  // members
  int *index;
  int *data;

  // private methods
  intArrayIteratorClass( const intArrayIteratorClass& it );

public:
  // constructors
  intArrayIteratorClass( register int *index = NULL, register int *data = NULL );

  // destructors
  virtual ~intArrayIteratorClass( void );

  // methods
  void set( register int *index, register int *data );
  int  end( void );
  int& getIndex( void );
  int& getData( void );

  // operators
  void operator++( void );
  void operator--( void );
  int& operator*( void );
};


class floatArrayIteratorClass
{
  // members
  int *index;
  float *data;

  // private methods
  floatArrayIteratorClass( const floatArrayIteratorClass& it );

public:
  // constructors
  floatArrayIteratorClass( register int *index = NULL, register float *data = NULL );

  // destructors
  virtual ~floatArrayIteratorClass( void );

  // methods
  void set( register int *index, register float *data );
  int  end( void );
  int& getIndex( void );
  float& getData( void );

  // operators
  void operator++( void );
  void operator--( void );
  float& operator*( void );
};


class floatArrayArrayIteratorClass
{
  // members
  int *index;
  floatArrayClass *data;

  // private methods
  floatArrayArrayIteratorClass( const floatArrayArrayIteratorClass& it );

public:
  // constructors
  floatArrayArrayIteratorClass( register int *index = NULL, register floatArrayClass *data = NULL );

  // destructors
  virtual ~floatArrayArrayIteratorClass( void );

  // methods
  void set( register int *index, register floatArrayClass *data );
  int  end( void );
  int& getIndex( void );
  floatArrayClass& getData( void );
  void setIterator( register floatArrayIteratorClass& it );

  // operators
  void operator++( void );
  void operator--( void );
  floatArrayClass& operator*( void );
};


///////////////////////////////////////////////////////////////////////////////
//
// Array Classes
//

class intArrayClass
{
private:
  // members
  int size;
  int *idx;
  int *data;

  // private methods
  intArrayClass( const intArrayClass& array );

public:
  intArrayClass()
    {
      size = 0;
      idx = NULL;
      data = NULL;
    }

  // destructors
  virtual ~intArrayClass( void );

  // static methods
  static void initialize( void );

  // methods
  void clean( void );
  int  empty( void );
  int  entryFor( register int index );
  void fill( register int *array, register int dimension );
  void setIterator( intArrayIteratorClass& it );
  void check( void );
  void print( register FILE *file, register char *fmt );

  //operator overload
  int& operator[]( register int index );
  intArrayClass& operator=( register const intArrayClass& array );
  int operator==( register const intArrayClass& array );

  // serialization
  void write( FILE *file );
  static intArrayClass *read( FILE *file );
  static void fill( FILE *file, intArrayClass *array );
};


class floatArrayClass
{
  // members
  int size;
  int num;
  int *idx;
  float *data;

  // static members
  static poolClass *idxPool;
  static poolClass *dataPool;

  // private methods
  floatArrayClass( const floatArrayClass& array );

public:
  // constructor
  floatArrayClass( int size = 0 );

  // destructors
  virtual ~floatArrayClass( void );

  // static methods
  static void initialize( void );

  // methods
  void clean( void );
  int  empty( void );
  int  entryFor( register int index );
  void fill( register float *array, register int dimension );
  void setIterator( floatArrayIteratorClass& it );
  int  randomSampling( void );
  void check( void );
  void print( register FILE *file, register char *fmt );

  //operator overload
  float& operator[]( register int index );
  floatArrayClass& operator=( register const floatArrayClass& array );
  int operator==( register const floatArrayClass& array );

  // serialization
  void write( FILE *file );
  static floatArrayClass *read( FILE *file );
  static void fill( FILE *file, floatArrayClass *array );

  // friends
  friend class standardBeliefClass;
  friend class standardBeliefHashClass;
  friend class floatArrayArrayIteratorClass;
};


class floatArrayArrayClass
{
  // members
  int size;
  int num;
  int *idx;
  floatArrayClass *data;

  // private methods
  floatArrayArrayClass( const floatArrayArrayClass& array );

public:
  // constructor
  floatArrayArrayClass( int size = 0 );

  // destructors
  virtual ~floatArrayArrayClass( void );

  // static methods
  static void initialize( void );

  // methods
  void clean( void );
  int  empty( void );
  int  entryFor( register int index );
  void setIterator( floatArrayArrayIteratorClass& it );
  void check( void );
  void print( register FILE *file, register char *fmt );

  //operator overload
  floatArrayClass& operator[]( register int index );
  floatArrayArrayClass& operator=( register const floatArrayArrayClass& array );
  int operator==( register const floatArrayArrayClass& array );

  // serialization
  void write( FILE *file );
  static floatArrayArrayClass *read( FILE *file );
  static void fill( FILE *file, floatArrayArrayClass *array );
};


#endif // _theArray_INCLUDE
