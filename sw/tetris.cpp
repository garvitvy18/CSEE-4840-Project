#include <tetris.hpp>

int block_matrix[4][4] // current falling tetromino
int playfield[COLS][ROWS] // fixed blocks on the board
main() {
   
}

//for drawing tetrominos
void draw_tetromino(int x, int y, char color) {

}

//initialize the playfield
void playfield_init() {
    
}

//gives shape and color to the current falling tetromino
void assign_shape_and_color(int shape, int color) {

}

//returns 1 when tetromino collides with edge of screen or another tetromino
int detect_collision(int dir) {

}

//moves the current tetromino in the specified direction, returns 0 if tetromino moved else returns 1
int move_block(int dir) {

}

//display the tetromino on screen
void display_block(int x, int y, int shape, int color, int rotation) {

}

//display the next tetromino box
void display_next_shape() {

}

//update background
void update_background() {

}

//spawn next tetromino on screen
void get_next_block() {

}

//rotate current tetromino
void RotateBlock(bool dir); 
void update_screen(); //update PPU VRAM and OAM
void check_clear(); //check for line clears
void clear_line(int Number); //perform line clear
int get_random_color(); //get random color
int get_random_shape(); //get random shape
void update_score(); //update the score graphic
void update_speed(); //update the speed graphic
void update_level(); //update the level graphic
void update_lines_cleared(); //update lines cleared
void start_screen(); //display start screen
int  increase_speed(); //increase falling speed
int  display_game_over(); //display game over screen