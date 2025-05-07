#ifndef ASSETS_H
#define ASSETS_H
#include <cstdint>

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
    TILE_WHITE = 14     /* for text blocks                     */
};

/* helper: palette index used by tile t (all solid‑fill)            */
static constexpr uint8_t TILE2PAL(uint8_t t)
{
    return (t==TILE_EMPTY?0:
           t==TILE_WALL ?7:
           t==TILE_RED  ?1:
           t==TILE_GREEN?2:
           t==TILE_BLUE ?3:
           t==TILE_YELLOW?4:
           t==TILE_CYAN ?5:
           t==TILE_MAG  ?6 : 14);
}

/* ────── 16 KiB Tileset – 8 solid‑fill tiles, rest 0 ────── */
static uint8_t TILESET[16384];
inline void build_tileset()
{
    for (int t = 0; t < 256; ++t) {
        uint8_t col = TILE2PAL(t);          // 0‑15 palette index
        for (int px = 0; px < 64; ++px)
            TILESET[t * 64 + px] = col;     // ONE pixel per byte  ← fix
    }
}

/* empty 8 KiB tilemap (all TILE_EMPTY)                           */
static uint8_t BLANK_TILEMAP[8192] = {0};

#endif