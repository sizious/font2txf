# Makefile for the font2txf program.

# Detect the host (extracted from dcload)
ROOTDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(ROOTDIR)/Makefile.hostdetect

# Include configuration
include $(ROOTDIR)/Makefile.cfg

# Name of the program
TARGET      = font2txf

# Utilities
CXX         = $(HOSTCXX)
STRIP       = $(HOSTSTRIP)

# Basic compiler flags
CCFLAGS     ?= -pipe -W -Wunused-parameter

# Basic linker flags
LDFLAGS     ?= -L/usr/local/lib
LDLIBS      ?= -lfreetype

# Sources and objects (dynamic here)
SOURCES     = $(wildcard *.cpp)
OBJECTS     = $(SOURCES:.cpp=.o)

# Install location
INSTALLDIR  = $(TOOLINSTALLDIR)

# Name of the binary file generated
OUTPUT      = $(TARGET)$(EXECUTABLEEXTENSION)

# Debug flag
BUILD_FLAG := -O2
ifeq ($(DEBUG),1)
  BUILD_FLAG := -g
  CCFLAGS += -Wall -D_DEBUG
endif
CCFLAGS += $(BUILD_FLAG)

# Static flag
ifeq ($(STANDALONE_BINARY),1)
  CCFLAGS += -Bstatic -static
endif

# Enable preview (uses GLUT) flag
ifeq ($(PREVIEW),1)
  # Static Freeglut
  LIB_FREEGLUT := -lfreeglut
  ifeq ($(STANDALONE_BINARY),1)
    CCFLAGS += -DFREEGLUT_STATIC
	LIB_FREEGLUT := -lfreeglut_static
  endif

  # Freeglut
  CCFLAGS += -DDISPLAY
  LDLIBS += $(LIB_FREEGLUT) -lopengl32 -lglu32

  # Windows specific libraries used by Freeglut
  ifdef WINDOWS
    LDLIBS += -lgdi32 -lwinmm
  endif
endif

ifdef MACOS
  CCFLAGS += -std=gnu++11
endif

# Final flags
INC_FLAGS   := -I/usr/local/include/freetype2 -I/usr/local/include
CXXFLAGS    := $(CCFLAGS) -DPROGRAM_VERSION=\"$(VERSION)\" $(INC_FLAGS)

# Makefile targets

all: $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT) $(OBJECTS) $(LDFLAGS) $(LDLIBS)	

install: $(TARGET)
	mkdir -p $(INSTALLDIR)
	$(STRIP) $(OUTPUT)
	mv $(OUTPUT) $(INSTALLDIR)	

.PHONY: clean
clean:
	-rm -f $(OUTPUT) *.o
