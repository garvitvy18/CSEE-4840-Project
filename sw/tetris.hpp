#define ROWS 20 // play‑field height
#define COLS 15 // play‑field width


// Block shapes
SHAPES = { I, O, T, S, Z, J, L }


// Matrices
BlockMatrix[4][4] // current falling tetromino
Playfield[COLS][ROWS] // fixed blocks on the board
main() {
   initMatrix() // clear the playfield
   prepareFirstBlock() // choose initial shape & colour
   loop until Quit OR GameOver
       userInput() // non‑blocking; may set flags
       stepTimer() // advances a tick
   end loop
   if GameOver
       showGameOver()
   end if
}


initMatrix()
// Set every cell of Playfield to BLANK (empty)


prepareFirstBlock()
// 1. Pick a random shape & colour for CurrentBlock
// 2. Initialise BlockMatrix accordingly
// 3. Place CurrentBlock at top‑centre start position


userInput()
// Get most recent button presses polled during Vblank


stepTimer()
// Called once per frame during vblank
// Move or rotate the current block based on userInput() unless user input is Down
// If accumulatedDelay < CurrentSpeed and userInput() is not Down
// 1. accumulatedDelay++
// Else
// accumulatedDelay = 0
// 1. moveBlock(DOWN)
// 2. If the move failed due to collision with bottom of playfield or fixed blocks:
//      • fixCurrentBlockIntoPlayfield()
//      • checkForLineClears()
//      • spawnNextBlock() (may set GameOver)
// 3. update score display


moveBlock(direction)
// Attempt to translate CurrentBlock by one cell in the specified direction
// Call detectCollision() first; if collision then return FAILURE
// Otherwise update newBlockX / newBlockY and return SUCCESS


rotateBlock()
// Compute the 90° rotated version of BlockMatrix
// If the rotated piece collides with walls or fixed blocks then cancel
// Else copy rotated data back into BlockMatrix


detectCollision(candidateX, candidateY)
// Given a hypothetical position of CurrentBlock:
//   • Check left/right boundaries
//   • Check bottom boundary
//   • Check overlap with filled cells in Playfield
// Return TRUE if any collision is detected, FALSE otherwise


fixCurrentBlockIntoPlayfield()
// Copy all non‑blank cells from BlockMatrix into Playfield
// at the current (BlockX, BlockY) offset


checkForLineClears()
// Iterate every row of Playfield
// If a row is completely filled:
//     • removeLine(rowIndex)
//     • increment LinesCleared and call calculateScore()
//     • after every 20 cleared lines call increaseSpeed()


removeLine(rowIndex)
// Delete the specified row by shifting all rows above it downward by one
// Insert a new blank row at the top of Playfield


increaseSpeed()
// Decrease the logical delay between automatic DOWN steps
// Increment Level counter (both affect difficulty)


spawnNextBlock()
// 1. Select NextShape & colour (random)
// 2. Reset BlockX, BlockY to the spawn coordinates
// 3. Initialise BlockMatrix for the new shape
// 4. If the new block immediately collides then set GameOver


calculateScore()
//Calculates score based on the number lines cleared at once and the current falling speed
