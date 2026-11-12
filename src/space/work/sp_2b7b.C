/* opts: -ms -v -w -cs */
/*
 * SPACE.EXE game code 2B7B-3DED: ship drawing, alien ship, docking, main loop.
 */
struct obj { int x, y, type, f6, f8, dir; };
struct sector { int count, f2, f4, f6, f8; struct obj o[4]; };

extern struct sector D_26C0[7][7];
extern int D_01EE, D_01F0, D_01F2, D_5E14;
extern int D_0126, D_0128, D_012A, D_012C, D_012E;
extern int D_0134, D_013A, D_013C;
extern int D_0C92, D_0C94, D_0C96, D_0C98, D_0C9A, D_0C9C, D_0C9E[];
extern int D_0DE0, D_0DE2, D_0DA6, D_0DA8;
extern int D_01F6[], D_0204[];
extern char D_2058[];
extern int D_2044, D_2046, D_2048, D_5AD2, D_5B14, D_5B0E, D_5B8C, D_5B8E;
extern int D_5E10, D_5E12, D_5FFE, D_6000, D_082C, D_2086, D_207C;
extern char *D_0E18[], *D_0166[], *D_0194[];
extern char D_2C5C[], D_4C67[];


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
extern unsigned char F_413C();

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

/* alien ship turn: move it toward the crosshair, redraw, fire */
/* NONMATCHING: dx/dy computed as `mov ax,[D_013A]; sub ax,[D_0C92]`; original keeps D_0C92/D_0C94 cached in AX/BX
   (`mov cx,[D_013A]; sub cx,ax; mov ax,[D_013C]; sub ax,bx`): 2 bytes shorter. Rest identical. */
F_3330()
{
    int r, dx, dy;

    if (D_012C)
        D_012C--;
    D_0DE0 = 0;
    F_1C2A();
    F_3CF1(D_0C9E[D_0126]);
    if (D_26C0[D_01EE][D_01F0].count != 0) {
        D_0C96 = D_0C92;
        D_0C98 = D_0C94;
        dx = D_013A - D_0C96;
        dy = D_013C - D_0C98;
        D_0C92 -= F_1BBF(dx);
        D_0C94 -= F_1BBF(dy);
        if (D_0C92 > 280 || D_0C92 < 30 || D_0C94 > 125 || D_0C94 < 24) {
            if (D_0C96)
                F_614E(D_0C96, D_0C98, D_0C9A, 0);
            F_254B();
        } else {
            if (D_0C96)
                F_614E(D_0C96, D_0C98, D_0C9A, 0);
            if (D_0C9C && D_0DE2 >= 8 - D_0126 || D_0DE2 > 15) {
                r = F_4069(1, (D_0126 + 3) * 10);
                if (r > 20 || D_0C9C)
                    D_0C9A++;
                if (D_0C9A > 6)
                    F_254B();
                D_0DE2 = 0;
            } else
                D_0DE2++;
        }
        if (D_0C92) {
            F_614E(D_0C92, D_0C94, D_0C9A, 1);
            F_31D7();
            if (((dx) < 0 ? -(dx) : (dx)) < 4 || ((dy) < 0 ? -(dy) : (dy)) < 4)
                F_1BE9(D_0DA6);
        }
    }
}

/* add speed D_0128 to x, wrapping at the screen edges 20..275 */
F_34A8(v)
int v;
{
    v += D_0128;
    if (v > 275)
        return 20;
    if (v < 20)
        return 275;
    return v;
}

/* add speed D_012A to y, wrapping 10..130 */
F_34D9(v)
int v;
{
    v += D_012A;
    if (v > 130)
        return 10;
    if (v < 10)
        return 130;
    return v;
}

/* collision: crunch, lose shield, bounce (reverse speeds) */
F_350A()
{
    F_2665();
    F_2731();
    F_3B46("Crunch!");
    F_387E(0, 24);
    F_0411(0);
    F_08C0(F_088E() / 4 + 5);
    if (F_088E() == 0) {
        F_3B46("Thy shield is drained!");
        F_0F25();
    } else {
        D_0128 = -D_0128;
        D_012A = -D_012A;
    }
}

/* dock at a base: pay 500p, choose a ship (1..4) at this sector */
F_3567()
{
    struct sector *s;
    struct obj *o;
    int i, n, done;
    unsigned char c;

    done = n = 0;
    s = &D_26C0[D_01EE][D_01F0];
    for (i = 0; i < 4; i++)
        if (s->o[i].type != -1)
            n++;
    o = &s->o[D_5E14];
    F_3B46("Docked!  Welcome to base!");
    D_0128 = D_012A = 0;
    if (D_2086 < 4) {
        F_3B46("The spacedoors open, exposing");
        F_3B46("thee to a complete vacuum!");
        F_0F25();
    }
    if (D_207C < 500) {
        F_3B46("Sorry, but thou canst not");
        F_3B46("refuel thy ship without");
        F_3599("the 500p docking fee.", D_2046);
        c = F_4161();
        F_3B46("");
    } else {
        D_207C -= 500;
        o->f6 = F_0774(o->type);
        o->f8 = F_07AB(o->type);
        if (n > 1)
            do {
                F_3599("Choose thy ship: ", D_2046);
                c = F_4161();
                F_3B46("");
                if (c >= 1 && c <= 4) {
                    c--;
                    i = F_37E0(c);
                    if (i == -1)
                        F_3B46("That port is not available.");
                    else {
                        D_6000 = D_26C0[D_01EE][D_01F0].o[i].dir;
                        D_5E14 = i;
                        done = 1;
                        F_3599("Thou art in the ", D_2046);
                        F_3599(D_0E18[c], D_2046);
                        F_3B46(" ship.");
                    }
                }
            } while (done == 0);
        F_0682();
        F_06FB();
        F_4558();
    }
    F_387E(0, 24);
    F_352C(1, 24);
}

/* index (0..3) of the docking slot object k sits in, else -1 */
F_3741(k)
int k;
{
    int i;
    struct obj *o;

    o = &D_26C0[D_01EE][D_01F0].o[k];
    for (i = 0; i < 4; i++)
        if (o->x == F_07E2(i, D_01EE, D_01F0, k) && o->y == F_0838(i, D_01EE, D_01F0, k) && o->dir == F_063A(i))
            break;
    if (i == 4)
        i = -1;
    return i;
}

/* find a free docking slot in the sector for ship k (-1 if none) */
F_37E0(k)
int k;
{
    int i;
    struct obj *o;

    for (i = 0; i < 4; i++) {
        o = &D_26C0[D_01EE][D_01F0].o[i];
        if (o->x == F_07E2(k, D_01EE, D_01F0, i) && o->y == F_0838(k, D_01EE, D_01F0, i))
            break;
    }
    if (i == 4)
        i = -1;
    return i;
}

/* one game turn: combat or fly the player ship, move it, check collisions, dock */
F_386B()
{
    int d, r;
    struct sector *s;
    struct obj *o;
    int moved;

    s = &D_26C0[D_01EE][D_01F0];
    if (D_01F2 == 1)
        F_3330();
    else {
        o = &s->o[D_5E14];
        d = o->dir;
        moved = D_6000 != d || D_0128 || D_012A;
        if (moved || D_012C) {
            if (moved)
                r = F_603E(o->type, D_6000, o->x, o->y);
            if (D_012C)
                F_3109(o, 0);
            if (moved) {
                o->x = F_34A8(o->x);
                o->y = F_34D9(o->y);
                r = F_603E(o->type, d, o->x, o->y);
                D_6000 = d;
                if (r)
                    F_350A();
                else if (F_3741(D_5E14) != -1) {
                    F_3567();
                    D_012C = 0;
                }
            }
            if (D_012C)
                F_3109(o, 1);
        }
    }
}

/* toggle Noise on/off */
F_3983()
{
    F_3599("Noise ", D_2046);
    D_5B14 = !D_5B14;
    if (D_5B14)
        F_3B46("on");
    else
        F_3B46("off");
}

/* prompt for the Ultima I diskette and wait for a space */
F_125C()
{
    F_3EED();
    F_352C(1, 23);
    F_3599("Insert Ultima I diskette", D_2046);
    F_352C(1, 24);
    F_3599("Hit space to continue...", D_2046);
    while (F_39C5(D_2046) != 32)
        ;
    F_352C(1, 23);
    F_3599("                        ", D_2046);
    F_352C(1, 24);
    F_3599("                        ", D_2046);
    F_352C(1, 23);
}

/* lift-off countdown: wait up to 10 beeps/keypress, then "Thou hast lifted off!" */
F_3A4E()
{
    int i, key;

    i = 10;
    key = 0;
    F_352C(1, 24);
    do {
        F_3C8B(i, 1, 32, D_2046, 1);
        if (i != 1)
            F_3599("..", D_2046);
        if (key == 0)
            key = F_413C(0);
        if (key == 0)
            F_3E5E(18);
        i--;
    } while (i != 0 && key == 0);
    F_3B46("");
    F_3B46("Thou hast lifted off!");
}

/* main(argc, argv): load data, set up the flight screen, key loop */
F_1853(argc, argv)
int argc;
unsigned char **argv;
{
    unsigned char c;
    int n;

    F_1D0D(1);
    D_2048 = F_3862(argv[1][0]) == 67;
    D_5E10 = argv[1];
    D_5E12 = argv[2];
    F_3F58(0);
    F_3E2F("inuse.u1", (int)D_2058, 820);
    F_3E2F(D_0166[D_082C], D_2C5C + 1406, 7296);
    F_3E2F(D_0194[D_082C], D_4C67 + 499, 3192);
    F_3A4E();
    if (D_082C == 0)
        D_5AD2 = D_0DA8;
    else
        D_5AD2 = D_2044;
    F_352C(31, 21);
    F_3599("Shld:", D_2046);
    F_352C(31, 22);
    F_3599("Fuel:", D_2046);
    D_5FFE = 2;
    F_37FD(D_5B0E * 2);
    F_0D88();
    F_37FD(D_5B0E + F_3EC4());
    F_1DDA();
    F_259E();
    F_0682();
    F_06FB();
    for (;;) {
        if (F_088E() == 0) {
            D_5B8E = D_5B8C = 0;
            F_0682();
            F_06FB();
            F_0F25();
        }
        n = 0;
        F_387E(0, 24);
        do {
            F_386B();
            c = F_3862(F_413C(D_2046));
            n++;
            if (D_01F2 == 0)
                F_3E5E(2);
        } while (c == 0 && n < 1500);
        if (c == 0)
            c = 32;
        if (c >= 49 && c <= 56 && D_01F2 == 1)
            F_165C(c - 48);
        else
            switch (c) {
            case 3:
            case 4:
            case 2:
            case 1:
                F_1919(c);
                break;
            case 65:
                F_19D3();
                break;
            case 66:
                F_19E1();
                break;
            case 67:
                F_19EF();
                break;
            case 68:
                F_19FD();
                break;
            case 69:
                F_1A0B();
                break;
            case 70:
                F_163C();
                break;
            case 71:
                F_1A19();
                break;
            case 72:
                F_2003();
                break;
            case 73:
                F_14E5();
                D_012E = D_0134 = 1;
                break;
            case 75:
                F_1A27();
                break;
            case 78:
                F_3983();
                break;
            case 79:
                F_1A35();
                break;
            case 81:
                F_1A43();
                break;
            case 82:
                F_1A51();
                break;
            case 83:
                F_1A5F();
                break;
            case 84:
                F_1A6D();
                break;
            case 85:
                F_1A7B();
                break;
            case 86:
                F_10B3();
                break;
            case 88:
                F_1A89();
                break;
            case 90:
                F_478E();
                D_012E = D_0134 = 1;
                F_387E(0, 24);
                break;
            case 32:
                F_1A97();
                break;
            case 27:
                (&D_01F2)[1] = 1;
            default:
                F_0F0E();
            }
        if (D_0134)
            F_259E();
        if (D_012E) {
            D_012E = 0;
            if (D_012C && D_01F2 == 0)
                D_0DE0 = !D_0DE0;
        }
    }
}

/* abnormal termination: print message and exit(3) */
F_3DD6()
{
    PUTS("\nAbnormal program termination\n");
    _EXIT(3);
}
