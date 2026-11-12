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
extern int D_5E1A[], D_5E6A[], D_5EBA[], D_5F0A[];

extern F_3B46(), F_0411(), F_0910(), F_0F25(), F_095F(), F_3109();
extern F_35CA(), F_39FC(), F_3D41();
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
                D_0128 = D_0128 < 9 ? D_0128 : 9;
            else
                D_0128 = D_0128 > -9 ? D_0128 : -9;
            if (D_012A > -1)
                D_012A = D_012A < 9 ? D_012A : 9;
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
