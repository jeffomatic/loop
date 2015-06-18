#pragma once

#include "util/spsc_stream.h"

enum {
	gSampleRate = 44100
};

struct stereoFrame {
	float left;
	float right;
};

typedef SpscStream<stereoFrame> StereoStream;