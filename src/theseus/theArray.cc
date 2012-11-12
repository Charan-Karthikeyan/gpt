#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>


///////////////////////////////////////////////////////////////////////////////
//
// intArray Iterator Class
//

intArrayIteratorClass::intArrayIteratorClass( const intArrayIteratorClass& /*it*/ )
{
}


intArrayIteratorClass::intArrayIteratorClass( register int *index, register int *data )
{
  this->index = index;
  this->data = data;
}


intArrayIteratorClass::~intArrayIteratorClass( void )
{
}


void
intArrayIteratorClass::set( register int *index, register int *data )
{
  this->index = index;
  this->data = data;
}


int
intArrayIteratorClass::end( void )
{
  return( *index == -1 );
}

int&
intArrayIteratorClass::getIndex( void )
{
  return( *index );
}

int&
intArrayIteratorClass::getData( void )
{
  return( *data );
}


void
intArrayIteratorClass::operator++( void )
{
  ++index;
  ++data;
}


void
intArrayIteratorClass::operator--( void )
{
  --index;
  --data;
}


int&
intArrayIteratorClass::operator*( void )
{
  return( *data );
}


///////////////////////////////////////////////////////////////////////////////
//
// floatArray Iterator Class
//

floatArrayIteratorClass::floatArrayIteratorClass( const floatArrayIteratorClass& /*it*/ )
{
}


floatArrayIteratorClass::floatArrayIteratorClass( register int *index, register float *data )
{
  this->index = index;
  this->data = data;
}


floatArrayIteratorClass::~floatArrayIteratorClass( void )
{
}


void
floatArrayIteratorClass::set( register int *index, register float *data )
{
  this->index = index;
  this->data = data;
}


int
floatArrayIteratorClass::end( void )
{
  return( *index == -1 );
}

int&
floatArrayIteratorClass::getIndex( void )
{
  return( *index );
}

float&
floatArrayIteratorClass::getData( void )
{
  return( *data );
}


void
floatArrayIteratorClass::operator++( void )
{
  ++index;
  ++data;
}


void
floatArrayIteratorClass::operator--( void )
{
  --index;
  --data;
}


float&
floatArrayIteratorClass::operator*( void )
{
  return( *data );

}


///////////////////////////////////////////////////////////////////////////////
//
// floatArrayArray Iterator Class
//

floatArrayArrayIteratorClass::floatArrayArrayIteratorClass( const floatArrayArrayIteratorClass& /*it*/ )
{
}


floatArrayArrayIteratorClass::floatArrayArrayIteratorClass( register int *index, register floatArrayClass *data )
{
  this->index = index;
  this->data = data;
}


floatArrayArrayIteratorClass::~floatArrayArrayIteratorClass( void )
{
}


void
floatArrayArrayIteratorClass::set( register int *index, register floatArrayClass *data )
{
  this->index = index;
  this->data = data;
}


int
floatArrayArrayIteratorClass::end( void )
{
  return( *index == -1 );
}

int&
floatArrayArrayIteratorClass::getIndex( void )
{
  return( *index );
}

floatArrayClass&
floatArrayArrayIteratorClass::getData( void )
{
  return( *data );
}


void
floatArrayArrayIteratorClass::setIterator( register floatArrayIteratorClass& it )
{
  it.set( data->idx, data->data );
}


void
floatArrayArrayIteratorClass::operator++( void )
{
  ++index;
  ++data;
}


void
floatArrayArrayIteratorClass::operator--( void )
{
  --index;
  --data;
}


floatArrayClass&
floatArrayArrayIteratorClass::operator*( void )
{
  return( *data );
}


///////////////////////////////////////////////////////////////////////////////
//
// intArray Class
//

poolClass *intArrayClass::idxPool = NULL;
poolClass *intArrayClass::dataPool = NULL;


intArrayClass::intArrayClass( const intArrayClass& /*array*/ )
{
}


intArrayClass::intArrayClass( register int size = 0 )
{
  num = 0;
  this->size = (size == 0 ? DEFAULT_SIZE : size);
  idx = (int*)idxPool->getMemoryArray( this->size );
  data = (int*)dataPool->getMemoryArray( this->size );
  idx[0] = -1;
  data[0] = -1;
}


intArrayClass::~intArrayClass( void )
{
  idxPool->freeMemory( idx );
  dataPool->freeMemory( data );
}


void
intArrayClass::initialize( void )
{
  idxPool = new poolClass( sizeof( int ) );
  dataPool = new poolClass( sizeof( int ) );
}


void
intArrayClass::clean( void )
{
  num = 0;
  *idx = -1;
}


int
intArrayClass::empty( void )
{
  return( *idx == -1 );
}


int
intArrayClass::entryFor( register int index )
{
  register int *ip;

  for( ip = idx; *ip != -1; ++ip )
    if( *ip == index )
      return( 1 );
  return( 0 );
}


void
intArrayClass::fill( register int *array, register int dimension )
{
  register int i;

  clean();
  for( i = 0; i < dimension; ++i )
    operator[]( i ) = array[i];
}


void 
intArrayClass::setIterator( intArrayIteratorClass& it )
{
  it.set( idx, data );
}


void
intArrayClass::check( void )
{
  register int *ip, n;

  for( ip = idx, n = 0; *ip != -1; ++ip, ++n );
  assert( n == num );
}
    

void
intArrayClass::print( register FILE *file, register char *fmt )
{
  register int *ip, *dp;

  for( ip = idx, dp = data; *ip != -1; ++ip, ++dp )
    fprintf( file, fmt, *ip, *dp );
}


int&
intArrayClass::operator[]( register int index )
{
  register int *ip, *dp;
  register int *newIdx, *newData;
  
  for( ip = idx, dp = data; *ip != -1; ++ip, ++dp )
    if( *ip >= index )
      break;

  if( *ip != index )
    {
      if( (num + 1) == size )
	{
	  newIdx = (int*)idxPool->getMemoryArray( 2 * size );
	  newData = (int*)dataPool->getMemoryArray( 2 * size );

	  // copy data leaving space for the new
	  memcpy( newIdx, idx, (ip - idx) * sizeof( int ) );
	  memcpy( newIdx + (ip - idx + 1), idx + (ip - idx), ((num + 1) - (ip - idx)) * sizeof( int ) );
	  memcpy( newData, data, (dp - data) * sizeof( int ) );
	  memcpy( newData + (dp - data + 1), data + (dp - data), ((num + 1) - (dp - data)) * sizeof( int ) );
	  
	  // set new pointers, free space, and set data
	  ip = newIdx + (ip - idx);
	  dp = newData + (dp - data);
	  idxPool->freeMemory( idx );
	  dataPool->freeMemory( data );

	  idx = newIdx;
	  data = newData;
	  size = 2 * size;
	}
      else
	{
	  // move data 
	  memmove( idx + (ip - idx + 1), idx + (ip - idx), ((num + 1) - (ip - idx)) * sizeof( int ) );
	  memmove( data + (dp - data + 1), data + (dp - data), ((num + 1) - (dp - data)) * sizeof( int ) );
	}
      ++num;
      *ip = index;
      *dp = 0;
    }
  check(); //xxxxx
  return( *dp );
}


intArrayClass&
intArrayClass::operator=( register const intArrayClass& array )
{
  if( size <= array.num )
    {
      idxPool->freeMemory( idx );
      dataPool->freeMemory( data );
      idx = (int*)idxPool->getMemoryArray( array.num + 1 );
      data = (int*)dataPool->getMemoryArray( array.num + 1 );
      size = array.num + 1;
    }

  num = array.num;
  memcpy( idx, array.idx, (num + 1) * sizeof( int ) );
  memcpy( data, array.data, (num + 1) * sizeof( int ) );

  check(); //xxxxxx
  return( *this );
}


int
intArrayClass::operator==( register const intArrayClass& array )
{
  return( (num == array.num) &&
	  !memcmp( idx, array.idx, num * sizeof( int ) ) &&
	  !memcmp( data, array.data, num * sizeof( int ) ) );
}


///////////////////////
//
// serialization

void
intArrayClass::write( FILE *file )
{
  int size;

  size = num + 1; 
  safeWrite( &size, sizeof( int ), 1, file );
  safeWrite( &num, sizeof( int ), 1, file );
  safeWrite( idx, sizeof( int ), size, file );
  safeWrite( data, sizeof( int ), size, file );
}


intArrayClass *
intArrayClass::read( FILE *file )
{
  intArrayClass *result;

  result = new intArrayClass;
  intArrayClass::fill( file, result );
  return( result );
}


void
intArrayClass::fill( FILE *file, intArrayClass *array )
{
  int size;

  safeRead( &size, sizeof( int ), 1, file );
  safeRead( &array->num, sizeof( int ), 1, file );

  if( array->size < size )
    {
      idxPool->freeMemory( array->idx );
      dataPool->freeMemory( array->data );
      array->idx = (int*)idxPool->getMemoryArray( size );
      array->data = (int*)dataPool->getMemoryArray( size );
    }
  safeRead( array->idx, sizeof( int ), size, file );
  safeRead( array->data, sizeof( int ), size, file );
  array->size = size;
}


///////////////////////////////////////////////////////////////////////////////
//
// floatArray Class
//

poolClass *floatArrayClass::idxPool = NULL;
poolClass *floatArrayClass::dataPool = NULL;


floatArrayClass::floatArrayClass( const floatArrayClass& /*array*/ )
{
}


floatArrayClass::floatArrayClass( register int size = 0 )
{
  num = 0;
  this->size = (size == 0 ? DEFAULT_SIZE : size);
  idx = (int*)idxPool->getMemoryArray( this->size );
  data = (float*)dataPool->getMemoryArray( this->size );
  idx[0] = -1;
  data[0] = -1.0;
}


floatArrayClass::~floatArrayClass( void )
{
  idxPool->freeMemory( idx );
  idxPool->freeMemory( data );
}


void
floatArrayClass::initialize( void )
{
  idxPool = new poolClass( sizeof( int ) );
  dataPool = new poolClass( sizeof( float ) );
}


void
floatArrayClass::clean( void )
{
  num = 0;
  *idx = -1;
}


int
floatArrayClass::empty( void )
{
  return( *idx == -1 );
}


int
floatArrayClass::entryFor( register int index )
{
  register int *ip;

  for( ip = idx; *ip != -1; ++ip )
    if( *ip == index )
      return( 1 );
  return( 0 );
}


void
floatArrayClass::fill( register float *array, register int dimension )
{
  register int i;

  clean();
  for( i = 0; i < dimension; ++i )
    if( array[i] > 0.0 )
      operator[]( i ) = array[i];
}


void 
floatArrayClass::setIterator( floatArrayIteratorClass& it )
{
  it.set( idx, data );
}


int
floatArrayClass::randomSampling( void )
{
  register float d, *dp;
  register int *ip;

  // intended only for stochastic arrays
  d = (float)drand48();
  for( ip = idx, dp = data; *ip != -1; ++ip, ++dp )
    if( (d -= *dp) <= 0.0 )
      return( *ip );

  // we can't reach here
  assert( 1 != 1 );
}


void
floatArrayClass::check( void )
{
  register int *ip, n;

  for( ip = idx, n = 0; *ip != -1; ++ip, ++n );
  assert( n == num );
}
    

void
floatArrayClass::print( register FILE *file, register char *fmt )
{
  register int *ip;
  register float *dp;

  for( ip = idx, dp = data; *ip != -1; ++ip, ++dp )
    fprintf( file, fmt, *ip, *dp );
}


float&
floatArrayClass::operator[]( register int index )
{
  register int *ip;
  register int *newIdx;
  register float *dp;
  register float *newData;
  
  for( ip = idx, dp = data; *ip != -1; ++ip, ++dp )
    if( *ip >= index )
      break;

  if( *ip != index )
    {
      if( (num + 1) == size )
	{
	  newIdx = (int*)idxPool->getMemoryArray( 2 * size );
	  newData = (float*)dataPool->getMemoryArray( 2 * size );

	  // copy data leaving space for the new
	  memcpy( newIdx, idx, (ip - idx) * sizeof( int ) );
	  memcpy( newIdx + (ip - idx + 1), idx + (ip - idx), ((num + 1) - (ip - idx)) * sizeof( int ) );
	  memcpy( newData, data, (dp - data) * sizeof( float ) );
	  memcpy( newData + (dp - data + 1), data + (dp - data), ((num + 1) - (dp - data)) * sizeof( float ) );
	  
	  // set new pointers, free space, and set data
	  ip = newIdx + (ip - idx);
	  dp = newData + (dp - data);
	  idxPool->freeMemory( idx );
	  dataPool->freeMemory( data );

	  idx = newIdx;
	  data = newData;
	  size = 2 * size;
	}
      else
	{
	  // move data 
	  memmove( idx + (ip - idx + 1), idx + (ip - idx), ((num + 1) - (ip - idx)) * sizeof( int ) );
	  memmove( data + (dp - data + 1), data + (dp - data), ((num + 1) - (dp - data)) * sizeof( float ) );
	}
      ++num;
      *ip = index;
      *dp = 0;
    }
  check(); //xxxxx
  return( *dp );
}


floatArrayClass&
floatArrayClass::operator=( register const floatArrayClass& array )
{
  if( size <= array.num )
    {
      idxPool->freeMemory( idx );
      dataPool->freeMemory( data );
      idx = (int*)idxPool->getMemoryArray( array.num + 1 );
      data = (float*)dataPool->getMemoryArray( array.num + 1 );
      size = array.num + 1;
    }

  num = array.num;
  memcpy( idx, array.idx, (num + 1) * sizeof( int ) );
  memcpy( data, array.data, (num + 1) * sizeof( float ) );

  check(); //xxxxxx
  return( *this );
}


int
floatArrayClass::operator==( register const floatArrayClass& array )
{
  return( (num == array.num) &&
	  !memcmp( idx, array.idx, num * sizeof( int ) ) &&
	  !memcmp( data, array.data, num * sizeof( float ) ) );
}


///////////////////////
//
// serialization

void
floatArrayClass::write( FILE *file )
{
  int size;

  size = num + 1; 
  safeWrite( &size, sizeof( int ), 1, file );
  safeWrite( &num, sizeof( int ), 1, file );
  safeWrite( idx, sizeof( int ), size, file );
  safeWrite( data, sizeof( float ), size, file );
}


floatArrayClass *
floatArrayClass::read( FILE *file )
{
  floatArrayClass *result;

  result = new floatArrayClass;
  floatArrayClass::fill( file, result );
  return( result );
}


void
floatArrayClass::fill( FILE *file, floatArrayClass *array )
{
  int size;
  safeRead( &size, sizeof( int ), 1, file );
  safeRead( &array->num, sizeof( int ), 1, file );

  if( array->size < size )
    {
      idxPool->freeMemory( array->idx );
      dataPool->freeMemory( array->data );
      array->idx = (int*)idxPool->getMemoryArray( size );
      array->data = (float*)dataPool->getMemoryArray( size );
    }
  safeRead( array->idx, sizeof( int ), size, file );
  safeRead( array->data, sizeof( float ), size, file );
  array->size = size;
}


///////////////////////////////////////////////////////////////////////////////
//
// floatArrayArray Class
//

floatArrayArrayClass::floatArrayArrayClass( const floatArrayArrayClass& /*array*/ )
{
}


floatArrayArrayClass::floatArrayArrayClass( register int size = 0 )
{
  num = 0;
  this->size = (size == 0 ? DEFAULT_SIZE : size);
  idx = new int[this->size];
  data = new floatArrayClass[this->size];
  idx[0] = -1;
}


floatArrayArrayClass::~floatArrayArrayClass( void )
{
  delete[] idx;
  delete[] data;
}


void
floatArrayArrayClass::initialize( void )
{
}


void
floatArrayArrayClass::clean( void )
{
  num = 0;
  *idx = -1;
}


int
floatArrayArrayClass::empty( void )
{
  return( *idx == -1 );
}


int
floatArrayArrayClass::entryFor( register int index )
{
  register int *ip;

  for( ip = idx; *ip != -1; ++ip )
    if( *ip == index )
      return( 1 );
  return( 0 );
}


void 
floatArrayArrayClass::setIterator( floatArrayArrayIteratorClass& it )
{
  it.set( idx, data );
}


void
floatArrayArrayClass::check( void )
{
  register int *ip, n;

  for( ip = idx, n = 0; *ip != -1; ++ip, ++n );
  assert( n == num );
}
    

void
floatArrayArrayClass::print( register FILE *file, register char *fmt )
{
  register int *ip;
  register floatArrayClass *dp;
  char newFmt[64];
  
  for( ip = idx, dp = data; *ip != -1; ++ip, ++dp )
    if( !dp->empty() )
      {
	// build new format string and do it
	sprintf( newFmt, "%s:%d:%%d \t%%f\n", fmt, *ip );
	dp->print( file, newFmt );
      }
}


floatArrayClass&
floatArrayArrayClass::operator[]( register int index )
{
  register int *ip, i;
  register int *newIdx;
  register floatArrayClass *dp;
  register floatArrayClass *newData;
  
  for( ip = idx, dp = data; *ip != -1; ++ip, ++dp )
    if( *ip >= index )
      break;

  if( *ip != index )
    {
      if( (num + 1) == size )
	{
	  newIdx = new int[2 * size];
	  newData = new floatArrayClass[2 * size];

	  // copy data leaving space for the new
	  memcpy( newIdx, idx, (ip - idx) * sizeof( int ) );
	  memcpy( newIdx + (ip - idx + 1), idx + (ip - idx), ((num + 1) - (ip - idx)) * sizeof( int ) );
	  for( i = 0; i < (dp - data); ++i )
	    newData[i] = data[i];
	  for( i = 0; i < ((num + 1) - (dp - data)); ++i )
	    newData[(dp - data + 1) + i] = data[(dp - data) + i];

	  // set new pointers, free space, and set data
	  ip = newIdx + (ip - idx);
	  dp = newData + (dp - data);
	  delete[] idx;
	  delete[] data;

	  idx = newIdx;
	  data = newData;
	  size = 2 * size;
	}
      else
	{
	  // move data 
	  memmove( idx + (ip - idx + 1), idx + (ip - idx), ((num + 1) - (ip - idx)) * sizeof( int ) );
	  for( i = ((num + 1) - (dp - data)); i > 0; --i )
	    data[(dp - data) + i] = data[(dp - data) + (i - 1)];
	}
      ++num;
      *ip = index;
      dp->clean();
      dp->check(); //xxxxx
    }
  check(); //xxxxx
  return( *dp );
}


floatArrayArrayClass&
floatArrayArrayClass::operator=( register const floatArrayArrayClass& array )
{
  register int i;

  if( size <= array.num )
    {
      delete[] idx;
      delete[] data;
      idx = new int[array.num + 1];
      data = new floatArrayClass[array.num + 1];
      size = array.num + 1;
    }

  num = array.num;
  memcpy( idx, array.idx, (num + 1) * sizeof( int ) );
  for( i = 0; i < num + 1; ++i )
    data[i] = array.data[i];

  check(); //xxxxxx
  return( *this );
}


int
floatArrayArrayClass::operator==( register const floatArrayArrayClass& array )
{
  register int i;

  if( num == array.num )
    {
      for( i = 0; i < num; ++i )
        {
	  if( !(data[i] == array.data[i]) )
            return( 0 );
	  return( 1 );
	}
    }
  return( 0 );
}


///////////////////////
//
// serialization

void
floatArrayArrayClass::write( FILE *file )
{
  int i, size;

  size = this->num + 1; 
  safeWrite( &size, sizeof( int ), 1, file );
  safeWrite( &num, sizeof( int ), 1, file );
  safeWrite( idx, sizeof( int ), size, file );

  for( i = 0; i < size; ++i )
    data[i].write( file );
}


floatArrayArrayClass *
floatArrayArrayClass::read( FILE *file )
{
  floatArrayArrayClass *result;

  result = new floatArrayArrayClass;
  floatArrayArrayClass::fill( file, result );
  return( result );
}


void
floatArrayArrayClass::fill( FILE *file, floatArrayArrayClass *array )
{
  int i, size;
  safeRead( &size, sizeof( int ), 1, file );
  safeRead( &array->num, sizeof( int ), 1, file );

  if( array->size < size )
    {
      delete[] array->idx;
      delete[] array->data;
      array->idx = new int[size];
      array->data = new floatArrayClass[size];
    }
  safeRead( array->idx, sizeof( int ), size, file );
  array->size = size;

  for( i = 0; i < size; ++i )
    floatArrayClass::fill( file, &(array->data[i]) );
}
