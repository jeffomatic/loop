VENDOR_DIR = vendor

SDL_DIR = $(VENDOR_DIR)/sdl
SDL_LIB = $(SDL_DIR)/build/.libs/libSDL2.a
SDL_FRAMEWORKS = ForceFeedback IOKit AppKit
SDL_DEPLIBS = iconv

PA_DIR = $(VENDOR_DIR)/portaudio
PA_LIB = $(PA_DIR)/lib/.libs/libportaudio.a
PA_FRAMEWORKS = CoreAudio AudioToolbox AudioUnit CoreServices Carbon

VENDOR_DEPLIBS = $(sort $(SDL_DEPLIBS))
VENDOR_FRAMEWORKS = $(sort $(PA_FRAMEWORKS) $(SDL_FRAMEWORKS))

LIB_DEP_FLAGS = $(patsubst %,-l%,$(VENDOR_DEPLIBS))
FRAMEWORK_FLAGS = $(patsubst %,-framework %,$(VENDOR_FRAMEWORKS))

VENDOR_OBJS = $(PA_LIB) $(SDL_LIB)

$(PA_LIB):
	cd $(PA_DIR); ./configure && make

$(SDL_LIB):
	cd $(SDL_DIR); ./configure && make

vendor-clean: pa-clean sdl-clean

pa-clean:
	cd $(PA_DIR); make clean

sdl-clean:
	cd $(SDL_DIR); make clean