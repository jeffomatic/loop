#include <stdio.h>

extern void testLfStack();

#define RUN_TEST(x) printf("Running test: %s\n", #x); x()

int main(int argc, char* args[]) {
	RUN_TEST(testLfStack);

	printf("Tests complete.\n");
	return 0;
}