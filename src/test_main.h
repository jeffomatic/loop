#include <stdio.h>

#include "util/lf_stack_test.h"

#define DO_TEST(x) { \
	int res = x(); \
	if (res) { \
		printf("%s: failed, res: %d\n", #x, res); \
	} else { \
		printf("%s: passed\n", #x); \
	} \
}

int main(int argc, char* args[]) {
	DO_TEST(testLfNode)
	DO_TEST(testLfStack)
	return 0;
}