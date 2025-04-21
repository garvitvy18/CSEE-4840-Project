#define ROWS 20 // play‑field height
#define COLS 15 // play‑field width

// Block Shapes
#define SHAPE_I 100
#define SHAPE_O 101
#define SHAPE_T 102
#define SHAPE_S 103
#define SHAPE_Z 104
#define SHAPE_J 105
#define SHAPE_L 106

/* Directions */
#define LEFT 201
#define RIGHT 202
#define DOWN 203

//Default Tetromino Shapes
int shape_I[4][4] = {
    0,1,0,0,
    0,1,0,0,
    0,1,0,0,
    0,1,0,0
};

int shape_T[4][4] = {
    0,1,0,0,
    0,1,1,0,
    0,1,0,0,
    0,0,0,0
};

int shape_L[4][4] = {
    0,0,1,0,
    1,1,1,0,
    0,0,0,0,
    0,0,0,0
};

int shape_J[4][4] = {
    1,0,0,0,
    1,1,1,0,
    0,0,0,0,
    0,0,0,0
};

int shape_S[4][4] = {
    0,1,1,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0
};

int shape_Z[4][4] = {
    1,1,0,0,
    0,1,1,0,
    0,0,0,0,
    0,0,0,0
};

int shape_O[4][4] = {
    1,1,0,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0
};

init_matrix()
// Set every cell of Playfield to BLANK (empty)

prepare_first_block()
// 1. Pick a random shape & color for CurrentBlock
// 2. Initialise BlockMatrix accordingly
// 3. Place CurrentBlock at top‑centre start position

user_input()
// Get most recent button presses polled during Vblank

step_timer()
// Called once per frame during vblank
// Move or rotate the current block based on userInput() unless user input is Down
// If accumulatedDelay < CurrentSpeed and userInput() is not Down
// 1. accumulatedDelay++
// Else
// accumulatedDelay = 0
// 1. move_block(DOWN)
// 2. If the move failed due to collision with bottom of playfield or fixed blocks:
//      • fix_current_blockIntoPlayfield()
//      • check_for_line_clears()
//      • spawn_next_block() (may set GameOver)
// 3. update score display

move_block(direction)
// Attempt to translate CurrentBlock by one cell in the specified direction
// Call detect_collision() first; if collision then return FAILURE
// Otherwise update BlockX / BlockY and return SUCCESS

rotate_block()
// Compute the 90° rotated version of BlockMatrix
// If the rotated piece collides with walls or fixed blocks then cancel
// Else copy rotated data back into BlockMatrix

detect_collision(candidateX, candidateY)
// Given a hypothetical position of CurrentBlock:
//   • Check left/right boundaries
//   • Check bottom boundary
//   • Check overlap with filled cells in Playfield
// Return TRUE if any collision is detected, FALSE otherwise

fix_current_block_into_playfield()
// Copy all non‑blank cells from BlockMatrix into Playfield
// at the current (BlockX, BlockY) offset

check_for_line_clears()
// Iterate every row of Playfield
// If a row is completely filled:
//     • remove_line(rowIndex)
//     • increment LinesCleared and call calculate_score()
//     • after every 20 cleared lines call increase_speed()

remove_line(rowIndex)
// Delete the specified row by shifting all rows above it downward by one
// Insert a new blank row at the top of Playfield

increase_speed()
// Decrease the logical delay between automatic DOWN steps
// Increment Level counter (both affect difficulty)

spawn_next_block()
// 1. Select NextShape & colour (random)
// 2. Reset BlockX, BlockY to the spawn coordinates
// 3. Initialise BlockMatrix for the new shape
// 4. If the new block immediately collides then set GameOver

calculate_score()
//Calculates score based on the number lines cleared at once and the current falling speed
