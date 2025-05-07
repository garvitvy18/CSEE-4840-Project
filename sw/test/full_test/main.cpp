#include "assets.h"
#include "Tetris.hpp"
#include "font5x7.h"
#include <fcntl.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* FPGA physical regions (lightweight bridge) */
constexpr off_t PHY_TILEMAP = 0xff200000;
constexpr off_t PHY_PALETTE = 0xff202000;
constexpr off_t PHY_TILESET = 0xff204000;
constexpr size_t SZ_TILEMAP = 8192, SZ_PALETTE = 64, SZ_TILESET = 16384;

/* tilemap grid is 80×60 */
static volatile uint8_t *tilemap,*palette,*tileset;

static void map_fpga()
{
    int fd=open("/dev/mem",O_RDWR|O_SYNC);
    if(fd<0){perror("mem");exit(1);}
#define MAP(off,sz,ptr) \
    ptr=(uint8_t*)mmap(nullptr,sz,PROT_READ|PROT_WRITE,MAP_SHARED,fd,off); \
    if(ptr==MAP_FAILED){perror("mmap");exit(1);}
    MAP(PHY_TILEMAP,SZ_TILEMAP,tilemap)
    MAP(PHY_PALETTE,SZ_PALETTE,palette)
    MAP(PHY_TILESET,SZ_TILESET,tileset)
#undef MAP
    close(fd);
}

static void load_assets()
{
    /* palette: R,G,B then dummy write byte to commit */
    for(int i=0;i<16;++i){
        uint32_t c=PALETTE24[i];
        palette[i*4+0]=c&0xFF;
        palette[i*4+1]=(c>>8)&0xFF;
        palette[i*4+2]=(c>>16)&0xFF;
        palette[i*4+3]=0;            /* commit */
    }
    build_tileset();
    memcpy((void*)tileset,TILESET,SZ_TILESET);
    memcpy((void*)tilemap,BLANK_TILEMAP,SZ_TILEMAP);
}

/* helpers for drawing --------------------------------------*/
static inline void put_tile(int col,int row,uint8_t t)
{ tilemap[row*128 + col] = t; }   // ← 128‑byte stride

static void draw_border()
{
    for(int x=0;x<=16;++x) put_tile(x,13,TILE_WALL), put_tile(x,34,TILE_WALL);
    for(int y=13;y<=34;++y) put_tile(0,y,TILE_WALL),  put_tile(16,y,TILE_WALL);
}

static void draw_playfield(const Game& g)
{
    for(int x=0;x<COLS;++x)
        for(int y=0;y<ROWS;++y)
            put_tile(1+x,14+y,g.playfield(x,y));
}

static void draw_piece(const Game& g)
{
    g.for_each_block([](int x,int y,uint8_t c){
        put_tile(1+x,14+y,c);
    });
}

static void draw_next(const Game& g)
{
    /* 6×6 box border */
    for(int x=18;x<=23;++x) put_tile(x,1,TILE_WALL),put_tile(x,6,TILE_WALL);
    for(int y=1;y<=6;++y) put_tile(18,y,TILE_WALL),put_tile(23,y,TILE_WALL);

    /* interior clear */
    for(int x=19;x<=22;++x)for(int y=2;y<=5;++y) put_tile(x,y,TILE_EMPTY);

    g.for_each_next([](int x,int y,uint8_t c){
        put_tile(19+x,2+y,c);
    });
}

/* render 5×7 font using TILE_WHITE blocks  */
static void draw_char(int col,int row,char ch)
{
    if(ch<' '||ch>'Z') return;
    const uint8_t *bmp = FONT[ch-32];
    for(int y=0;y<7;++y){
        for(int x=0;x<5;++x){
            bool on = bmp[y]&(1<<(4-x));
            put_tile(col+x,row+y,on?TILE_WHITE:TILE_EMPTY);
        }
    }
}
static void draw_string(int col,int row,const char*str)
{
    for(int i=0;str[i];++i) draw_char(col+i*6,row,str[i]);
}
static void draw_hud(const Game& g)
{
    draw_string(19,8,"SCORE");
    draw_string(19,10,"LINES");
    char buf[8];
    sprintf(buf,"%d",g.score());  draw_string(19,9,buf);
    sprintf(buf,"%d",g.lines());  draw_string(19,11,buf);
}

/* keyboard --------------------------------------------------*/
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
static void poll(Game& g,int fd)
{
    struct input_event ev;
    while(read(fd,&ev,sizeof(ev))>0){
        if(ev.type==EV_KEY && ev.value==1){
            switch(ev.code){
            case KEY_LEFT:  g.move_left(); break;
            case KEY_RIGHT: g.move_right();break;
            case KEY_UP:    g.rotate();    break;
            case KEY_DOWN:  g.soft_drop(); break;
            case KEY_SPACE: g.hard_drop(); break;
            case KEY_P:     g.toggle_pause(); break;
            }
        }
    }
}

/* -----------------------------------------------------------*/
int main()
{
    map_fpga();
    load_assets();
    draw_border();

    Game g;
    int kbd=open_kbd();
    if(kbd<0){fprintf(stderr,"kbd not found\n");return 1;}

    while(true){
        poll(g,kbd);
        g.step();
        draw_playfield(g);
        draw_piece(g);
        draw_next(g);
        draw_hud(g);
        usleep(16666);   /* ~60 fps */
    }
}