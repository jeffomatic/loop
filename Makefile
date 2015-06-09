CC = llvm-g++

SOURCEDIR = src
BUILDDIR = build
VENDORDIR = vendor

SDL_DIR = $(VENDORDIR)/sdl
SDL_LIB = $(SDL_DIR)/build/.libs/libSDL2.a
SDL_FRAMEWORKS = ForceFeedback IOKit AppKit
SDL_LIB_DEPS = iconv

PA_DIR = $(VENDORDIR)/portaudio
PA_LIB = $(PA_DIR)/lib/.libs/libportaudio.a
PA_FRAMEWORKS = CoreAudio AudioToolbox AudioUnit CoreServices Carbon

LIB_DEPS = $(SDL_LIB_DEPS)
FRAMEWORKS = $(PA_FRAMEWORKS) $(SDL_FRAMEWORKS)

LIB_DEP_FLAGS = $(patsubst %,-l%,$(LIB_DEPS))
FRAMEWORK_FLAGS = $(patsubst %,-framework %,$(FRAMEWORKS))

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
VENDORDEPS = $(PA_LIB) $(SDL_LIB)

EXECUTABLE = loop

all: $(BUILDDIR)/$(EXECUTABLE)

$(BUILDDIR)/$(EXECUTABLE) : $(OBJECTS) $(VENDORDEPS)
	$(CC) $(OBJECTS) $(PA_LIB) $(SDL_LIB) $(LIB_DEP_FLAGS) $(FRAMEWORK_FLAGS) -o $@

$(OBJECTS): $(SOURCES)
	$(CC) -g -c $< -o $@

$(PA_LIB):
	cd $(PA_DIR); ./configure && make

$(SDL_LIB):
	cd $(SDL_DIR); ./configure && make

clean-all: clean vendorclean

clean:
	rm -rf $(BUILDDIR)/*

vendorclean: pa-clean sdl-clean

pa-clean:
	cd $(PA_DIR); make clean

sdl-clean:
	cd $(SDL_DIR); make clean