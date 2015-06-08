#include <stdio.h>

#include "../vendor/portaudio/include/portaudio.h"

int main() {
	printf("Hello world!, %p\n", &Pa_GetDefaultHostApi);
	return 0;
}