#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define PALETTE_ADDR 0xFF202000UL
#define TILEMAP_ADDR 0xFF200000UL
#define TILESET_ADDR 0xFF204000UL

#define PALETTE_SIZE 64
#define TILEMAP_SIZE 8192
#define TILESET_SIZE 16384

int main(void) {
    int mem_fd, file_fd;
    void *map_ptr;
    struct stat st;
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t map_size;
    char *buffer;

    // Open /dev/mem for direct physical access (requires appropriate privileges)
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("open(/dev/mem)");
        return EXIT_FAILURE;
    }
    printf("Opened /dev/mem\n");

    // --- Helper lambda to load a file and write to a physical address ---
    #define LOAD_FILE_TO_PHYS(fname, phys_addr, expected_size)               \
    do {                                                                    \
        printf("Loading %s... ", fname);                                    \
        file_fd = open(fname, O_RDONLY);                                    \
        if (file_fd < 0) { perror("open"); return EXIT_FAILURE; }           \
        if (fstat(file_fd, &st) < 0) { perror("fstat"); return EXIT_FAILURE; } \
        if (st.st_size != (expected_size)) {                                \
            fprintf(stderr, "Unexpected size of %s (got %ld, expected %d)\n", fname, (long)st.st_size, expected_size); \
            return EXIT_FAILURE;                                            \
        }                                                                   \
        buffer = malloc(expected_size);                                     \
        if (!buffer) { fprintf(stderr, "malloc failed\n"); return EXIT_FAILURE; } \
        if (read(file_fd, buffer, expected_size) != (expected_size)) { perror("read"); free(buffer); return EXIT_FAILURE; } \
        close(file_fd);                                                     \
                                                                           \
        /* Map the physical memory region */                                \
        map_size = ((expected_size + page_size - 1) / page_size) * page_size; \
        map_ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, (off_t)(phys_addr)); \
        if (map_ptr == MAP_FAILED) { perror("mmap"); free(buffer); return EXIT_FAILURE; } \
                                                                           \
        /* Copy buffer into mapped memory */                                \
        memcpy(map_ptr, buffer, expected_size);                             \
        /* Optionally, ensure write completes */                            \
        if (msync(map_ptr, expected_size, MS_SYNC) < 0) { perror("msync"); } \
        munmap(map_ptr, map_size);                                          \
        free(buffer);                                                       \
                                                                           \
        printf("Wrote %d bytes to 0x%08lX\n", expected_size, (unsigned long)(phys_addr)); \
    } while (0)

    // Load each binary into its target physical address
    LOAD_FILE_TO_PHYS("palette1.bin", PALETTE_ADDR, PALETTE_SIZE);
    LOAD_FILE_TO_PHYS("tilemap1.bin", TILEMAP_ADDR, TILEMAP_SIZE);
    LOAD_FILE_TO_PHYS("tileset1.bin", TILESET_ADDR, TILESET_SIZE);

    close(mem_fd);
    return EXIT_SUCCESS;
}
