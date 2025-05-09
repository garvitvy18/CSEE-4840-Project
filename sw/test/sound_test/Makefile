# Makefile for usb_audio_player

CC       := gcc
PKGCFG   := pkg-config

CFLAGS   := -Wall -O2 $(shell $(PKGCFG) --cflags libusb-1.0 libmpg123)
LDFLAGS  := $(shell $(PKGCFG) --libs libusb-1.0 libmpg123) -pthread

SRC      := usb_audio_player.c
OBJ      := $(SRC:.c=.o)
TARGET   := usb_audio_player

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)