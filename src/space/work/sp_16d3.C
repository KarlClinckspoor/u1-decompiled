/* opts: -ms -v -w -cs */
/*
 * SPACE.EXE game code 16D3-2B7B: space flight and space combat.
 */

/* a sector of the 7x7 galaxy: 0x3A bytes */
struct obj { int x, y, type, f6, f8, dir; };
struct sector { int count, f2, f4, f6, f8; struct obj o[4]; };

extern struct sector D_26C0[7][7];
extern int D_01EE, D_01F0;              /* current sector */
extern int D_5E14;                      /* current ship */
extern int D_0126, D_0128, D_012A, D_012C;
extern int D_0134, D_0136, D_0138, D_013A, D_013C;
extern int D_01F2;
extern char *D_01C4[], *D_01E4[];
extern int D_0BD2[][4];
extern int D_0DA6, D_0DA8, D_2044, D_2046, D_5AD2;
extern int D_5FFA, D_5FFE;
extern int D_5EBA[], D_5F0A[];
extern int D_5E1A[], D_5E6A[];

extern F_3B46(), F_0411(), F_0910(), F_0F25(), F_095F(), F_3109();
extern F_35CA(), F_39FC(), F_3D41(), F_3DD6(), F_254B(), F_3599(), F_3EC4(), F_37FD();
extern F_68F8(), F_0EBD(), F_614E(), F_450D(), F_0524(), F_0425(), F_04CA(), F_603E();
extern int D_5B8C, D_5B8E;
extern int D_0C9A, D_01F6[], D_0204[], D_0212[], D_0220[], D_207E, D_20FE, D_082C;
extern F_3330(), F_3CF1(), F_4224(), F_352C(), F_0FBC(), F_0FE1(), F_3E5E(), F_0F3B(), F_259E();
extern int D_0C9C, D_0DE2, D_0C92, D_0C94, D_0C96, D_0C98, D_0C9E[];
extern int F_092D(), F_3741(), F_088E(), F_4069();

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
