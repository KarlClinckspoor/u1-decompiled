/* opts: -ms -v -w -cs */
/*
 * MONDAIN.EXE - the final battle against Mondain, code 12EC-2230.
 */

/* Origin support routines */
extern F_3B46(), F_352C(), F_3599(), F_3E5E(), F_3E85(), F_355D(), F_3C8B();
extern F_4437(), F_44A2(), F_4031(), F_3FF9();
extern int F_4069();
/* Lattice library */
extern EXECL();
/* other MONDAIN routines (03E9-12EC) */
extern F_0518(), F_053D(), F_05B1(), F_125C();

extern int D_0DA8, D_2046;
extern char *D_2E0C, *D_2E0E;
extern int D_206E[];
extern int D_2080, D_204E;
extern int D_0190, D_0192, D_0240, D_0242, D_012A, D_012C, D_0130;
extern int D_2054, D_2056, D_2074, D_2086, D_2078, D_207A;
extern unsigned char D_0194[][9];
extern char D_2058[];

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
    F_3E85("inuse.u1", D_2058, 0x334);
    for (;;) {
        r = EXECL("out.exe", "out.exe", D_2E0C, D_2E0E, "M", 0);
        F_125C();
    }
}

/* the player dies: hit points and food to 0, then defeat */
F_138D()
{
    D_206E[0] = 0;
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
    switch (d) {
    case 3:
    case 4:
        return 0;
    }
    if (d == 2)
        return 1;
    return -1;
}

/* y step for a direction code */
F_1447(d)
unsigned char d;
{
    switch (d) {
    case 1:
    case 2:
        return 0;
    }
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
    D_206E[0] -= n;
    F_053D(2);
    F_4437();
}

/* Mondain attacks in melee */
F_1664()
{
    int r;

    F_3B46("Attacked by Mondain!");
    F_053D(3);
    r = F_4069(1, 255);
    if (r > (0xb9 - D_2074 / 2 + (D_2086 << 3)) * 2)
        F_3B46("Missed!");
    else {
        r = D_206E[0] / 32;
        F_161E(r + F_4069(1, 20));
    }
}

/* Mondain casts a spell: magic missile, mind blaster or psionic shock */
F_16E0()
{
    int a, b, c, kind, thr, hit, i;

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
                if (D_206E[i] > 16)
                    D_206E[i] -= D_206E[i] / 8;
                else if (D_206E[i] >= 2)
                    D_206E[i] -= 2;
            }
        }
        break;
    case 2:
        F_3B46("psionic shock!");
        F_4031();
        F_053D(5);
        F_3FF9();
        if (hit = F_4069(1, 200) > 140) {
            i = D_206E[0] / 32;
            if (i > 255)
                i -= 254;
            F_161E(i);
        }
        break;
    }
    if (!hit)
        F_3B46("Missed!");
}
