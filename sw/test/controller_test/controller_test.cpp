// test_snes_controller.cpp
// A minimal program to open your Kiwitata USB SNES‑style controller on Linux
// and print mapped Tetris actions to stdout for testing.

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cstdio>

// Try /dev/input/event0..event31, look for “Kiwitata” or “SNES” in the device name.
int open_controller() {
    char path[64];
    for (int i = 0; i < 32; ++i) {
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;
        char name[256] = "Unknown";
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) {
            close(fd);
            continue;
        }
        if (strstr(name, "Kiwitata") || strstr(name, "SNES")) {
            std::cout << "Opened controller: “" << name << "” at " << path << "\n";
            return fd;
        }
        close(fd);
    }
    return -1;
}

// Map events to Tetris actions and print them.
void handle_event(const struct input_event &ev) {
    // D‑pad
    if (ev.type == EV_ABS) {
        if (ev.code == ABS_HAT0X) {
            if (ev.value < 0)      std::cout << "← Move Left\n";
            else if (ev.value > 0) std::cout << "→ Move Right\n";
        }
        else if (ev.code == ABS_HAT0Y) {
            if (ev.value > 0)      std::cout << "↓ Soft Drop\n";
        }
    }
    // Buttons
    else if (ev.type == EV_KEY && ev.value == 1) {
        switch (ev.code) {
            case BTN_TL:      // Left shoulder
            case BTN_TR:      // Right shoulder
            case BTN_X:       std::cout << "⟳ Rotate\n";               break;
            case BTN_A:       std::cout << "↓ Soft Drop (A)\n";        break;
            case BTN_B:       std::cout << "⤵ Hard Drop (B)\n";        break;
            case BTN_Y:       std::cout << "⏸ Pause/Resume (Y)\n";     break;
            case BTN_START:   std::cout << "▶ Start / Restart\n";      break;
            default:                                           break;
        }
    }
}

int main() {
    int fd = open_controller();
    if (fd < 0) {
        std::cerr << "Error: could not find Kiwitata SNES controller\n";
        return 1;
    }

    struct input_event ev;
    while (true) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n == sizeof(ev)) {
            handle_event(ev);
        }
        // avoid busy‑spin
        usleep(10 * 1000);  // 10 ms
    }

    close(fd);
    return 0;
}