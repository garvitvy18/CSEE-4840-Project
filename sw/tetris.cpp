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
