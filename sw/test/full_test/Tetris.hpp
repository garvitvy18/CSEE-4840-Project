#ifndef TETRIS_HPP
#define TETRIS_HPP
#include <array>
#include <cstdint>
#include <functional>

/* grid: 15 × 20 interior cells */
constexpr int COLS = 15;
constexpr int ROWS = 20;

/* Tile indices for blocks (match palette / tileset) */
enum Cell : uint8_t {
    EMPTY = 0,
    RED   = 2,  /* I */
    GREEN = 3,  /* S */
    BLUE  = 4,  /* J */
    YELLOW= 5,  /* O */
    CYAN  = 6,  /* Z */
    MAG   = 7   /* T */
};

struct Piece {
    std::array<std::array<uint8_t,4>,4> mask;  /* 4×4 bitmap of tile index */
};

class Game {
public:
    Game();
    void step();              /* gravity / timing tick   */
    void move_left();
    void move_right();
    void rotate();
    void soft_drop();
    void hard_drop();
    void toggle_pause();

    /* provide data to renderer */
    uint8_t playfield(int x,int y) const { return field[y][x]; }
    void for_each_block(std::function<void(int,int,uint8_t)> cb) const;
    void for_each_next (std::function<void(int,int,uint8_t)> cb) const;
    int  score() const { return score_val; }
    int  lines() const { return lines_cleared; }

private:
    std::array<std::array<uint8_t,COLS>,ROWS> field{};

    Piece bag[7];
    int   bag_idx = 0;
    Piece cur, nxt;
    int   px = 5, py = 0, rot = 0;
    int   tick = 0;
    bool  paused=false;

    int   score_val = 0;
    int   lines_cleared = 0;

    void spawn();
    bool collision(int nx,int ny,const Piece& p,int r) const;
    void lock_piece();
    void clear_lines();
    Piece rotate_piece(const Piece& p,int r) const;
};

#endif