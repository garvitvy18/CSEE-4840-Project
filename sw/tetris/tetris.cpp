#include "tetris.hpp"
#include <algorithm>
#include <array>
#include <random>

//Make tetromino piece
static Tetromino make_piece(std::initializer_list<const char*> rows, uint8_t tile) {
    Tetromino t{};
    int row = 0;
    for(auto line:rows) {
        for(int col = 0; col < 4 && line[col]; ++col)
            if(line[col] == '#') t.mask[row][col] = tile;
        ++row;
    }
    return t;
}

//Define tetromino shapes
static const std::array<Tetromino, 7> SHAPES = {
    make_piece({"####", "...." ,"...." ,"...."}, RED), /* I */
    make_piece({"#..", "###", "...", "..."}, BLUE), /* J */
    make_piece({"..#", "###", "...", "..."}, MAG), /* L */
    make_piece({"##", "##", "..", ".."}, YELLOW), /* O */
    make_piece({".##", "##.", "...", "..."}, GREEN), /* S */
    make_piece({".#.", "###", "...", "..."}, CYAN), /* T */
    make_piece({"##.", ".##", "...", "..."}, PURPLE) /* Z */
};

//Rotate tetromino right
static Tetromino rot_right(const Tetromino& t) {
    Tetromino r{};
    for(int y = 0; y < 4; ++y)
        for(int x = 0; x < 4; ++x)
            r.mask[x][3 - y]= t.mask[y][x];
    return r;
}

//Rotate piece a set number of times
Tetromino Tetris::rotate_piece(const Tetromino& t, int num_rot) const {
    Tetromino q = t;
    for(int i = 0; i < num_rot; ++i) q = rot_right(q);
    return q;
}

//Generate random piece
static Tetromino rnd_piece() {
    static std::mt19937 gen{std::random_device{}()};
    static std::uniform_int_distribution<int>d(0, 6);
    return SHAPES[d(gen)];
}

//Set pieces
Tetris::Tetris() { cur=rnd_piece(); nxt=rnd_piece(); spawn();}

//Spawn current piece
void Tetris::spawn() {
    px = 5; py = 0; rot = 0; cur = nxt; nxt = rnd_piece();
    if (collision(px, py, cur, rot)){ over = true; }
}

//Calculate collisions
bool Tetris::collision(int nx, int ny, const Tetromino& pc, int r) const {
    Tetromino p = rotate_piece(pc, r);
    for(int y = 0; y < 4; ++y)
        for(int x = 0; x < 4; ++x)
            if (p.mask[y][x]) {
                int gx = nx + x, gy = ny + y;
                if (gx < 0 || gx >= COLS || gy >= ROWS) return true;
                if (gy >= 0 && field[gy][gx]) return true;
            }
    return false;
}

//Input movements
void Tetris::move_left (){ if (!paused && !over && !collision(px - 1, py, cur, rot))--px; }
void Tetris::move_right(){ if (!paused && !over && !collision(px + 1, py, cur, rot))++px; }
void Tetris::rotate() { if (!paused && !over && !collision(px, py, cur, (rot + 1) % 4))rot = (rot + 1) % 4; }
void Tetris::soft_drop() { if(!paused && !over && !collision(px, py + 1, cur, rot)) ++py; }
void Tetris::hard_drop() {
    if(paused || over) return;
    while(!collision(px, py + 1, cur, rot)) ++py;
    lock_piece();
}
void Tetris::toggle_pause(){ if (!over) paused = !paused; }

//Gravity for tetrominos
void Tetris::step() {
    if (paused || over) return;
    if (++tick % 30 == 0) {
        if(!collision(px, py + 1, cur, rot)) ++py;
        else lock_piece();
    }
}

//Lock tetromino in place
void Tetris::lock_piece() {
    Tetromino p = rotate_piece(cur, rot);
    for(int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            if (p.mask[y][x]) field[py + y][px + x] = p.mask[y][x];
    clear_lines();
    spawn();
}

//Clear lines
void Tetris::clear_lines() {
    for(int y = 0; y < ROWS; ++y){
        bool full = true;
        for(int x = 0; x < COLS; ++x) if (!field[y][x]) {full = false; break;}
        if(full) {
            for (int k = y; k > 0;--k) field[k] = field[k - 1];
            field[0].fill(0);
            ++lines_cleared; 
            score_val += 100;
        }
    }
}

void Tetris::reset() {
    lines_cleared = 0;
    score_val = 0;
    over = 0;
    for (int i = 0; i < 20; ++i) field[i].fill(0);

}

//Render helper functions

void Tetris::for_each_block(std::function<void(int, int, uint8_t)> cb) const
{
    Tetromino t = rotate_piece(cur,rot);
    for(int y = 0; y < 4; ++y)
        for(int x = 0; x < 4; ++x)
            if(t.mask[y][x]) cb(px + x, py + y, t.mask[y][x]);
}
void Tetris::for_each_next(std::function<void(int, int, uint8_t)> cb) const
{
    for(int y=0; y<4; ++y)
        for(int x=0; x<4; ++x)
            if(nxt.mask[y][x]) cb(x, y, nxt.mask[y][x]);
}