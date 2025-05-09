#include "assets.h"
#include "font.h"
#include "tetris.hpp"
#include <fcntl.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip> 
/*
######################################
Memory Mapping Functions and Constants
######################################
*/

//Memory Map
constexpr off_t PHY_TM = 0xff200000;
constexpr off_t PHY_PA = 0xff202000;
constexpr off_t PHY_TS = 0xff204000;
static volatile uint8_t *TM,*PA,*TS;

//Map FPGA memory
static void map_fpga() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {perror("mem"); _exit(1);}
    #define MAP(base,sz,ptr) \
        ptr = (uint8_t*) mmap(nullptr, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base); \
        if (ptr == MAP_FAILED){perror("mmap"); _exit(1);}
    MAP(PHY_TM, 8192, TM)
    MAP(PHY_PA, 64, PA)
    MAP(PHY_TS, 16384, TS)
    close(fd);
}

//Load palette and tile graphics
static void load_assets() {
    for (int i = 0; i < 16; ++i){
        uint32_t color = PALETTE24[i];
        PA[i*4+0] = color & 0xFF;
        PA[i*4+1] = (color >> 8) & 0xFF;
        PA[i*4+2] = (color >> 16) & 0xFF;
        PA[i*4+3] = 0;
    }
    {
        std::ifstream tf("tiles.hex");
        if (tf) {
            tf >> std::hex;  // parse from hex format
            for (size_t i = 0; i < sizeof(TILESET); ++i) {
                unsigned int v;
                if (!(tf >> v)) {
                    // malformed or too short: fall back to building
                    build_tileset();
                    break;
                }
                TILESET[i] = static_cast<uint8_t>(v);
            }
        } else {
            // couldn’t open file: build at runtime
            build_tileset();
        }
    }
    memcpy((void*) TS, TILESET, 16384);
    memset((void*) TM, 0, 8192);
}

/*
######################################
Helper Functions to Draw Objects
######################################
*/

//Put tile in tile map
static inline void put(int col, int row, uint8_t tile)
{ TM[row * TM_STRIDE + col] = tile; }

//Draw a rectangle using tiles
static void rect(int x0, int y0, int w, int h, uint8_t tile) {
    for (int y = y0; y < y0 + h; ++y)
        for (int x = x0; x < x0 + w; ++x) put(x, y, tile);
}

//Draw a frame using tiles
static void frame(int x0, int y0, int w, int h, uint8_t tile){
    for (int x = x0; x < x0 + w; ++x) put(x, y0, tile), put(x, y0 + h - 1, tile);
    for (int y = y0; y < y0 + h; ++y) put(x0, y, tile), put(x0 + w - 1, y, tile);
}

//Render char using font
static void draw_char(int col,int row,char ch) {
    if (ch < ' ' || ch > '_') return;
    const uint8_t* bmp = FONT[ch - 32];
    for (int y = 0; y < 7; ++y)
        for (int x = 0; x < 5; ++x)
            if (bmp[y] & (1 << (4 - x))) put(col + x, row + y, TILE_WHITE);
}

//Render string using draw_char
static void draw_string(int col, int row, const char*str) {
    for (int i = 0; str[i]; ++i) draw_char(col + i * 6, row, str[i]);
}

//Clear area
static void clear_area(int col, int row, int w, int h) {rect(col, row, w, h, TILE_EMPTY); }


/*
######################################
Function to Render Tetris
######################################
*/

//Draw playfield borders
static void draw_borders() {
    frame(PF_LEFT, PF_TOP, PF_WIDTH, PF_HEIGHT, TILE_WALL);
    frame(NEXT_COL - 1, NEXT_ROW - 1, 6, 6, TILE_WALL);
}

//Draw playfield
static void draw_playfield(const Tetris& t) {
    for (int y = 0; y < ROWS; ++y)
        for (int x = 0; x < COLS; ++x)
            put(PF_LEFT + 1 + x, PF_TOP + 1 + y, t.playfield(x,y));
}

//Draw tetromino piece
static void draw_piece(const Tetris& t) {
    t.for_each_block([](int x, int y, uint8_t tile){
        put(PF_LEFT + 1 + x, PF_TOP + 1 + y, tile);
    });
}

//Draw next tetromino piece
static void draw_next(const Tetris& t) {

    rect(NEXT_COL,NEXT_ROW,4,4,TILE_EMPTY); //Clear next piece square
    t.for_each_next([](int x, int y, uint8_t tile){
        put(NEXT_COL + x, NEXT_ROW + y, tile);
    });
}

//Draw Score and Lines cleared
static void draw_hud(const Tetris& t) {
    char buf[8];
    sprintf(buf, "%d", t.score());
    clear_area(HUD_COL + 40, HUD_SCORE_ROW, 40, 10);
    draw_string(HUD_COL, HUD_SCORE_ROW, "SCORE");
    draw_string(HUD_COL + 40, HUD_SCORE_ROW, buf);

    sprintf(buf, "%d", t.lines());
    clear_area(HUD_COL + 40, HUD_LINES_ROW, 40, 10);
    draw_string(HUD_COL, HUD_LINES_ROW, "LINES");
    draw_string(HUD_COL + 40, HUD_LINES_ROW, buf);
}

/*
######################################
Game Logic State Machine
######################################
*/

//Game logic state machine
enum State {START, PLAY, OVER};
static State state = START;

//Open USB Controller
static int open_kbd() {
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

        // match on exact name or known VID/PID
        if (strcmp(name, "USB Gamepad") == 0 ||
            (id.vendor == 0x0079 && id.product == 0x0011))
        {
            printf("Using controller: %s (%s)\n", name, path);
            return fd;
        }
        close(fd);
    }
    return -1;
}


//Read Keyboard input
static void poll_input(Tetris& t, int fd) {
    struct input_event ev;
    // read all pending events
    while (read(fd, &ev, sizeof(ev)) == sizeof(ev)) {
        switch (state) {
            case START:
                // Start button → PLAY
                if (ev.type == EV_KEY && ev.value == 1 && ev.code == 297) {
                    state = PLAY;
                    clear_area(0, 0, 80, 60);
                }
                break;

            case PLAY:
                // D‑pad (ABS_HAT0X = code 0, ABS_HAT0Y = code 1)
                if (ev.type == EV_ABS) {
                    if (ev.code == 0) {              // left/right
                        if (ev.value == 0)          t.move_left();
                        else if (ev.value == 255)   t.move_right();
                    }
                    else if (ev.code == 1) {         // down
                        if (ev.value == 255)        t.soft_drop();
                    }
                }
                // Buttons (EV_KEY + value==1)
                else if (ev.type == EV_KEY && ev.value == 1) {
                    switch (ev.code) {
                        case 288:  // X
                        case 292:  // L
                        case 293:  // R
                            t.rotate();
                            break;
                        case 289:  // A
                            t.soft_drop();
                            break;
                        case 290:  // B
                            t.hard_drop();
                            break;
                        case 296:  // Select
                            t.toggle_pause();
                            break;
                    }
                }
                break;

            case OVER:
                // Start → reset + PLAY
                if (ev.type == EV_KEY && ev.value == 1 && ev.code == 297) {
                    t.reset();
                    clear_area(0, 0, 80, 60);
                    state = PLAY;
                }
                break;
        }
    }
}

//Show start screen
static void show_start() {
    memset((void*)TM, 0, 8192);
    draw_string(10, 20, "TETRIS FPGA");
    draw_string(10, 40, "PRESS START");
    draw_string(10, 50, "TO START");
}

//Show game over screen
static void show_game_over(Tetris& t) {
    char buf[8];
    clear_area(0, 0, 80, 60);
    draw_string(10, 10, "GAME OVER");
    draw_string(10, 40, "START:");
    draw_string(20, 50, "RESTART");
    sprintf(buf, "%d", t.score());
    draw_string(20, 20, "SCORE");
    draw_string(60, 20, buf);
    sprintf(buf, "%d", t.lines());
    draw_string(20, 30, "LINES");
    draw_string(60, 30, buf);
}

//Main program loop
int main() {
    map_fpga();
    load_assets();
    int kbd = open_kbd(); if (kbd < 0) {perror("kbd"); return 1;}

    Tetris tetris;
    show_start();

    while(true) {
        poll_input(tetris, kbd);
        if (state == PLAY) {
            tetris.step();
            draw_borders();
            draw_playfield(tetris);
            draw_piece(tetris);
            draw_next(tetris);
            draw_hud(tetris);
            if (tetris.game_over()) { state=OVER; show_game_over(tetris); }
        } else if (state == START){
            /* nothing */
        } else if(state == OVER){
            /* waiting for key */
        }
        usleep(16666); //Frame timer
    }
}
