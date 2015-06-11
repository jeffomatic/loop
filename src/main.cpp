#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../vendor/portaudio/include/portaudio.h"
#include "../vendor/sdl/include/SDL.h"

#define UNUSED(x) (void)(x)

void dieOnPaErr(PaError err, const char* context) {
	if (err == paNoError) return;

	printf("%s paError: %s", context, Pa_GetErrorText(err));
	exit(1);
}

struct paTestData {
	float left_phase;
	float right_phase;
};

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

int patestCallback(
	const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData
) {
	UNUSED(inputBuffer);

    /* Cast data passed through stream to our structure. */
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;

    for (int i = 0; i < framesPerBuffer; i++) {
        *out++ = data->left_phase;
        *out++ = data->right_phase;

        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.01f;
        /* When signal reaches top, drop back down. */
        if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;

        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.03f;
        if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
    }

    return 0;
}

int main(int argc, char* args[]) {
	initSignalHandler();

	printf("Sizeof int is %d\n", sizeof(int));

	PaError paErr = Pa_Initialize();
	dieOnPaErr(paErr, "Pa_Initialize");

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

	paTestData data;
	PaStream* stream;

    paErr = Pa_OpenDefaultStream(
    	&stream, 0, 2, paFloat32, 44100, paFramesPerBufferUnspecified,
        patestCallback, &data
	);
    dieOnPaErr(paErr, "Pa_OpenDefaultStream");

    paErr = Pa_StartStream(stream);
    dieOnPaErr(paErr, "Pa_StartStream");

    for (;;) {
    	if (gQuit) break;
    }

	paErr = Pa_Terminate();
	dieOnPaErr(paErr, "Pa_Terminate");

	return 0;
}