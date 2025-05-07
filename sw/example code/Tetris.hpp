#ifndef TETRIS_HPP
#define TETRIS_HPP
#include <array>
#include <cstdint>
#include <functional>

constexpr int COLS = 15;
constexpr int ROWS = 20;

/* tile colours match assets.h */
enum Cell : uint8_t {
    EMPTY = 0,
    RED   = 2,
    GREEN = 3,
    BLUE  = 4,
    YELLOW= 5,
    CYAN  = 6,
    MAG   = 7
};

struct Piece { std::array<std::array<uint8_t,4>,4> mask{}; };

class Game {
public:
    Game();
    void step();              /* gravity tick */
    void move_left();
    void move_right();
    void rotate();
    void soft_drop();
    void hard_drop();
    void toggle_pause();

    /* expose for renderer */
    uint8_t playfield(int x,int y) const { return field[y][x]; }
    void for_each_block(std::function<void(int,int,uint8_t)> cb) const;
    void for_each_next (std::function<void(int,int,uint8_t)> cb) const;
    int score() const { return score_val; }
    int lines() const { return lines_cleared; }
    bool game_over() const { return over; }

private:
    std::array<std::array<uint8_t,COLS>,ROWS> field{};
    Piece cur, nxt;
    int   px = 5, py = 0, rot = 0;
    int   tick = 0;
    bool  paused=false, over=false;
    int   score_val = 0;
    int   lines_cleared = 0;

    void spawn();
    bool collision(int nx,int ny,const Piece& p,int r) const;
    void lock_piece();
    void clear_lines();
    Piece rotate_piece(const Piece& p,int r) const;
};

#endif