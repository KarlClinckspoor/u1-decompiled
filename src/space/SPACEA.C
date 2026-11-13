/* opts: -ms -v -w -cs */
/*
 * SPACE.EXE game code 03E9-16D3: sector/ship accessors, ship pictures, ...
 */

/* a sector of the 7x7 galaxy: 0x3A bytes */
struct obj { int x, y, type, f6, f8, dir; };
struct sector { int count, f2, f4, f6, f8; struct obj o[4]; };

extern struct sector galaxy[7][7];
extern int sectx, secty;              /* current sector */
extern int curship;                      /* current ship */
extern int vidmode, colline, colfill, colhigh, coltext, plnoise;
extern int speed, velx, vely, redrflag, frontvw, entrspd, plstat, plfood;
extern int plseed, shutfuel, shutshld, argv1, argv2;
extern int D_2C30, D_2C32, D_2C34, D_2C36, D_2C38, D_2C3A, D_2C3C, D_2C3E;
extern int D_2C40, D_2C42, D_2C44, D_2C46, D_2C48, D_2C4A, D_2C4C, D_2C4E;
extern int D_2C50, D_2C52, D_2C54, D_2C56, D_2C58, D_2C5A, spacebnk;
extern char plname[];
extern playsnd(), pixel(), gotoxy(), putnum(), textmode(), abnormal();
extern rndrange(), seedrnd(), writfile(), msgline(), showhp(), showfood(), diskprmt();
extern drawline(), lineto(), fillrect(), putstr(), portof(), waittick();
extern refuse(), cmdlaser();
extern EXECL(), MEMSET();
extern unsigned char getkey();

/*
 * Initialised data, DGROUP 0124-0B1E (the literals of the functions follow it)
 */
int gmode = 4;
int speed = 5, velx = 0, vely = 0, exhcnt = 0, exhtog = 0;
int _STACK = 0x800;
int D_0132 = 0;
int redrflag = 0, crosvx = 0, crosvy = 0;
int crosx = 0x9F, crosy = 0x4F;
char *spacefil[] = { "CgaSpace.Bin", "EgaSpace.Bin", "T1KSpace.Bin" };   /* data files per video mode */
char *fightfil[] = { "CgaFight.Bin", "EgaFight.Bin", "T1KFight.Bin" };
char *flycmd[] = { "", "Counter-Clockwise", "Clockwise", "Thrust", "Retro" };
char *movecmd[] = { "", "Left", "Right", "Climb", "Dive" };
int sectx = 3, secty = 3;
int frontvw = 0, escflag = 0;
int hitmaxx[7] = { 5, 6, 8, 10, 12, 16, 18 };
int hitmaxy[7] = { 4, 6, 8, 10, 12, 14, 18 };
int hitminx[7] = { 1, 1, 1, 1, 0, 0, 0 };
int hitminy[7] = { 5, 5, 4, 4, 3, 2, 1 };
int starpic[18][19] = {
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
};
int basepic[20][20] = {
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
};
int planpic[18][21] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 4, 1, 1, 1, 2, 3, 2, 3, 2, 3, 1, 1, 4, 0, 0, 0, 0},
    {0, 0, 0, 4, 1, 1, 2, 3, 2, 2, 2, 3, 2, 3, 2, 1, 1, 4, 0, 0, 0},
    {0, 0, 4, 1, 1, 3, 2, 2, 2, 2, 2, 3, 2, 3, 2, 2, 1, 1, 4, 0, 0},
    {0, 4, 1, 1, 3, 3, 2, 2, 2, 2, 2, 3, 2, 2, 2, 3, 2, 1, 1, 4, 0},
    {0, 4, 1, 1, 3, 3, 3, 2, 2, 3, 3, 3, 2, 3, 3, 3, 3, 1, 1, 4, 0},
    {4, 1, 1, 2, 3, 2, 2, 2, 2, 3, 3, 3, 3, 2, 3, 3, 3, 2, 1, 1, 4},
    {4, 1, 1, 1, 3, 2, 2, 3, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 4},
    {4, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 3, 2, 2, 1, 1, 4},
    {4, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 4},
    {0, 4, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 4, 0},
    {0, 4, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 4, 0},
    {0, 0, 4, 1, 1, 2, 2, 3, 2, 3, 3, 3, 3, 2, 2, 2, 1, 1, 4, 0, 0},
    {0, 0, 0, 4, 1, 1, 2, 3, 2, 3, 2, 3, 2, 2, 2, 1, 1, 4, 0, 0, 0},
    {0, 0, 0, 0, 4, 1, 1, 1, 2, 3, 2, 3, 2, 1, 1, 1, 4, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0},
};
int portdx[4][3] = { { -17, -15, -15 }, { 18, 18, 18 }, { 0, 1, 2 }, { 0, 1, 2 } };
int portdy[4][3] = { { 1, 1, 1 }, { 1, 1, 1 }, { -17, -16, -16 }, { 18, 17, 17 } };
char inusefil[] = "inuse.u1";

extern int speed, velx, vely, exhcnt;
extern int redrflag, crosvx, crosvy, crosx, crosy;
extern int frontvw;
extern char *flycmd[], *movecmd[];
extern int colline, colfill, colhigh, coltext, shipcol;
extern int starquad, starmode;
extern int starx[], stary[];
extern int staroffx[], staroffy[];
extern msgline(), playif(), shldcrsh(), die(), usefuel(), exhaust();
extern fillrect(), drawline(), pixel(), abnormal(), newenemy(), putstr(), clockrnd(), seedrnd();
extern plotclip(), toout(), drawfite(), showexp(), drawplan(), drawstar(), drawbase(), drawship();
extern int shutfuel, shutshld;
extern int hitmaxx[], hitmaxy[], hitminx[], hitminy[], plexp, plkills, vidmode;
extern fightfrm(), vwait(), flushkey(), gotoxy(), clrview(), printat(), waittick(), keypoll(), drawtop();
extern int growcnt;
extern int getfuel(), portof(), getshld(), rndrange();
extern int sectx, secty, frontvw, curship;
extern int speed, velx, vely, exhcnt, exhtog;
extern int redrflag, crosx, crosy;
extern int enemyx, enemyy, enemypx, enemypy, enemykd, jumping, speeddly[];
extern int exhdrawn, growcnt, colline, colfill;
extern int hitmaxx[], hitmaxy[];
extern int colhigh, coltext, forcecga, shipcol, plnoise, plseed, shutfuel, shutshld;
extern int argv1, argv2, starmode, lastdir, vidmode, rdyarm, plcoin;
extern char *portname[], *spacefil[], *fightfil[];

extern plotvp(), exhaust0(), msgline(), drawline(), playif(), waittick(), useshld(), die();
extern arrowr(), movestar(), vwait(), drawfite(), newenemy(), drawcros(), starhit(), planhit();
extern putstr(), gotoxy(), shipfuel(), shipshld(), showfuel(), showshld(), showcoin();
extern scrlmsg(), putnum(), readfile(), vidinit(), seedrnd(), mkgalaxy(), initstar(), drawtop();
extern erron(), cmdarrow(), cmdattk(), cmdboard(), cmdcast(), cmddrop(), cmdenter(), cmdfire();
extern cmdget(), cmdhyper(), cmdinfo(), cmdklimb(), cmdopen(), cmdquit(), cmdready(), cmdsteal();
extern cmdtrans(), cmdunlk(), cmdview(), cmdxit(), ztats(), cmdpass(), cmdhuh(), cmdspeed();
extern exhaust2(), exhaust1(), fightfrm(), alienfir(), crash(), dock(), liftoff(), cmdnoise();
extern PUTS(), _EXIT();
extern int rndrange(), getshld(), sign(), drawship(), portof(), shipin(), advx(), advy();
extern int portx(), porty(), opposite(), clockrnd();
extern char keywait(), waitkey(), upcase();

#define ABS(x) ((x) < 0 ? -(x) : (x))

/* number of the largest square (1,4,9,..) that fits in the argument */
ceilsqrt(a) int a;
{
    int n;
    n = 0;
    while (a > 0) {
        n++;
        a -= 2 * n - 1;
    }
    return n;
}

/* call playsnd if plnoise is set */
playif(c) int c;
{
    if (plnoise)
        playsnd(c);
}

/* draw 18x19 picture starpic at (x,y) in two colours */
drawstar(x, y) int x, y;
{
    int a, b, i, j;
    a = 12;
    b = 14;
    if (vidmode == 0) {
        a = colfill;
        b = colline;
    }
    for (i = 0; i < 18; i++)
        for (j = 0; j < 19; j++) {
            if (starpic[i][j] == 1)
                pixel(x + j, y + i, b);
            else if (starpic[i][j] == 2)
                pixel(x + j, y + i, a);
        }
}

/* draw 20x20 picture basepic at (x,y) */
drawbase(x, y) int x, y;
{
    int i, j;
    for (i = 0; i < 20; i++)
        for (j = 0; j < 20; j++)
            if (basepic[i][j])
                pixel(x + j, y + i, colline);
}

/* draw 18x21 picture planpic at (x,y) in four colours */
drawplan(x, y) int x, y;
{
    int a, b, c, i, j;
    a = 1;
    b = 10;
    c = 7;
    if (vidmode == 0) {
        a = colhigh;
        b = colfill;
        c = colline;
    }
    for (i = 0; i < 18; i++)
        for (j = 0; j < 21; j++) {
            if (planpic[i][j] == 1)
                pixel(x + j, y + i, a);
            else if (planpic[i][j] == 2)
                pixel(x + j, y + i, a);
            else if (planpic[i][j] == 4)
                pixel(x + j, y + i, c);
            else if (planpic[i][j] == 3)
                pixel(x + j, y + i, b);
        }
}

/* opposite direction (0<->1, 2<->3) */
opposite(d) int d;
{
    if (d == 0)
        return 1;
    if (d == 1)
        return 0;
    if (d == 2)
        return 3;
    if (d == 3)
        return 2;
    textmode();
    abnormal();
}

/* print the current ship's f6 (fuel) at (0x24,0x16) */
showfuel()
{
    int c, v;
    v = galaxy[sectx][secty].o[curship].f6;
    gotoxy(0x24, 0x16);
    if (v < 100)
        c = colhigh;
    else
        c = coltext;
    putnum(v, 4, 0x20, c, 0);
    gotoxy(1, 0x18);
}

/* print the current ship's f8 (shield) at (0x24,0x15) */
showshld()
{
    int c, v;
    v = galaxy[sectx][secty].o[curship].f8;
    gotoxy(0x24, 0x15);
    if (v < 100)
        c = colhigh;
    else
        c = coltext;
    putnum(v, 4, 0x20, c, 0);
    gotoxy(1, 0x18);
}

shipfuel(n) int n;
{
    if (n == 0)
        return 1000;
    if (n == 2)
        return 2500;
    if (n == 1)
        return 5000;
    return 0;
}

shipshld(n) int n;
{
    if (n == 0)
        return 1000;
    if (n == 2)
        return 5000;
    if (n == 1)
        return 1000;
    return 0;
}

portx(k, sx, sy, s) int k, sx, sy, s;
{
    int typ;
    typ = galaxy[sx][sy].o[s].type;
    return portdx[k][typ] + galaxy[sx][sy].f6;
}

porty(k, sx, sy, s) int k, sx, sy, s;
{
    int typ;
    typ = galaxy[sx][sy].o[s].type;
    return portdy[k][typ] + galaxy[sx][sy].f8;
}

getshld()
{
    return galaxy[sectx][secty].o[curship].f8;
}

/* reduce the shield (o[ship].f8) by n, not below 0, and redraw it */
useshld(n) int n;
{
    int v;
    v = getshld();
    if (v < n)
        v = 0;
    else
        v -= n;
    galaxy[sectx][secty].o[curship].f8 = v;
    showshld();
}

shldcrsh()
{
    useshld(getshld() / 4 + 5);
}

getfuel()
{
    return galaxy[sectx][secty].o[curship].f6;
}

usefuel(n) int n;
{
    int v;
    v = getfuel();
    if (v < n)
        v = 0;
    else
        v -= n;
    galaxy[sectx][secty].o[curship].f6 = v;
    showfuel();
}

/* fill sector (sx,sy) of the galaxy with random contents */
mksector(sx, sy) int sx, sy;
{
    int a, b, c, d, e, f, i;
    for (i = 0; i < 4; i++)
        galaxy[sx][sy].o[i].type = -1;
    galaxy[sx][sy].count = rndrange(0, 0xF0) / 0x28;
    if (galaxy[sx][sy].count > 3)
        galaxy[sx][sy].count = 0;
    d = 0;
    c = 0;
    b = 0;
    a = 0;
    if (rndrange(1, 200) < 0x4D) {
        a = rndrange(0x32, 200);
        b = rndrange(0x32, 0x78);
    }
    galaxy[sx][sy].f2 = a;
    galaxy[sx][sy].f4 = b;
    if (rndrange(1, 200) < 0x4D) {
        c = rndrange(0x32, 200);
        d = rndrange(0x32, 100);
        if (ABS(c - a) < 0x28 || ABS(d - b) < 0x28) {
            d = 0;
            c = 0;
        }
    }
    galaxy[sx][sy].f6 = c;
    galaxy[sx][sy].f8 = d;
    if (c) {
        a = rndrange(0, 0x62) % 3;
        if (a) {
            e = rndrange(0, 199) % 4;
            c = rndrange(0, 0x62) % 3;
            galaxy[sx][sy].o[0].type = c;
            galaxy[sx][sy].o[0].f6 = shipfuel(c);
            galaxy[sx][sy].o[0].f8 = shipshld(c);
            galaxy[sx][sy].o[0].x = portx(e, sx, sy, 0);
            galaxy[sx][sy].o[0].y = porty(e, sx, sy, 0);
            galaxy[sx][sy].o[0].dir = opposite(e);
            if (a == 2) {
                do
                    d = rndrange(0, 199) % 3;
                while (d == c);
                do
                    f = rndrange(0, 0x62) % 3;
                while (e == f);
                galaxy[sx][sy].o[1].type = d;
                galaxy[sx][sy].o[1].f6 = shipfuel(d);
                galaxy[sx][sy].o[1].f8 = shipshld(d);
                galaxy[sx][sy].o[1].x = portx(f, sx, sy, 1);
                galaxy[sx][sy].o[1].y = porty(f, sx, sy, 1);
                galaxy[sx][sy].o[1].dir = opposite(f);
            }
        }
    }
}

/* initialise the galaxy and the ships */
mkgalaxy()
{
    int i, j;
    seedrnd(plseed * 3 / 2);
    MEMSET(galaxy, 0, 0xB1A);
    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            mksector(i, j);
    curship = 2;
    D_2C30 = 0;
    D_2C32 = 0x8C;
    D_2C34 = 0x46;
    D_2C36 = 200;
    D_2C38 = 100;
    D_2C3E = 2;
    D_2C40 = shipfuel(2);
    D_2C42 = shipshld(2);
    D_2C3A = portx(3, 3, 3, 0);
    D_2C3C = porty(3, 3, 3, 0);
    D_2C44 = opposite(3);
    D_2C4A = 1;
    D_2C4C = shipfuel(1);
    D_2C4E = shipshld(1);
    D_2C46 = portx(0, 3, 3, 1);
    D_2C48 = porty(0, 3, 3, 1);
    D_2C50 = opposite(0);
    D_2C56 = 0;
    D_2C58 = shutfuel;
    D_2C5A = shutshld;
    D_2C52 = 0x104;
    D_2C54 = 0x5A;
    spacebnk = 0;
}

/* leave to OUT.EXE: save state to inuse.u1 and execl("out.exe") forever */
toout()
{
    int u;
    if (plstat == 0) {
        useshld(getshld());
        usefuel(getfuel());
    }
    writfile(inusefil, (int)plname, 0x334);
    for (;;) {
        u = EXECL("out.exe", "out.exe", argv1, argv2, "S", 0);
        diskprmt();
    }
}

cmdhuh()
{
    msgline("Huh?");
    playif(1);
}

die()
{
    plfood = plstat = 0;
    showhp();
    showfood();
    toout();
}

/* wait until a key is pressed (or ~10000 polls) */
keypoll()
{
    int i;
    i = 0;
    while (!getkey(0) && i++ < 10000)
        ;
}

/* draw the frame of the main viewing window in colour c */
viewfram(c) int c;
{
    drawline(0x22, 0x1A, 0x11C, 0x1A, c);
    lineto(0x11C, 0x7C, c);
    lineto(0x22, 0x7C, c);
    lineto(0x22, 0x1A, c);
    gotoxy(5, 4);
}

/* clear the viewing window and draw its frame in colour c */
clrview(c) int c;
{
    fillrect(0x1F, 0x17, 0x11F, 0x7F, 0);
    viewfram(c);
}

/* print string s at (a,b) */
printat(a, b, s) int a, b;
char *s;
{
    gotoxy(a, b);
    putstr(s, coltext);
}

/* round to nearest 100 and to nearest 1000 */
rndspeed(n) int n;
{
    if (n % 100 > 0x31)
        n += 100;
    if (n % 1000 > 0x1F3)
        n += 1000;
    return n;
}

scalevx()
{
    int k;
    k = speed * 1000 / entrspd;
    velx *= k;
    velx = rndspeed(velx);
    velx /= 1000;
    if (velx > 6)
        velx = 6;
}

scalevy()
{
    int k;
    k = speed * 1000 / entrspd;
    vely *= k;
    vely = rndspeed(vely);
    vely /= 1000;
    if (vely > 6)
        vely = 6;
}

/* View command: switch between the cockpit and the sector view */
cmdview()
{
    putstr("View", coltext);
    if (frontvw == 0) {
        if (portof(curship) != -1) {
            msgline("?");
            msgline("View can't be changed");
            msgline("whilst in space dock.");
        } else {
            msgline("");
            if (getfuel() == 0)
                msgline("Not enough fuel!");
            else {
                redrflag = frontvw = 1;
                speed = ceilsqrt(velx * velx + vely * vely);
                if (speed > 8)
                    speed = 8;
                else if (speed == 0) {
                    speed = 1;
                    switch (galaxy[sectx][secty].o[curship].dir) {
                    case 0:
                        velx = -1;
                        break;
                    case 1:
                        velx = 1;
                        break;
                    case 2:
                        vely = -1;
                        break;
                    case 3:
                        vely = 1;
                        break;
                    }
                }
                entrspd = speed;
            }
        }
    } else if (galaxy[sectx][secty].count != 0) {
        msgline("?");
        msgline("Thou must eliminate all");
        msgline("enemy craft first!");
    } else {
        frontvw = 0;
        msgline("");
        redrflag = 1;
        scalevx();
        scalevy();
        if (velx == 0 && vely == 0) {
            switch (galaxy[sectx][secty].o[curship].dir) {
            case 0:
                velx = -1;
                break;
            case 1:
                velx = 1;
                break;
            case 2:
                vely = -1;
                break;
            case 3:
                vely = 1;
                break;
            }
        }
    }
}

/* draw a sector symbol at (x,y): two vertical bars and a crossbar */
symenemy(x, y) int x, y;
{
    int i;
    for (i = 0; i < 2; i++) {
        drawline(x + i, y, x + i, y + 7, colline);
        drawline(x + i + 5, y, x + i + 5, y + 7, colline);
        drawline(x, y + 3 + i, x + 6, y + 3 + i, colline);
    }
}

symbase(x, y) int x, y;
{
    int i;
    for (i = 0; i < 2; i++) {
        drawline(x + i + 3, y, x + i + 3, y + 7, colline);
        drawline(x, y + i + 3, x + 7, y + i + 3, colline);
    }
}

symstar(x, y) int x, y;
{
    int i;
    for (i = 0; i < 3; i++) {
        drawline(x + 3 - i, y + i + 1, x + 3 + i, y + i + 1, colline);
        drawline(x + 3 - i, y + 6 - i, x + 3 + i, y + 6 - i, colline);
    }
}

symempty(x, y) int x, y;
{
    pixel(x + 1, y + 1, colline);
    pixel(x + 3, y + 3, colhigh);
    pixel(x + 6, y + 2, colfill);
    pixel(x + 4, y + 5, colline);
    pixel(x + 2, y + 6, colfill);
}

/* draw the map symbol of sector (i,j) */
sectsym(i, j) int i, j;
{
    int px, py;
    px = i * 2 * 5 + 0xA5 - 0x23 + 2;
    py = j * 2 * 5 + 0x4F - 0x23 + 2;
    if (galaxy[i][j].count)
        symenemy(px, py);
    else if (galaxy[i][j].f6)
        symbase(px, py);
    else if (galaxy[i][j].f2)
        symstar(px, py);
    else
        symempty(px, py);
}

/* Inform and Search: draw the 7x7 sector map, blink the current sector until a key */
cmdinfo()
{
    int i, j, px, py;
    putstr("Inform and Search", coltext);
    clrview(colfill);
    printat(5, 4, "Sector Scan:");
    drawline(0x82, 0x2C, 200, 0x2C, colline);
    lineto(200, 0x73, colline);
    lineto(0x82, 0x73, colline);
    lineto(0x82, 0x2C, colline);
    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            sectsym(i, j);
    px = sectx * 2 * 5 + 0xA5 - 0x23 + 2;
    py = secty * 2 * 5 + 0x4F - 0x23 + 2;
    do {
        if (!(j = getkey(0))) {
            waittick(7);
            for (i = 0; i < 8; i++)
                drawline(px + i, py, px + i, py + 8, 0);
            if (!(j = getkey(0))) {
                waittick(7);
                sectsym(sectx, secty);
            }
        }
    } while (j == 0);
    redrflag = 1;
    msgline("");
}

/* Fire (in space) or Fire? / space-dock action */
cmdfire()
{
    if (frontvw == 0)
        refuse("Fire?");
    else
        cmdlaser();
}

/* Speed command: set speed to n, costing fuel */
cmdspeed(n) int n;
{
    int k;
    putstr("Speed ", coltext);
    putnum(n, 1, 0x20, coltext, 1);
    msgline("");
    k = ABS(n - speed) * 4;
    if (k > getfuel())
        msgline("Not enough fuel!");
    else {
        speed = n;
        usefuel(k);
    }
}

int turntab[2][4] = { { 3, 2, 0, 1 }, { 2, 3, 1, 0 } };      /* direction changes */

/* fly: turn, thrust or retro (k 1..4); costs fuel, may crash into an object */
fly(k)
unsigned char k;
{
    int cost, delta;

    cost = 5;
    delta = -1;
    msgline(flycmd[k]);
    if (k == 1 || k == 2)
        cost = 2;
    if (getfuel() == 0) {
        msgline("No fuel!!");
        msgline("Wilt thou drift forever?!?");
    } else {
        if (getfuel() < cost)
            cost = getfuel();
        if (portof(curship) != -1 && k != 4) {
            msgline("Crunch!");
            playif(0);
            shldcrsh();
            if (!getshld()) {
                msgline("Thy shield is drained!");
                die();
            }
        } else {
            usefuel(cost);
            cost = galaxy[sectx][secty].o[curship].dir;
            if (k == 1 || k == 2) {
                if (exhcnt)
                    exhaust(&galaxy[sectx][secty].o[curship], 0);
                galaxy[sectx][secty].o[curship].dir = turntab[k - 1][cost];
                delta = exhcnt = 0;
            } else if (k == 3)
                delta = 1;
            switch (cost) {
            case 2:
                vely -= delta;
                break;
            case 3:
                vely += delta;
                break;
            case 0:
                velx -= delta;
                break;
            case 1:
                velx += delta;
                break;
            }
            if (velx > -1)
                velx = velx <= 9 ? velx : 9;
            else
                velx = velx > -9 ? velx : -9;
            if (vely > -1)
                vely = vely <= 9 ? vely : 9;
            else
                vely = vely > -9 ? vely : -9;
            if (k == 3)
                exhcnt += 10;
            else if (k == 4) {
                exhaust(&galaxy[sectx][secty].o[curship], 0);
                exhcnt = 0;
            }
        }
    }
}

/* move in combat view (k 1..4: left, right, climb, dive); outside combat fly instead */
cmdarrow(k)
unsigned char k;
{
    if (frontvw == 0)
        fly(k);
    else {
        msgline(movecmd[k]);
        if (getfuel() < 2)
            msgline("Not enough fuel!");
        else {
            usefuel(2);
            switch (k) {
            case 1:
                crosvx = -4;
                crosvy = 0;
                break;
            case 2:
                crosvx = 4;
                crosvy = 0;
                break;
            case 3:
                crosvy = -4;
                crosvx = 0;
                break;
            case 4:
                crosvy = 4;
                crosvx = 0;
                break;
            }
        }
    }
}

/* print a command name and make the "not here" sound */
refuse(s)
char *s;
{
    msgline(s);
    playif(1);
}

cmdattk() { refuse("Attack?"); }
cmdboard() { refuse("Board?"); }
cmdcast() { refuse("Cast?"); }
cmddrop() { refuse("Drop?"); }
cmdenter() { refuse("Enter?"); }
cmdget() { refuse("Get?"); }
cmdklimb() { refuse("Klimb?"); }
cmdopen() { refuse("Open?"); }
cmdquit() { refuse("Quit?"); }
cmdready() { refuse("Ready?"); }
cmdsteal() { refuse("Steal?"); }
cmdtrans() { refuse("Transact?"); }
cmdunlk() { refuse("Unlock?"); }
cmdxit() { refuse("X-it?"); }

/* pass; in combat: center the crosshair and stop sideways motion */
cmdpass()
{
    if (frontvw == 0)
        msgline("Pass");
    else {
        drawcros(0);
        crosx = 0x9f;
        crosy = 0x4f;
        crosvx = crosvy = 0;
        fillrect(16, 16, 0x12f, 0x8f, 0);
        drawcros(colline);
        msgline("Center");
    }
}

/* is (x, y) inside the combat viewport? */
inview(x, y)
int x, y;
{
    return x < 300 && x > 20 && y < 140 && y > 20;
}

/* place star i at a random offset from the crosshair */
newstar(i)
int i;
{
    int dx, dy;

    starquad = rndrange(0, 255) / 64;
    dx = rndrange(2, 255) / 2;
    dy = rndrange(4, 255) / 4;
    if (starquad == 1 || starquad == 2)
        dx = -dx;
    if (starquad > 1)
        dy = -dy;
    if (!inview(dx + crosx, dy + crosy))
        dx = dy = 5000;
    staroffx[i] = dx;
    staroffy[i] = dy;
}

/* sign of v */
sign(v)
int v;
{
    if (v > 0)
        return 1;
    if (v < 0)
        return -1;
    return 0;
}

/* draw the crosshair in colour c */
drawcros(c)
int c;
{
    drawline(crosx - 2, crosy, crosx + 2, crosy, c);
    drawline(crosx, crosy - 2, crosx, crosy + 2, c);
}

/* move the 40 stars one step, redraw them and the crosshair, update the crosshair position */
movestar()
{
    int i, dx, dy, moving;

    playif(9);
    moving = crosvx || crosvy || starmode != 2;
    if (moving)
        drawcros(0);
    for (i = 0; i < 40; i++) {
        dx = staroffx[i] / 8;
        dy = staroffy[i] / 6;
        if (dx == 0)
            dx = sign(staroffx[i]);
        if (dy == 0)
            dy = sign(staroffy[i]);
        if (starmode != 1 && inview(starx[i], stary[i]))
            pixel(starx[i], stary[i], 0);
        staroffx[i] += dx;
        staroffy[i] += dy;
        if (!inview(staroffx[i] + crosx, staroffy[i] + crosy))
            newstar(i);
        else
            pixel(staroffx[i] + crosx, staroffy[i] + crosy, colline);
        starx[i] = staroffx[i] + crosx;
        stary[i] = staroffy[i] + crosy;
    }
    if (starmode == 2) {
        crosx += crosvx;
        crosy += crosvy;
        if (crosx >= 300 || crosx <= 20) {
            crosx -= crosvx;
            crosvx = 0;
        }
        if (crosy >= 140 || crosy <= 20) {
            crosy -= crosvy;
            crosvy = 0;
        }
        if (moving)
            drawcros(colline);
    }
    playif(9);
}

/* place all 40 stars */
initstar()
{
    int i;

    starquad = 0;
    for (i = 0; i < 40; i++) {
        newstar(i);
        starx[i] = crosx + 3;
        stary[i] = crosy + 3;
    }
}

/* leave the sector through the edge: wrap the galaxy, move the ship object to a free slot of the new sector */
newsect()
{
    int dx, dy, k, ox, oy;

    k = galaxy[sectx][secty].o[curship].dir;
    dx = sign(velx);
    dy = sign(vely);
    if (dx == 0 && dy == 0) {
        if (k == 0)
            dx--;
        else if (k == 1)
            dx++;
        else if (k == 2)
            dy--;
        else if (k == 3)
            dy++;
    }
    ox = sectx;
    oy = secty;
    sectx += dx;
    secty += dy;
    if (sectx < 0)
        sectx = 6;
    else
        sectx = sectx % 7;
    if (secty < 0)
        secty = 6;
    else
        secty = secty % 7;
    for (k = 0; k < 4 && galaxy[sectx][secty].o[k].type != -1; k++)
        ;
    if (k == 4)
        abnormal();
    galaxy[sectx][secty].o[k] = galaxy[ox][oy].o[curship];
    galaxy[ox][oy].o[curship].type = -1;
    curship = k;
    galaxy[sectx][secty].o[curship].x = 260;
    galaxy[sectx][secty].o[curship].y = 90;
    if (galaxy[sectx][secty].count)
        newenemy();
}

int enemyx = 0, enemyy = 0, enemypx = 0, enemypy = 0, enemykd = 0, jumping = 0;
int speeddly[9] = { 0, 8, 7, 6, 5, 4, 3, 2, 1 };

/* Hyperjump command: only from front view; costs fuel; lightspeed animation, then new sector */
cmdhyper()
{
    int i, j, x, n, r;

    putstr("HyperJump", coltext);
    if (frontvw == 0) {
        msgline("?");
        msgline("Hyperjump can only be made");
        msgline("from front view.");
    } else if (getfuel() < 100) {
        msgline("");
        msgline("Not enough fuel!");
    } else {
        growcnt = 20;
        putstr("...", coltext);
        r = clockrnd() | 0xBBBB;
        x = speed;
        crosvx = crosvy = 0;
        if (x < 4)
            n = 10;
        else
            n = 15;
        jumping = 1;
        for (j = x; j < 9; j++) {
            speed = j;
            for (i = 0; i <= n; i++)
                fightfrm();
        }
        speed = 0;
        for (i = 0; i < 40; i++)
            if (inview(starx[i], stary[i]))
                pixel(starx[i], stary[i], 0);
        putstr("Lightspeed!", coltext);
        gotoxy(10, 24);
        seedrnd(r);
        initstar();
        starmode = 1;
        for (i = 0; i <= 200; i++)
            movestar();
        seedrnd(r);
        initstar();
        starmode = 3;
        for (i = 0; i <= 199; i++)
            movestar();
        starmode = 2;
        for (j = 8; j > x; j--) {
            speed = j;
            for (i = 0; i <= n; i++) {
                movestar();
                vwait(speeddly[speed]);
            }
        }
        drawcros(colline);
        msgline(" completed.     ");
        usefuel(100);
        seedrnd(clockrnd() + 1234);
        speed = x;
        flushkey();
        enemyx = enemyy = enemypx = enemypy = 0;
        newsect();
        exhcnt = jumping = 0;
    }
}

/* victory screen: "Thou hast achieved the rank of Space Ace!" */
spaceace()
{
    flushkey();
    clrview(colfill);
    printat(11, 6, "Thou hast achieved");
    printat(14, 8, "the rank of");
    printat(15, 10, "Space Ace!");
    waittick(5);
    flushkey();
    keypoll();
    drawtop();
}

/* Fire command: laser bolts from both sides to the crosshair; hit test against the target */
cmdlaser()
{
    int px, py, xhi, yhi;

    msgline("Fire!");
    if (getfuel() < 10)
        msgline("Thou hast not enough fuel!");
    else {
        usefuel(10);
        if (enemyx) {
            drawfite(enemyx, enemyy, enemykd, 1);
            px = crosx - enemyx;
            py = crosy - enemyy;
        } else
            px = py = -100;
        drawline(20, 140, crosx, crosy, colhigh);
        drawline(300, 140, crosx, crosy, colhigh);
        playif(8);
        xhi = hitmaxx[enemykd];
        yhi = hitmaxy[enemykd];
        if (enemyx)
            drawfite(enemyx, enemyy, enemykd, 0);
        if (hitminx[enemykd] <= px && px <= xhi && py >= hitminy[enemykd] && py <= yhi) {
            playif(2);
            msgline("Hit!!!");
            plexp += 100;
            showexp();
            plkills++;
            if (plkills == 20)
                spaceace();
            else if (plkills > 20)
                msgline("Thou art still a Space ace!");
            if (--galaxy[sectx][secty].count)
                newenemy();
            else
                enemyx = enemyy = 0;
        }
        drawline(20, 140, crosx, crosy, 0);
        drawline(300, 140, crosx, crosy, 0);
        drawcros(colline);
    }
}

/* draw the cockpit frame and its rivets */
cockpit()
{
    int i, c;

    c = 7;
    if (vidmode == 0)
        c = colline;
    fillrect(8, 8, 311, 15, c);
    fillrect(8, 144, 311, 151, c);
    fillrect(8, 16, 15, 144, c);
    fillrect(304, 16, 311, 144, c);
    for (i = 1; i < 20; i++) {
        pixel(i * 15 + 10, 10, 0);
        pixel(i * 15 + 10, 13, 0);
        pixel(i * 15 + 10, 146, 0);
        pixel(i * 15 + 10, 149, 0);
    }
    for (i = 1; i < 10; i++) {
        pixel(10, i * 15 + 3, 0);
        pixel(13, i * 15 + 3, 0);
        pixel(306, i * 15 + 3, 0);
        pixel(309, i * 15 + 3, 0);
    }
}

/* put a target in view: random position, unless one is already there */
newenemy()
{
    if (jumping == 0) {
        enemyx = rndrange(40, 280);
        enemyy = rndrange(40, 120);
        enemykd = 0;
    } else
        enemyx = enemyy = 0;
}

/* redraw the combat view: stars and crosshair */
redrfrnt()
{
    initstar();
    drawcros(colline);
}

/* draw the front view of the current sector: frame, base, planet/sun, ships */
drawtop()
{
    int i;
    struct sector *s;
    struct obj *p;

    redrflag = 0;
    fillrect(16, 16, 303, 143, 0);
    cockpit();
    if (frontvw == 1)
        redrfrnt();
    else {
        if (sectx == 3 && secty == 3)
            drawplan(60, 75);
        s = &galaxy[sectx][secty];
        if (s->f2)
            drawstar(s->f2, s->f4);
        if (s->f6)
            drawbase(s->f6, s->f8);
        for (i = 0; i < 4; i++) {
            p = &s->o[i];
            if (p->type > -1)
                drawship(p->type, p->dir, p->x, p->y);
        }
    }
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

/* collision check with the object at (f2,f4) of the sector: closer than 25 in both axes */
starhit()
{
    struct sector *s;

    s = &galaxy[sectx][secty];
    if (s->f2) {
        if (ABS(s->f2 - s->o[curship].x) < 25 && ABS(s->f4 - s->o[curship].y) < 25) {
            msgline("Thy ship melts near the star!");
            die();
        }
    }
}

/* is the ship near the planet (60,75) of sector 3,3? landing on a planet, or melting */
planhit()
{
    struct obj *p;

    p = &galaxy[sectx][secty].o[curship];
    if (sectx == 3 && secty == 3) {
        if (ABS(p->x - 60) < 25 && ABS(p->y - 75) < 25) {
            if (p->type == 0) {
                msgline("Thou hast landed safely!");
                shutfuel = p->f6;
                shutshld = p->f8;
                toout();
            } else {
                msgline("Only space shuttles are heat");
                msgline("shielded for landings!");
                shutfuel = shutshld = 0;
                die();
            }
        }
    }
}

/* plot a pixel if inside the viewport */
plotvp(x, y, c)
int x, y, c;
{
    if (x > 20 && x < 300 && y > 18 && y < 141)
        plotclip(x, y, c);
}

/* draw a small ship (a sel = 0..3: facing) with its lights at (x,y) */
exhaust0(sel, x, y)
unsigned sel;
int x, y;
{
    int i;

    switch (sel) {
    case 1:
        for (i = 0; i < 3; i++) {
            plotvp(x - 1, y + 4 + i, shipcol);
            plotvp(x - 1, y + 11 + i, shipcol);
        }
        plotvp(x - 2, y + 5, colline);
        plotvp(x - 2, y + 6, colline);
        plotvp(x - 2, y + 11, colline);
        plotvp(x - 2, y + 12, colline);
        plotvp(x - 3, y + 6, shipcol);
        plotvp(x - 3, y + 11, shipcol);
        break;
    case 0:
        for (i = 0; i < 3; i++) {
            plotvp(x + 19, y + 4 + i, shipcol);
            plotvp(x + 19, y + 11 + i, shipcol);
        }
        plotvp(x + 20, y + 5, colline);
        plotvp(x + 20, y + 6, colline);
        plotvp(x + 20, y + 11, colline);
        plotvp(x + 20, y + 12, colline);
        plotvp(x + 21, y + 6, shipcol);
        plotvp(x + 21, y + 11, shipcol);
        break;
    case 2:
        for (i = 0; i < 3; i++) {
            plotvp(x + 5 + i, y + 19, shipcol);
            plotvp(x + 12 + i, y + 19, shipcol);
        }
        plotvp(x + 6, y + 20, colline);
        plotvp(x + 7, y + 20, colline);
        plotvp(x + 12, y + 20, colline);
        plotvp(x + 13, y + 20, colline);
        plotvp(x + 7, y + 21, shipcol);
        plotvp(x + 12, y + 21, shipcol);
        break;
    case 3:
        for (i = 0; i < 3; i++) {
            plotvp(x + 5 + i, y - 1, shipcol);
            plotvp(x + 12 + i, y - 1, shipcol);
        }
        plotvp(x + 6, y - 2, colline);
        plotvp(x + 7, y - 2, colline);
        plotvp(x + 12, y - 2, colline);
        plotvp(x + 13, y - 2, colline);
        plotvp(x + 7, y - 3, shipcol);
        plotvp(x + 12, y - 3, shipcol);
        break;
    }
}

/* draw ship (sel = facing 0..3) at x,y: big wedge design */
exhaust2(sel, x, y)
unsigned sel;
int x, y;
{
    int i;

    switch (sel) {
    case 1:
        for (i = 0; i < 2; i++) {
            plotvp(x - 1 - i, y + 1, shipcol);
            plotvp(x - 1 - i, y + 2, shipcol);
            plotvp(x - 1 - i, y + 15, shipcol);
            plotvp(x - 1 - i, y + 16, shipcol);
        }
        for (i = 0; i < 4; i++)
            plotvp(x - 1, y + 7 + i, shipcol);
        plotvp(x - 2, y + 6, shipcol);
        plotvp(x - 2, y + 11, shipcol);
        break;
    case 0:
        for (i = 0; i < 2; i++) {
            plotvp(x + 17 + i, y + 1, shipcol);
            plotvp(x + 17 + i, y + 2, shipcol);
            plotvp(x + 17 + i, y + 15, shipcol);
            plotvp(x + 17 + i, y + 16, shipcol);
        }
        for (i = 0; i < 4; i++)
            plotvp(x + 17, y + 7 + i, shipcol);
        plotvp(x + 18, y + 6, shipcol);
        plotvp(x + 18, y + 11, shipcol);
        break;
    case 2:
        for (i = 0; i < 2; i++) {
            plotvp(x + i, y + 18, shipcol);
            plotvp(x + i, y + 19, shipcol);
            plotvp(x + 14 + i, y + 18, shipcol);
            plotvp(x + 14 + i, y + 19, shipcol);
        }
        for (i = 0; i < 4; i++)
            plotvp(x + 6 + i, y + 18, shipcol);
        plotvp(x + 5, y + 19, shipcol);
        plotvp(x + 10, y + 19, shipcol);
        break;
    case 3:
        for (i = 0; i < 2; i++) {
            plotvp(x + i, y, shipcol);
            plotvp(x + i, y - 1, shipcol);
            plotvp(x + 14 + i, y, shipcol);
            plotvp(x + 14 + i, y - 1, shipcol);
        }
        for (i = 0; i < 4; i++)
            plotvp(x + 6 + i, y, shipcol);
        plotvp(x + 5, y - 1, shipcol);
        plotvp(x + 10, y - 1, shipcol);
        break;
    }
}

/* draw ship (sel = facing 0..3) at x,y: second design (alien) */
exhaust1(sel, x, y)
unsigned sel;
int x, y;
{
    int i;

    switch (sel) {
    case 1:
        for (i = 0; i < 2; i++) {
            plotvp(x - 1, y + 7 + i, shipcol);
            plotvp(x - 1, y + 9 + i, shipcol);
            plotvp(x - 2, y + 8 + i, shipcol);
        }
        for (i = 0; i < 6; i++)
            plotvp(x - 3, y + 6 + i, shipcol);
        break;
    case 0:
        for (i = 0; i < 2; i++) {
            plotvp(x + 17, y + 7 + i, shipcol);
            plotvp(x + 17, y + 9 + i, shipcol);
            plotvp(x + 18, y + 8 + i, shipcol);
        }
        for (i = 0; i < 6; i++)
            plotvp(x + 19, y + 6 + i, shipcol);
        break;
    case 2:
        for (i = 0; i < 2; i++) {
            plotvp(x + 7 + i, y + 18, shipcol);
            plotvp(x + 9 + i, y + 18, shipcol);
            plotvp(x + 8 + i, y + 19, shipcol);
        }
        for (i = 0; i < 6; i++)
            plotvp(x + 6 + i, y + 20, shipcol);
        break;
    case 3:
        for (i = 0; i < 2; i++) {
            plotvp(x + 7 + i, y, shipcol);
            plotvp(x + 9 + i, y, shipcol);
            plotvp(x + 8 + i, y - 1, shipcol);
        }
        for (i = 0; i < 6; i++)
            plotvp(x + 6 + i, y - 2, shipcol);
        break;
    }
}

/* toggle drawing of the alien ship object p: erase (k=0) or draw (k=1) by type */
exhaust(p, k)
struct obj *p;
int k;
{
    int x, y, dir;

    dir = p->dir;
    x = p->x;
    y = p->y;
    if (k == 0)
        exhcnt--;
    if (exhdrawn && k == 0 || exhdrawn == 0 && k != 0) {
        exhdrawn = !exhdrawn;
        switch (p->type) {
        case 0:
            exhaust0(dir, x, y);
            break;
        case 1:
            exhaust1(dir, x, y);
            break;
        case 2:
            exhaust2(dir, x, y);
            break;
        }
    }
}

/* random turn: 1, -1 or 0 */
randturn()
{
    int r;

    r = rndrange(1, 255);
    if (r > 129)
        return 1;
    if (r < 125)
        return -1;
    return 0;
}

int exhdrawn = 0, growcnt = 0;

/* alien ship fires at the player (random), may drain shield */
alienfir()
{
    int a, r, px, py;

    a = 321;
    if (rndrange(1, 200) < 4 && enemykd && !jumping && enemyx) {
        msgline("Alien fires!");
        r = rndrange(1, 255);
        if (r < 64) {
            px = 159;
            py = 135;
        } else if (r < 128) {
            px = 159;
            py = 20;
        } else if (r > 192) {
            px = 20;
            py = 79;
        } else {
            px = 290;
            py = 79;
        }
        drawline(enemyx + hitmaxx[enemykd] / 2, enemyy + hitmaxy[enemykd], px, py, colhigh);
        playif(8);
        waittick(2);
        drawline(enemyx + hitmaxx[enemykd] / 2, enemyy + hitmaxy[enemykd], px, py, 0);
        if (r < 64) {
            msgline("You've been hit!");
            playif(2);
            if (getshld() < 321)
                a = getshld();
            useshld(a);
            if (getshld() == 0) {
                msgline("Thy shield is drained!");
                die();
            }
        }
        arrowr(0, 24);
    }
}
