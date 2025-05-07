#include "Tetris.hpp"
#include <algorithm>
#include <array>
#include <random>

static Piece make_piece(std::initializer_list<const char*> rows,uint8_t tile)
{
    Piece p{};
    int r=0;
    for(auto line:rows){
        for(int c=0;c<4&&line[c];++c)
            if(line[c]=='#') p.mask[r][c]=tile;
        ++r;
    }
    return p;
}

static const std::array<Piece,7> SHAPES={
    make_piece({"####","....","....","...."},RED),      /* I */
    make_piece({"#..","###","...","..."   },BLUE),     /* J */
    make_piece({"..#","###","...","..."   },MAG),      /* L */
    make_piece({"##","##","..",".."       },YELLOW),   /* O */
    make_piece({".##","##.","...","..."   },GREEN),    /* S */
    make_piece({".#.","###","...","..."   },CYAN),     /* T */
    make_piece({"##."," .##","...","..."  },MAG)       /* Z  (space fixed)*/
};

static Piece rotR(const Piece& p){
    Piece r{};
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            r.mask[x][3-y]=p.mask[y][x];
    return r;
}
Piece Game::rotate_piece(const Piece& p,int r) const
{
    Piece q=p;
    for(int i=0;i<r;++i) q=rotR(q);
    return q;
}

static Piece rnd_piece(){
    static std::mt19937 gen{std::random_device{}()};
    static std::uniform_int_distribution<int>d(0,6);
    return SHAPES[d(gen)];
}

Game::Game(){ cur=rnd_piece(); nxt=rnd_piece(); spawn(); }

void Game::spawn()
{
    px=5; py=0; rot=0; cur=nxt; nxt=rnd_piece();
    if(collision(px,py,cur,rot)){ over=true; }
}

bool Game::collision(int nx,int ny,const Piece& pc,int r) const
{
    Piece p=rotate_piece(pc,r);
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            if(p.mask[y][x]){
                int gx=nx+x, gy=ny+y;
                if(gx<0||gx>=COLS||gy>=ROWS) return true;
                if(gy>=0 && field[gy][gx]) return true;
            }
    return false;
}

/* input ops */
void Game::move_left (){ if(!paused&&!over&&!collision(px-1,py,cur,rot))--px; }
void Game::move_right(){ if(!paused&&!over&&!collision(px+1,py,cur,rot))++px; }
void Game::rotate()    { if(!paused&&!over&&!collision(px,py,cur,(rot+1)%4))rot=(rot+1)%4; }
void Game::soft_drop() { if(!paused&&!over) ++py; }
void Game::hard_drop()
{
    if(paused||over)return;
    while(!collision(px,py+1,cur,rot)) ++py;
    lock_piece();
}
void Game::toggle_pause(){ if(!over) paused=!paused; }

/* gravity */
void Game::step()
{
    if(paused||over) return;
    if(++tick%30==0){
        if(!collision(px,py+1,cur,rot)) ++py;
        else lock_piece();
    }
}

void Game::lock_piece()
{
    Piece p=rotate_piece(cur,rot);
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            if(p.mask[y][x])
                field[py+y][px+x]=p.mask[y][x];
    clear_lines();
    spawn();
}

void Game::clear_lines()
{
    for(int y=0;y<ROWS;++y){
        bool full=true;
        for(int x=0;x<COLS;++x) if(!field[y][x]){full=false;break;}
        if(full){
            for(int yy=y;yy>0;--yy) field[yy]=field[yy-1];
            field[0].fill(0);
            ++lines_cleared; score_val+=100;
        }
    }
}

/* expose to renderer */
void Game::for_each_block(std::function<void(int,int,uint8_t)> cb) const
{
    Piece p=rotate_piece(cur,rot);
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            if(p.mask[y][x]) cb(px+x,py+y,p.mask[y][x]);
}
void Game::for_each_next(std::function<void(int,int,uint8_t)> cb) const
{
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            if(nxt.mask[y][x]) cb(x,y,nxt.mask[y][x]);
}