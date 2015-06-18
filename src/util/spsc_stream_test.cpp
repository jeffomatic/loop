#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "spsc_stream.h"

void testSpscStream() {
	double buf[16];
	SpscStream<double> s(buf);
	size_t n;
	double dst[16];

	// Reading out of an empty stream
	assert(s.size() == 0);
	n = s.read(dst, arraySize(dst));
	assert(n == 0);

	// Writing and reading a single element.
	const double a[] = {1.1};
	n = s.write(a, arraySize(a));
	assert(n == 1);
	assert(s.size() == 1);

	n = s.read(dst, arraySize(dst));
	assert(n == 1);
	assert(s.size() == 0);
	for (int i = 0; i < 1; ++i) assert(a[i] == dst[i]);

	// Fill the stream to the back of the buffer
	const double b[] = {2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16};
	n = s.write(b, arraySize(b));
	assert(n == 15);
	assert(s.size() == 15);

	n = s.read(dst, arraySize(dst));
	assert(n == 15);
	assert(s.size() == 0);
	for (int i = 0; i < 15; ++i) assert(b[i] == dst[i]);

	// Next write should begin writing at the beginning of the buffer.
	const double c[] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
	n = s.write(c, arraySize(c));
	assert(n == 8);
	assert(s.size() == 8);

	n = s.read(dst, arraySize(dst));
	assert(n == 8);
	assert(s.size() == 0);
	for (int i = 0; i < 8; ++i) assert(c[i] == dst[i]);

	// Next write should cause wraparound
	const double d[] = {
		1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8,
		9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16,
	};
	n = s.write(d, arraySize(d));
	assert(n == 16);
	assert(s.size() == 16);

	n = s.read(dst, arraySize(dst));
	assert(n == 16);
	assert(s.size() == 0);
	for (int i = 0; i < 16; ++i) assert(d[i] == dst[i]);

	// Now read in successive chunks
	const double e[] = {
		1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8,
		9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16,
	};
	n = s.write(e, arraySize(e));
	assert(n == 16);
	assert(s.size() == 16);

	n = s.read(dst, 1);
	assert(n == 1);
	assert(s.size() == 15);
	for (int i = 0; i < n; ++i) assert(dst[i] == e[i]);

	n = s.read(dst, 2);
	assert(n == 2);
	assert(s.size() == 13);
	for (int i = 0; i < n; ++i) assert(dst[i] == e[i + 1]);

	n = s.read(dst, 3);
	assert(n == 3);
	assert(s.size() == 10);
	for (int i = 0; i < n; ++i) assert(dst[i] == e[i + 3]);

	n = s.read(dst, 4);
	assert(n == 4);
	assert(s.size() == 6);
	for (int i = 0; i < n; ++i) assert(dst[i] == e[i + 6]);

	// Now read everything possible.
	n = s.read(dst, arraySize(dst));
	assert(n == 6);
	assert(s.size() == 0);
	for (int i = 0; i < n; ++i) assert(dst[i] == e[i + 10]);
}