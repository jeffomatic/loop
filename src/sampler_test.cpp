#include <assert.h>

#include "sampler.h"

void testSampler() {
	const float buf[16] = {
		1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8,
		9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16
	};
	Sampler<float> s(buf);

	size_t nSampled;
	float sampled[32];
	for (int i = 0; i < 32; ++i) {
		sampled[i] = 0;
	}

	// Non-looping behavior
	nSampled = s.sample(sampled, 1);
	assert(nSampled == 1);
	assert(sampled[0] == 1.1f);

	nSampled = s.sample(sampled);
	assert(nSampled = 15);

	float wantNoLoop[16] = {
		2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8,
		9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16
	};
	for (int i = 0; i < 15; ++i) {
		assert(sampled[i] == wantNoLoop[i]);
	}

	nSampled = s.sample(sampled);
	assert(nSampled == 0);

	// Looping behavior
	s.setLoop(true);
	s.reset();

	nSampled = s.sample(sampled);
	assert(nSampled == 32);

	float wantLoop[32] = {
		1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8,
		9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16,
		1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8,
		9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16,
	};
	for (int i = 0; i < 32; ++i) {
		assert(sampled[i] == wantLoop[i]);
	}
}