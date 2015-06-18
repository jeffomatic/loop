#include "mixer.h"

extern bool gQuit;

void Mixer::run() {
	enum {samplesPerWrite = 128};

	while (!gQuit) {
		stereoFrame streamMsg[samplesPerWrite];
		memset(streamMsg, 0, sizeof(streamMsg));
		size_t maxSampleable = min(m_out->headroom(), (size_t)samplesPerWrite);
		size_t maxSampled = 0;

		for (auto i = m_loopSamplers.begin(); i != m_loopSamplers.end(); ++i) {
			stereoFrame loopMsg[samplesPerWrite];
			auto nSampled = (*i)->sample(loopMsg, maxSampleable);
			maxSampled = max(maxSampled, nSampled);

			for (size_t i = 0; i < nSampled; ++i) {
				streamMsg[i].left += loopMsg[i].left;
				streamMsg[i].right += loopMsg[i].right;
			}
		}

		m_out->write(streamMsg, maxSampled);
	}
}

int Mixer::threadFunc(void *data) {
	Mixer* m = (Mixer*)data;
	m->run();
	return 0;
}