# Makefile for usb_audio_player

CC       := gcc
PKGCFG   := pkg-config

CFLAGS   := -Wall -O2 $$( $(PKGCFG) --cflags libusb-1.0 mpg123 )
LDFLAGS  := $$( $(PKGCFG) --libs libusb-1.0 mpg123 ) -pthread

SRC      := usb_audio_player.c
OBJ      := $(SRC:.c=.o)
TARGET   := usb_audio_player

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean