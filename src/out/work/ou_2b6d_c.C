/* opts: -ms -v -w -cs */
/*
 * OUT.EXE game code 33B0-3722 (work file, agent ou_2b6d): dungeon wall lines.
 */
extern F_39FC(), F_AC91(), F_3812(), F_3B46(), F_3599(), F_387E(), F_3920(), F_352C();
extern F_33B0(), F_3430(), F_34B0(), F_355A(), F_35DC(), F_365E(), F_368F(), F_36C0(), F_36F1();
extern F_3722(), F_37DB(), F_390D(), F_3A54(), F_3B25(), F_3B65(), F_3BA5(), F_3BF5(), F_3C13();
extern F_3D6C(), F_3430(), F_390D();
#define ABS(x) ((x) < 0 ? -(x) : (x))
extern int D_0DA6, D_2044, D_2046, D_AD52, D_AD3E, D_AD40;
extern char *D_3930[];
struct cell { int a, b, c, d; };
extern struct cell D_AD56[][11];
extern int D_3808[], D_3814[];

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
    F_3B46("Coffin");
    F_AC91(25, x, D_0DA6);
}

/* chest at depth x */
F_3C13(x)
int x;
{
    F_3B46("Chest");
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
