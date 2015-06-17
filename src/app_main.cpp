#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../vendor/portaudio/include/portaudio.h"
#include "../vendor/sdl/include/SDL.h"

#include "util/basicmath.h"
#include "util/spsc_stream.h"

#include "sampler.h"

#define UNUSED(x) (void)(x)

enum {
	gSampleRate = 44100
};

void dieOnPaErr(PaError err, const char* context) {
	if (err == paNoError) return;

	printf("%s paError: %s", context, Pa_GetErrorText(err));
	exit(1);
}

void dieOnSDLError(const char* context) {
	printf("%s SDL error: %s", context, SDL_GetError());
	exit(1);
}

struct stereoFrame {
	float left;
	float right;
};

typedef SpscStream<stereoFrame> stereoStream;

bool gQuit = false;
void signalHandler(int signum) {
	switch (signum) {
	case SIGINT:
	case SIGQUIT:
	case SIGTERM:
		gQuit = true;
		break;
	}
}

void initSignalHandler() {
	// Signals range from [1, 31]
	for (int i = 1; i <= 31; i++) {
		signal(i, signalHandler);
	}
}

struct Mixer {
	SpscStream<stereoFrame>* m_out = nullptr;
	Sampler<stereoFrame>* m_sampler = nullptr;

	void run() {
		enum {samplesPerWrite = 128};

		while (!gQuit) {
			stereoFrame streamMsg[samplesPerWrite];
			size_t msgSize = min(m_out->headroom(), (size_t)samplesPerWrite);
			msgSize = m_sampler->sample(streamMsg, msgSize);
			m_out->write(streamMsg, msgSize);
		}
	}

	static int threadFunc(void* data) {
		Mixer* m = (Mixer*)data;
		m->run();
		return 0;
	}
};

int patestCallback(
	const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData
) {
	auto mixerOutStream = (stereoStream*)userData;
	auto out = (stereoFrame*)outputBuffer;
	auto nRead = mixerOutStream->read(out, framesPerBuffer);

	// zerofill in case the stream drops out
	if (framesPerBuffer > nRead) {
		memset(out, 0, (framesPerBuffer-nRead)*sizeof(stereoFrame));
	}

	return 0;
}

void paDumpInfo() {
	const int nDevices = Pa_GetDeviceCount();
	if (nDevices < 0) dieOnPaErr((PaError)nDevices, "Pa_GetDeviceCount");

	printf("Audio devices: %d\n", nDevices);

	for (int i = 0; i < nDevices; i++) {
		const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);

		printf("\n");
		printf("Device %d: %s\n", i, deviceInfo->name);
		printf("Max channels:\n");
		printf("- input:\t%d\n", deviceInfo->maxInputChannels);
		printf("- output:\t%d\n", deviceInfo->maxOutputChannels);
		printf("Default latency:\n");
		printf("- low input:\t%f\n", deviceInfo->defaultLowInputLatency);
		printf("- low output:\t%f\n", deviceInfo->defaultLowOutputLatency);
		printf("- high input:\t%f\n", deviceInfo->defaultHighInputLatency);
		printf("- high output:\t%f\n", deviceInfo->defaultHighOutputLatency);
		printf("Default sample rate: %f\n", deviceInfo->defaultSampleRate);
	}
}

union SDLAudioFormat {
	SDL_AudioFormat saf;
	struct {
		uint8_t bits;
		uint8_t isFloat:1;
		uint8_t pad1:3;
		uint8_t isBigEndian:1;
		uint8_t pad2:2;
		uint8_t isSigned:1;
	} fields;
};

void SDLWavDumpInfo(const SDL_AudioSpec* spec) {
	printf("amenbreak.wav:\n");
	printf("- channels: %d\n", spec->channels);
	printf("- freq: %d\n", spec->freq);
	printf("- format: %d\n", spec->format);

	SDLAudioFormat formatFields = {.saf = spec->format};
	printf("  - bits: %d\n", formatFields.fields.bits);
	printf("  - isFloat: %d\n", formatFields.fields.isFloat);
	printf("  - isBigEndian: %d\n", formatFields.fields.isBigEndian);
	printf("  - isSigned: %d\n", formatFields.fields.isSigned);
}

int appMain(int argc, char* args[]) {
	initSignalHandler();

	SDL_AudioSpec wavSpec;
	Uint32 wavLength;
	Uint8* wavBuf;
	SDL_AudioSpec* retSpec = SDL_LoadWAV("assets/amenbreak.wav", &wavSpec, &wavBuf, &wavLength);
	if (retSpec == NULL) {
		dieOnSDLError("SDL_LoadWAV(\"assets/amenbreak.wav\", &wavSpec, &wavBuf, &wavLength)");
	}
	SDLWavDumpInfo(retSpec);

	size_t nSamples = wavLength / (2*sizeof(int16_t));
	auto sampleBuf = new stereoFrame[nSamples];
	scaleIntSamples((int16_t*)wavBuf, (float*)sampleBuf, 2*nSamples);
	Sampler<stereoFrame> sampler(sampleBuf, nSamples);
	sampler.setLoop(true);

	stereoFrame buf[1024];
	stereoStream outstream(buf);
	Mixer mixer;
	mixer.m_out = &outstream;
	mixer.m_sampler = &sampler;

	PaError paErr = Pa_Initialize();
	dieOnPaErr(paErr, "Pa_Initialize");

	PaStream* paStream;
	paErr = Pa_OpenDefaultStream(
		&paStream, 0, 2, paFloat32, gSampleRate, paFramesPerBufferUnspecified,
		patestCallback, &outstream
	);
	dieOnPaErr(paErr, "Pa_OpenDefaultStream");

	paErr = Pa_StartStream(paStream);
	dieOnPaErr(paErr, "Pa_StartStream");

	SDL_Thread* mixerThread = SDL_CreateThread(Mixer::threadFunc, "mixer", &mixer);
	if (mixerThread == nullptr) {
		dieOnSDLError("SDL_CreateThread(Mixer::threadFunc, \"mixer\", &mixer);");
	}
	SDL_WaitThread(mixerThread, nullptr);

	paErr = Pa_Terminate();
	dieOnPaErr(paErr, "Pa_Terminate");

	return 0;
}