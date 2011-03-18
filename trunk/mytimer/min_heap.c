#include <stdlib.h>

#include "min_heap.h"
#include "timer.h"


//--------------------------------------
#define evutil_timercmp(tvp, uvp, cmp)                          \
	(((tvp)->tv_sec == (uvp)->tv_sec) ?                         \
	((tvp)->tv_usec cmp (uvp)->tv_usec) :                      \
	((tvp)->tv_sec cmp (uvp)->tv_sec))
//--------------------------------------

int min_heap_elem_greater(struct timer *a, struct timer *b)
{
	return evutil_timercmp(&a->timeout, &b->timeout, >);
}

void min_heap_ctor(min_heap_t* s)
{
	s->p = 0;
	s->n = 0;
	s->a = 0;
}
void min_heap_dtor(min_heap_t* s)
{
	free(s->p);
}
void min_heap_elem_init(struct timer* e)
{
	e->min_heap_idx = -1;
}
int min_heap_empty(min_heap_t* s)
{
	return 0u == s->n;
}
unsigned min_heap_size(min_heap_t* s)
{
	return s->n;
}
struct timer* min_heap_top(min_heap_t* s) {
	return s->n ? *s->p : 0;
}

int min_heap_push(min_heap_t* s, struct timer* e)
{
	if(min_heap_reserve(s, s->n + 1)) {
		return -1;
	}

	min_heap_shift_up_(s, s->n++, e);
	return 0;
}

struct timer* min_heap_pop(min_heap_t* s) {
	if(s->n) {
		struct timer* e = *s->p;
		min_heap_shift_down_(s, 0u, s->p[--s->n]);
		e->min_heap_idx = -1;
		return e;
	}

	return 0;
}

int min_heap_erase(min_heap_t* s, struct timer* e)
{
	if(((unsigned int)-1) != e->min_heap_idx) {
		min_heap_shift_down_(s, e->min_heap_idx, s->p[--s->n]);
		e->min_heap_idx = -1;
		return 0;
	}

	return -1;
}

int min_heap_reserve(min_heap_t* s, unsigned n)
{
	if(s->a < n) {
		struct timer** p;
		unsigned a = s->a ? s->a * 2 : 8;

		if(a < n) {
			a = n;
		}

		if(!(p = (struct timer**)realloc(s->p, a * sizeof *p))) {
			return -1;
		}

		s->p = p;
		s->a = a;
	}

	return 0;
}

void min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct timer* e)
{
	unsigned parent = (hole_index - 1) / 2;

	while(hole_index && min_heap_elem_greater(s->p[parent], e)) {
		(s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}

	(s->p[hole_index] = e)->min_heap_idx = hole_index;
}

void min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct timer* e)
{
	unsigned min_child = 2 * (hole_index + 1);

	while(min_child <= s->n) {
		min_child -= min_child == s->n || min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]);

		if(!(min_heap_elem_greater(e, s->p[min_child]))) {
			break;
		}

		(s->p[hole_index] = s->p[min_child])->min_heap_idx = hole_index;
		hole_index = min_child;
		min_child = 2 * (hole_index + 1);
	}

	min_heap_shift_up_(s, hole_index,  e);
}
