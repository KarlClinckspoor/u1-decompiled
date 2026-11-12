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
extern F_0518(), F_053D(), F_125C();
extern int F_05B1();
extern F_047D(), F_04A2(), F_055F(), F_0551(), F_0696(), F_0A0D(), F_0576();
extern F_0ED3(), F_1057(), F_1124(), F_1194(), F_11A2(), F_11B0(), F_11BE();
extern F_11CC(), F_11DA(), F_11E8(), F_11F6(), F_1204(), F_121B(), F_1232();
extern F_1240(), F_124E();
extern F_357C(), F_35CA(), F_3AE2(), F_3CF1(), F_3D41(), F_3E2F(), F_3EC4();
extern F_3F58(), F_37FD(), F_387E(), F_4224(), F_43C8(), F_45A3();
extern F_478E(), F_1D0D();
extern char F_3862();
extern unsigned char F_413C();

extern int D_0DA8, D_2046;
extern char *D_2E0C, *D_2E0E;
extern int D_206E[];
extern int D_2080, D_204E;
extern int D_0190, D_0192, D_0240, D_0242, D_012A, D_012C, D_0130;
extern int D_2054, D_2056, D_2074, D_2086, D_2078, D_207A;
extern unsigned char D_0194[][9];
extern char D_2058[];
extern int D_0126, D_0128, D_012E, D_0244, D_0246, D_0248, D_024A;
extern int D_082C, D_0DA6, D_2048, D_204A, D_204C, D_2090, D_208A;
extern int D_0184[], D_018A[], D_0156[];
extern char D_238C[];
extern unsigned char D_2E10[], D_08BB[];

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
    D_206E[0] -= n;
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
        F_161E(D_206E[0] / 32 + F_4069(1, 20));
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
        if (D_206E[0] > 0 && D_2080 > 0)
            continue;
        F_138D();
    }
}
