#pragma once

#include <vector>

#include "audio_common.h"
#include "sampler.h"

typedef Sampler<stereoFrame> StereoSampler;

struct Mixer {
	StereoStream* m_out = nullptr;
	std::vector<StereoSampler*> m_loopSamplers;

	void run();

	static int threadFunc(void* data);
};