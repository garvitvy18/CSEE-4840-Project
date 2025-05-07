#include "assets.h"
#include "font.h"
#include "tetris.hpp"
#include <fcntl.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

/*
################################################
Functions and constants to Handle Memory Mapping
################################################
*/

// Memory Map
constexpr off_t PHY_TM = 0xff200000;
constexpr off_t PHY_PA = 0xff202000;
constexpr off_t PHY_TS = 0xff204000;
static volatile uint8_t *TM,*PA,*TS;

// Function to Map FPGA
static void map_fpga()
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd<0){perror("mem"); _exit(1);}
    #define MAP(base, size, ptr) \
        ptr=(uint8_t*)mmap(nullptr, size, PROT_READ | PROT_WRITE,MAP_SHARED, fd, base); \
        if(ptr == MAP_FAILED) {perror("mmap"); _exit(1);}
    MAP(PHY_TM, 8192, TM)
    MAP(PHY_PA, 64, PA)
    MAP(PHY_TS, 16384, TS)
    close(fd);
}

//Load palette and tiles into memory
static void load_assets()
{
    for (int i= 0; i < 16; ++i){
        uint32_t c = PALETTE24[i];
        PA[i*4 + 0] = c & 0xFF;
        PA[i*4 + 1] = (c>>8) & 0xFF;
        PA[i*4 + 2] = (c >> 16) & 0xFF;
        PA[i*4 + 3] = 0;
    }
    build_tileset();
    memcpy((void*)TS, TILESET, 16384);
    memset((void*)TM, 0, 8192);
}

/*
################################################
Helper functions to handle drawing to screen
################################################
*/

//Write tile to screen
static inline void put(int col, int row, uint8_t tile) {TM[row*TM_STRIDE+col]=tile;}

//Draw a rectangle out of tiles
static void rect(int x0, int y0, int w, int h, uint8_t tile) {
    for (int y = y0 ; y < y0 + h ; ++y) 
        for (int x = x0 ; x < x0 + w; ++x) put(x, y, tile);
}

//Draw a frame out of tiles
static void frame(int x0, int y0, int w, int h, uint8_t tile){
    for (int x = x0; x < x0 + w; ++x) put(x, y0, tile), put(x, y0+h-1, tile); 
    for (int y = y0; y < y0 + h; ++y) put(x0 ,y ,tile), put( x0 + w-1 ,y ,tile);
}

//Draw char using font
static void draw_char(int col, int row, char ch)
{
    if(ch < ' '|| ch > '_') return;
    const uint8_t* bmp = FONT[ch - 32];
    for (int y = 0; y < 7; ++y) for (int x = 0; x < 5; ++x)
            if(bmp[y] & (1 << (4 - x))) put(col + x, row + y, TILE_WHITE);
}

//Draw string by calling draw char
static void draw_string(int col, int row, const char* str)
{for (int i = 0; str[i]; ++i) draw_char(col + i*6, row, str[i]);}

//Clear area
static void clear_area(int col,int row,int w,int h)
{ rect(col, row, w, h, TILE_EMPTY); }

/*
################################################
Functions for game rendering
################################################
*/

//Draw playfield border
static void draw_borders() {
    frame(PF_LEFT, PF_TOP, PF_WIDTH, PF_HEIGHT, TILE_WALL);
    frame(NEXT_COL-1, NEXT_ROW-1, 6, 6, TILE_WALL);
}

//Draw playfield play area
static void draw_playfield(const Tetris& game) {
    for (int y = 0; y < ROWS; ++y)
        for(int x = 0;x < COLS; ++x)
            put(PF_LEFT + 1 + x, PF_TOP + 1 + y, game.playfield(x,y));
}