/* opts: -ms -v -w -cs */
/*
 * MONDAIN.EXE - the final battle against Mondain (game code 03E9-2230).
 * Builds byte-identical with Lattice C 3.10 (lc -ms -v -w -cs).
 * Names are still the addresses they had in the original (F_xxxx code, D_xxxx DGROUP data).
 */

/* Origin support routines and their data */
extern int D_082C;
extern int D_0CD2[];
extern char *D_0DF8[], *D_0F04[], *D_1DC8[];
extern F_39FC(), F_3812(), F_352C(), F_35CA(), F_3599(), F_3F62(), F_3B46();
extern F_3E5E(), F_355D(), F_4031(), F_3C8B(), F_4437(), F_3FF9();
extern F_3E85(), F_44A2(), F_3EED(), F_357C(), F_3AE2(), F_3CF1(), F_3D41();
extern F_3E2F(), F_3EC4(), F_3F58(), F_37FD(), F_387E(), F_4224(), F_43C8();
extern F_45A3(), F_478E();
extern int F_4069();
extern char F_3862(), F_39C5();
extern unsigned char F_413C();
/* Lattice library */
extern EXECL(), F_1D0D();

/*
 * Initialised data (DGROUP 0124-0256)
 */
int _stack = 3000;                      /* stack size */
int D_0126 = 3, D_0128 = 4;             /* player position */
int D_012A = 15, D_012C = 4;            /* Mondain position */
int D_012E = 1000;                      /* Mondain hit points */
int D_0130 = -1;                        /* frame of the spell effect */
char *D_0156[3] = { "CgaMond.Bin", "EgaMond.Bin", "T1KMond.Bin" };
char *D_0184[3] = { "CgaTiles.Bin", "EgaTiles.Bin", "T1KTiles.Bin" };
int D_018A[3] = { 0x2c0, 0x580, 0x580 };
int D_0190 = 0, D_0192 = 0;             /* bouncing gem */
unsigned char D_0194[19][9] = {         /* the battle field */
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 20, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 5, 5, 5, 0, 5, 5, 5, 0},
    {0, 5, 0, 0, 0, 0, 0, 5, 0},
    {0, 5, 0, 0, 0, 0, 0, 5, 0},
    {0, 0, 0, 0, 6, 0, 0, 0, 0},
    {0, 5, 0, 0, 0, 0, 0, 5, 0},
    {0, 5, 0, 0, 0, 0, 0, 5, 0},
    {0, 5, 5, 5, 0, 5, 5, 5, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0}
};
int D_0240 = 2, D_0242 = 0, D_0244 = 20;        /* Mondain's tile, frame, state */
int D_0246 = 0, D_0248 = 0, D_024A = 0;         /* fight on, gem destroyed, victory */

/*
 * Uninitialised data (DGROUP 2040-2E10 + 1A90)
 */
int D_0DA6, D_0DA8;                     /* colours */
int D_2044, D_2046, D_2048, D_204A, D_204C;
int D_204E, U_2050, U_2052;
int D_2054, D_2056;
char D_2058[16];
int D_2068, D_206A, D_206C;
int D_206E, D_2070, D_2072, D_2074, U_2076, D_2078, D_207A, D_207C, D_207E;
int D_2080, D_2082, D_2084, D_2086, D_2088;
int D_208A, U_208C, U_208E;
int D_2090, U_2092[9];
char D_20A4[8];
char D_20AC[12];
char D_20B8[32];
int D_20D8[11];
char D_20EE[16];
char D_20FE[0x28e];
char D_238C[0x78d];
char D_2B19[0x82];
char D_2B9B[0x45];
char D_2BE0[0x22c];
char *D_2E0C, *D_2E0E;
unsigned char D_2E10[0x1a90];

/* wait about a while, or until a key is pressed */
F_03E9() { long n; for (n = 0; !F_413C(0) && ++n < 0x124F8; ) ; }

/* draw the frame of the big window in colour c and home the text cursor */
F_0423(c) int c; { F_39FC(0x19, 0x12, 0x129, 0x12, c); F_3812(0x129, 0x8c, c); F_3812(0x19, 0x8c, c); F_3812(0x19, 0x12, c); F_352C(4, 3); }

/* clear the big window and frame it */
F_047D(c) int c; { F_35CA(0x16, 0xf, 0x12c, 0x8f, 0); F_0423(c); }

/* print string s at text position (a, b) */
F_04A2(a, b, s) int a, b, s; { F_352C(a, b); F_3599(s, D_2046); }

/* draw the frame of the smaller window in colour c */
F_04BE(c)
int c;
{
    F_39FC(0x22, 0x1a, 0x11c, 0x1a, c);
    F_3812(0x11c, 0x7c, c);
    F_3812(0x22, 0x7c, c);
    F_3812(0x22, 0x1a, c);
    F_352C(5, 4);
}

/* clear the smaller window and frame it */
F_0518(c)
int c;
{
    F_35CA(0x1f, 0x17, 0x11f, 0x7f, 0);
    F_04BE(c);
}

/* play sound n if sound is on */
F_053D(n)
int n;
{
    if (D_2090)
        F_3F62(n);
}

/* "Pass" */
F_0551()
{
    F_3B46("Pass");
}

/* "Huh?" and a beep */
F_055F()
{
    F_3B46("Huh?");
    F_053D(1);
}

/* toggle the sound */
F_0576()
{
    F_3599("Noise ", D_2046);
    if (D_2090 = !D_2090)
        F_3B46("on");
    else
        F_3B46("off");
}

/* sign of x */
F_05B1(x)
int x;
{
    if (x < 0)
        return -1;
    if (x > 0)
        return 1;
    return 0;
}

/* wait for a key, animating meanwhile */
unsigned char F_05DB()
{
    char c, k;

    do {
        c = F_413C(D_2046);
        F_1477();
        if (c)
            F_3E5E(1);
        else
            F_3E5E(3);
    } while (c == 0);
    return c;
}

/* random check against the player's agility (or D_206A == 1) */
F_0618()
{
    return F_4069(1, 0xf9) < D_207A + 0xa0 || D_206A == 1;
}

#define abs(x) ((x) < 0 ? -(x) : (x))

/* player within 7 steps of Mondain */
F_064A()
{
    return abs(D_0126 - D_012A) + abs(D_0128 - D_012C) < 7;
}

/* move the player one step in direction dir */
F_0696(dir)
unsigned char dir;
{
    int t, x, y;

    x = D_0126 + F_1415(dir);
    y = D_0128 + F_1447(dir);
    if (x < 0 || x >= 19 || y < 0 || y >= 9)
        t = 2;
    else if (x == D_012A && y == D_012C)
        t = 2;
    else
        t = D_0194[x][y];
    if (t == 0) {
        F_3B46(D_1DC8[dir]);
        F_355D(0, D_0126, D_0128);
        D_0194[D_0126][D_0128] = 0;
        D_0126 += F_1415(dir);
        D_0128 += F_1447(dir);
        D_0194[D_0126][D_0128] = 0x14;
        F_355D(0x14, D_0126, D_0128);
        F_053D(4);
    } else if (t == 1) {
        F_3B46("Blocked!");
        F_053D(0);
    } else {
        F_053D(0);
        F_4031();
        F_3599("Burned! ", D_2046);
        t = D_206E / 10;
        if (t == 0)
            t++;
        F_3C8B(t, 1, 0x20, D_2046, 1);
        F_3B46(" damage!");
        F_053D(2);
        D_206E -= t;
        F_4437();
        F_3FF9();
    }
    if (F_0FC7() && D_0190 == 0)
        D_0246 = D_0190 = 1;
}

/* fire a shot in direction dir: range, damage and hit chance given */
F_0831(dir, snd, range, damage, chance)
int dir, snd, range, damage, chance;
{
    int t, dx, dy, x, y, i;

    dy = dx = 0;
    i = 1;
    F_053D(snd);
    dx = F_1415(dir);
    dy = F_1447(dir);
    do {
        x = dx * i + D_0126;
        y = i * dy + D_0128;
        if ((t = D_0194[x][y]) == 0)
            if (x == D_012A && y == D_012C)
                t = 2;
        i++;
    } while (i <= range && t != 2 && x > 0 && y > 0 && x < 19 && y < 9);
    if (t != 2)
        F_3B46("Missed!");
    else if (F_4069(1, 100) <= chance) {
        D_012E -= damage;
        F_3599("Hit Mondain! ", D_2046);
        F_3C8B(damage, 1, 0x20, D_2046, 1);
        F_3B46(" damage!");
        F_053D(2);
    } else
        F_3B46("Missed!");
}

/* attack in direction dir with weapon w (7: ranged) or spell */
F_0944(dir, w)
int dir, w;
{
    int damage, chance, range;

    if (w == 7) {
        range = D_0CD2[D_2082];
        chance = D_2072 + 50;
        damage = F_4069(2, D_2070 + D_2082 * 3);
    } else {
        range = 3;
        chance = 0x50;
        damage = F_4069(1, D_2088 * 10) + 30;
    }
    F_0831(dir, w, range, damage, chance);
}

/* aim: read a direction (or use dir), say it, and attack with weapon w */
F_09C0(dir, w)
int dir, w;
{
    int d;

    if (dir == -1)
        d = F_05DB();
    else
        d = dir;
    if (d > 4)
        F_3B46("nothing");
    else {
        F_3B46(D_1DC8[d]);
        F_0944(d, w);
    }
}

/* Attack: choose weapon, ask for a direction */
F_0A0D(dir)
int dir;
{
    D_0246 = 1;
    F_3599("Attack with ", D_2046);
    F_3599(D_0DF8[D_2082], D_2046);
    if (D_0CD2[D_2082] == 0) {
        F_3B46("?");
        F_053D(1);
    } else {
        F_3599(": ", D_2046);
        F_09C0(dir, 7);
    }
}

/* damage of the spell: random up to D_207A, scaled by the spell */
F_0A7A()
{
    int t;

    t = F_4069(2, D_207A);
    if (D_2082 == 9)
        t <<= 1;
    else if (D_2082 == 8) {
        t *= 3;
        t /= 2;
    } else if (D_2082 == 10 || D_2082 == 11)
        t *= 3;
    return t;
}

/* spell with no effect (D_0618 test) */
F_0AD9()
{
    F_3B46("");
    if (F_0618) {
        F_3B46("Hmmm...no effect!");
        F_053D(6);
        return 1;
    }
    return 0;
}

/* spell: ask a direction, put a block (5) in the empty square there */
F_0B07()
{
    unsigned char c;
    int x, y;

    F_3599(": ", D_2046);
    c = F_05DB();
    if (c > 4) {
        F_3B46("nothing");
        F_053D(6);
        return 1;
    }
    F_3B46(D_1DC8[c]);
    if (!F_0618())
        return 0;
    x = D_0126 + F_1415(c);
    y = D_0128 + F_1447(c);
    if (D_0194[x][y] == 0 && x != D_012A && y != D_012C) {
        D_0194[x][y] = 5;
        F_355D(5, x, y);
        F_053D(5);
        F_3B46("Done.");
        return 1;
    }
    return 0;
}

/* spell: ask a direction, remove a block (5) from the square there */
F_0BE4()
{
    unsigned char c;
    int x, y;

    F_3599(": ", D_2046);
    c = F_05DB();
    if (c > 4) {
        F_3B46("nothing");
        F_053D(6);
        return 1;
    }
    F_3B46(D_1DC8[c]);
    if (!F_0618())
        return 0;
    x = D_0126 + F_1415(c);
    y = D_0128 + F_1447(c);
    if (D_0194[x][y] == 5) {
        D_0194[x][y] = 0;
        F_355D(0, x, y);
        F_053D(5);
        F_3B46("Done.");
        return 1;
    }
    return 0;
}

/* spell: strengthen Mondain (doubles his hit points below 15000) */
F_0CAC()
{
    F_3B46("");
    if (F_0618() && F_064A()) {
        F_3B46("\"INTERFICIO-NUNC!\"");
        F_3B46("The spell doth seem");
        F_3B46("to make him stronger!");
        if (D_012E < 15000)
            D_012E <<= 1;
        F_053D(5);
        return 1;
    }
    return 0;
}

/* spell: ask a direction and fire a bolt of spell damage */
F_0D08()
{
    unsigned char c;
    int t;

    F_3599(": ", D_2046);
    c = F_05DB();
    if (c > 4) {
        F_3B46("nothing");
        F_053D(6);
        return 0;
    }
    F_3B46(D_1DC8[c]);
    if (F_0618() && F_064A()) {
        t = F_0A7A();
        F_0831(c, 5, 5, t, 500);
        return 1;
    }
    return 0;
}

/* spell: teleport the player to a random empty square */
F_0D88()
{
    int x, y, r;

    r = 0;
    F_3B46("");
    if (F_0618()) {
        do {
            x = F_4069(1, 255) % 19;
            y = F_4069(1, 255) % 9;
        } while (D_0194[x][y] != 0);
        F_355D(0, D_0126, D_0128);
        F_355D(0x14, x, y);
        D_0194[x][y] = 0x14;
        D_0194[D_0126][D_0128] = 0;
        D_0126 = x;
        D_0128 = y;
        r = 1;
        F_053D(5);
        F_3B46("Done.");
    }
    return r;
}

/* cast spell number w */
F_0E56(w)
int w;
{
    int t;

    t = 0;
    switch (w) {
    case 0:
        t = F_0AD9();
        break;
    case 8:
        t = F_0B07();
        break;
    case 9:
        t = F_0BE4();
        break;
    case 10:
        t = F_0CAC();
        break;
    case 7:
        t = F_0D88();
        break;
    default:
        t = F_0D08();
    }
    if (t == 0) {
        F_053D(6);
        F_3B46("Failed!");
    }
}

/* Cast: use up a charge of spell D_2084 and cast it if it is a usable one */
F_0ED3()
{
    F_3599("Cast ", D_2046);
    F_3599(D_0F04[D_2084], D_2046);
    if (D_20D8[D_2084] == 0) {
        F_3B46("");
        F_3B46("You've used up that spell!");
        F_053D(6);
    } else {
        if (D_2084)
            D_20D8[D_2084]--;
        if (D_2084 != 3 && D_2084 != 7 && D_2084 != 8 && D_2084 != 9 && D_2084 != 10) {
            F_3B46("");
            F_3B46("Failed!");
            F_053D(6);
        } else
            F_0E56(D_2084);
    }
}

/* is the square (player + (dx, dy)) inside the field and of kind 6 (the gem) */
F_0F7B(dx, dy)
int dx, dy;
{
    int x, y;

    x = D_0126 + dx;
    y = D_0128 + dy;
    if (x > 0 && x < 19 && y > 0 && y < 9 && D_0194[x][y] == 6)
        return 1;
    return 0;
}

/* is the gem on any of the eight squares around the player */
F_0FC7()
{
    return F_0F7B(1, 1) || F_0F7B(1, 0) || F_0F7B(0, 1) || F_0F7B(-1, -1)
        || F_0F7B(-1, 0) || F_0F7B(0, -1) || F_0F7B(1, -1) || F_0F7B(-1, 1);
}

/* Get (Gem): take the gem next to the player and destroy it (hurts the player) */
F_1057()
{
    int t, i;

    F_3599("Get (Gem)", D_2046);
    if (D_0248 != 0 || !F_0FC7()) {
        F_3B46("..'tis nothing here!");
        F_053D(1);
    } else {
        D_0248 = D_204E = D_0246 = 1;
        D_0194[14][4] = 0;
        D_0190 = 0;
        t = D_206E * 3;
        t /= 4;
        F_3599("... ", D_2046);
        F_3C8B(t, 1, 0x20, D_2046, 1);
        F_3B46(" damage!");
        for (i = 0; i < 5; i++)
            F_053D(2);
        F_3B46("The Gem is DESTROYED!");
        D_206E -= t;
        F_4437();
        if (D_0244 == 30)
            D_024A = 1;
    }
}

/* Inform and Search: report on the gem */
F_1124()
{
    F_3599("Inform and Search", D_2046);
    if (D_0248) {
        F_3B46("");
        F_3B46("Mondain's magical aura doth");
        F_3B46("seem substantially diminished");
        F_3B46("in the absence of the gem.");
    } else {
        F_3B46("...it looks");
        F_3B46("as if he is creating the evil");
        F_3B46("gem!");
    }
}

/* print message s and beep */
F_117E(s)
char *s;
{
    F_3B46(s);
    F_053D(1);
}

/* "Board?" */
F_1194() { F_117E("Board?"); }
/* "Drop?" */
F_11A2() { F_117E("Drop?"); }
/* "Enter?" */
F_11B0() { F_117E("Enter?"); }
/* "Fire?" */
F_11BE() { F_117E("Fire?"); }
/* "Hyperjump?" */
F_11CC() { F_117E("Hyperjump?"); }
/* "Klimb?" */
F_11DA() { F_117E("Klimb?"); }
/* "Open?" */
F_11E8() { F_117E("Open?"); }
/* "Quit...is not allowed!" */
F_11F6() { F_117E("Quit...is not allowed!"); }

/* Steal: refused */
F_1204()
{
    F_3B46("Steal...he's watching,");
    F_117E("That would be foolish!");
}

/* Transact: refused */
F_121B()
{
    F_3B46("Transact...Mondain will");
    F_117E("not negotiate!");
}

/* "Unlock?" */
F_1232() { F_117E("Unlock?"); }
/* "View?" */
F_1240() { F_117E("View?"); }
/* "Xit?" */
F_124E() { F_117E("Xit?"); }

/* prompt for the Ultima I diskette and wait for a space */
F_125C()
{
    F_3EED();
    F_352C(1, 0x17);
    F_3599("Insert Ultima I diskette", D_2046);
    F_352C(1, 0x18);
    F_3599("Hit space to continue...", D_2046);
    while (F_39C5(D_2046) != 0x20)
        ;
    F_352C(1, 0x17);
    F_3599("                        ", D_2046);
    F_352C(1, 0x18);
    F_3599("                        ", D_2046);
    F_352C(1, 0x17);
}

/* "THOU ART DEAD!": defeat message, save inuse.u1, back to OUT.EXE */
F_12EC()
{
    int r;

    F_3B46("THOU ART DEAD!");
    F_0518(D_0DA8);
    F_352C(5, 6);
    F_3599("   Thou hast been defeated by", D_2046);
    F_352C(5, 7);
    F_3599("      Mondain the Wizard!", D_2046);
    F_352C(5, 12);
    F_3599("    THE UNIVERSE IS DOOMED!", D_2046);
    F_3E5E(50);
    F_3E85("inuse.u1", (int)D_2058, 0x334);
    for (;;) {
        r = EXECL("out.exe", "out.exe", D_2E0C, D_2E0E, "M", 0);
        F_125C();
    }
}

/* the player dies: hit points and food to 0, then defeat */
F_138D()
{
    D_206E = 0;
    D_2080 = 0;
    F_4437();
    F_44A2();
    F_12EC();
}

/* draw the whole 19x9 battle map and the player */
F_13A3()
{
    int i, j;

    D_204E = 0;
    for (i = 0; i < 19; i++)
        for (j = 0; j < 9; j++)
            F_355D(D_0194[i][j], i, j);
    F_355D(D_0240 + D_0242, D_012A, D_012C);
    if (D_0190) {
        D_0190 = 1;
        D_0192 = 1;
    }
}

/* x step for a direction code */
F_1415(d)
unsigned char d;
{
    if (d == 3 || d == 4)
        return 0;
    if (d == 2)
        return 1;
    return -1;
}

/* y step for a direction code */
F_1447(d)
unsigned char d;
{
    if (d == 2 || d == 1)
        return 0;
    if (d == 3)
        return -1;
    return 1;
}

/* animate: spell effect, the bouncing tile, the player's tile */
F_1477()
{
    if (D_0130 >= 0) {
        if (D_0130 == 6) {
            D_0130 = -1;
            F_355D(0, D_2054, D_2056);
            D_0194[D_2054][D_2056] = 0;
        } else {
            F_355D(D_0130 + 14, D_2054, D_2056);
            D_0130++;
        }
    }
    if (D_0190) {
        D_0190 += D_0192;
        if (D_0190 == 1)
            D_0192 = 1;
        else if (D_0190 > 4)
            D_0192 = -1;
        F_355D(D_0190 + 6 - 1, 14, 4);
    }
    if (D_0240 == 2)
        D_0242 = F_4069(1, 200) < 100;
    else if (D_0240 == 11) {
        if (D_0242 == 2)
            D_0242 = 0;
        else
            D_0242++;
    }
    F_355D(D_0240 + D_0242, D_012A, D_012C);
}

/* random sound/colour effect */
F_1565()
{
    int r;

    r = F_4069(1, 255);
    F_05B1(r - 128);
}

/* move the player by (dx, dy) if the target square is free; returns 1 if moved */
F_1589(dx, dy)
int dx, dy;
{
    int x, y, moved;

    moved = 0;
    x = D_012A + dx;
    y = D_012C + dy;
    if (x >= 0 && y >= 0 && x < 19 && y < 9 && D_0194[x][y] == 0) {
        D_0194[D_012A][D_012C] = 0;
        F_355D(0, D_012A, D_012C);
        D_012A = x;
        D_012C = y;
        F_355D(D_0240 + D_0242, D_012A, D_012C);
        moved = 1;
    }
    return moved;
}

/* the player takes n points of damage */
F_161E(n)
int n;
{
    F_3599("Hit! ", D_2046);
    F_3C8B(n, 1, 0x20, D_2046, 1);
    F_3B46(" damage!");
    D_206E -= n;
    F_053D(2);
    F_4437();
}

/* Mondain attacks in melee */
F_1664()
{
    F_3B46("Attacked by Mondain!");
    F_053D(3);
    if (F_4069(1, 255) > (0xb9 - D_2074 / 2 + (D_2086 << 3)) * 2)
        F_3B46("Missed!");
    else
        F_161E(D_206E / 32 + F_4069(1, 20));
}

/* Mondain casts a spell: magic missile, mind blaster or psionic shock */
F_16E0()
{
    int kind, thr, hit, i;

    kind = F_4069(1, 0xee) % 3;
    F_3599("Mondain casts ", D_2046);
    switch (kind) {
    case 0:
        F_3B46("magic missile!");
        F_053D(5);
        thr = (0xc3 - (D_2078 + D_207A / 2)) * 2;
        if (hit = F_4069(1, 255) < thr)
            F_161E(F_4069(1, 100));
        break;
    case 1:
        F_3B46("mind blaster!");
        F_053D(5);
        if (hit = F_4069(1, 200) > 140) {
            F_3B46("Hit!  Stats are reduced!");
            for (i = 1; i <= 6; i++) {
                if ((&D_206E)[i] > 16)
                    (&D_206E)[i] -= (&D_206E)[i] / 8;
                else if ((&D_206E)[i] >= 2)
                    (&D_206E)[i] -= 2;
            }
        }
        break;
    case 2:
        F_3B46("psionic shock!");
        F_4031();
        F_053D(5);
        F_3FF9();
        if (hit = F_4069(1, 200) > 140) {
            i = D_206E / 32;
            if (i > 255)
                i -= 254;
            F_161E(i);
        }
        break;
    }
    if (!hit)
        F_3B46("Missed!");
}

/* the monsters act: Mondain chants, melee, casts, or the minions move */
F_186F()
{
    int dx, dy, dist, moved = 0;

    if (!D_0246 && F_4069(1, 200) < 17)
        F_3B46("You hear a strange chanting!");
    else if (D_0246) {
        if (D_0240 == 4)
            D_012E += 25;
        else {
            dx = D_012A - D_0126;
            dy = D_012C - D_0128;
            dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
            if (dist == 1 && D_0244 == 20)
                F_1664();
            else if (dist < 7 && F_4069(1, 255) < 128 && D_0244 == 20)
                F_16E0();
            else {
                if (D_0244 == 40) {
                    if (dx == 0)
                        dx = F_1565();
                    if (dy == 0)
                        dy = F_1565();
                }
                if (F_4069(1, 255) & 1) {
                    moved = F_1589(0, F_05B1(dy));
                    if (!moved)
                        moved = F_1589(F_05B1(dx), 0);
                } else {
                    moved = F_1589(F_05B1(dx), 0);
                    if (!moved)
                        moved = F_1589(0, F_05B1(dy));
                }
                if (!moved && D_0244 == 40)
                    D_012E += 5;
            }
        }
    }
}

/* Mondain's state: spawn a magic square, change form by his hit points, or die */
F_19C8()
{
    int n = 0;

    if (D_0246 && D_0130 == -1 && F_4069(1, 200) < 36) {
        do {
            D_2054 = (F_4069(1, 255) + F_4069(1, 255)) % 19;
            D_2056 = (F_4069(1, 255) + F_4069(1, 255)) % 9;
        } while (D_0194[D_2054][D_2056] != 0 && ++n < 100);
        if (n < 100 && D_2054 != D_012A && D_2054 != D_0126 &&
            D_2056 != D_012C && D_2056 != D_0128) {
            D_0130 = 0;
            D_0194[D_2054][D_2056] = 14;
        }
    }
    if (D_012E > 500) {
        D_0240 = 2;
        D_0244 = 20;
    } else if (D_012E <= 500 && D_012E > 0 && D_0240 != 11) {
        D_0240 = 11;
        D_0244 = 40;
        D_0242 = 0;
    } else if (D_012E <= 0 && D_0244 != 30) {
        F_3599("Mondain is dead!", D_2046);
        if (D_0248 == 0)
            F_3B46("...or is he?");
        else {
            F_3B46("");
            D_024A = 1;
        }
        D_0240 = 4;
        D_0244 = 30;
        D_0242 = 0;
    }
}

/* read a key (waits with the animation, 0 if none) and return it upper-cased */
char F_1B46()
{
    unsigned char k;

    k = F_3862(F_413C(D_2046));
    F_3E5E(1);
    if (k == 0) {
        F_3E5E(1);
        if (D_0190 == 0)
            F_3E5E(1);
        else
            F_3CF1(4);
    }
    return k;
}

/* story screen 1: entering the craft, the gems, the colour legend */
F_1B9B()
{
    int c[4];
    int i;

    if (D_082C) {
        c[0] = 12;
        c[1] = 10;
        c[2] = 9;
        c[3] = 15;
    } else {
        c[0] = 2;
        c[1] = c[2] = 1;
        c[3] = 3;
    }
    F_047D(D_0DA8);
    F_04A2(5, 3, "Entering the craft, thou dost");
    F_04A2(5, 4, "remark upon four holes marked:");
    F_04A2(12, 6, "R    G    B    W");
    for (i = 0; i < 4; i++) {
        F_352C(i * 5 + 12, 7);
        F_357C(4, c[i]);
    }
    F_04A2(4, 9, "The proper gems fit in each hole.");
    F_04A2(5, 11, "Further examination leads thee");
    F_04A2(11, 12, "to two discoveries:");
    F_04A2(4, 14, "There is a button marked LAUNCH,");
    F_04A2(6, 15, "and thou art locked in, with");
    F_04A2(6, 16, "nothing to do but press it!");
}

/* story screen 2: the launch button */
F_1CAF()
{
    F_047D(D_0DA8);
    F_04A2(4, 4, "As soon as thou hast pressed the");
    F_04A2(4, 5, "large black button, lights begin");
    F_04A2(4, 6, "  to dance across the control");
    F_04A2(16, 7, "screens.");
    F_04A2(4, 9, " Generators shake the very floor");
    F_04A2(12, 10, "beneath thy feet.");
    F_04A2(4, 12, "The small craft begins to tremble");
    F_04A2(8, 13, "and rattle as thy journey");
    F_04A2(9, 14, "through time begins...");
}

/* story screen 3: pulled from the craft */
F_1D53()
{
    F_047D(D_0DA8);
    F_04A2(4, 8, "After but a few moments thou dost");
    F_04A2(4, 9, "feel a strong magic pulling thee");
    F_04A2(12, 10, "from thy craft...");
}

/* story screen 4: face to face with Mondain */
F_1D94()
{
    F_047D(D_0DA8);
    F_04A2(11, 6, "...A moment later,");
    F_04A2(9, 7, "thou art face to face");
    F_04A2(4, 8, " with the evil Mondain himself!");
    F_04A2(9, 13, "Good luck, this is it!");
}

/* the four story screens, each followed by a pause */
F_1DE6()
{
    F_1B9B();
    F_3AE2();
    F_1CAF();
    F_3AE2();
    F_1D53();
    F_3AE2();
    F_1D94();
    F_3AE2();
}

unsigned char D_08BB[8] = { 0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1 };

/* load file `name` (n bytes) and draw it as a 1-bit 320-wide image */
F_1E03(n, name)
int n;
char *name;
{
    int x, y, i, j, col;
    unsigned char b;

    j = i = y = x = 0;
    col = 15;
    F_3E2F(name, D_2E10, n);
    if (D_082C == 0)
        col = 3;
    for (i = 0; i < n; i++) {
        b = D_2E10[i];
        for (j = 0; j < 8; j++) {
            if (b & D_08BB[j])
                F_3D41(x, y, col);
            else
                F_3D41(x, y, 0);
            x++;
            if (x > 319) {
                x = 0;
                y++;
            }
        }
    }
}

/* victory: show the player, wait for a key */
F_1EA8()
{
    int i;

    F_355D(4, D_012A, D_012C);
    F_3D41(50, 7, D_0DA6);
    D_2090 = 1;
    F_0518(D_0DA8);
    F_04A2(11, 8, "THOU ART VICTORIOUS!");
    for (i = 0; i < 3; i++)
        F_053D(5);
    F_3E5E(10);
    F_4224();
    while (!F_413C(0) && i++ < 10000)
        ;
}

/* end of the game: victory screen, nif.bin picture, restart message */
F_1F31()
{
    F_1EA8();
    F_35CA(0, 0, 319, 199, 0);
    F_1E03(0x1a40, "nif.bin");
    F_3E5E(75);
    F_352C(4, 24);
    F_3599("Press CONTROL-ALT-DEL to Restart", D_2046);
    F_3E5E(0);
}

/* main: load data files, story screens, then the key-driven battle loop */
F_1853(argc, argv)
int argc;
unsigned char **argv;
{
    unsigned char key;
    int i, sz;

    sz = 64;
    F_1D0D(1);
    D_2048 = F_3862(argv[1][0]) == 'C';
    D_2E0C = argv[1];
    D_2E0E = argv[2];
    F_3F58(0);
    if (D_082C)
        sz = 128;
    for (i = 0; i < 128; i++)
        D_238C[i] = 0;
    F_3E2F("inuse.u1", (int)D_2058, 0x334);
    F_3E2F(D_0184[D_082C], D_238C + sz * 10, D_018A[D_082C]);
    F_3E2F(D_0156[D_082C], D_238C + sz, 0xa80 - sz);
    D_204A = 5;
    F_45A3();
    F_37FD(D_208A + F_3EC4());
    F_1DE6();
    F_13A3();
    for (;;) {
        i = 0;
        F_387E(0, 24);
        do {
            key = F_1B46();
            i++;
            F_1477();
        } while (key == 0 && i < 1500);
        if (key == 0)
            key = ' ';
        switch (key) {
        case 3:
        case 4:
        case 2:
        case 1:
            F_0696(key);
            break;
        case 'A':
            F_0A0D(-1);
            break;
        case '8':
            F_0A0D(3);
            break;
        case '2':
            F_0A0D(4);
            break;
        case '6':
            F_0A0D(2);
            break;
        case '4':
            F_0A0D(1);
            break;
        case 'B':
            F_1194();
            break;
        case 'C':
            F_0ED3();
            break;
        case 'D':
            F_11A2();
            break;
        case 'E':
            F_11B0();
            break;
        case 'F':
            F_11BE();
            break;
        case 'G':
            F_1057();
            break;
        case 'H':
            F_11CC();
            D_0190 = 1;
            break;
        case 'I':
            F_1124();
            break;
        case 'K':
            F_11DA();
            break;
        case 'N':
            F_0576();
            break;
        case 'O':
            F_11E8();
            break;
        case 'Q':
            F_11F6();
            break;
        case 'R':
            F_43C8();
            break;
        case 'S':
            F_1204();
            break;
        case 'T':
            F_121B();
            break;
        case 'U':
            F_1232();
            break;
        case 'V':
            F_1240();
            break;
        case 'X':
            F_124E();
            break;
        case 'Z':
            F_478E();
            D_204E = 1;
            F_387E(0, 24);
            break;
        case ' ':
            F_0551();
            break;
        case 27:
            D_204C = 1;
        default:
            F_055F();
        }
        if (D_204E)
            F_13A3();
        F_19C8();
        if (D_024A) {
            F_1F31();
            D_204C = 1;
        }
        F_186F();
        if (D_206E > 0 && D_2080 > 0)
            continue;
        F_138D();
    }
}
