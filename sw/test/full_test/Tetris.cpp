/*
 * Tetris game logic for FPGA tile‑graphics demo
 * --------------------------------------------
 *  • Playfield: 15 × 20 (COLS × ROWS) interior cells
 *  • Each cell stores a TILE_‥ index that the renderer writes
 *    directly into the VGA tile‑map at run‑time.
 *  • No STL containers except <array>; works on bare‑bones
 *    cross‑compiler as long as <random> and <algorithm> exist.
 */

 #include "Tetris.hpp"
 #include <algorithm>
 #include <array>
 #include <random>
 
 /* ────────────────────────────────────────────────────────────── */
 /* Helper: build a 4×4 piece mask from ASCII art rows ('#' = block) */
 static Piece make_piece(std::initializer_list<const char*> rows, uint8_t tile)
 {
     Piece p{};
     int r = 0;
     for (auto line : rows) {
         for (int c = 0; line[c] && c < 4; ++c)
             if (line[c] == '#') p.mask[r][c] = tile;
         ++r;
     }
     return p;
 }
 
 /* Seven tetrominoes in spawn orientation (SRS order) */
 static const std::array<Piece, 7> SHAPES = {
     /* I (RED)    */ make_piece({"####","....","....","...."}, RED),
     /* J (BLUE)   */ make_piece({"#..","###","...","..."   }, BLUE),
     /* L (MAG)    */ make_piece({"..#","###","...","..."   }, MAG),
     /* O (YELLOW) */ make_piece({"##","##","..",".."       }, YELLOW),
     /* S (GREEN)  */ make_piece({".##","##.","...","..."   }, GREEN),
     /* T (CYAN)   */ make_piece({".#.","###","...","..."   }, CYAN),
     /* Z (MAG)    */ make_piece({"##."," .##","...","..."  }, MAG)
 };
 
 /* ───────────────────────── Piece rotation ───────────────────── */
 static Piece rot_right(const Piece& p)
 {
     Piece r{};
     for (int y = 0; y < 4; ++y)
         for (int x = 0; x < 4; ++x)
             r.mask[x][3 - y] = p.mask[y][x];
     return r;
 }
 
 Piece Game::rotate_piece(const Piece& p, int rot) const
 {
     Piece r = p;
     for (int i = 0; i < rot; ++i) r = rot_right(r);
     return r;
 }
 
 /* ───────────────────────── RNG + bag logic ──────────────────── */
 static std::mt19937& rng()
 {
     static std::mt19937 gen([] {
         std::random_device rd;
         return std::mt19937(rd());
     }());
     return gen;
 }
 
 static Piece random_piece()
 {
     static std::uniform_int_distribution<int> dist(0, 6);
     return SHAPES[dist(rng())];
 }
 
 /* ───────────────────────── Game ctor / state ────────────────── */
 Game::Game()
 {
     cur = random_piece();
     nxt = random_piece();
     spawn();
 }
 
 /* place current piece at top‑middle */
 void Game::spawn()
 {
     px = 5;
     py = 0;
     rot = 0;
     cur = nxt;
     nxt = random_piece();
 }
 
 /* collision with walls, floor or locked blocks */
 bool Game::collision(int nx, int ny,
                      const Piece& piece, int r) const
 {
     Piece p = rotate_piece(piece, r);
     for (int y = 0; y < 4; ++y)
         for (int x = 0; x < 4; ++x)
             if (p.mask[y][x]) {
                 int gx = nx + x;
                 int gy = ny + y;
                 if (gx < 0 || gx >= COLS || gy >= ROWS) return true;
                 if (gy >= 0 && field[gy][gx])            return true;
             }
     return false;
 }
 
 /* ─────────── public mutators called from input handler ───────── */
 void Game::move_left () { if (!paused && !collision(px - 1, py, cur, rot)) --px; }
 void Game::move_right() { if (!paused && !collision(px + 1, py, cur, rot)) ++px; }
 void Game::rotate    () { if (!paused && !collision(px, py, cur, (rot + 1) % 4)) rot = (rot + 1) % 4; }
 void Game::soft_drop()  { if (!paused) ++py; }
 void Game::hard_drop()
 {
     if (paused) return;
     while (!collision(px, py + 1, cur, rot)) ++py;
     lock_piece();
 }
 void Game::toggle_pause() { paused = !paused; }
 
 /* ─────────── one step of gravity (called every 1/60 s) ───────── */
 void Game::step()
 {
     if (paused) return;
     if (++tick % 30 == 0) {                    /* ~2 Hz fall speed */
         if (!collision(px, py + 1, cur, rot))
             ++py;
         else
             lock_piece();
     }
 }
 
 /* merge piece into playfield */
 void Game::lock_piece()
 {
     Piece p = rotate_piece(cur, rot);
     for (int y = 0; y < 4; ++y)
         for (int x = 0; x < 4; ++x)
             if (p.mask[y][x]) {
                 int gx = px + x;
                 int gy = py + y;
                 if (gx >= 0 && gx < COLS && gy >= 0 && gy < ROWS)
                     field[gy][gx] = p.mask[y][x];
             }
     clear_lines();
     spawn();
 }
 
 /* remove full rows and shift */
 void Game::clear_lines()
 {
     for (int y = 0; y < ROWS; ++y) {
         bool full = true;
         for (int x = 0; x < COLS; ++x)
             if (!field[y][x]) { full = false; break; }
         if (full) {
             for (int yy = y; yy > 0; --yy) field[yy] = field[yy - 1];
             field[0].fill(0);
             ++lines_cleared;
             score_val += 100;
         }
     }
 }
 
 /* helpers used by renderer --------------------------------------*/
 void Game::for_each_block(std::function<void(int,int,uint8_t)> cb) const
 {
     Piece p = rotate_piece(cur, rot);
     for (int y = 0; y < 4; ++y)
         for (int x = 0; x < 4; ++x)
             if (p.mask[y][x])
                 cb(px + x, py + y, p.mask[y][x]);
 }
 
 void Game::for_each_next(std::function<void(int,int,uint8_t)> cb) const
 {
     for (int y = 0; y < 4; ++y)
         for (int x = 0; x < 4; ++x)
             if (nxt.mask[y][x])
                 cb(x, y, nxt.mask[y][x]);
 }