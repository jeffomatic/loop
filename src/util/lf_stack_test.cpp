#include <assert.h>
#include <stdio.h>

#include "lf_stack.h"

void testLfStack() {
	// Tests for LfNode

	LfNode* a = new LfNode;
	LfNode* b = new LfNode;
	LfNode* c = new LfNode;

	a->next = b;
	b->next = c;
	c->next = nullptr;

	assert(a->length() == 3);
	assert(a->tail() == c);
	assert(a->reverse() == c);
	assert(c->length() == 3);
	assert(c->tail() == a);
	assert(c->reverse() == a);

	delete a;
	delete b;
	delete c;

	// Tests for LfStack

	LfStack* s = new LfStack();

	a = new LfNode;
	b = new LfNode;
	c = new LfNode;

	bool empty;
	empty = s->push(a);
	assert(empty);
	assert(!s->isEmpty());
	empty = s->push(b);
	assert(!empty);
	assert(!s->isEmpty());
	empty = s->push(c);
	assert(!empty);
	assert(!s->isEmpty());

	LfNode* n = s->popAll();
	assert(s->isEmpty());
	assert(n == c);
	assert(n->length() == 3);
	assert(n->tail() == a);
	assert(n->reverse() == a);
}