#include <tetris.hpp>

block_matrix[4][4] // current falling tetromino
playfield[COLS][ROWS] // fixed blocks on the board
main() {
   init_matrix() // clear the playfield
   prepare_first_block() // choose initial shape & colour
   loop until quit OR game_over
       user_input() // non‑blocking; may set flags
       step_timer() // advances a tick
   end loop
   if game_over
       show_game_over()
   end if
}