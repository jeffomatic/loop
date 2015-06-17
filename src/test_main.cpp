#include <stdio.h>

#define RUN_TEST(x) extern void x(); printf("Running test: %s\n", #x); x()

int testMain(int argc, char* args[]) {
	RUN_TEST(testBasicmath);
	RUN_TEST(testLfStack);
	RUN_TEST(testSpscStream);
	RUN_TEST(testSampler);

	printf("Tests complete.\n");
	return 0;
}