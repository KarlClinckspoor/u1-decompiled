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
extern int D_022E[18][19];
/* D_04DA (20x20) is followed directly by D_07FA_S (18x21), whose name is ambiguous in the .syms */
extern struct { int a[20][20]; int b[18][21]; } D_04DA;
extern int D_0AEE[][3], D_0B06[][3];
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
            if (D_04DA.a[i][j])
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
            if (D_04DA.b[i][j] == 1)
                F_3D41(x + j, y + i, a);
            else if (D_04DA.b[i][j] == 2)
                F_3D41(x + j, y + i, a);
            else if (D_04DA.b[i][j] == 4)
                F_3D41(x + j, y + i, c);
            else if (D_04DA.b[i][j] == 3)
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
    F_3E85("inuse.u1", (int)D_2058, 0x334);
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
