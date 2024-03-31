# Makefile for the font2txf program.

# Detect the host (extracted from dcload)
ROOTDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(ROOTDIR)/Makefile.hostdetect

# Include configuration
include Makefile.cfg

TARGET 		= font2txf

CXX 		= $(HOSTCXX)
STRIP 		= $(HOSTSTRIP)

CCFLAGS		?= -pipe -W

INC_FLAGS 	:= -I/usr/local/include/freetype2 -I/usr/local/include
CXXFLAGS 	:= $(CCFLAGS) -DPROGRAM_VERSION=\"$(VERSION)\" $(INC_FLAGS)

LDFLAGS		= -L/usr/local/lib
LDLIBS		= -lfreetype

SOURCES 	= $(wildcard *.cpp)
OBJECTS 	= $(SOURCES:.cpp=.o)

INSTALLDIR 	= $(TOOLINSTALLDIR)

OUTPUT 		= $(TARGET)$(EXECUTABLEEXTENSION)

# Static
ifeq ($(STANDALONE_BINARY),1)
  CXXFLAGS += -Bstatic -static
endif

# Debug
ifeq ($(DEBUG),1)
  CXXFLAGS += -Wall -g -D_DEBUG
endif

# Enable Preview (uses GLUT)
ifeq ($(PREVIEW),1)
  # Static Freeglut
  LIB_FREEGLUT := -lfreeglut
  ifeq ($(STANDALONE_BINARY),1)
    CXXFLAGS += -DFREEGLUT_STATIC
	LIB_FREEGLUT := -lfreeglut_static
  endif

  # Freeglut
  CXXFLAGS += -DDISPLAY
  LDLIBS += $(LIB_FREEGLUT) -lopengl32 -lglu32

  # Windows specific libraries used by Freeglut
  ifdef WINDOWS
    LDLIBS += -lgdi32 -lwinmm
  endif
endif

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
