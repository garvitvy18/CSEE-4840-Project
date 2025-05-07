#include "assets.h"
#include "font5x7.h"
#include "Tetris.hpp"
#include <fcntl.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

/* physical addresses (lw bridge) */
constexpr off_t PHY_TM = 0xff200000;
constexpr off_t PHY_PA = 0xff202000;
constexpr off_t PHY_TS = 0xff204000;
static volatile uint8_t *TM,*PA,*TS;

/* ────── mmap FPGA ────── */
static void map_fpga()
{
    int fd=open("/dev/mem",O_RDWR|O_SYNC);
    if(fd<0){perror("mem");_exit(1);}
#define MAP(base,sz,ptr) \
    ptr=(uint8_t*)mmap(nullptr,sz,PROT_READ|PROT_WRITE,MAP_SHARED,fd,base);\
    if(ptr==MAP_FAILED){perror("mmap");_exit(1);}
    MAP(PHY_TM,8192 ,TM)
    MAP(PHY_PA,64   ,PA)
    MAP(PHY_TS,16384,TS)
    close(fd);
}

/* ────── palette + solid tiles ────── */
static void load_assets()
{
    for(int i=0;i<16;++i){
        uint32_t c=PALETTE24[i];
        PA[i*4+0]=c&0xFF;
        PA[i*4+1]=(c>>8)&0xFF;
        PA[i*4+2]=(c>>16)&0xFF;
        PA[i*4+3]=0;
    }
    build_tileset();
    memcpy((void*)TS,TILESET,16384);
    memset((void*)TM,0,8192);
}

/* helpers: write tile */
static inline void put(int col,int row,uint8_t t)
{ TM[row*TM_STRIDE+col]=t; }

/* ────── draw primitives ────── */
static void rect(int x0,int y0,int w,int h,uint8_t t)
{
    for(int y=y0;y<y0+h;++y)
        for(int x=x0;x<x0+w;++x) put(x,y,t);
}
static void frame(int x0,int y0,int w,int h,uint8_t t)
{
    for(int x=x0;x<x0+w;++x) put(x,y0,t), put(x,y0+h-1,t);
    for(int y=y0;y<y0+h;++y) put(x0,y,t), put(x0+w-1,y,t);
}

/* 5×7 font rendered with TILE_WHITE */
static void draw_char(int col,int row,char ch)
{
    if(ch<' '||ch>'_') return;
    const uint8_t* bmp = FONT[ch-32];
    for(int y=0;y<7;++y)
        for(int x=0;x<5;++x)
            if(bmp[y]&(1<<(4-x))) put(col+x,row+y,TILE_WHITE);
}
static void draw_string(int col,int row,const char*str)
{
    for(int i=0;str[i];++i) draw_char(col+i*6,row,str[i]);
}
static void clear_area(int col,int row,int w,int h)
{ rect(col,row,w,h,TILE_EMPTY); }

/* ────── game renderer ────── */
static void draw_borders()
{
    frame(PF_LEFT,PF_TOP,PF_WIDTH,PF_HEIGHT,TILE_WALL);
    frame(NEXT_COL-1,NEXT_ROW-1,6,6,TILE_WALL);
}
static void draw_playfield(const Game& g)
{
    for(int y=0;y<ROWS;++y)
        for(int x=0;x<COLS;++x)
            put(PF_LEFT+1+x,PF_TOP+1+y,g.playfield(x,y));
}
static void draw_piece(const Game& g)
{
    g.for_each_block([](int x,int y,uint8_t c){
        put(PF_LEFT+1+x,PF_TOP+1+y,c);
    });
}
static void draw_next(const Game& g)
{
    /* clear interior */
    rect(NEXT_COL,NEXT_ROW,4,4,TILE_EMPTY);
    g.for_each_next([](int x,int y,uint8_t c){
        put(NEXT_COL+x,NEXT_ROW+y,c);
    });
}
static void draw_hud(const Game& g)
{
    char buf[8];
    sprintf(buf,"%d",g.score());
    clear_area(HUD_COL,HUD_SCORE_ROW,24,7);
    draw_string(HUD_COL,HUD_SCORE_ROW,"SCORE");
    draw_string(HUD_COL,HUD_SCORE_ROW+2,buf);

    sprintf(buf,"%d",g.lines());
    clear_area(HUD_COL,HUD_LINES_ROW,24,7);
    draw_string(HUD_COL,HUD_LINES_ROW,"LINES");
    draw_string(HUD_COL,HUD_LINES_ROW+2,buf);
}

/* ────── state machine ────── */
enum State {START, PLAY, OVER};
static State state = START;

/* ────── keyboard (evdev) ────── */
static int open_kbd()
{
    for(int i=0;i<32;++i){
        char p[32]; sprintf(p,"/dev/input/event%d",i);
        int fd=open(p,O_RDONLY|O_NONBLOCK);
        if(fd<0)continue;
        char name[64]; ioctl(fd,EVIOCGNAME(sizeof(name)),name);
        if(strstr(name,"kbd")||strstr(name,"Keyboard")) return fd;
        close(fd);
    }
    return -1;
}
static void poll_input(Game& g,int fd)
{
    struct input_event ev;
    while(read(fd,&ev,sizeof(ev))>0)
    if(ev.type==EV_KEY && ev.value==1){
        switch(state){
        case START:
            if(ev.code==KEY_SPACE){
                state=PLAY;
                memset((void*)TM,0,8192);   /* ← clear start screen */
                draw_borders();              /* redraw walls once   */
            }
            break;
        case PLAY:
            /* ...input for game... */
            break;
        case OVER:
            if(ev.code==KEY_SPACE){
                state=START;
                memset((void*)TM,0,8192);   /* ← clear game‑over   */
                show_start();
            }
            break;
        }
    }
}

/* ────── screens ────── */
static void show_start()
{
    memset((void*)TM,0,8192);
    draw_string(20,20,"TETRIS  FPGA");
    draw_string(12,30,"PRESS SPACE TO START");
}
static void show_game_over()
{
    draw_string(PF_LEFT+2,PF_TOP+PF_HEIGHT/2-3,"GAME OVER");
    draw_string(PF_LEFT-2,PF_TOP+PF_HEIGHT/2+3,"SPACE: RESTART");
}

/* ────── main loop ────── */
int main()
{
    map_fpga();
    load_assets();
    int kbd=open_kbd(); if(kbd<0){perror("kbd");return 1;}

    Game game;
    show_start();

    while(true){
        poll_input(game,kbd);

        if(state==PLAY){
            game.step();
            draw_borders();
            draw_playfield(game);
            draw_piece(game);
            draw_next(game);
            draw_hud(game);
            if(game.game_over()){
                state = OVER;
                memset((void*)TM,0,8192);       /* ← erase playfield   */
                show_game_over();
            }
        }else if(state==START){
            /* nothing */
        }else if(state==OVER){
            /* waiting for key */
        }
        usleep(16666);
    }
}