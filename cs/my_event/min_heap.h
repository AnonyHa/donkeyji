#ifndef _MIN_HEAP_H
#define _MIN_HEAP_H

#include "event.h"
#include "evutil.h"

typedef struct min_heap 
{
	struct event** p; 
	unsigned n;
	unsigned a;
}min_heap_t;

static inline void min_heap_ctor(min_heap_t* s);
static inline void min_heap_dtor(min_heap_t* s);
static inline void min_heap_elem_init(struct event* e); 













#endif
