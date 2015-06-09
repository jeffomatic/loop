#include <stdio.h>
#include <stdlib.h>

#include "../vendor/portaudio/include/portaudio.h"
#include "../vendor/sdl/include/SDL.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void dieOnPaErr(PaError err, const char* context) {
	if (err == paNoError) return;

	printf("%s paError: %s", context, Pa_GetErrorText(err));
	exit(1);
}

int main(int argc, char* args[]) {
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

	paErr = Pa_Terminate();
	dieOnPaErr(paErr, "Pa_Terminate");

    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    } else {
        SDL_Window* window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if (window == NULL) {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        } else {
            SDL_Surface* screenSurface = SDL_GetWindowSurface( window );
		    SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
            SDL_UpdateWindowSurface( window );
            SDL_Delay(2000);
        }
    }

    SDL_Quit();

	return 0;
}