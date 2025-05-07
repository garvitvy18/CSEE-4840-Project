// list_input_devices.cpp
// Enumerate /dev/input/event* devices and print their name, vendor, product, and version.
// Compile with:
//   g++ list_input_devices.cpp -o list_input_devices
// Run as root (or with proper /dev/input permissions):
//   sudo ./list_input_devices

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cstdio>

int main() {
    char path[64];
    for (int i = 0; i < 32; ++i) {
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        int fd = open(path, O_RDONLY);
        if (fd < 0) continue;

        // Get device name
        char name[256] = "Unknown";
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) {
            strncpy(name, "Unknown", sizeof(name));
        }

        // Get vendor/product/version
        struct input_id id;
        if (ioctl(fd, EVIOCGID, &id) < 0) {
            memset(&id, 0, sizeof(id));
        }

        std::cout << "Device:   " << path << "\n"
                  << "  Name:    " << name << "\n"
                  << "  Vendor:  0x" << std::hex << id.vendor
                  << ", Product: 0x" << id.product
                  << ", Version: 0x" << id.version << std::dec
                  << "\n\n";

        close(fd);
    }
    return 0;
}