#include <stdio.h>
#include <stdlib.h>
#include <tetris.hpp>

int block_matrix[4][4] // current falling tetromino
int playfield[COLS][ROWS] // fixed blocks on the board
main() {
    int Return = 0;
    char Key, ScanCode;
    int Counter = 0;
//Start the game and keep it running continuously
    while (!Quit && !GameOver) {
        if (++Counter >= Speed) {
            Counter = 0;
            move_block(DOWN);
            SoundDrop();
        }
   
    }
}
//for drawing tetrominos
void draw_tetromino(int x, int y, char color) {
   //Convert cell coordinates to pixel coordinates
    int pixel_x = LEFT + x * 21;
    int pixel_y = TOP  + y * 21;
   

}

//initialize the playfield
void playfield_init() {
    //Initialize the playfield based on the total columns and rows
    for(int i=0;i<COLS;i++){
        for(int j=0;j<ROWS;j++){
            playfield[i][j]=0;
        }
    }
    
}

//gives shape and color to the current falling tetromino
void assign_shape_and_color(int shape, int color) {
     switch(shape){
        case SHAPE_I:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    block_matrix[j][i] = shape_I[i][j] ? color : 0;
                }
            }
            break;
        case SHAPE_O:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    block_matrix[j][i] = shape_O[i][j] ? color : 0;
                }
            }
            break;
         case SHAPE_T:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    block_matrix[j][i] = shape_T[i][j] ? color : 0;
                }
            }
            break;
        case SHAPE_S:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    block_matrix[j][i] = shape_S[i][j] ? color : 0;
                }
            }
            break;
         case SHAPE_Z:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    block_matrix[j][i] = shape_Z[i][j] ? color : 0;
                }
            }
            break;
        case SHAPE_J:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    block_matrix[j][i] = shape_J[i][j] ? color : 0;
                }
            }
            break;
        case SHAPE_L:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    block_matrix[j][i] = shape_L[i][j] ? color : 0;
                }
            }
            break;
        default:
            // Unknown shape: clear out
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    block_matrix[j][i] = 0;
            break;
        
    }
    current_shape=shape;
    current_color=color;


}

//returns 1 when tetromino collides with edge of screen or another tetromino
int detect_collision(int dir) {
    //For computing the new position based on the current position
    int new_x=current_x;
    int new_y=current_y;

    // Adjust for the attempted move
    if (dir == LEFT)       new_x--;
    else if (dir == RIGHT) new_x++;
    else if (dir == DOWN)  new_y++;
 // For each cell in the 4×4 block matrix
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            // Skip empty cells
            if (block_matrix[i][j] == 0) 
                continue;

            // Compute the board coordinates of this cell
            int bx = new_x + i;
            int by = new_y + j;

            // Collision if out of horizontal bounds
            if (bx < 0 || bx >= COLS)
                return 1;
             // Collision if below the bottom
            if (by >= ROWS)
                return 1;

            // Collision if landing on an occupied cell
            if (playfield[bx][by] != 0)
                return 1;
        }
    }

    // No collision detected
    return 0;

}

//moves the current tetromino in the specified direction, returns 0 if tetromino moved else returns 1
int move_block(int dir) {
    // First check for collision by calling the function 
    if (detect_collision(dir)) {
        // If we hit something when moving down, lock the piece and spawn next
        if (dir == DOWN) {
            get_next_block();
        }
        return 1;  // collision occurred
        } else {
        // No collision: update the current position
        if (dir == LEFT) {
            current_x--;
        } 
        else if (dir == RIGHT) {
            current_x++;
        } 
        else if (dir == DOWN) {
            current_y++;
        }
        return 0;  // moved successfully
    }

}

//display the tetromino on screen
void display_block(int x, int y, int shape, int color, int rotation) {
    current_x=x;
    current_y=y;
    current_shape=shape;
    current_color=color;

    assign_shape_and_color(shape,color);

      // Apply rotation
    int times = 0;
    switch (rotation) {
        case NINETY:     times = 1; break;
        case ONE_EIGHTY: times = 2; break;
        case TWO_SEVENTY:times = 3; break;
        case ZERO:
        default:         times = 0; break;
    }
    for (int r = 0; r < times; ++r) {
        // rotate 90° CW
        RotateBlock(true);
    }

    //Display the blocks
      //Draw each occupied cell
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (block_matrix[i][j] != 0) {
                draw_tetromino(x + i, y + j, block_matrix[i][j]);
            }
        }
    }

}

//display the next tetromino box
void display_next_shape() {
        // Build a temporary 4×4 color matrix for the next piece
    int temp[4][4] = { 0 };

    switch (next_shape) {
        case SHAPE_I:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    temp[i][j] = shape_I[i][j] ? next_color : 0;
                }
            }
            break;
        case SHAPE_O:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    temp[i][j] = shape_O[i][j] ? next_color : 0;
                }
            }
            break;
          case SHAPE_T:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    temp[i][j] = shape_T[i][j] ? next_color : 0;
                }
            }
            break;
        case SHAPE_S:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    temp[i][j] = shape_S[i][j] ? next_color : 0;
                }
            }
            break;
        case SHAPE_Z:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    temp[i][j] = shape_Z[i][j] ? next_color : 0;
                }
            }
            break;
         case SHAPE_J:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    temp[i][j] = shape_J[i][j] ? next_color : 0;
                }
            }
            break;
        case SHAPE_L:
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    temp[i][j] = shape_L[i][j] ? next_color : 0;
                }
            }
            break;
        default:
            // If unknown shape, leave temp all zero
            break;
    }
     // Draw the preview at a fixed offset (in grid cells)
    const int PREVIEW_X = COLS + 2;
    const int PREVIEW_Y = 0;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (temp[i][j] != 0) {
                draw_tetromino(PREVIEW_X + j,
                               PREVIEW_Y + i,
                               temp[i][j]);
            }
        }
    }

}

//update background
void update_background() {
 // Mark every cell dirty so that draw_tetromino always redraws it
    for (int x = 0; x < COLS; ++x) {
        for (int y = 0; y < ROWS; ++y) {
            PreviousScreenLayout[x][y] = BLANK - 1;
        }
    }

    // Redraw the entire playfield
    for (int x = 0; x < COLS; ++x) {
        for (int y = 0; y < ROWS; ++y) {
            if (playfield[x][y] != 0) {
                // occupied → draw that block color
                draw_tetromino(x, y, playfield[x][y]);
            } else {
                // empty → clear it
                draw_tetromino(x, y, BLANK);
            }
        }
    }

}

//spawn next tetromino on screen
void get_next_block() {

}

//rotate current tetromino
void RotateBlock(bool dir) {

}

//update PPU VRAM and OAM
void update_screen() {

}

//check for line clears
void check_clear() {

}

//perform line clear
void clear_line(int num) {

}

//get random color
int get_random_color() {

}

//get random shape
int get_random_shape() {

}

//update the score graphic
void update_score() {

} 

//update the speed graphic
void update_speed() {

} 

//update the level graphic
void update_level() {

}

//update lines cleared
void update_lines_cleared() {

}

//display start screen
void start_screen() {

}

//increase falling speed
int  increase_speed() {

}

//display game over screen
int  display_game_over() {

}
