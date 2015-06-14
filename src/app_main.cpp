#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../vendor/portaudio/include/portaudio.h"
#include "../vendor/sdl/include/SDL.h"

#include "util/basicmath.h"
#include "util/spsc_stream.h"

#define UNUSED(x) (void)(x)

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

typedef SpscStream<stereoFrame, 10> stereoStream;

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

int mixerThreadFunc(void* data) {
	stereoStream* mixerOutStream = (stereoStream*)data;
	stereoFrame writeState = {0, 0};
	enum {samplesPerWrite = 32};

	while (!gQuit) {
		size_t headroom = mixerOutStream->headroom();
		stereoFrame streamMsg[samplesPerWrite];
		size_t msgSize = min(headroom, (size_t)samplesPerWrite);

		for (int i = 0; i < msgSize; i++) {
			writeState.left += 0.01f;
			if (writeState.left >= 1.0f) writeState.left -= 2.0f;

			writeState.right += 0.03f;
			if (writeState.right >= 1.0f) writeState.right -= 2.0f;

			streamMsg[i] = writeState;
		}

		mixerOutStream->write(streamMsg, msgSize);
	}

	return 0;
}

int patestCallback(
	const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData
) {
	UNUSED(inputBuffer);
	stereoStream* mixerOutStream = (stereoStream*)userData;
	stereoFrame* out = (stereoFrame*)outputBuffer;
	mixerOutStream->read(out, framesPerBuffer);
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

int appMain(int argc, char* args[]) {
	initSignalHandler();

	PaError paErr = Pa_Initialize();
	dieOnPaErr(paErr, "Pa_Initialize");

	stereoStream mixerOutStream;
	PaStream* paStream;
	paErr = Pa_OpenDefaultStream(
		&paStream, 0, 2, paFloat32, 44100, paFramesPerBufferUnspecified,
		patestCallback, &mixerOutStream
	);
	dieOnPaErr(paErr, "Pa_OpenDefaultStream");

	paErr = Pa_StartStream(paStream);
	dieOnPaErr(paErr, "Pa_StartStream");

	SDL_Thread* mixerThread = SDL_CreateThread(mixerThreadFunc, "mixer", &mixerOutStream);
	if (mixerThread == nullptr) {
		dieOnSDLError("SDL_CreateThread(mixerThreadFunc, \"mixer\", &mixerOutStream)");
	}
	SDL_WaitThread(mixerThread, nullptr);

	paErr = Pa_Terminate();
	dieOnPaErr(paErr, "Pa_Terminate");

	return 0;
}