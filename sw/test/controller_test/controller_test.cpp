// test_usb_gamepad.cpp
// A minimal program to open your USB Gamepad (Vendor 0x0079, Product 0x0011)
// and print mapped Tetris actions to stdout for testing.

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <cstring>

// Scan /dev/input/event0..event31 for our gamepad by name or VID/PID
int open_controller() {
    struct input_id id;
    char path[64], name[256];
    for (int i = 0; i < 32; ++i) {
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        // get device name
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0)
            name[0] = '\0';

        // get vendor/product
        if (ioctl(fd, EVIOCGID, &id) < 0)
            memset(&id, 0, sizeof(id));

        // match either exact name or VID/PID
        if (strcmp(name, "USB Gamepad") == 0 ||
            (id.vendor == 0x0079 && id.product == 0x0011)) {
            std::cout << "Opened controller: “" << name
                      << "” at " << path
                      << " (vid=0x" << std::hex << id.vendor
                      << " pid=0x" << id.product << std::dec << ")\n";
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
            case BTN_TL:    // Left shoulder
            case BTN_TR:    // Right shoulder
            case BTN_X:     std::cout << "⟳ Rotate\n";           break;
            case BTN_A:     std::cout << "↓ Soft Drop (A)\n";    break;
            case BTN_B:     std::cout << "⤵ Hard Drop (B)\n";    break;
            case BTN_Y:     std::cout << "⏸ Pause/Resume (Y)\n"; break;
            case BTN_START: std::cout << "▶ Start / Restart\n";  break;
            default:                                           break;
        }
    }
}

int main() {
    int fd = open_controller();
    if (fd < 0) {
        std::cerr << "Error: could not find USB Gamepad\n";
        return 1;
    }

    struct input_event ev;
    while (true) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n == sizeof(ev)) {
            handle_event(ev);
        }
        usleep(10 * 1000);  // 10 ms throttle
    }

    close(fd);
    return 0;
}