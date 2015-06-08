CC = llvm-g++

SOURCEDIR = src
BUILDDIR = build
VENDORDIR = vendor

PA_DIR = $(VENDORDIR)/portaudio
PA_LIB = $(PA_DIR)/lib/.libs
PA_FRAMEWORKS = CoreAudio AudioToolbox AudioUnit CoreServices Carbon

FRAMEWORKS = $(PA_FRAMEWORKS)
FRAMEWORK_FLAGS = $(patsubst %,-framework %,$(FRAMEWORKS))

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
VENDORDEPS = $(PA_LIB)

EXECUTABLE = loop

all: $(BUILDDIR)/$(EXECUTABLE)

$(BUILDDIR)/$(EXECUTABLE) : $(OBJECTS) $(VENDORDEPS)
	$(CC) $(OBJECTS) $(PA_LIB)/libportaudio.a $(FRAMEWORK_FLAGS) -o $@

$(OBJECTS): $(SOURCES)
	$(CC) -g -c $< -o $@

$(PA_LIB):
	cd $(PA_DIR); ./configure && make

clean-all: clean vendorclean

clean:
	rm -rf $(BUILDDIR)/*

vendorclean:
	cd $(PA_DIR); make clean
