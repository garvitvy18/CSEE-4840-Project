# Makefile for play_mp3_usb

# Compiler and flags
CXX       := g++
CXXFLAGS  := -Wall -std=c++11 $(shell pkg-config --cflags mpg123 alsa)
LDLIBS    := $(shell pkg-config --libs mpg123 alsa)

# Target executable and source
TARGET    := play_mp3_usb
SRC       := play_mp3_usb.cpp

# Default MP3 to play (override with ‘make run MP3=path/to/file.mp3’)
MP3       ?= Tetris.mp3

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
<TAB>$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

run: $(TARGET)
<TAB>./$(TARGET) $(MP3)

clean:
<TAB>rm -f $(TARGET)