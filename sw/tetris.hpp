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

void draw_tetromino(int x, int y, char color); //for drawing tetrominos
void playfield_init(); //initialize the playfield
void assign_shape_and_color(int shape, int color); //gives shape and color to the current falling tetromino
int detect_collision(int dir); //returns 1 when tetromino collides with edge of screen or another tetromino
int move_block(int dir); //moves the current tetromino in the specified direction, returns 0 if tetromino moved else returns 1
void display_block(int x, int y, int shape, int color, int rotation); //display the tetromino on screen
void display_next_shape(); //display the next tetromino box
void update_background (); //update background
void get_next_block(); //spawn next tetromino on screen
void RotateBlock(bool dir); //rotate current tetromino
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
