# Makefile for the font2txf program.

TARGET 		= font2txf
VERSION 	= 1.11.2

CXX 		= g++
STRIP 		= strip

CCFLAGS		?= -pipe -Wall -W -g -DDEBUG -DDISPLAY -DFREEGLUT_STATIC -Bstatic -static

INC_FLAGS 	:= -I/usr/local/include/freetype2 -I/usr/local/include
CXXFLAGS 	:= $(CCFLAGS) -DPROGRAM_VERSION=\"$(VERSION)\" $(INC_FLAGS)

LDFLAGS		= -L/usr/local/lib
LDLIBS		= -lfreetype -lfreeglut_static -lopengl32 -lglu32 -lgdi32 -lwinmm

SOURCES 	= $(wildcard *.cpp)
OBJECTS 	= $(SOURCES:.cpp=.o)

INSTALLDIR 	= ..

EXECUTABLEEXTENSION =
ifeq ($(shell echo $(OS)),Windows_NT)
	EXECUTABLEEXTENSION := .exe
endif

OUTPUT 		= $(TARGET)$(EXECUTABLEEXTENSION)

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
