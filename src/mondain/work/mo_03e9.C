/* opts: -ms -v -w -cs */
/*
 * MONDAIN.EXE - the final battle against Mondain (code 03E9-12EC).
 * Work file; names are still the original addresses.
 */
extern int D_2046;
extern int D_2090;
extern int D_207A, D_206A, D_206E;
extern int D_0126, D_0128, D_012A, D_012C, D_012E;
extern int D_0190, D_0246;
extern unsigned char D_0194[][9];
extern char *D_1DC8[];
extern int D_0CD2[];
extern int D_2070, D_2072, D_2082, D_2088;

extern F_39FC(), F_3812(), F_352C(), F_35CA(), F_3599(), F_3F62(), F_3B46();
extern F_3E5E(), F_1477(), F_355D(), F_4031(), F_3C8B(), F_4437(), F_3FF9();
extern int F_4069(), F_1415(), F_1447(), F_0FC7();
extern unsigned char F_413C();

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
char F_05DB()
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
