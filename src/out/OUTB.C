/* opts: -ms -v -w -cs */
/*
 * OUT.EXE - the game, part B of 3 (game code 32F8-63F2: dungeon view, dungeon generation, enter/Get/Inform/Steal commands, shops, story pages (F_32F8-F_63D4)).
 * The three parts are one program split for the compiler's memory (LC1 stops at about 2200 lines);
 * string literals used in more than one part are named arrays (D_xxxx = their DGROUP address).
 * Builds byte-identical with Lattice C 3.10 (lc -ms -v -w -cs).
 * Names are still the addresses they had in the original (F_xxxx code, D_xxxx DGROUP data).
 */

/* Origin library and Lattice library */
extern F_1D0D(), F_352C(), F_353A(), F_355D(), F_357C(), F_3599(), F_35CA();
extern F_37FD(), F_3812(), F_387E(), F_3920(), F_39FC(), F_3AE2(), F_3B46();
extern F_3BF6(), F_3C46(), F_3C8B(), F_3E2F(), F_3E5E(), F_3E85(), F_3EC4(), F_3EED();
extern F_3F58(), F_3F62(), F_4069(), F_40A8(), F_40E9(), F_40FE(), F_43C8(), F_4437();
extern F_44A2(), F_450D(), F_4558(), F_45A3(), F_478E(), F_5643(), F_91D4(), F_A363();
extern F_A3BA(), F_A554(), F_AC91(), F_AF57(), F_B0B5(), F_B34F(), F_B502(), F_B545();
extern F_B58E(), F_B5E1(), F_B617(), F_B665(), F_B70F(), F_B7CD(), F_B884(), F_C2AF();
extern F_C2EC(), F_C31C(), F_C3CA(), F_C47C();
extern EXECL(), EXIT();
extern unsigned char F_33BB(), F_3862(), F_39C5(), F_4127(), F_413C(), F_4161();
extern char F_411C();

#define ABS(x) ((x) < 0 ? -(x) : (x))

struct cell { int a, b, c, d; };
struct mon { int type, f2, x, y, f8, fa, fc, fe; };
struct ent { int f0, f2, f4, f6; };

/* data of the Origin text module (OM4437): tables of names and messages */
extern int D_082C;
extern int D_0CD2[];
extern unsigned char *D_0D72[], *D_0DF8[], *D_0FD8[];
extern char *D_0E9E[], *D_0F04[], *D_0F5C[], *D_0FA2[];
extern int D_1DC8[];
/* tables inside that module that have no label of their own (absolute names in OUTSYM.ASM) */
extern struct ent D_31DC[][15];
extern int D_368C[], D_3698[], D_36E8[], D_36F8[], D_31CC[], D_3708[], D_3808[], D_3814[];
extern char *D_307C[], *D_3174[], *D_37AE[], *D_3930[];

/* BSS (defined in OUTBSS.C) */
extern int D_0DA6, D_0DA8, D_2044, D_2046, D_2048, D_204A, D_204E;
extern char D_2058[];
extern int D_206A, D_206C, D_206E[];
extern int D_207C, D_207E, D_2080, D_2082, D_2084, D_2086, D_2088;
extern int D_20A4, D_20AC[], D_20B8[], D_20D8[], D_20EE[], D_20FE;
extern int D_3A90, D_3A92, D_3A94, D_3A96, D_3A98, D_3A9A;
extern int D_3AAA, D_3AAC, D_3AAE, D_3AB0, D_3AB2[];
extern int D_3AC6, D_3AC8, D_3ACA, D_3AFA[];
extern int D_3B1A, D_3B1C, D_3B20, D_3B22, D_3B28, D_3B2A;
extern long D_3B24;
extern struct mon D_3B2C[];
extern char D_3DAC[], D_238C[];
extern int D_8B88, D_8B8A, D_8B8C, D_8B8E, D_8B90, D_8B92, D_8B94, D_8B96, D_8B98;
extern int D_8B9C, D_8B9E, D_8BA0, D_8BA2, D_8BA4, D_8BA6;
extern long D_8BA8;
extern unsigned char D_8B31, D_8BAE[][38][18];
extern struct ent D_ACC6[];
extern int D_AD3E, D_AD40, D_AD42, D_AD46, D_AD48, D_AD4A, D_AD4C, D_AD4E;
extern int D_AD50, D_AD52, D_AD54;
extern struct cell D_AD56[][11];
extern int D_ADB6, D_ADBE, D_AE96, D_AFD6;
extern char D_B11E[];

extern char D_018D[];
extern char D_019B[];
extern char D_01C7[];
extern char D_0220[];
extern char D_0235[];
extern char D_02E1[];
extern char D_02E8[];

#define MT ((int (*)[8])D_3B2C)

extern char *D_0172, *D_0180;
extern int D_0182;
/* pub sign number n; always returns 6 */
F_32F8(n)
unsigned n;
{
    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(14, 4, "Pub de Varg");
        break;
    case 1:
        F_21E7(11, 4, "Ye Olde Local Pub");
        break;
    case 2:
        F_21E7(13, 4, "Dr. Cat's Lair");
        break;
    case 3:
        F_21E7(13, 4, "Teaser's House");
        break;
    case 5:
        F_21E7(12, 4, "Mary's Midway");
        break;
    case 6:
        F_21E7(10, 4, "Kurtstable's Korner");
        break;
    case 7:
        F_21E7(11, 4, "Dav's House-o-Suds");
        break;
    }
    return 6;
}

/* ---- ou_2b6d_c ---- */
/* draw the left wall edges (top and bottom) of dungeon depth step i */
F_33B0(i)
int i;
{
    if (i <= 5) {
        F_39FC(D_3808[i - 1] + 16, D_3814[i - 1] + 8, D_3808[i] + 16, D_3814[i] + 8, D_0DA6);
        F_39FC(D_3808[i - 1] + 16, 151 - D_3814[i - 1], D_3808[i] + 16, 151 - D_3814[i], D_0DA6);
    }
}

/* draw the right wall edges (top and bottom) of dungeon depth step i */
F_3430(i)
int i;
{
    if (i <= 5) {
        F_39FC(303 - D_3808[i - 1], D_3814[i - 1] + 8, 303 - D_3808[i], D_3814[i] + 8, D_0DA6);
        F_39FC(303 - D_3808[i - 1], 151 - D_3814[i - 1], 303 - D_3808[i], 151 - D_3814[i], D_0DA6);
    }
}

/* draw the front wall rectangle of depth step i (inset by 8 pixels when i is 0) */
F_34B0(i)
int i;
{
    int dx, dy;

    dy = dx = 0;
    if (i == 0)
        dx = dy = 8;
    if (i <= 5) {
        F_39FC(D_3808[i] + 16 + dx, D_3814[i] + 8 + dy, 303 - D_3808[i] - dx, D_3814[i] + 8 + dy, D_0DA6);
        F_39FC(D_3808[i] + 16 + dx, 151 - D_3814[i] - dy, 303 - D_3808[i] - dx, 151 - D_3814[i] - dy, D_0DA6);
    }
}

/* draw the left wall's outline at depth step i */
F_355A(i)
int i;
{
    if (i <= 5) {
        F_39FC(D_3808[i - 1] + 16, D_3814[i] + 8, D_3808[i] + 16, D_3814[i] + 8, D_0DA6);
        F_39FC(D_3808[i - 1] + 16, 151 - D_3814[i], D_3808[i] + 16, 151 - D_3814[i], D_0DA6);
    }
}

/* draw the right wall's outline at depth step i */
F_35DC(i)
int i;
{
    if (i <= 5) {
        F_39FC(303 - D_3808[i - 1], D_3814[i] + 8, 303 - D_3808[i], D_3814[i] + 8, D_0DA6);
        F_39FC(303 - D_3808[i - 1], 151 - D_3814[i], 303 - D_3808[i], 151 - D_3814[i], D_0DA6);
    }
}

/* vertical lines at x=26 and x=28 spaced by step i */
F_365E(i)
int i;
{
    if (i <= 5) {
        F_AC91(26, i, D_0DA6);
        F_AC91(28, i, D_0DA6);
    }
}

F_368F(i)
int i;
{
    if (i <= 5) {
        F_AC91(27, i, D_0DA6);
        F_AC91(28, i, D_0DA6);
    }
}

F_36C0(i)
int i;
{
    if (i <= 5) {
        F_AC91(27, i, D_0DA6);
        F_AC91(29, i, D_0DA6);
    }
}

F_36F1(i)
int i;
{
    if (i <= 5) {
        F_AC91(26, i, D_0DA6);
        F_AC91(29, i, D_0DA6);
    }
}

/* draw three horizontal rungs (a ladder) across the wall at depth step i */
F_3722(i)
int i;
{
    int k, d;

    if (i <= 5) {
        d = D_3814[i] + 8 - (151 - D_3814[i]);
        d = ABS(d / 4);
        for (k = 1; k <= 3; k++)
            F_39FC(D_3808[i] + 16, D_3814[i] + 8 + k * d + i / 2, 303 - D_3808[i],
                   D_3814[i] + 8 + k * d + i / 2, D_2044);
    }
}

/* draw the left side door at depth step i: wall lines plus door frame */
F_37DB(i)
int i;
{
    int a, b;

    if (i <= 5) {
        F_33B0(i);
        a = D_3808[i - 1] - D_3808[i];
        b = D_3814[i - 1] - D_3814[i];
        a = ABS(a / 9);
        b = ABS(b / 5);
        F_39FC(a + 16 + a + D_3808[i - 1], 151 - b - D_3814[i - 1] - 1, a + 16 + a + D_3808[i - 1],
               D_3814[i] + 8 - b, D_0DA6);
        F_3812(a * 6 + 16 + D_3808[i - 1], D_3814[i] + 8 + b, D_0DA6);
        F_3812(a * 6 + 16 + D_3808[i - 1], 151 - D_3814[i] + b + b - (i == 1) * 2, D_0DA6);
    }
}

/* draw the right side door at depth step i */
F_390D(i)
int i;
{
    int a, b;

    if (i <= 5) {
        F_3430(i);
        a = D_3808[i - 1] - D_3808[i];
        b = D_3814[i - 1] - D_3814[i];
        a = ABS(a / 9);
        b = ABS(b / 5);
        F_39FC(303 - (a + a + D_3808[i - 1]), 151 - b - D_3814[i - 1] - 1, 303 - (a + a + D_3808[i - 1]),
               D_3814[i] + 8 - b, D_0DA6);
        F_3812(303 - (a * 6 + D_3808[i - 1]), D_3814[i] + 8 + b, D_0DA6);
        F_3812(303 - (a * 6 + D_3808[i - 1]), 151 - D_3814[i] + b + b - (i == 1) * 2, D_0DA6);
    }
}

/* draw the front wall at depth step i with a doorway */
F_3A54(i)
int i;
{
    int col, dy;

    dy = col = 0;
    if (i)
        col = D_0DA6;
    else
        dy = 8;
    if (i < 5) {
        F_34B0(i);
        F_39FC(303 - D_3808[i + 1], 151 - D_3814[i] - dy, D_3808[i + 1] + 16, 151 - D_3814[i] - dy, col);
        F_3812(D_3808[i + 1] + 16, D_3814[i + 1] + 8, D_0DA6);
        F_3812(303 - D_3808[i + 1], D_3814[i + 1] + 8, D_0DA6);
        F_3812(303 - D_3808[i + 1], 151 - D_3814[i] - dy, D_0DA6);
    }
}

/* vertical line at the left edge of depth step i */
F_3B25(i)
int i;
{
    if (i <= 5)
        F_39FC(D_3808[i] + 16, D_3814[i] + 8, D_3808[i] + 16, 151 - D_3814[i], D_0DA6);
}

/* vertical line at the right edge of depth step i */
F_3B65(i)
int i;
{
    if (i <= 5)
        F_39FC(303 - D_3808[i], D_3814[i] + 8, 303 - D_3808[i], 151 - D_3814[i], D_0DA6);
}

/* name the dungeon feature kind y and draw it at depth x (kind 9 = front wall outline) */
F_3BA5(x, y)
int x, y;
{
    if (x < 5) {
        F_3B46(D_3930[y]);
        if (y != 9)
            F_AC91(y, x, D_0DA6);
        else {
            F_34B0(x);
            F_3B25(x);
            F_3B65(x);
        }
    }
}

/* coffin at depth x */
F_3BF5(x)
int x;
{
    F_3B46(D_02E1);
    F_AC91(25, x, D_0DA6);
}

/* chest at depth x */
F_3C13(x)
int x;
{
    F_3B46(D_02E8);
    F_AC91(11, x, D_0DA6);
}

/* print the name of feature kind k ("Mimic" for 11) */
F_3C31(k)
int k;
{
    if (k != 11)
        F_3599(D_3930[k], D_2046);
    else
        F_3599("Mimic", D_2046);
}

/* sign of x: -1, 0 or 1 */
F_3C64(x)
int x;
{
    if (x < 0)
        return -1;
    if (x > 0)
        return 1;
    return 0;
}

/* dx of the facing direction D_AD52: east 1, west -1 */
F_3C8E()
{
    int r;

    r = 0;
    if (D_AD52 == 2)
        r = 1;
    else if (D_AD52 == 1)
        r = -1;
    return r;
}

/* dy of the facing direction D_AD52: north -1, south 1 */
F_3CBB()
{
    int r;

    r = 0;
    if (D_AD52 == 3)
        r = -1;
    else if (D_AD52 == 4)
        r = 1;
    return r;
}

/* print the compass direction at the status line */
F_3CE8()
{
    F_387E(16, 19);
    switch (D_AD52) {
    case 3:
        F_3599(" North ", D_2046);
        break;
    case 4:
        F_3599(" South ", D_2046);
        break;
    case 2:
        F_3599(" East  ", D_2046);
        break;
    case 1:
        F_3599(" West  ", D_2046);
        break;
    }
    F_3920(24, 19);
    F_352C(1, 24);
}

/* draw the dungeon cell (x, y) at depth step depth */
F_3D6C(depth, x, y)
int depth, x, y;
{
    int t;

    t = D_AD56[x][y].a;
    if (t == 3 || t == 2 || t == 1)
        F_34B0(depth);
    if (t == 6)
        F_AC91(27, depth + 1, D_0DA6);
    if (t == 7)
        F_AC91(26, depth + 1, D_0DA6);
    F_3BA5(depth, D_AD56[x][y].b);
}

/* draw the dungeon cell (x, y) seen at depth step depth */
F_3DF5(depth, x, y)
int depth, x, y;
{
    if (D_AD56[x][y].b != -1)
        F_3D6C(depth, x, y);
    else {
        switch (D_AD56[x][y].a) {
        case 1:
        case 2:
            F_34B0(depth);
            break;
        case 3:
            F_3A54(depth);
            break;
        case 8:
            F_3722(depth);
            break;
        case 6:
            if (D_AD52 == 3 || D_AD52 == 4)
                F_368F(depth + 1);
            else
                F_36C0(depth + 1);
            break;
        case 7:
            if (D_AD52 == 3 || D_AD52 == 4)
                F_365E(depth + 1);
            else
                F_36F1(depth + 1);
            break;
        }
    }
    if (D_AD56[x][y].c == 5)
        F_3BF5(depth + 1);
    else if (D_AD56[x][y].c == 4)
        F_3C13(depth + 1);
}

/* draw the wall piece at depth step depth, type side: 1/2 left-ish, 3 door, else right */
F_3F19(depth, side)
int depth, side;
{
    switch (side) {
    case 2:
    case 1:
        F_3430(depth);
        break;
    case 3:
        F_390D(depth);
        break;
    default:
        F_35DC(depth);
    }
}

/* same for the other pair of wall lines */
F_3F4C(depth, side)
int depth, side;
{
    switch (side) {
    case 2:
    case 1:
        F_33B0(depth);
        break;
    case 3:
        F_37DB(depth);
        break;
    default:
        F_355A(depth);
    }
}

/* is the square at (D_AD3E + a, D_AD40 + b) walkable? 1 yes, 0 no */
F_3F7F_O(a, b)
int a, b;
{
    int v;

    v = D_AD56[D_AD3E + a][D_AD40 + b].a;
    if (v == 1 || v == 2 || v == 8)
        return 1;
    if (v == 3 && D_AD56[D_AD3E][D_AD40].a == 3)
        return 1;
    if (D_AD56[D_AD3E + a][D_AD40 + b].b == -1)
        return 0;
    return 1;
}

/* ---- ou_4013 ---- */
/* passable test for map tile (x,y): 0 if blocked */
F_4013(x, y)
int x, y;
{
    if (D_AD56[x][y].b == -1 || D_AD56[x][y].b == 20 || D_AD56[x][y].b == 11 || D_AD56[x][y].b == 9)
        return 0;
    return 1;
}

/* is square at offset (dx,dy) from player walkable */
F_407E(dx, dy)
int dx, dy;
{
    int t;

    t = D_AD56[D_AD3E + dx][D_AD40 + dy].a;
    if (t == 1 || t == 2 || t == 3)
        return 1;
    return F_4013(D_AD3E + dx, D_AD40 + dy);
}

/* distance (1..5) walking along direction (a,b) until blocked */
F_40D4(a, b)
int a, b;
{
    int n;

    n = 1;
    while (!F_407E(a, b)) {
        n++;
        a += F_3C64(a);
        b += F_3C64(b);
    }
    if (n > 5)
        n = 5;
    return n;
}

/* tile type is not 0/6/7/8 */
char F_411C(c)
unsigned char c;
{
    if (c == 0 || c == 7 || c == 6 || c == 8)
        return 0;
    return 1;
}

/* draw the first-person dungeon view in the facing direction D_AD52 */
F_414A()
{
    int a, b, c, d, ex, ey, dx, dy, n, dist, fx, fy, f1, f2;
    unsigned char pl, pr, l, r, fl, fr;

    d = c = b = a = 0;
    if (D_204E) {
        F_456D();
        D_204E = 0;
    }
    fl = fr = pl = pr = 100;
    dx = F_3C8E();
    dy = F_3CBB();
    switch (D_AD52) {
    case 3:
        a = -1;
        b = 1;
        break;
    case 4:
        a = 1;
        b = -1;
        break;
    case 2:
        c = -1;
        d = 1;
        break;
    case 1:
        c = 1;
        d = -1;
        break;
    }
    F_35CA(8, 8, 0x137, 0x97, 0);
    F_3CE8();
    f1 = D_AD56[D_AD3E][D_AD40].a == 3;
    f2 = D_AD56[D_AD3E][D_AD40].a == 2 || D_AD56[D_AD3E][D_AD40].a == 1;
    dist = F_40D4(dx, dy);
    if (f2)
        F_34B0(0);
    else {
        n = dist;
        fx = ex = F_3C64(dx) * n;
        fy = ey = F_3C64(dy) * n;
        if (f1 && (D_AD56[D_AD3E + dx][D_AD40 + dy].a == 1 || D_AD56[D_AD3E + dx][D_AD40 + dy].a == 3
                   || D_AD56[D_AD3E + dx][D_AD40 + dy].a == 2))
            F_34B0(0);
        else {
            if (f1)
                F_3A54(0);
            while (n) {
                ex -= dx;
                ey -= dy;
                if (!f1 || n > 1) {
                    l = D_AD56[D_AD3E + ex + a][D_AD40 + ey + c].a;
                    r = D_AD56[D_AD3E + ex + b][D_AD40 + ey + d].a;
                    F_3F4C(n, l);
                    F_3F19(n, r);
                    l = F_411C(l);
                    r = F_411C(r);
                    if (n == dist) {
                        fl = l;
                        fr = r;
                    }
                    if (n != dist) {
                        if (pl != l)
                            F_3B25(n);
                        if (pr != r)
                            F_3B65(n);
                    }
                }
                F_3DF5(n, D_AD3E + ex + dx, D_AD40 + ey + dy);
                pl = l;
                pr = r;
                n--;
            }
            if (!f1)
                F_3DF5(0, D_AD3E, D_AD40);
            if (F_4013(D_AD3E + fx, D_AD40 + fy) && D_AD56[D_AD3E + fx][D_AD40 + fy].a != 3 && dist < 5) {
                l = D_AD56[a + D_AD3E + fx][c + D_AD40 + fy].a;
                r = D_AD56[D_AD3E + b + fx][D_AD40 + d + fy].a;
                F_3F4C(dist + 1, l);
                F_3F19(dist + 1, r);
                if (!F_411C(l))
                    F_3B25(dist);
                if (!F_411C(r))
                    F_3B65(dist);
            } else {
                if (fl == 1)
                    F_3B25(dist);
                if (fr == 1)
                    F_3B65(dist);
            }
        }
        if (f1) {
            if (D_AD56[D_AD3E][D_AD40].c == 5)
                F_3BF5(1);
            else if (D_AD56[D_AD3E][D_AD40].c == 4)
                F_3C13(1);
        }
    }
}

/* clear the screen area and print the status line with the dungeon level */
F_456D()
{
    F_387E(15, 0);
    F_3599(" Level    ", D_2046);
    F_3920(26, 0);
    F_352C(23, 0);
    F_3C8B(D_AD50, 2, 32, D_2046, 0);
    F_352C(1, 24);
}

/* set the cell (D_3AAA + 3*D_3AAE + 5*D_3AAC + 17*D_AD50) */
F_45C8()
{
    int t;

    t = D_3AAC * 5 + D_3AAE * 3 + D_AD50 * 17;
    F_37FD(t + D_3AAA);
}

/* place one monster/ladder on a random free square of the dungeon level */
F_4602()
{
    int x, y, q, ok, cnt;

    cnt = 0;
    do {
        x = F_4069(0, 242) % 9 + 1;
        y = F_4069(0, 242) % 9 + 1;
        ok = D_AD56[x][y].b == -1 && D_AD56[x][y].a != 1 && D_AD56[x][y].a != 2
             && D_AD56[x][y].a != 8 && D_AD56[x][y].c != 4;
    } while (!ok && ++cnt <= 500);
    if (cnt < 500 && !(x == D_AD3E && y == D_AD40)) {
        do {
            cnt = F_4069(0, 49) / 10;
        } while (D_B11E[cnt]);
        D_B11E[cnt] = 1;
        q = (D_AD50 - 1) / 2 * 5 + cnt;
        D_AD56[x][y].b = q;
        D_AD56[x][y].d = F_4069(1, D_AD50 * D_AD50 + 1) + 10 + q;
    }
}

/* generate a new dungeon level: walls, doors, traps, monsters */
F_4761()
{
    int i, j, k, m, d;

    F_456D();
    F_45C8();
    for (i = 0; i <= 10; i++)
        for (j = 0; j <= 10; j++) {
            D_AD56[i][j].a = 0;
            D_AD56[i][j].c = 0;
            D_AD56[i][j].b = -1;
            D_AD56[i][j].d = 0;
        }
    for (i = 0; i <= 10; i++) {
        D_AD56[i][0].a = 1;
        D_AD56[0][i].a = 1;
        D_AD56[10][i].a = 1;
        D_AD56[i][10].a = 1;
    }
    for (i = 2; i <= 8; i += 2) {
        D_AD56[i][2].a = 1;
        D_AD56[i][4].a = 1;
        D_AD56[i][6].a = 1;
        D_AD56[i][8].a = 1;
    }
    for (i = 2; i <= 8; i += 2)
        for (j = 1; j <= 9; j++)
            D_AD56[i][j].a = F_4069(1, 4) - 1;
    m = D_AD50 << 1;
    k = 0;
    while (k <= m) {
        if (F_4069(1, 255) > 160)
            d = 8;
        else
            d = 1;
        i = F_4069(1, 16) - 1;
        if (D_AD50 & 1)
            D_AD56[D_1DC8[38 + i]][D_1DC8[54 + i]].a = d;
        else
            D_AD56[D_1DC8[54 + i]][D_1DC8[38 + i]].a = d;
        k++;
    }
    F_37FD(F_3EC4() + 0x6F1);
    k = 0;
    while (k <= D_AD50) {
        d = (F_4069(1, 100) & 1) + 4;
        i = F_4069(10, 99) / 10;
        j = F_4069(10, 99) / 10;
        if (D_AD56[i][j].a != 1 && D_AD56[i][j].a != 2 && D_AD56[i][j].a != 8)
            D_AD56[i][j].c = d;
        k++;
    }
    D_ADBE = 0;
    if (D_AD50 & 1) {
        D_AFD6 = 7;
        D_AE96 = 6;
    } else {
        D_AFD6 = 6;
        D_AE96 = 7;
    }
    if (D_AD50 == 10)
        D_AFD6 = 0;
    if (D_AD50 == 1) {
        D_ADB6 = 7;
        D_AFD6 = 0;
    }
    for (i = 0; i < 5; i++)
        D_B11E[i] = 0;
    F_4602();
    F_4602();
    F_4602();
}

/* Turn around command */
F_4A38()
{
    F_3B46("Turn around");
    switch (D_AD52) {
    case 3:
        D_AD52 = 4;
        break;
    case 4:
        D_AD52 = 3;
        break;
    case 2:
        D_AD52 = 1;
        break;
    case 1:
        D_AD52 = 2;
        break;
    }
}

/* Turn left command */
F_4A82()
{
    F_3B46("Turn left");
    switch (D_AD52) {
    case 3:
        D_AD52 = 1;
        break;
    case 4:
        D_AD52 = 2;
        break;
    case 2:
        D_AD52 = 3;
        break;
    case 1:
        D_AD52 = 4;
        break;
    }
}

/* Turn right command */
F_4ACC()
{
    F_3B46("Turn right");
    switch (D_AD52) {
    case 3:
        D_AD52 = 2;
        break;
    case 4:
        D_AD52 = 1;
        break;
    case 2:
        D_AD52 = 4;
        break;
    case 1:
        D_AD52 = 3;
        break;
    }
}

/* Forward command: step one square unless blocked */
F_4B16()
{
    int dx, dy;

    dx = F_3C8E();
    dy = F_3CBB();
    F_3599("Forward", D_2046);
    if (F_3F7F_O(dx, dy) == 0) {
        D_AD3E += dx;
        D_AD40 += dy;
        F_3B46(D_019B);
    } else {
        F_0476(0);
        F_3B46(" - path blocked!");
    }
}

/* dungeon command dispatch: 1 left, 2 right, 3 forward, 4 turn around */
F_4B73(c)
unsigned char c;
{
    switch (c) {
    case 4:
        F_4A38();
        break;
    case 3:
        F_4B16();
        break;
    case 1:
        F_4A82();
        break;
    case 2:
        F_4ACC();
        break;
    }
}

/* enter the first dungeon level (start state) */
F_4BB0()
{
    D_204A = 3;
    D_AD50 = D_AD3E = D_AD40 = 1;
    D_AD52 = 4;
    D_AD54 = 0;
    F_4761();
}

/* princess rescued: rewards and the time-machine message */
F_4BD6()
{
    int rx, ry, x, y, flg, cnt, off;

    off = cnt = 0;
    flg = D_20FE >= 20 && D_207E >= 7000;
    if (!flg)
        off = 2;
    F_21C2(D_0DA8);
    F_21E7(5, off + 5, " Thou hast saved the princess");
    F_21E7(5, off + 6, D_31CC[D_AD4A]);
    F_3599(".  She giveth thee", D_2046);
    F_21E7(8, off + 7, "500 hit points, pence and");
    F_21E7(12, off + 8, "experience points.");
    D_207C += 500;
    D_206E[0] += 500;
    D_207E += 500;
    F_4558();
    F_4437();
    F_450D();
    if (flg) {
        F_21E7(5, 10, "  She informs thee that thou");
        F_21E7(5, 11, "art now ready for time travel,");
        F_21E7(5, 12, "    and that there is a time");
        F_21E7(5, 13, " machine far to the northwest!");
        if (D_3B1C < 9) {
            D_3B1C++;
            switch (F_40FE(D_3AAC, D_3AAE)) {
            case 0:
                x = y = 0;
                break;
            case 1:
                x = 83;
                y = 0;
                break;
            case 2:
                x = 0;
                y = 77;
                break;
            case 3:
                x = 83;
                y = 77;
                break;
            }
            do {
                rx = F_4069(12, 21);
                ry = F_4069(11, 15);
                if (F_C2AF(x + rx, y + ry, 1) == 2) {
                    cnt = 500;
                    F_B502(18, 2, x + rx, y + ry, 0);
                }
            } while (cnt++ < 499);
        }
    }
    F_3E5E(5);
    F_2141();
    F_352C(1, 24);
}

/* enter a castle (c = 0x21..0x28): set up the level, player position */
F_4DBF(c)
int c;
{
    D_AD4A = c - 33;
    D_AD42 = (c - 1) % 2;
    D_AD4C = D_AD4A + 1;
    D_204A = 2;
    F_20FC();
    D_AD3E = 0;
    D_AD40 = 9;
    D_AD46 = 0;
    D_8BA0 = 0;
    D_8B9E = 0;
    D_8BA4 = 0;
    D_8BAE[D_AD42][35][(D_AD42 == 0) * 10 + 4] = 11;
    D_8BAE[D_AD42][31][(D_AD42 == 0) * 10 + 4] = 11;
    if (F_4069(1, 255) & 1)
        D_AD4E = 61;
    else
        D_AD4E = 60;
}

/* enter a town (c = 1..0x20): set up the level and position */
F_4E63(c)
int c;
{
    D_AD48 = 0;
    D_AD42 = (c - 1) % 8 + 2;
    D_204A = 1;
    F_20FC();
    D_AD3E = 19;
    D_AD40 = 17;
    F_2231();
    D_204E = 0;
}

/* show the sign / inscription text for a place (c = 0..7) */
F_4EA2(c)
unsigned c;
{
    F_21C2(D_0DA8);
    switch (c) {
    case 0:
        F_21E7(8, 8, "You feel a strong magic");
        F_21E7(12, 9, "surrounding you!");
        break;
    case 1:
        F_21E7(10, 7, "You hear someone say,");
        F_21E7(6, 9, " \"TURIS-SCIENTIA-MAGNOPERE\"");
        break;
    case 2:
        F_21E7(13, 7, "A sign reads:");
        F_21E7(8, 9, "\"FORTES FORTUNA ADIUVAT!\"");
        break;
    case 3:
        F_21E7(13, 5, "On a pedestal,");
        F_21E7(10, 6, "these words appear:");
        F_21E7(8, 7, "\"MY NAME IS OZYMANDIAS,");
        F_21E7(13, 8, "KING OF KINGS:");
        F_21E7(11, 9, "LOOK AT MY WORKS,");
        F_21E7(8, 10, "YE MIGHTY, AND DESPAIR!\"");
        F_21E7(8, 12, "Nothing beside remains.");
        F_21E7(7, 13, "You feel a strange force!");
        break;
    case 4:
        F_21E7(12, 6, "The sign reads:");
        F_21E7(12, 8, "\"ULTIMA THULE!\"");
        F_21E7(7, 10, "The sky grows dark, and a");
        F_21E7(7, 11, "strong magic engulfs you!");
        break;
    case 5:
        F_21E7(12, 7, "The sign reads:");
        F_21E7(11, 9, "\"OMNIA MUTANTUR!\"");
        break;
    case 6:
        F_21E7(12, 7, "The sign reads:");
        F_21E7(9, 9, "\"GO EAST TO GO EAST!\"");
        break;
    case 7:
        F_21E7(10, 7, "The grave is marked:");
        F_21E7(14, 9, "\"VAE VICTIS\"");
        break;
    }
    F_352C(1, 24);
}

/* first visit message "A quest has been completed!" for an odd place */
F_5085(c)
int c;
{
    if (D_3AB2[8 - c] == -1) {
        D_3AB2[8 - c] = 1;
        F_3599("A quest has been completed!", D_2046);
        F_0476(5);
        F_3B46(D_019B);
    }
}

/* enter a sign/shrine place (c = 0x29..0x30): text, gifts, stat gain */
F_50C6(c)
int c;
{
    int v, hp, gain, ok;

    ok = 1;
    c -= 41;
    F_4EA2(c);
    if (c & 1)
        F_5085(c);
    if (c == D_3B20)
        ok = 0;
    else {
        D_3B20 = c;
        v = D_3708[c];
        if (v == 0 && c == 2) {
            v = 0;
            do
                v++;
            while (D_20B8[v] != 0);
            if (v > 15)
                ok = 0;
            else {
                D_20B8[v]++;
                F_3599("You find a", D_2046);
                if (F_40A8(D_0DF8[v][0]))
                    F_357C('n', D_2046);
                F_357C(' ', D_2046);
                F_3599(D_0D72[v], D_2046);
                F_3599(D_018D, D_2046);
            }
        } else if (c != 6) {
            hp = D_206E[v];
            if (hp == 99)
                ok = 0;
            else {
                gain = (hp + 8) / 10;
                if (gain + hp > 99)
                    gain = 99 - hp;
                F_3599("Thou dost gain ", D_2046);
                F_3C8B(gain, 1, 32, D_2046, 1);
                F_357C(' ', D_2046);
                F_3599(D_0E9E[v], D_2046);
                D_206E[v] += gain;
            }
        }
    }
    if (c == 6)
        ok = 0;
    if (ok) {
        F_2168(D_0DA6);
        F_0476(5);
        F_2168(D_0DA8);
    } else if (c != 6) {
        F_3599("Hmmmm...no effect!", D_2046);
        F_0476(6);
    }
    F_2141();
    if (c != 6)
        F_3B46(D_019B);
}

/* Enter command: enter the town/castle/dungeon/shrine at the player's spot */
F_52AB()
{
    int t, i, j, k;

    F_3599("Enter", D_2046);
    t = F_A3BA(D_3AAC, D_3AAE);
    if (!t || D_204A) {
        F_3B46(D_0235);
        F_0476(1);
    } else {
        F_3B46("ing...");
        D_AD46 = D_8B94 = D_8B96 = D_8B98 = 0;
        D_204E = 1;
        for (i = -1; i <= 1; i++)
            for (j = -1; j <= 1; j++) {
                k = F_C2AF(D_3AAC + i, D_3AAE + j, 1);
                if (k == 0)
                    D_8B94++;
                else if (k == 1)
                    D_8B96++;
                else if (k == 2)
                    D_8B98++;
            }
        D_8BA2 = t == 33;
        if (t < 33)
            F_3599("The city of ", D_2046);
        F_3B46(D_307C[t]);
        if (t < 33)
            F_4E63(t);
        else if (t < 41)
            F_4DBF(t);
        else if (t < 49)
            F_50C6(t);
        else
            F_4BB0(t);
    }
}

/* Fire command (ship cannons / lasers) */
F_53DA()
{
    if (D_204A == 1 || D_204A == 2 || D_204A == 3) {
        F_3B46("Fire?");
        F_0476(1);
    } else {
        F_3599("Fire ", D_2046);
        if (D_2088 != 4 && D_2088 != 5) {
            F_3B46("what?");
            F_0476(1);
        } else {
            if (D_2088 == 4)
                F_3599("cannons: ", D_2046);
            else
                F_3599("lasers: ", D_2046);
            F_1072(-1, 8);
        }
    }
}

/* ---- ou_5469 ---- */
/* "not the king's permission" message and sound */
F_5469()
{
    F_3B46(D_019B);
    F_3B46("Thou hast not the king's");
    F_3B46("permission!");
    F_0476(1);
}

/* shop counter type 0x37: needs a turn counter, then F_5D34 (armour) */
F_5492()
{
    if (D_8BA4 == 0)
        F_5469();
    else {
        D_8BA4--;
        F_5D34(0);
    }
}

/* shop counter type 0x3B: F_5BFA (weapons) */
F_54B5()
{
    if (D_8BA4 == 0)
        F_5469();
    else {
        D_8BA4--;
        F_5BFA(0);
    }
}

/* shop counter type 0x39: F_5CBA (food) */
F_54D8()
{
    if (D_8BA4 == 0)
        F_5469();
    else {
        D_8BA4--;
        F_5CBA(0);
    }
}

/* dispatch on the counter tile the player stands on */
F_54FB()
{
    int c;

    c = D_8BAE[D_AD42][D_AD3E][D_AD40];
    switch (c) {
    case 0x39:
        F_54D8();
        break;
    case 0x37:
        F_5492();
        break;
    case 0x3B:
        F_54B5();
        break;
    default:
        F_3B46(" - nothing here!");
    }
}

/* Get command */
F_5550()
{
    F_3599("Get", D_2046);
    if (D_204A == 2)
        F_54FB();
    else {
        if (D_204A == 1)
            F_3599(" what", D_2046);
        F_3B46(D_0235);
        F_0476(1);
    }
}

/* HyperJump? message */
F_559A()
{
    F_3B46("HyperJump?");
    F_0476(1);
}

/* dungeon: look at the cell in the facing direction (secret door -> ladder) */
F_55B1()
{
    int a, dx, dy;

    dx = F_3C8E();
    dy = F_3CBB();
    a = D_AD56[D_AD3E + dx][D_AD40 + dy].a;
    F_3599("Thou dost find ", D_2046);
    if (a == 2 && D_AD56[D_AD3E][D_AD40].a != 3) {
        F_3B46("a secret door!");
        D_AD56[D_AD3E + dx][D_AD40 + dy].a = 3;
    } else
        F_3B46(D_0220);
}

/* Inform command: names the place you are on or looking at */
F_564B()
{
    int m;

    F_3B46("Inform and search");
    if (D_204A == 3)
        F_55B1();
    else {
        m = F_A3BA(D_3AAC, D_3AAE);
        if (m) {
            if (m < 33)
                F_3599("the city of ", D_2046);
            F_3B46(D_307C[m]);
        } else {
            m = F_A363(D_3AAC, D_3AAE);
            if (m == 0)
                F_3B46("You are at sea");
            else if (m == 2)
                F_3B46("You are in the woods");
            else {
                F_3B46("You are in the lands");
                F_3B46(D_3174[F_40FE(D_3AAC, D_3AAE)]);
            }
        }
    }
}

/* Climb up a dungeon ladder: out of the dungeon on level 0 (hit point bonus) */
F_56F7()
{
    D_AD50--;
    F_3599(" up to level ", D_2046);
    F_3C8B(D_AD50, 1, ' ', D_2046, 1);
    F_3B46(D_019B);
    if (D_AD50 == 0) {
        if (D_AD54 + D_206E[0] > 9999)
            D_AD54 = 9999 - D_206E[0];
        if (D_AD54) {
            F_3599("Thou dost gain ", D_2046);
            F_3C8B(D_AD54, 1, ' ', D_2046, 1);
            F_3599(" hit ", D_2046);
            if (D_AD54 > 999)
                F_3B46(D_019B);
            F_3B46("points");
            D_206E[0] += D_AD54;
            F_4437();
        }
        D_204A = 0;
        F_3C46();
        F_3BF6();
    } else {
        F_4761();
        D_AD56[D_AD3E][D_AD40].b = -1;
    }
}

/* Climb down a dungeon ladder */
F_57DF()
{
    D_AD50++;
    F_3599(" down to level ", D_2046);
    F_3C8B(D_AD50, 1, ' ', D_2046, 1);
    F_3B46(D_019B);
    F_4761();
}

char D_0CDC[] = "K-Limb";
char D_0CE3[] = " what?";
char D_0CEA[] = "Thou must face the ladder!";
/* K-Limb command (dungeon ladders only) */
F_581A()
{
    unsigned char c;

    F_3599(D_0CDC, D_2046);
    if (D_204A != 3) {
        F_3B46(D_0235);
        F_0476(1);
    } else {
        c = D_AD56[D_AD3E][D_AD40].a;
        if (c != 7 && c != 6) {
            F_3B46(D_0CE3);
            F_0476(1);
        } else if (D_AD52 == 2 || D_AD52 == 1) {
            F_3B46(D_019B);
            F_3B46(D_0CEA);
            F_0476(1);
        } else if (c == 7)
            F_56F7();
        else
            F_57DF();
    }
}

/* Noise command: toggle sound */
F_58BD()
{
    F_3599("Noise ", D_2046);
    if (D_3AB0 = !D_3AB0)
        F_3B46("on");
    else
        F_3B46("off");
}

/* put a chest-kept monster/treasure number at (x,y) in the dungeon level */
F_58F8(x, y)
int x, y;
{
    int t, r, n;

    n = 0;
    t = D_AD56[x][y].a;
    if (t == 0 || t == 7 || t == 6) {
        if (D_AD56[x][y].b == -1) {
            do {
                t = F_4069(0, 49) / 10;
            } while (D_B11E[t] && ++n < 50);
            D_B11E[t] = 1;
            r = (D_AD50 - 1) / 2 * 5 + t;
            D_AD56[x][y].b = r;
            D_AD56[x][y].d = F_4069(1, D_AD50 * D_AD50 + 1) + 10 + r;
        }
    }
}

char D_0D13[] = "Open";
char D_0D18[] = " coffin";
char D_0D20[] = " - none here!";
/* Open coffin command (dungeon): monster or pence */
F_59D7()
{
    int x, y;

    x = F_3C8E() + D_AD3E;
    y = F_3CBB() + D_AD40;
    F_3599(D_0D13, D_2046);
    if (D_204A != 3) {
        F_3B46(D_0235);
        F_0476(1);
    } else {
        F_3599(D_0D18, D_2046);
        if (D_AD56[D_AD3E][D_AD40].c != 5) {
            F_3B46(D_0D20);
            F_0476(1);
        } else {
            F_3B46(D_019B);
            if (F_4069(1, 255) < 104 && D_AD56[x][y].a != 1 && D_AD56[x][y].a != 2 && D_AD56[x][y].b == -1)
                F_58F8(x, y);
            else {
                D_AD56[D_AD3E][D_AD40].c = 0;
                F_3599(D_01C7, D_2046);
                F_0761(F_4069(3, D_AD50 * D_AD50 + 9), 0);
            }
        }
    }
}

/* save the game to the player file */
F_5B0E()
{
    F_3599("saving game...", D_2046);
    D_3B22 = D_8BA6;
    D_3B24 = D_8BA8;
    F_3E85(D_0180, (int)D_2058, 820);
    F_3B46("saved.");
}

/* Quit command (outdoors only) */
F_5B54()
{
    F_3599("Quit - ", D_2046);
    if (D_204A != 0) {
        F_3B46("only allowed outdoors!");
        F_0476(1);
    } else
        F_5B0E();
}

/* did the thief get caught? (returns 1 if not caught) */
F_5B8A()
{
    int r, flag;

    flag = 0;
    r = F_4069(1, 255);
    if (D_AD46 || r < 38)
        flag = 1;
    if (!flag && D_206A == 3)
        return 0;
    if (!flag && r > 77)
        return 0;
    F_3B46(D_019B);
    F_3B46("Oh no!  Thou wert caught!");
    return D_AD46 = 1;
}

/* steal a weapon (arg: check for being caught) */
F_5BFA(chk)
int chk;
{
    int i, r;

    r = 0;
    if (chk)
        r = F_5B8A();
    if (r == 0) {
        F_3B46(D_019B);
        F_3599("Thou dost find a", D_2046);
        i = F_4069(1, 15);
        if (F_40A8(D_0D72[i][0]))
            F_357C('n', D_2046);
        if (F_353A(D_0D72[i]) > 8)
            F_3B46(D_019B);
        else
            F_357C(' ', D_2046);
        F_3B46(D_0D72[i]);
        if (D_20B8[i] < 255)
            D_20B8[i]++;
    }
}

/* steal food */
F_5CBA(chk)
int chk;
{
    int n, r;

    r = 0;
    if (chk)
        r = F_5B8A();
    if (r == 0) {
        F_3B46(D_019B);
        F_3599("Thou dost find ", D_2046);
        n = F_4069(2, 31);
        F_3C8B(n, 1, ' ', D_2046, 1);
        F_3B46(" bags of");
        F_3B46("food!");
        D_2080 += n;
        F_44A2();
    }
}

/* steal armour */
F_5D34(chk)
int chk;
{
    int i, r;

    r = 0;
    if (chk)
        r = F_5B8A();
    if (r == 0) {
        F_3B46(D_019B);
        F_3599("Thou dost find ", D_2046);
        i = F_4069(1, 5);
        if (i > 4)
            F_3599("a ", D_2046);
        F_3B46(D_0F5C[i]);
        if (D_20AC[i] < 255)
            D_20AC[i]++;
    }
}

/* Steal command */
F_5DB1()
{
    int c;

    F_3599("Steal", D_2046);
    if (D_204A != 1 && D_204A != 2) {
        F_3B46(D_0235);
        F_0476(1);
    } else {
        c = D_8BAE[D_AD42][D_AD3E][D_AD40];
        switch (c) {
        case 0x37:
            F_5D34(1);
            break;
        case 0x3B:
            F_5BFA(1);
            break;
        case 0x39:
            F_5CBA(1);
            break;
        default:
            F_3B46(" - nothing here!");
            F_0476(1);
        }
    }
}

/* clear the shop message box */
F_5E4F()
{
    F_35CA(39, 48, 280, 120, 0);
}

/* "Sold!" message */
F_5E6C()
{
    F_5E4F();
    F_21E7(18, 9, "Sold!");
    F_2141();
}

/* "Done!" message */
F_5E88()
{
    F_5E4F();
    F_21E7(18, 9, "Done!");
    F_2141();
}

/* "Thou canst not afford it!" message */
F_5EA4()
{
    F_5E4F();
    F_21E7(8, 9, "Thou canst not afford it!");
    F_0476(1);
    F_2141();
}

/* story pages 0-8 (tavern seduction, advice from the king / castle people) */
F_5EC9(n)
int n;
{
    F_5E4F();
    if (n)
        F_21E7(11, 7, "Thou had best know");
    switch (n) {
    case 0:
        F_21E7(9, 8, "Thou hast been seduced!");
        F_21E7(11, 10, "After a long night,");
        F_21E7(13, 11, "thou art back.");
        break;
    case 1:
        F_21E7(10, 8, "about space travel!");
        F_21E7(10, 9, "Thou must destroy at");
        F_21E7(9, 10, "least 20 enemy vessels");
        F_21E7(11, 11, "to become an ace!");
        break;
    case 2:
        F_21E7(11, 8, "to watch the ");
        if (D_206C == 0)
            F_3599("wench.", D_2046);
        else
            F_3599("lecher.", D_2046);
        break;
    case 3:
        F_21E7(5, 8, " that the princess will give");
        F_21E7(5, 9, " great reward to the one who");
        F_21E7(5, 10, "rescues her, and an extra gift");
        F_21E7(10, 11, "to an 8th level ace!");
        break;
    case 4:
        F_21E7(7, 8, "thou must go back in time.");
        break;
    case 5:
        F_21E7(10, 8, "thou should destroy");
        F_21E7(13, 9, "the evil gem!");
        break;
    case 6:
        F_21E7(5, 8, "   that many lakes and ponds");
        F_21E7(5, 9, "  have strong magical powers!");
        break;
    case 7:
        F_21E7(10, 8, "this is a great game!");
        break;
    case 8:
        F_21E7(5, 8, "   that over 1000 years ago,");
        F_21E7(5, 9, "Mondain the Wizard created an");
        F_21E7(5, 10, " evil gem.  With this gem, he");
        F_21E7(5, 11, "   is immortal and cannot be");
        F_21E7(15, 12, "defeated.");
        while (!F_413C(0))
            ;
        F_5E4F();
        F_21E7(5, 7, "The quest of --Ultima-- is to");
        F_21E7(5, 8, " traverse the lands in search");
        F_21E7(8, 9, "of a time machine.  Upon");
        F_21E7(5, 10, " finding such a device, thou");
        F_21E7(5, 11, "should go back in time to the");
        F_21E7(5, 12, " days before Mondain created");
        F_21E7(5, 13, "the evil gem and destroy him!");
        while (!F_413C(0))
            ;
    }
    if (n != 8)
        F_2141();
}

/* is the cell at offset (dx,dy) from the player a type-50 entity? */
F_616F(dx, dy)
int dx, dy;
{
    return F_2355(D_AD3E + dx, D_AD40 + dy) == 50;
}

/* is a type-50 entity adjacent (8 directions)? */
F_619C()
{
    return F_616F(1, 1) || F_616F(1, 0) || F_616F(1, -1) || F_616F(0, 1) ||
           F_616F(0, -1) || F_616F(-1, 1) || F_616F(-1, 0) || F_616F(-1, -1);
}

/* tavern: order ale (arg 'B') or refuse */
F_622C(a)
char a;
{
    int r;

    r = F_32F8(D_AD42 - 2);
    if (a == 'B') {
        if (D_207C == 0) {
            F_21E7(12, r + 1, "Thou art broke!");
            F_21E7(8, r + 4, "Come back when thou hast");
            F_21E7(10, r + 5, "some money to spend.");
            F_2141();
            F_352C(15, 24);
            F_3599(D_0220, D_2046);
        } else {
            D_207C--;
            F_4558();
            F_21E7(8, r + 1, "The tavern keeper sayeth:");
            F_21E7(9, r + 4, "Here, have a cold one!");
            F_352C(15, 24);
            F_3599("ale", D_2046);
            D_AD48++;
            F_2141();
            if (D_AD48 > D_3A94 / 4 && F_619C()) {
                F_5EC9(0);
                D_207C = D_207C / 2;
                F_4558();
                D_3A98--;
                if (D_3A98 == 4)
                    D_3A98++;
            }
            if (F_4069(1, 255) < 75)
                F_5EC9(F_4069(11, 89) / 10);
        }
    } else {
        F_21E7(8, r + 2, "We have plenty of booze");
        F_21E7(17, r + 3, "already!");
        F_2141();
        F_352C(16, 24);
        F_3599(D_0220, D_2046);
    }
    F_3B46(D_019B);
}

/* armour/weapon buying price for item n (charisma D_3A9A) */
F_63B3(n)
int n;
{
    return (255 - D_3A9A) * n * n / 256 + 5;
}

/* selling price for item n (D_3A96) */
F_63D4(n)
int n;
{
    return (D_3A96 + 40) * n * n / 256 + 1;
}
