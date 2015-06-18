#pragma once

#include <stdint.h>
#include <stdlib.h>

inline bool wrapLess(uint8_t a, uint8_t b) {
	return (int8_t)(a - b) < 0;
}

inline bool wrapLess(uint16_t a, uint16_t b) {
	return (int16_t)(a - b) < 0;
}

inline bool wrapLess(uint32_t a, uint32_t b) {
	return (int32_t)(a - b) < 0;
}

inline bool wrapLess(uint64_t a, uint64_t b) {
	return (int64_t)(a - b) < 0;
}

template<typename T>
bool powerOfTwo(T n) {
	return n != 0 && (n&(n-1)) == 0;
}

template<typename T>
const T& min(const T& a, const T& b) {
 	return (b < a) ? b : a;
}

template<typename T>
const T& max(const T& a, const T& b) {
	return (b < a) ? a : b;
}

template<typename T, size_t N>
constexpr size_t arraySize(const T (&)[N]) {
	return N;
}

inline void scaleIntSamples(const int16_t* src, float* dst, size_t amount) {
	for (size_t i = 0; i < amount; ++i) {
		dst[i] = (float)src[i] / (float)(INT16_MAX + 1);
	}
}
