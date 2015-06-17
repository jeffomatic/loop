#pragma once

#include <stdlib.h>
#include <string.h>

#include "util/basicmath.h"

#include <stdio.h>

template<typename T>
struct Sampler {
	Sampler(const T* data, size_t size) : m_data(data), m_size(size) {}

	template<size_t N>
	Sampler(const T (&data)[N]) : m_data(data), m_size(N) {}

	void setLoop(bool b) { m_loop = b; }
	bool isLoop() const { return m_loop; }
	void reset() { m_pos = 0; }

	size_t sample(T* dst, size_t amount) {
		size_t copied = 0;
		while (amount > 0) {
			auto toEnd = m_size - m_pos;
			auto copySize = min(toEnd, amount);

			memcpy(dst, m_data + m_pos, copySize * sizeof(T));

			m_pos += copySize;
			copied += copySize;

			// Non-loops finish when they've reached the end.
			if (!m_loop) break;

			// This is a loop, so handle position wraparound.
			if (m_pos >= m_size) m_pos = 0;

			// If we've covered exactly the remainder, then we're done.
			if (copySize == amount) break;

			// Advance to next tick
			amount -= copySize;
			dst += copySize;
		}
		return copied;
	}

	template<size_t N>
	size_t sample(T (&dst)[N]) { return sample(dst, N); }

	const T* m_data = nullptr;
	size_t m_size = 0;
	size_t m_pos = 0;
	bool m_loop = false;
};