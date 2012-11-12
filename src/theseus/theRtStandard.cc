//  Theseus
//  theRtStandard.cc -- RunTime Standard Support
//
//  Blai Bonet, Hector Geffner
//  Universidad Simon Bolivar, 1998, 1999, 2000, 2001, 2002

#include <stdio.h>
#include <limits.h>
#include <assert.h>

#include <iostream>

using namespace std;

#include <theseus/theRtStandard.h>

int stateClass::stateSize = 0;

stateListClass*
stateListClass::concat( stateListClass *sl )
{
  register int i;
  register stateListClass *result, *t, *p;

  if( sl == NULL )
    result = this;
  else
    {
      // counting stateList entries
      for( i = 0, t = sl; t->probability != -1; ++t, ++i );
      for( t = this; t->probability != -1; ++t, ++i );

      // clonning
      result = p = alloc( i + 1 );
      for( t = this; t->probability != -1; *p++ = *t++ );
      for( t = sl; t->probability != -1; *p++ = *t++ );

      // set end marker
      p->probability = -1;
    }
  return( result );
}

void
stateListClass::print( ostream& os, int indent ) const
{
  const stateListClass *p;

  for( p = this; p->probability != -1; ++p )
    {
      (os.width( indent ), os) << "";
      os << "action = " << p->action << ", probability = " << p->probability << endl;
      p->state->print( os, indent + 2 );
      if( (p + 1)->probability != -1 )
	os << endl;
    }
}

void
observationListClass::print( ostream& os, int indent ) const
{
  const observationListClass *p;

  for( p = this; p->probability != -1; ++p )
    {
      (os.width( indent ), os) << "";
      os << "probability = " << p->probability << endl;
      p->observation->print( os, indent + 2 );
      if( (p + 1)->probability != -1 )
	os << endl;
    }
}


// utilities

void //stateClass::node_t*
recover( const stateClass::node_t *node, stateClass *s )
{
  while( node->parent != NULL )
    {
      s->recover( node );
      node = node->parent;
    }
}

void
printNodeTree( stateClass::node_t *node )
{
  stateClass::node_t **ch;
  printf( "%*saddr=%p, value=%d, par=%p\n", node->depth, "", node, node->value, node->parent );
  if( node->children )
    for( ch = node->children; *ch != NULL; ++ch )
      printNodeTree( *ch );
}

void
concatLists( stateListClass*& list1, stateListClass* list2 )
{
  if( list1 == NULL )
    {
      list1 = list2;
    }
  else
    {
      stateListClass *last, *ptr;
      last = list1->last;
      for( ptr = list1; ptr != NULL; ptr = ptr->next )
	ptr->last = list2->last;
      last->next = list2;
      list2->prev = last;
    }
}

void
removeElement( stateListClass*& list, stateListClass* elem )
{
  if( elem == list )
    {
      if( list->next == NULL )
	{
	  list = NULL;
	}
      else
	{
	  list->next->prev = NULL;
	  list = list->next;
	}
    }
  else if( elem == list->last )
    {
      for( stateListClass *ptr = list; ptr != NULL; ptr = ptr->next )
	ptr->last = elem->prev;
      elem->prev->next = NULL;
    }
  else
    {
      elem->prev->next = elem->next;
      elem->next->prev = elem->prev;
    }
  delete elem;
}

void
printBits( ostream &os, unsigned pack )
{
  for( int i = 8*sizeof(unsigned) - 1; i >= 0; --i )
    os << ((pack>>i) & 1);
}

ostream&
operator<<( ostream& os, const stateClass& s )
{
  (&s)->print( os, 0 );
  return( os );
}

ostream&
operator<<( ostream& os, const observationClass& s )
{
  (&s)->print( os, 0 );
  return( os );
}

ostream&
operator<<( ostream& os, const stateListClass& s )
{
  (&s)->print( os, 0 );
  return( os );
}

ostream& operator<<( ostream& os, const observationListClass& s )
{
  (&s)->print( os, 0 );
  return( os );
}
