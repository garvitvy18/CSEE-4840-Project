#include "Tetris.hpp"
#include <random>

static Piece make_piece(std::initializer_list<const char*> rows, uint8_t tile)
{
    Piece p{};
    int r=0;
    for (auto line: rows) {
        for (int c=0; c<4 && line[c]; ++c)
            if (line[c]=='#') p.mask[r][c]=tile;
        ++r;
    }
    return p;
}

/* 7 tetrominoes in spawn orientation (SRS order) */
static Piece PIECES[7] = {
    /* I (RED)    */ make_piece({"####","....","....","...."},RED),
    /* J (BLUE)   */ make_piece({"#..","###","...","..."},BLUE),
    /* L (MAG)    */ make_piece({"..#","###","...","..."},MAG),
    /* O (YELLOW) */ make_piece({"##","##","..",".."},YELLOW),
    /* S (GREEN)  */ make_piece({".##","##.","...","..."},GREEN),
    /* T (CYAN)   */ make_piece({".#.","###","...","..."},CYAN),
    /* Z (MAG again to differ) */ make_piece({"##."," .##","...","..."},MAG)
};

static Piece rotate_right(const Piece& p)
{
    Piece r{};
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            r.mask[x][3-y]=p.mask[y][x];
    return r;
}

Piece Game::rotate_piece(const Piece& p,int rot) const
{
    Piece r=p;
    for(int i=0;i<rot;i++) r=rotate_right(r);
    return r;
}

Game::Game()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(std::begin(PIECES), std::end(PIECES), gen);
    std::copy(std::begin(PIECES), std::end(PIECES), bag);
    cur = bag[0]; nxt = bag[1]; bag_idx=2;
    spawn();
}

void Game::spawn()
{
    px=5; py=0; rot=0; cur=nxt;
    nxt = bag[bag_idx%7]; ++bag_idx;
}

bool Game::collision(int nx,int ny,const Piece& p,int r) const
{
    Piece q = rotate_piece(p,r);
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x) {
            if(!q.mask[y][x]) continue;
            int gx = nx+x, gy = ny+y;
            if(gx<0||gx>=COLS||gy>=ROWS) return true;
            if(gy>=0 && field[gy][gx]) return true;
        }
    return false;
}

void Game::move_left()  { if(!paused&& !collision(px-1,py,cur,rot)) px--; }
void Game::move_right() { if(!paused&& !collision(px+1,py,cur,rot)) px++; }
void Game::rotate()     { if(!paused&& !collision(px,py,cur,(rot+1)%4)) rot=(rot+1)%4; }
void Game::soft_drop()  { if(!paused) py++; }
void Game::hard_drop()
{
    if(paused) return;
    while(!collision(px,py+1,cur,rot)) py++;
    lock_piece();
}
void Game::toggle_pause(){ paused=!paused; }

void Game::step()
{
    if(paused) return;
    if(++tick % 30 == 0) {       /* ~2 Hz gravity at 60 fps loop */
        if(!collision(px,py+1,cur,rot)) py++;
        else lock_piece();
    }
}

void Game::lock_piece()
{
    Piece q=rotate_piece(cur,rot);
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            if(q.mask[y][x])
                field[py+y][px+x]=q.mask[y][x];
    clear_lines();
    spawn();
}

void Game::clear_lines()
{
    for(int y=0;y<ROWS;++y) {
        bool full=true;
        for(int x=0;x<COLS;++x) if(!field[y][x]) {full=false;break;}
        if(full) {
            for(int yy=y;yy>0;--yy) field[yy]=field[yy-1];
            field[0].fill(0);
            lines_cleared++; score_val+=100;
        }
    }
}

void Game::for_each_block(std::function<void(int,int,uint8_t)> cb) const
{
    Piece q=rotate_piece(cur,rot);
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            if(q.mask[y][x]) cb(px+x,py+y,q.mask[y][x]);
}

void Game::for_each_next(std::function<void(int,int,uint8_t)> cb) const
{
    for(int y=0;y<4;++y)
        for(int x=0;x<4;++x)
            if(nxt.mask[y][x]) cb(x,y,nxt.mask[y][x]);
}