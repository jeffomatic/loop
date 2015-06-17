#include <assert.h>
#include <math.h>

#include "basicmath.h"

void testBasicmath() {
	const float epsilon = 0.0001;

	int16_t ai[] = {INT16_MAX, 0, INT16_MIN};
	float afWant[] = {1.0, 0.0, -1.0};
	float afGot[3];

	scaleIntSamples(ai, afGot, 3);

	for (int i = 0; i < 3; i++) {
		assert(fabs(afWant[i] - afGot[i]) < epsilon);
	}
}