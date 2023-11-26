
# Makefile for the ttf2txf program.

TARGET 		= ttf2txf
VERSION 	= 1.11.2

CXX 		= g++
STRIP 		= strip

CCFLAGS		?= -pipe -Wall -W -g

INC_FLAGS 	:= -I/usr/local/include/freetype2
CXXFLAGS 	:= $(CCFLAGS) -DTTF2TXF_VERSION=\"$(VERSION)\" $(INC_FLAGS)

LDFLAGS		= -L/usr/local/lib
LDLIBS		= -lfreetype

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
