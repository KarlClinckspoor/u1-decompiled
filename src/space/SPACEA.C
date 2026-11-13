/* opts: -ms -v -w -cs */
/*
 * SPACE.EXE game code 03E9-16D3: sector/ship accessors, ship pictures, ...
 */

/* a sector of the 7x7 galaxy: 0x3A bytes */
struct obj { int x, y, type, f6, f8, dir; };
struct sector { int count, f2, f4, f6, f8; struct obj o[4]; };

extern struct sector D_26C0[7][7];
extern int D_01EE, D_01F0;              /* current sector */
extern int D_5E14;                      /* current ship */
extern int D_082C, D_0DA6, D_0DA8, D_2044, D_2046, D_5B14;
extern int D_0126, D_0128, D_012A, D_0134, D_01F2, D_5E18, D_206E, D_2080;
extern int D_5B0E, D_5B8C, D_5B8E, D_5E10, D_5E12;
extern int D_2C30, D_2C32, D_2C34, D_2C36, D_2C38, D_2C3A, D_2C3C, D_2C3E;
extern int D_2C40, D_2C42, D_2C44, D_2C46, D_2C48, D_2C4A, D_2C4C, D_2C4E;
extern int D_2C50, D_2C52, D_2C54, D_2C56, D_2C58, D_2C5A, D_2C5C;
extern char D_2058[];
extern F_3F62(), F_3D41(), F_352C(), F_3C8B(), F_5643(), F_3DD6();
extern F_4069(), F_37FD(), F_3E85(), F_3B46(), F_4437(), F_44A2(), F_125C();
extern F_39FC(), F_3812(), F_35CA(), F_3599(), F_3741(), F_3E5E();
extern F_19BD(), F_2252();
extern EXECL(), MEMSET();
extern unsigned char F_413C();

/*
 * Initialised data, DGROUP 0124-0B1E (the literals of the functions follow it)
 */
int D_204A = 4;
int D_0126 = 5, D_0128 = 0, D_012A = 0, D_012C = 0, D_012E = 0;
int _STACK = 0x800;
int D_0132 = 0;
int D_0134 = 0, D_0136 = 0, D_0138 = 0;
int D_013A = 0x9F, D_013C = 0x4F;
char *D_0166[] = { "CgaSpace.Bin", "EgaSpace.Bin", "T1KSpace.Bin" };   /* data files per video mode */
char *D_0194[] = { "CgaFight.Bin", "EgaFight.Bin", "T1KFight.Bin" };
char *D_01C4[] = { "", "Counter-Clockwise", "Clockwise", "Thrust", "Retro" };
char *D_01E4[] = { "", "Left", "Right", "Climb", "Dive" };
int D_01EE = 3, D_01F0 = 3;
int D_01F2 = 0, D_01F4 = 0;
int D_01F6[7] = { 5, 6, 8, 10, 12, 16, 18 };
int D_0204[7] = { 4, 6, 8, 10, 12, 14, 18 };
int D_0212[7] = { 1, 1, 1, 1, 0, 0, 0 };
int D_0220[7] = { 5, 5, 4, 4, 3, 2, 1 };
int D_022E[18][19] = {
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
int D_04DA[20][20] = {
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
int D_07FA_S[18][21] = {
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
int D_0AEE[4][3] = { { -17, -15, -15 }, { 18, 18, 18 }, { 0, 1, 2 }, { 0, 1, 2 } };
int D_0B06[4][3] = { { 1, 1, 1 }, { 1, 1, 1 }, { -17, -16, -16 }, { 18, 17, 17 } };
char D_0B1E[] = "inuse.u1";

extern int D_0126, D_0128, D_012A, D_012C;
extern int D_0134, D_0136, D_0138, D_013A, D_013C;
extern int D_01F2;
extern char *D_01C4[], *D_01E4[];
extern int D_0DA6, D_0DA8, D_2044, D_2046, D_5AD2;
extern int D_5FFA, D_5FFE;
extern int D_5EBA[], D_5F0A[];
extern int D_5E1A[], D_5E6A[];
extern F_3B46(), F_0411(), F_0910(), F_0F25(), F_095F(), F_3109();
extern F_35CA(), F_39FC(), F_3D41(), F_3DD6(), F_254B(), F_3599(), F_3EC4(), F_37FD();
extern F_68F8(), F_0EBD(), F_614E(), F_450D(), F_0524(), F_0425(), F_04CA(), F_603E();
extern int D_5B8C, D_5B8E;
extern int D_01F6[], D_0204[], D_0212[], D_0220[], D_207E, D_20FE, D_082C;
extern F_3330(), F_3CF1(), F_4224(), F_352C(), F_0FBC(), F_0FE1(), F_3E5E(), F_0F3B(), F_259E();
extern int D_0DE2;
extern int F_092D(), F_3741(), F_088E(), F_4069();
extern int D_01EE, D_01F0, D_01F2, D_5E14;
extern int D_0126, D_0128, D_012A, D_012C, D_012E;
extern int D_0134, D_013A, D_013C;
extern int D_0C92, D_0C94, D_0C96, D_0C98, D_0C9A, D_0C9C, D_0C9E[];
extern int D_0DE0, D_0DE2, D_0DA6, D_0DA8;
extern int D_01F6[], D_0204[];
extern int D_2044, D_2046, D_2048, D_5AD2, D_5B14, D_5B0E, D_5B8C, D_5B8E;
extern int D_5E10, D_5E12, D_5FFE, D_6000, D_082C, D_2086, D_207C;
extern char *D_0E18[], *D_0166[], *D_0194[];

extern F_27EE(), F_281F(), F_3B46(), F_39FC(), F_0411(), F_3E5E(), F_08C0(), F_0F25();
extern F_387E(), F_1C2A(), F_3CF1(), F_614E(), F_254B(), F_1BE9(), F_2665(), F_2731();
extern F_3599(), F_352C(), F_0774(), F_07AB(), F_0682(), F_06FB(), F_4558();
extern F_3EED(), F_3C8B(), F_3E2F(), F_3F58(), F_37FD(), F_0D88(), F_1DDA(), F_259E();
extern F_1D0D(), F_1919(), F_19D3(), F_19E1(), F_19EF(), F_19FD(), F_1A0B(), F_163C();
extern F_1A19(), F_2003(), F_14E5(), F_1A27(), F_1A35(), F_1A43(), F_1A51(), F_1A5F();
extern F_1A6D(), F_1A7B(), F_10B3(), F_1A89(), F_478E(), F_1A97(), F_0F0E(), F_165C();
extern F_2B7B(), F_2EC1(), F_3330(), F_31D7(), F_350A(), F_3567(), F_3A4E(), F_3983();
extern PUTS(), _EXIT();
extern int F_4069(), F_088E(), F_1BBF(), F_603E(), F_3741(), F_37E0(), F_34A8(), F_34D9();
extern int F_07E2(), F_0838(), F_063A(), F_3EC4();
extern char F_4161(), F_39C5(), F_3862();

#define ABS(x) ((x) < 0 ? -(x) : (x))

/* number of the largest square (1,4,9,..) that fits in the argument */
F_03E9(a) int a;
{
    int n;
    n = 0;
    while (a > 0) {
        n++;
        a -= 2 * n - 1;
    }
    return n;
}

/* call F_3F62 if D_5B14 is set */
F_0411(c) int c;
{
    if (D_5B14)
        F_3F62(c);
}

/* draw 18x19 picture D_022E at (x,y) in two colours */
F_0425(x, y) int x, y;
{
    int a, b, i, j;
    a = 12;
    b = 14;
    if (D_082C == 0) {
        a = D_0DA8;
        b = D_0DA6;
    }
    for (i = 0; i < 18; i++)
        for (j = 0; j < 19; j++) {
            if (D_022E[i][j] == 1)
                F_3D41(x + j, y + i, b);
            else if (D_022E[i][j] == 2)
                F_3D41(x + j, y + i, a);
        }
}

/* draw 20x20 picture D_04DA at (x,y) */
F_04CA(x, y) int x, y;
{
    int i, j;
    for (i = 0; i < 20; i++)
        for (j = 0; j < 20; j++)
            if (D_04DA[i][j])
                F_3D41(x + j, y + i, D_0DA6);
}

/* draw 18x21 picture D_07FA_S at (x,y) in four colours */
F_0524(x, y) int x, y;
{
    int a, b, c, i, j;
    a = 1;
    b = 10;
    c = 7;
    if (D_082C == 0) {
        a = D_2044;
        b = D_0DA8;
        c = D_0DA6;
    }
    for (i = 0; i < 18; i++)
        for (j = 0; j < 21; j++) {
            if (D_07FA_S[i][j] == 1)
                F_3D41(x + j, y + i, a);
            else if (D_07FA_S[i][j] == 2)
                F_3D41(x + j, y + i, a);
            else if (D_07FA_S[i][j] == 4)
                F_3D41(x + j, y + i, c);
            else if (D_07FA_S[i][j] == 3)
                F_3D41(x + j, y + i, b);
        }
}

/* opposite direction (0<->1, 2<->3) */
F_063A(d) int d;
{
    if (d == 0)
        return 1;
    if (d == 1)
        return 0;
    if (d == 2)
        return 3;
    if (d == 3)
        return 2;
    F_5643();
    F_3DD6();
}

/* print current ship's f6 (shields?) at (0x24,0x16) */
F_0682()
{
    int c, v;
    v = D_26C0[D_01EE][D_01F0].o[D_5E14].f6;
    F_352C(0x24, 0x16);
    if (v < 100)
        c = D_2044;
    else
        c = D_2046;
    F_3C8B(v, 4, 0x20, c, 0);
    F_352C(1, 0x18);
}

/* print current ship's f8 at (0x24,0x15) */
F_06FB()
{
    int c, v;
    v = D_26C0[D_01EE][D_01F0].o[D_5E14].f8;
    F_352C(0x24, 0x15);
    if (v < 100)
        c = D_2044;
    else
        c = D_2046;
    F_3C8B(v, 4, 0x20, c, 0);
    F_352C(1, 0x18);
}

F_0774(n) int n;
{
    if (n == 0)
        return 1000;
    if (n == 2)
        return 2500;
    if (n == 1)
        return 5000;
    return 0;
}

F_07AB(n) int n;
{
    if (n == 0)
        return 1000;
    if (n == 2)
        return 5000;
    if (n == 1)
        return 1000;
    return 0;
}

F_07E2(k, sx, sy, s) int k, sx, sy, s;
{
    int typ;
    typ = D_26C0[sx][sy].o[s].type;
    return D_0AEE[k][typ] + D_26C0[sx][sy].f6;
}

F_0838(k, sx, sy, s) int k, sx, sy, s;
{
    int typ;
    typ = D_26C0[sx][sy].o[s].type;
    return D_0B06[k][typ] + D_26C0[sx][sy].f8;
}

F_088E()
{
    return D_26C0[D_01EE][D_01F0].o[D_5E14].f8;
}

/* reduce fuel (o[ship].f8) by n, not below 0, and redraw it */
F_08C0(n) int n;
{
    int v;
    v = F_088E();
    if (v < n)
        v = 0;
    else
        v -= n;
    D_26C0[D_01EE][D_01F0].o[D_5E14].f8 = v;
    F_06FB();
}

F_0910()
{
    F_08C0(F_088E() / 4 + 5);
}

F_092D()
{
    return D_26C0[D_01EE][D_01F0].o[D_5E14].f6;
}

F_095F(n) int n;
{
    int v;
    v = F_092D();
    if (v < n)
        v = 0;
    else
        v -= n;
    D_26C0[D_01EE][D_01F0].o[D_5E14].f6 = v;
    F_0682();
}

/* fill sector (sx,sy) of the galaxy with random contents */
F_09AF(sx, sy) int sx, sy;
{
    int a, b, c, d, e, f, i;
    for (i = 0; i < 4; i++)
        D_26C0[sx][sy].o[i].type = -1;
    D_26C0[sx][sy].count = F_4069(0, 0xF0) / 0x28;
    if (D_26C0[sx][sy].count > 3)
        D_26C0[sx][sy].count = 0;
    d = 0;
    c = 0;
    b = 0;
    a = 0;
    if (F_4069(1, 200) < 0x4D) {
        a = F_4069(0x32, 200);
        b = F_4069(0x32, 0x78);
    }
    D_26C0[sx][sy].f2 = a;
    D_26C0[sx][sy].f4 = b;
    if (F_4069(1, 200) < 0x4D) {
        c = F_4069(0x32, 200);
        d = F_4069(0x32, 100);
        if (ABS(c - a) < 0x28 || ABS(d - b) < 0x28) {
            d = 0;
            c = 0;
        }
    }
    D_26C0[sx][sy].f6 = c;
    D_26C0[sx][sy].f8 = d;
    if (c) {
        a = F_4069(0, 0x62) % 3;
        if (a) {
            e = F_4069(0, 199) % 4;
            c = F_4069(0, 0x62) % 3;
            D_26C0[sx][sy].o[0].type = c;
            D_26C0[sx][sy].o[0].f6 = F_0774(c);
            D_26C0[sx][sy].o[0].f8 = F_07AB(c);
            D_26C0[sx][sy].o[0].x = F_07E2(e, sx, sy, 0);
            D_26C0[sx][sy].o[0].y = F_0838(e, sx, sy, 0);
            D_26C0[sx][sy].o[0].dir = F_063A(e);
            if (a == 2) {
                do
                    d = F_4069(0, 199) % 3;
                while (d == c);
                do
                    f = F_4069(0, 0x62) % 3;
                while (e == f);
                D_26C0[sx][sy].o[1].type = d;
                D_26C0[sx][sy].o[1].f6 = F_0774(d);
                D_26C0[sx][sy].o[1].f8 = F_07AB(d);
                D_26C0[sx][sy].o[1].x = F_07E2(f, sx, sy, 1);
                D_26C0[sx][sy].o[1].y = F_0838(f, sx, sy, 1);
                D_26C0[sx][sy].o[1].dir = F_063A(f);
            }
        }
    }
}

/* initialise the galaxy and the ships */
F_0D88()
{
    int i, j;
    F_37FD(D_5B0E * 3 / 2);
    MEMSET(D_26C0, 0, 0xB1A);
    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            F_09AF(i, j);
    D_5E14 = 2;
    D_2C30 = 0;
    D_2C32 = 0x8C;
    D_2C34 = 0x46;
    D_2C36 = 200;
    D_2C38 = 100;
    D_2C3E = 2;
    D_2C40 = F_0774(2);
    D_2C42 = F_07AB(2);
    D_2C3A = F_07E2(3, 3, 3, 0);
    D_2C3C = F_0838(3, 3, 3, 0);
    D_2C44 = F_063A(3);
    D_2C4A = 1;
    D_2C4C = F_0774(1);
    D_2C4E = F_07AB(1);
    D_2C46 = F_07E2(0, 3, 3, 1);
    D_2C48 = F_0838(0, 3, 3, 1);
    D_2C50 = F_063A(0);
    D_2C56 = 0;
    D_2C58 = D_5B8C;
    D_2C5A = D_5B8E;
    D_2C52 = 0x104;
    D_2C54 = 0x5A;
    D_2C5C = 0;
}

/* leave to OUT.EXE: save state to inuse.u1 and execl("out.exe") forever */
F_0EBD()
{
    int u;
    if (D_206E == 0) {
        F_08C0(F_088E());
        F_095F(F_092D());
    }
    F_3E85(D_0B1E, (int)D_2058, 0x334);
    for (;;) {
        u = EXECL("out.exe", "out.exe", D_5E10, D_5E12, "S", 0);
        F_125C();
    }
}

F_0F0E()
{
    F_3B46("Huh?");
    F_0411(1);
}

F_0F25()
{
    D_2080 = D_206E = 0;
    F_4437();
    F_44A2();
    F_0EBD();
}

/* wait until a key is pressed (or ~10000 polls) */
F_0F3B()
{
    int i;
    i = 0;
    while (!F_413C(0) && i++ < 10000)
        ;
}

/* draw the frame of the main viewing window in colour c */
F_0F62(c) int c;
{
    F_39FC(0x22, 0x1A, 0x11C, 0x1A, c);
    F_3812(0x11C, 0x7C, c);
    F_3812(0x22, 0x7C, c);
    F_3812(0x22, 0x1A, c);
    F_352C(5, 4);
}

/* clear the viewing window and draw its frame in colour c */
F_0FBC(c) int c;
{
    F_35CA(0x1F, 0x17, 0x11F, 0x7F, 0);
    F_0F62(c);
}

/* print string s at (a,b) */
F_0FE1(a, b, s) int a, b;
char *s;
{
    F_352C(a, b);
    F_3599(s, D_2046);
}

/* round to nearest 100 and to nearest 1000 */
F_0FFD(n) int n;
{
    if (n % 100 > 0x31)
        n += 100;
    if (n % 1000 > 0x1F3)
        n += 1000;
    return n;
}

F_1031()
{
    int k;
    k = D_0126 * 1000 / D_5E18;
    D_0128 *= k;
    D_0128 = F_0FFD(D_0128);
    D_0128 /= 1000;
    if (D_0128 > 6)
        D_0128 = 6;
}

F_1072()
{
    int k;
    k = D_0126 * 1000 / D_5E18;
    D_012A *= k;
    D_012A = F_0FFD(D_012A);
    D_012A /= 1000;
    if (D_012A > 6)
        D_012A = 6;
}

/* View command: switch between the cockpit and the sector view */
F_10B3()
{
    F_3599("View", D_2046);
    if (D_01F2 == 0) {
        if (F_3741(D_5E14) != -1) {
            F_3B46("?");
            F_3B46("View can't be changed");
            F_3B46("whilst in space dock.");
        } else {
            F_3B46("");
            if (F_092D() == 0)
                F_3B46("Not enough fuel!");
            else {
                D_0134 = D_01F2 = 1;
                D_0126 = F_03E9(D_0128 * D_0128 + D_012A * D_012A);
                if (D_0126 > 8)
                    D_0126 = 8;
                else if (D_0126 == 0) {
                    D_0126 = 1;
                    switch (D_26C0[D_01EE][D_01F0].o[D_5E14].dir) {
                    case 0:
                        D_0128 = -1;
                        break;
                    case 1:
                        D_0128 = 1;
                        break;
                    case 2:
                        D_012A = -1;
                        break;
                    case 3:
                        D_012A = 1;
                        break;
                    }
                }
                D_5E18 = D_0126;
            }
        }
    } else if (D_26C0[D_01EE][D_01F0].count != 0) {
        F_3B46("?");
        F_3B46("Thou must eliminate all");
        F_3B46("enemy craft first!");
    } else {
        D_01F2 = 0;
        F_3B46("");
        D_0134 = 1;
        F_1031();
        F_1072();
        if (D_0128 == 0 && D_012A == 0) {
            switch (D_26C0[D_01EE][D_01F0].o[D_5E14].dir) {
            case 0:
                D_0128 = -1;
                break;
            case 1:
                D_0128 = 1;
                break;
            case 2:
                D_012A = -1;
                break;
            case 3:
                D_012A = 1;
                break;
            }
        }
    }
}

/* draw a sector symbol at (x,y): two vertical bars and a crossbar */
F_127E(x, y) int x, y;
{
    int i;
    for (i = 0; i < 2; i++) {
        F_39FC(x + i, y, x + i, y + 7, D_0DA6);
        F_39FC(x + i + 5, y, x + i + 5, y + 7, D_0DA6);
        F_39FC(x, y + 3 + i, x + 6, y + 3 + i, D_0DA6);
    }
}

F_12F1(x, y) int x, y;
{
    int i;
    for (i = 0; i < 2; i++) {
        F_39FC(x + i + 3, y, x + i + 3, y + 7, D_0DA6);
        F_39FC(x, y + i + 3, x + 7, y + i + 3, D_0DA6);
    }
}

F_1349(x, y) int x, y;
{
    int i;
    for (i = 0; i < 3; i++) {
        F_39FC(x + 3 - i, y + i + 1, x + 3 + i, y + i + 1, D_0DA6);
        F_39FC(x + 3 - i, y + 6 - i, x + 3 + i, y + 6 - i, D_0DA6);
    }
}

F_13B3(x, y) int x, y;
{
    F_3D41(x + 1, y + 1, D_0DA6);
    F_3D41(x + 3, y + 3, D_2044);
    F_3D41(x + 6, y + 2, D_0DA8);
    F_3D41(x + 4, y + 5, D_0DA6);
    F_3D41(x + 2, y + 6, D_0DA8);
}

/* draw the map symbol of sector (i,j) */
F_142B(i, j) int i, j;
{
    int px, py;
    px = i * 2 * 5 + 0xA5 - 0x23 + 2;
    py = j * 2 * 5 + 0x4F - 0x23 + 2;
    if (D_26C0[i][j].count)
        F_127E(px, py);
    else if (D_26C0[i][j].f6)
        F_12F1(px, py);
    else if (D_26C0[i][j].f2)
        F_1349(px, py);
    else
        F_13B3(px, py);
}

/* Inform and Search: draw the 7x7 sector map, blink the current sector until a key */
F_14E5()
{
    int i, j, px, py;
    F_3599("Inform and Search", D_2046);
    F_0FBC(D_0DA8);
    F_0FE1(5, 4, "Sector Scan:");
    F_39FC(0x82, 0x2C, 200, 0x2C, D_0DA6);
    F_3812(200, 0x73, D_0DA6);
    F_3812(0x82, 0x73, D_0DA6);
    F_3812(0x82, 0x2C, D_0DA6);
    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            F_142B(i, j);
    px = D_01EE * 2 * 5 + 0xA5 - 0x23 + 2;
    py = D_01F0 * 2 * 5 + 0x4F - 0x23 + 2;
    do {
        if (!(j = F_413C(0))) {
            F_3E5E(7);
            for (i = 0; i < 8; i++)
                F_39FC(px + i, py, px + i, py + 8, 0);
            if (!(j = F_413C(0))) {
                F_3E5E(7);
                F_142B(D_01EE, D_01F0);
            }
        }
    } while (j == 0);
    D_0134 = 1;
    F_3B46("");
}

/* Fire (in space) or Fire? / space-dock action */
F_163C()
{
    if (D_01F2 == 0)
        F_19BD("Fire?");
    else
        F_2252();
}

/* Speed command: set speed to n, costing fuel */
F_165C(n) int n;
{
    int k;
    F_3599("Speed ", D_2046);
    F_3C8B(n, 1, 0x20, D_2046, 1);
    F_3B46("");
    k = ABS(n - D_0126) * 4;
    if (k > F_092D())
        F_3B46("Not enough fuel!");
    else {
        D_0126 = n;
        F_095F(k);
    }
}

int D_0BD2[2][4] = { { 3, 2, 0, 1 }, { 2, 3, 1, 0 } };      /* direction changes */

/* fly: turn, thrust or retro (k 1..4); costs fuel, may crash into an object */
F_16D3(k)
unsigned char k;
{
    int cost, delta;

    cost = 5;
    delta = -1;
    F_3B46(D_01C4[k]);
    if (k == 1 || k == 2)
        cost = 2;
    if (F_092D() == 0) {
        F_3B46("No fuel!!");
        F_3B46("Wilt thou drift forever?!?");
    } else {
        if (F_092D() < cost)
            cost = F_092D();
        if (F_3741(D_5E14) != -1 && k != 4) {
            F_3B46("Crunch!");
            F_0411(0);
            F_0910();
            if (!F_088E()) {
                F_3B46("Thy shield is drained!");
                F_0F25();
            }
        } else {
            F_095F(cost);
            cost = D_26C0[D_01EE][D_01F0].o[D_5E14].dir;
            if (k == 1 || k == 2) {
                if (D_012C)
                    F_3109(&D_26C0[D_01EE][D_01F0].o[D_5E14], 0);
                D_26C0[D_01EE][D_01F0].o[D_5E14].dir = D_0BD2[k - 1][cost];
                delta = D_012C = 0;
            } else if (k == 3)
                delta = 1;
            switch (cost) {
            case 2:
                D_012A -= delta;
                break;
            case 3:
                D_012A += delta;
                break;
            case 0:
                D_0128 -= delta;
                break;
            case 1:
                D_0128 += delta;
                break;
            }
            if (D_0128 > -1)
                D_0128 = D_0128 <= 9 ? D_0128 : 9;
            else
                D_0128 = D_0128 > -9 ? D_0128 : -9;
            if (D_012A > -1)
                D_012A = D_012A <= 9 ? D_012A : 9;
            else
                D_012A = D_012A > -9 ? D_012A : -9;
            if (k == 3)
                D_012C += 10;
            else if (k == 4) {
                F_3109(&D_26C0[D_01EE][D_01F0].o[D_5E14], 0);
                D_012C = 0;
            }
        }
    }
}

/* move in combat view (k 1..4: left, right, climb, dive); outside combat fly instead */
F_1919(k)
unsigned char k;
{
    if (D_01F2 == 0)
        F_16D3(k);
    else {
        F_3B46(D_01E4[k]);
        if (F_092D() < 2)
            F_3B46("Not enough fuel!");
        else {
            F_095F(2);
            switch (k) {
            case 1:
                D_0136 = -4;
                D_0138 = 0;
                break;
            case 2:
                D_0136 = 4;
                D_0138 = 0;
                break;
            case 3:
                D_0138 = -4;
                D_0136 = 0;
                break;
            case 4:
                D_0138 = 4;
                D_0136 = 0;
                break;
            }
        }
    }
}

/* print a command name and make the "not here" sound */
F_19BD(s)
char *s;
{
    F_3B46(s);
    F_0411(1);
}

F_19D3() { F_19BD("Attack?"); }
F_19E1() { F_19BD("Board?"); }
F_19EF() { F_19BD("Cast?"); }
F_19FD() { F_19BD("Drop?"); }
F_1A0B() { F_19BD("Enter?"); }
F_1A19() { F_19BD("Get?"); }
F_1A27() { F_19BD("Klimb?"); }
F_1A35() { F_19BD("Open?"); }
F_1A43() { F_19BD("Quit?"); }
F_1A51() { F_19BD("Ready?"); }
F_1A5F() { F_19BD("Steal?"); }
F_1A6D() { F_19BD("Transact?"); }
F_1A7B() { F_19BD("Unlock?"); }
F_1A89() { F_19BD("X-it?"); }

/* pass; in combat: center the crosshair and stop sideways motion */
F_1A97()
{
    if (D_01F2 == 0)
        F_3B46("Pass");
    else {
        F_1BE9(0);
        D_013A = 0x9f;
        D_013C = 0x4f;
        D_0136 = D_0138 = 0;
        F_35CA(16, 16, 0x12f, 0x8f, 0);
        F_1BE9(D_0DA6);
        F_3B46("Center");
    }
}

/* is (x, y) inside the combat viewport? */
F_1AF5(x, y)
int x, y;
{
    return x < 300 && x > 20 && y < 140 && y > 20;
}

/* place star i at a random offset from the crosshair */
F_1B21(i)
int i;
{
    int dx, dy;

    D_5FFA = F_4069(0, 255) / 64;
    dx = F_4069(2, 255) / 2;
    dy = F_4069(4, 255) / 4;
    if (D_5FFA == 1 || D_5FFA == 2)
        dx = -dx;
    if (D_5FFA > 1)
        dy = -dy;
    if (!F_1AF5(dx + D_013A, dy + D_013C))
        dx = dy = 5000;
    D_5E1A[i] = dx;
    D_5E6A[i] = dy;
}

/* sign of v */
F_1BBF(v)
int v;
{
    if (v > 0)
        return 1;
    if (v < 0)
        return -1;
    return 0;
}

/* draw the crosshair in colour c */
F_1BE9(c)
int c;
{
    F_39FC(D_013A - 2, D_013C, D_013A + 2, D_013C, c);
    F_39FC(D_013A, D_013C - 2, D_013A, D_013C + 2, c);
}

/* move the 40 stars one step, redraw them and the crosshair, update the crosshair position */
F_1C2A()
{
    int i, dx, dy, moving;

    F_0411(9);
    moving = D_0136 || D_0138 || D_5FFE != 2;
    if (moving)
        F_1BE9(0);
    for (i = 0; i < 40; i++) {
        dx = D_5E1A[i] / 8;
        dy = D_5E6A[i] / 6;
        if (dx == 0)
            dx = F_1BBF(D_5E1A[i]);
        if (dy == 0)
            dy = F_1BBF(D_5E6A[i]);
        if (D_5FFE != 1 && F_1AF5(D_5EBA[i], D_5F0A[i]))
            F_3D41(D_5EBA[i], D_5F0A[i], 0);
        D_5E1A[i] += dx;
        D_5E6A[i] += dy;
        if (!F_1AF5(D_5E1A[i] + D_013A, D_5E6A[i] + D_013C))
            F_1B21(i);
        else
            F_3D41(D_5E1A[i] + D_013A, D_5E6A[i] + D_013C, D_0DA6);
        D_5EBA[i] = D_5E1A[i] + D_013A;
        D_5F0A[i] = D_5E6A[i] + D_013C;
    }
    if (D_5FFE == 2) {
        D_013A += D_0136;
        D_013C += D_0138;
        if (D_013A >= 300 || D_013A <= 20) {
            D_013A -= D_0136;
            D_0136 = 0;
        }
        if (D_013C >= 140 || D_013C <= 20) {
            D_013C -= D_0138;
            D_0138 = 0;
        }
        if (moving)
            F_1BE9(D_0DA6);
    }
    F_0411(9);
}

/* place all 40 stars */
F_1DDA()
{
    int i;

    D_5FFA = 0;
    for (i = 0; i < 40; i++) {
        F_1B21(i);
        D_5EBA[i] = D_013A + 3;
        D_5F0A[i] = D_013C + 3;
    }
}

/* leave the sector through the edge: wrap the galaxy, move the ship object to a free slot of the new sector */
F_1E1C()
{
    int dx, dy, k, ox, oy;

    k = D_26C0[D_01EE][D_01F0].o[D_5E14].dir;
    dx = F_1BBF(D_0128);
    dy = F_1BBF(D_012A);
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
    ox = D_01EE;
    oy = D_01F0;
    D_01EE += dx;
    D_01F0 += dy;
    if (D_01EE < 0)
        D_01EE = 6;
    else
        D_01EE = D_01EE % 7;
    if (D_01F0 < 0)
        D_01F0 = 6;
    else
        D_01F0 = D_01F0 % 7;
    for (k = 0; k < 4 && D_26C0[D_01EE][D_01F0].o[k].type != -1; k++)
        ;
    if (k == 4)
        F_3DD6();
    D_26C0[D_01EE][D_01F0].o[k] = D_26C0[ox][oy].o[D_5E14];
    D_26C0[ox][oy].o[D_5E14].type = -1;
    D_5E14 = k;
    D_26C0[D_01EE][D_01F0].o[D_5E14].x = 260;
    D_26C0[D_01EE][D_01F0].o[D_5E14].y = 90;
    if (D_26C0[D_01EE][D_01F0].count)
        F_254B();
}

int D_0C92 = 0, D_0C94 = 0, D_0C96 = 0, D_0C98 = 0, D_0C9A = 0, D_0C9C = 0;
int D_0C9E[9] = { 0, 8, 7, 6, 5, 4, 3, 2, 1 };

/* Hyperjump command: only from front view; costs fuel; lightspeed animation, then new sector */
F_2003()
{
    int i, j, x, n, r;

    F_3599("HyperJump", D_2046);
    if (D_01F2 == 0) {
        F_3B46("?");
        F_3B46("Hyperjump can only be made");
        F_3B46("from front view.");
    } else if (F_092D() < 100) {
        F_3B46("");
        F_3B46("Not enough fuel!");
    } else {
        D_0DE2 = 20;
        F_3599("...", D_2046);
        r = F_3EC4() | 0xBBBB;
        x = D_0126;
        D_0136 = D_0138 = 0;
        if (x < 4)
            n = 10;
        else
            n = 15;
        D_0C9C = 1;
        for (j = x; j < 9; j++) {
            D_0126 = j;
            for (i = 0; i <= n; i++)
                F_3330();
        }
        D_0126 = 0;
        for (i = 0; i < 40; i++)
            if (F_1AF5(D_5EBA[i], D_5F0A[i]))
                F_3D41(D_5EBA[i], D_5F0A[i], 0);
        F_3599("Lightspeed!", D_2046);
        F_352C(10, 24);
        F_37FD(r);
        F_1DDA();
        D_5FFE = 1;
        for (i = 0; i <= 200; i++)
            F_1C2A();
        F_37FD(r);
        F_1DDA();
        D_5FFE = 3;
        for (i = 0; i <= 199; i++)
            F_1C2A();
        D_5FFE = 2;
        for (j = 8; j > x; j--) {
            D_0126 = j;
            for (i = 0; i <= n; i++) {
                F_1C2A();
                F_3CF1(D_0C9E[D_0126]);
            }
        }
        F_1BE9(D_0DA6);
        F_3B46(" completed.     ");
        F_095F(100);
        F_37FD(F_3EC4() + 1234);
        D_0126 = x;
        F_4224();
        D_0C92 = D_0C94 = D_0C96 = D_0C98 = 0;
        F_1E1C();
        D_012C = D_0C9C = 0;
    }
}

/* victory screen: "Thou hast achieved the rank of Space Ace!" */
F_21FC()
{
    F_4224();
    F_0FBC(D_0DA8);
    F_0FE1(11, 6, "Thou hast achieved");
    F_0FE1(14, 8, "the rank of");
    F_0FE1(15, 10, "Space Ace!");
    F_3E5E(5);
    F_4224();
    F_0F3B();
    F_259E();
}

/* Fire command: laser bolts from both sides to the crosshair; hit test against the target */
F_2252()
{
    int px, py, xhi, yhi;

    F_3B46("Fire!");
    if (F_092D() < 10)
        F_3B46("Thou hast not enough fuel!");
    else {
        F_095F(10);
        if (D_0C92) {
            F_614E(D_0C92, D_0C94, D_0C9A, 1);
            px = D_013A - D_0C92;
            py = D_013C - D_0C94;
        } else
            px = py = -100;
        F_39FC(20, 140, D_013A, D_013C, D_2044);
        F_39FC(300, 140, D_013A, D_013C, D_2044);
        F_0411(8);
        xhi = D_01F6[D_0C9A];
        yhi = D_0204[D_0C9A];
        if (D_0C92)
            F_614E(D_0C92, D_0C94, D_0C9A, 0);
        if (D_0212[D_0C9A] <= px && px <= xhi && py >= D_0220[D_0C9A] && py <= yhi) {
            F_0411(2);
            F_3B46("Hit!!!");
            D_207E += 100;
            F_450D();
            D_20FE++;
            if (D_20FE == 20)
                F_21FC();
            else if (D_20FE > 20)
                F_3B46("Thou art still a Space ace!");
            if (--D_26C0[D_01EE][D_01F0].count)
                F_254B();
            else
                D_0C92 = D_0C94 = 0;
        }
        F_39FC(20, 140, D_013A, D_013C, 0);
        F_39FC(300, 140, D_013A, D_013C, 0);
        F_1BE9(D_0DA6);
    }
}

/* draw the cockpit frame and its rivets */
F_23F3()
{
    int i, c;

    c = 7;
    if (D_082C == 0)
        c = D_0DA6;
    F_35CA(8, 8, 311, 15, c);
    F_35CA(8, 144, 311, 151, c);
    F_35CA(8, 16, 15, 144, c);
    F_35CA(304, 16, 311, 144, c);
    for (i = 1; i < 20; i++) {
        F_3D41(i * 15 + 10, 10, 0);
        F_3D41(i * 15 + 10, 13, 0);
        F_3D41(i * 15 + 10, 146, 0);
        F_3D41(i * 15 + 10, 149, 0);
    }
    for (i = 1; i < 10; i++) {
        F_3D41(10, i * 15 + 3, 0);
        F_3D41(13, i * 15 + 3, 0);
        F_3D41(306, i * 15 + 3, 0);
        F_3D41(309, i * 15 + 3, 0);
    }
}

/* put a target in view: random position, unless one is already there */
F_254B()
{
    if (D_0C9C == 0) {
        D_0C92 = F_4069(40, 280);
        D_0C94 = F_4069(40, 120);
        D_0C9A = 0;
    } else
        D_0C92 = D_0C94 = 0;
}

/* redraw the combat view: stars and crosshair */
F_258D()
{
    F_1DDA();
    F_1BE9(D_0DA6);
}

/* draw the front view of the current sector: frame, base, planet/sun, ships */
F_259E()
{
    int i;
    struct sector *s;
    struct obj *p;

    D_0134 = 0;
    F_35CA(16, 16, 303, 143, 0);
    F_23F3();
    if (D_01F2 == 1)
        F_258D();
    else {
        if (D_01EE == 3 && D_01F0 == 3)
            F_0524(60, 75);
        s = &D_26C0[D_01EE][D_01F0];
        if (s->f2)
            F_0425(s->f2, s->f4);
        if (s->f6)
            F_04CA(s->f6, s->f8);
        for (i = 0; i < 4; i++) {
            p = &s->o[i];
            if (p->type > -1)
                F_603E(p->type, p->dir, p->x, p->y);
        }
    }
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

/* collision check with the object at (f2,f4) of the sector: closer than 25 in both axes */
F_2665()
{
    struct sector *s;

    s = &D_26C0[D_01EE][D_01F0];
    if (s->f2) {
        if (ABS(s->f2 - s->o[D_5E14].x) < 25 && ABS(s->f4 - s->o[D_5E14].y) < 25) {
            F_3B46("Thy ship melts near the star!");
            F_0F25();
        }
    }
}

/* is the ship near the planet (60,75) of sector 3,3? landing on a planet, or melting */
F_2731()
{
    struct obj *p;

    p = &D_26C0[D_01EE][D_01F0].o[D_5E14];
    if (D_01EE == 3 && D_01F0 == 3) {
        if (ABS(p->x - 60) < 25 && ABS(p->y - 75) < 25) {
            if (p->type == 0) {
                F_3B46("Thou hast landed safely!");
                D_5B8C = p->f6;
                D_5B8E = p->f8;
                F_0EBD();
            } else {
                F_3B46("Only space shuttles are heat");
                F_3B46("shielded for landings!");
                D_5B8C = D_5B8E = 0;
                F_0F25();
            }
        }
    }
}

/* plot a pixel if inside the viewport */
F_27EE(x, y, c)
int x, y, c;
{
    if (x > 20 && x < 300 && y > 18 && y < 141)
        F_68F8(x, y, c);
}

/* draw a small ship (a sel = 0..3: facing) with its lights at (x,y) */
F_281F(sel, x, y)
unsigned sel;
int x, y;
{
    int i;

    switch (sel) {
    case 1:
        for (i = 0; i < 3; i++) {
            F_27EE(x - 1, y + 4 + i, D_5AD2);
            F_27EE(x - 1, y + 11 + i, D_5AD2);
        }
        F_27EE(x - 2, y + 5, D_0DA6);
        F_27EE(x - 2, y + 6, D_0DA6);
        F_27EE(x - 2, y + 11, D_0DA6);
        F_27EE(x - 2, y + 12, D_0DA6);
        F_27EE(x - 3, y + 6, D_5AD2);
        F_27EE(x - 3, y + 11, D_5AD2);
        break;
    case 0:
        for (i = 0; i < 3; i++) {
            F_27EE(x + 19, y + 4 + i, D_5AD2);
            F_27EE(x + 19, y + 11 + i, D_5AD2);
        }
        F_27EE(x + 20, y + 5, D_0DA6);
        F_27EE(x + 20, y + 6, D_0DA6);
        F_27EE(x + 20, y + 11, D_0DA6);
        F_27EE(x + 20, y + 12, D_0DA6);
        F_27EE(x + 21, y + 6, D_5AD2);
        F_27EE(x + 21, y + 11, D_5AD2);
        break;
    case 2:
        for (i = 0; i < 3; i++) {
            F_27EE(x + 5 + i, y + 19, D_5AD2);
            F_27EE(x + 12 + i, y + 19, D_5AD2);
        }
        F_27EE(x + 6, y + 20, D_0DA6);
        F_27EE(x + 7, y + 20, D_0DA6);
        F_27EE(x + 12, y + 20, D_0DA6);
        F_27EE(x + 13, y + 20, D_0DA6);
        F_27EE(x + 7, y + 21, D_5AD2);
        F_27EE(x + 12, y + 21, D_5AD2);
        break;
    case 3:
        for (i = 0; i < 3; i++) {
            F_27EE(x + 5 + i, y - 1, D_5AD2);
            F_27EE(x + 12 + i, y - 1, D_5AD2);
        }
        F_27EE(x + 6, y - 2, D_0DA6);
        F_27EE(x + 7, y - 2, D_0DA6);
        F_27EE(x + 12, y - 2, D_0DA6);
        F_27EE(x + 13, y - 2, D_0DA6);
        F_27EE(x + 7, y - 3, D_5AD2);
        F_27EE(x + 12, y - 3, D_5AD2);
        break;
    }
}

/* draw ship (sel = facing 0..3) at x,y: big wedge design */
F_2B7B(sel, x, y)
unsigned sel;
int x, y;
{
    int i;

    switch (sel) {
    case 1:
        for (i = 0; i < 2; i++) {
            F_27EE(x - 1 - i, y + 1, D_5AD2);
            F_27EE(x - 1 - i, y + 2, D_5AD2);
            F_27EE(x - 1 - i, y + 15, D_5AD2);
            F_27EE(x - 1 - i, y + 16, D_5AD2);
        }
        for (i = 0; i < 4; i++)
            F_27EE(x - 1, y + 7 + i, D_5AD2);
        F_27EE(x - 2, y + 6, D_5AD2);
        F_27EE(x - 2, y + 11, D_5AD2);
        break;
    case 0:
        for (i = 0; i < 2; i++) {
            F_27EE(x + 17 + i, y + 1, D_5AD2);
            F_27EE(x + 17 + i, y + 2, D_5AD2);
            F_27EE(x + 17 + i, y + 15, D_5AD2);
            F_27EE(x + 17 + i, y + 16, D_5AD2);
        }
        for (i = 0; i < 4; i++)
            F_27EE(x + 17, y + 7 + i, D_5AD2);
        F_27EE(x + 18, y + 6, D_5AD2);
        F_27EE(x + 18, y + 11, D_5AD2);
        break;
    case 2:
        for (i = 0; i < 2; i++) {
            F_27EE(x + i, y + 18, D_5AD2);
            F_27EE(x + i, y + 19, D_5AD2);
            F_27EE(x + 14 + i, y + 18, D_5AD2);
            F_27EE(x + 14 + i, y + 19, D_5AD2);
        }
        for (i = 0; i < 4; i++)
            F_27EE(x + 6 + i, y + 18, D_5AD2);
        F_27EE(x + 5, y + 19, D_5AD2);
        F_27EE(x + 10, y + 19, D_5AD2);
        break;
    case 3:
        for (i = 0; i < 2; i++) {
            F_27EE(x + i, y, D_5AD2);
            F_27EE(x + i, y - 1, D_5AD2);
            F_27EE(x + 14 + i, y, D_5AD2);
            F_27EE(x + 14 + i, y - 1, D_5AD2);
        }
        for (i = 0; i < 4; i++)
            F_27EE(x + 6 + i, y, D_5AD2);
        F_27EE(x + 5, y - 1, D_5AD2);
        F_27EE(x + 10, y - 1, D_5AD2);
        break;
    }
}

/* draw ship (sel = facing 0..3) at x,y: second design (alien) */
F_2EC1(sel, x, y)
unsigned sel;
int x, y;
{
    int i;

    switch (sel) {
    case 1:
        for (i = 0; i < 2; i++) {
            F_27EE(x - 1, y + 7 + i, D_5AD2);
            F_27EE(x - 1, y + 9 + i, D_5AD2);
            F_27EE(x - 2, y + 8 + i, D_5AD2);
        }
        for (i = 0; i < 6; i++)
            F_27EE(x - 3, y + 6 + i, D_5AD2);
        break;
    case 0:
        for (i = 0; i < 2; i++) {
            F_27EE(x + 17, y + 7 + i, D_5AD2);
            F_27EE(x + 17, y + 9 + i, D_5AD2);
            F_27EE(x + 18, y + 8 + i, D_5AD2);
        }
        for (i = 0; i < 6; i++)
            F_27EE(x + 19, y + 6 + i, D_5AD2);
        break;
    case 2:
        for (i = 0; i < 2; i++) {
            F_27EE(x + 7 + i, y + 18, D_5AD2);
            F_27EE(x + 9 + i, y + 18, D_5AD2);
            F_27EE(x + 8 + i, y + 19, D_5AD2);
        }
        for (i = 0; i < 6; i++)
            F_27EE(x + 6 + i, y + 20, D_5AD2);
        break;
    case 3:
        for (i = 0; i < 2; i++) {
            F_27EE(x + 7 + i, y, D_5AD2);
            F_27EE(x + 9 + i, y, D_5AD2);
            F_27EE(x + 8 + i, y - 1, D_5AD2);
        }
        for (i = 0; i < 6; i++)
            F_27EE(x + 6 + i, y - 2, D_5AD2);
        break;
    }
}

/* toggle drawing of the alien ship object p: erase (k=0) or draw (k=1) by type */
F_3109(p, k)
struct obj *p;
int k;
{
    int x, y, dir;

    dir = p->dir;
    x = p->x;
    y = p->y;
    if (k == 0)
        D_012C--;
    if (D_0DE0 && k == 0 || D_0DE0 == 0 && k != 0) {
        D_0DE0 = !D_0DE0;
        switch (p->type) {
        case 0:
            F_281F(dir, x, y);
            break;
        case 1:
            F_2EC1(dir, x, y);
            break;
        case 2:
            F_2B7B(dir, x, y);
            break;
        }
    }
}

/* random turn: 1, -1 or 0 */
F_319F()
{
    int r;

    r = F_4069(1, 255);
    if (r > 129)
        return 1;
    if (r < 125)
        return -1;
    return 0;
}

int D_0DE0 = 0, D_0DE2 = 0;

/* alien ship fires at the player (random), may drain shield */
F_31D7()
{
    int a, r, px, py;

    a = 321;
    if (F_4069(1, 200) < 4 && D_0C9A && !D_0C9C && D_0C92) {
        F_3B46("Alien fires!");
        r = F_4069(1, 255);
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
        F_39FC(D_0C92 + D_01F6[D_0C9A] / 2, D_0C94 + D_0204[D_0C9A], px, py, D_2044);
        F_0411(8);
        F_3E5E(2);
        F_39FC(D_0C92 + D_01F6[D_0C9A] / 2, D_0C94 + D_0204[D_0C9A], px, py, 0);
        if (r < 64) {
            F_3B46("You've been hit!");
            F_0411(2);
            if (F_088E() < 321)
                a = F_088E();
            F_08C0(a);
            if (F_088E() == 0) {
                F_3B46("Thy shield is drained!");
                F_0F25();
            }
        }
        F_387E(0, 24);
    }
}
