#pragma once

#include <assert.h>
#include <atomic>
#include <stdint.h>
#include <string.h>

#include "basicmath.h"

template<typename T>
struct SpscStream {
	size_t size() const { return m_writePos - m_readPos; }
	size_t headroom() const { return m_capacity - size(); }

	size_t read(T* dst, size_t amount) {
		const size_t numRead = min(amount, size());
		const size_t i = readIndex();
		const size_t toEnd = m_capacity - i;

		if (numRead > toEnd) {
			memcpy(dst, &m_data[i], sizeof(T) * toEnd);
			memcpy(dst + toEnd, &m_data[0], sizeof(T) * (numRead - toEnd));
		} else {
			memcpy(dst, &m_data[i], sizeof(T) * numRead);
		}

		m_readPos += numRead;
		return numRead;
	}

	size_t write(const T* src, size_t amount) {
		const size_t numWritten = min(headroom(), amount);
		const size_t i = writeIndex();
		const size_t toEnd = m_capacity - i;

		if (numWritten > toEnd) {
			memcpy(&m_data[i], src, sizeof(T) * toEnd);
			memcpy(&m_data[0], src + toEnd, sizeof(T) * (numWritten - toEnd));
		} else {
			memcpy(&m_data[i], src, sizeof(T) * numWritten);
		}

		m_writePos += numWritten;
		return numWritten;
	}

	SpscStream(T* data, size_t cap)
		: m_data(data)
		, m_capacity(cap)
		, m_writePos(0)
		, m_readPos(0)
	{
		assert(powerOfTwo(cap));
	}

	template<size_t N>
	SpscStream(T (&data)[N])
		: m_data(data)
		, m_capacity(N)
		, m_writePos(0)
		, m_readPos(0)
	{
		assert(powerOfTwo(N));
	}

	// Internals
	T* m_data;
	size_t m_capacity;
	std::atomic_size_t m_writePos;
	std::atomic_size_t m_readPos;

	size_t readIndex() const { return m_readPos & (m_capacity - 1); }
	size_t writeIndex() const { return m_writePos & (m_capacity - 1); }
};