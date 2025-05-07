#ifndef ASSETS_H
#define ASSETS_H
#include <cstdint>

/* ────── Hardware layout constants ───────── */
constexpr int TILE_COLS = 80;         /* FPGA tilemap width  */
constexpr int TILE_ROWS = 60;         /* height              */
constexpr int TM_STRIDE = 128;        /* bytes per tile‑row  */

/* playfield: 15×20 interior, framed 1‑tile border = 17×22  */
constexpr int PF_WIDTH  = 17;
constexpr int PF_HEIGHT = 22;         /* top & bottom walls  */

/* centred before ───────── PF_LEFT/PF_TOP */
constexpr int PF_LEFT  = 4;   /* was (TILE_COLS-PF_WIDTH)/2  = 31 */
constexpr int PF_TOP   = 6;   /* was 19 */

/* HUD offsets (SCORE/LINES) – bottom‑right corner area      */
constexpr int HUD_COL = PF_LEFT + PF_WIDTH + 2;      /* two tiles gap */
constexpr int HUD_SCORE_ROW = PF_TOP + PF_HEIGHT - 9;/* 9 tiles above bottom */
constexpr int HUD_LINES_ROW = HUD_SCORE_ROW + 4;

/* NEXT box (4×4 interior + walls) – above playfield */
constexpr int NEXT_COL = PF_LEFT + PF_WIDTH + 2;
constexpr int NEXT_ROW = PF_TOP - 8;

/* ────── Palette (index → 24‑bit RGB) ────── */
static constexpr uint32_t PALETTE24[16] = {
    0x000000, 0xFF0000, 0x00FF00, 0x0000FF,
    0xFFFF00, 0x00FFFF, 0xFF00FF, 0x808080,
    0x404040, 0x606060, 0xA0A0A0, 0xC0C0C0,
    0xE0E0E0, 0xF0F0F0, 0xFFFFFF, 0xFFFFFF
};

/* ────── Tile indices ────── */
enum : uint8_t {
    TILE_EMPTY = 0,
    TILE_WALL  = 1,
    TILE_RED   = 2,
    TILE_GREEN = 3,
    TILE_BLUE  = 4,
    TILE_YELLOW= 5,
    TILE_CYAN  = 6,
    TILE_MAG   = 7,
    TILE_WHITE = 14
};

/* palette index each solid‑fill tile uses */
static constexpr uint8_t TILE2PAL(uint8_t t)
{
    switch(t){
        case TILE_WALL : return 7;
        case TILE_RED  : return 1;
        case TILE_GREEN: return 2;
        case TILE_BLUE : return 3;
        case TILE_YELLOW:return 4;
        case TILE_CYAN : return 5;
        case TILE_MAG  : return 6;
        case TILE_WHITE: return 14;
        default        : return 0;
    }
}

/* ────── Tileset: 8 solid‑colour tiles only ────── */
static uint8_t TILESET[16384];
inline void build_tileset()
{
    for (int t = 0; t < 256; ++t) {
        uint8_t col = TILE2PAL(t);
        for (int px = 0; px < 64; ++px)
            TILESET[t * 64 + px] = col;           /* 1 pixel / byte */
    }
}

/* blank tilemap (filled runtime) */
static uint8_t BLANK_TILEMAP[8192] = {0};

#endif