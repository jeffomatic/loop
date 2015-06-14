#pragma once

#include <stdint.h>
#include <string.h>

#include "basicmath.h"

template<typename T, size_t BIN_CAPACITY>
struct SpscStream {
	enum { CAPACITY = 1 << BIN_CAPACITY };

	size_t capacity() const { return CAPACITY; }
	size_t size() const { return writePos - readPos; }
	size_t headroom() const { return CAPACITY - size(); }

	size_t read(T* dst, size_t amount) {
		const size_t toRead = min(amount, size());
		const size_t i = readIndex();
		const size_t toEnd = CAPACITY - i;

		if (toRead > toEnd) {
			memcpy(dst, &data[i], sizeof(T) * toEnd);
			memcpy(dst + toEnd, &data[0], sizeof(T) * (toRead - toEnd));
		} else {
			memcpy(dst, &data[i], sizeof(T) * toRead);
		}

		readPos += toRead;
		return toRead;
	}

	size_t write(const T* src, size_t amount) {
		const size_t toWrite = min(headroom(), amount);
		const size_t i = writeIndex();
		const size_t toEnd = CAPACITY - i;

		if (toWrite > toEnd) {
			memcpy(&data[i], src, sizeof(T) * toEnd);
			memcpy(&data[0], src + toEnd, sizeof(T) * (toWrite - toEnd));
		} else {
			memcpy(&data[i], src, sizeof(T) * toWrite);
		}

		writePos += toWrite;
		return toWrite;
	}

	// Internals
	T data[CAPACITY];
	size_t writePos = 0;
	size_t readPos = 0;

	size_t readIndex() const { return readPos & (CAPACITY - 1); }
	size_t writeIndex() const { return writePos & (CAPACITY - 1); }
};