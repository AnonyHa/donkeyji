#ifndef _MIN_HEAP_H
#define _MIN_HEAP_H

struct timer;

typedef struct min_heap {
	struct timer** p;
	unsigned n, a;
} min_heap_t;

void           min_heap_ctor(min_heap_t* s);
void           min_heap_dtor(min_heap_t* s);
void           min_heap_elem_init(struct timer* e);
int            min_heap_elem_greater(struct timer *a, struct timer *b);
int            min_heap_empty(min_heap_t* s);
unsigned       min_heap_size(min_heap_t* s);
struct timer*  min_heap_top(min_heap_t* s);
int            min_heap_reserve(min_heap_t* s, unsigned n);
int            min_heap_push(min_heap_t* s, struct timer* e);
struct timer*  min_heap_pop(min_heap_t* s);
int            min_heap_erase(min_heap_t* s, struct timer* e);
void           min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct timer* e);
void           min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct timer* e);


#endif
