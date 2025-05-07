// debug_usb_gamepad.cpp
// Opens your USB Gamepad and prints *all* input_event structs so you can see
// exactly which type/code/value your controller is sending when you press buttons.

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <cstring>

// Try /dev/input/event0..event31 and match Vendor=0x0079, Product=0x0011 or name "USB Gamepad"
int open_controller() {
    struct input_id id;
    char path[64], name[256];
    for (int i = 0; i < 32; ++i) {
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) name[0] = '\0';
        if (ioctl(fd, EVIOCGID, &id) < 0) memset(&id, 0, sizeof(id));

        if (strcmp(name, "USB Gamepad") == 0 ||
            (id.vendor == 0x0079 && id.product == 0x0011)) {
            std::cout << "Opened controller: \"" << name
                      << "\" at " << path
                      << " (vid=0x" << std::hex << id.vendor
                      << " pid=0x" << id.product << std::dec << ")\n";
            return fd;
        }
        close(fd);
    }
    return -1;
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
            // Print timestamp, type, code, and value
            std::cout << "Event: time " << ev.time.tv_sec << "." << ev.time.tv_usec
                      << "  type=" << ev.type
                      << "  code=" << ev.code
                      << "  value=" << ev.value
                      << "\n";
        }
        // small sleep to avoid busy spin
        usleep(5 * 1000);
    }

    close(fd);
    return 0;
}