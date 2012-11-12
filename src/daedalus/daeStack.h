#ifndef _daeStack_INCLUDE_
#define _daeStack_INCLUDE_


class stackClass
{
public:
  // members
  void     **stack;
  unsigned size;
  unsigned topPtr;

  // constructors
  stackClass();
  ~stackClass();

  // methods
  void push( register void *data );
  void *pop( void );
  void *top( void );
};


#endif /* _daeStack_INCLUDE_ */
