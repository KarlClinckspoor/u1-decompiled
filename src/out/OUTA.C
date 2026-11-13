/* opts: -ms -v -w -cs */
/*
 * OUT.EXE - the game, part A of 3 (game code 03E9-32F8: save/load, movement, combat and spells, drop lists, dungeon entities, shops signs, wall drawing start (F_03E9-F_3240)).
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

/* initialised data (DGROUP 0124-0184); the strings of the literals follow in order of use */
int _stack = 3072;
char *D_014E[3] = { "CgaTiles.Bin", "EgaTiles.Bin", "T1KTiles.Bin" };
char *D_0172 = "Thy canst not leave it here!";
char *D_0180 = "playerx.u1";
int D_0182 = 0;


#define MT ((int (*)[8])D_3B2C)

/* ---- ou_03e9 ---- */
/* write the player file (slot digit c, '1'-'4') and the 'inuse' file */
F_03E9(c)
unsigned char c;
{
    if (c < '1' || c > '4') {
        F_5643();
        EXIT(1);
    }
    D_0180[6] = c;
    if (D_8B8C == 0)
        F_3E2F(D_0180, (int)D_2058, 820);
    else
        F_3E2F("inuse.u1", (int)D_2058, 820);
}

/* read/write the town or dungeon file for the current level */
F_0441()
{
    int t;

    if (D_082C == 0)
        t = 0x0D00;
    else
        t = 0x1A00;
    F_3E2F(D_014E[D_082C], D_238C, t);
}

/* play sound effect n if sound is on */
F_0476(n)
int n;
{
    if (D_3AB0)
        F_3F62(n);
}

/* print blocked message n, play sound 0, return 0 */
F_048A(n)
int n;
{
    F_3B46(D_37AE[n]);
    F_0476(0);
    return 0;
}

char D_018D[] = "!";
/* try to step in direction c: returns 1 if the square can be entered, else prints why */
F_04AF(c)
unsigned char c;
{
    int dx, dy, t, ok, lvl, idx;

    dy = 0;
    dx = 0;
    switch (c) {
    case 3: dy = -1; break;
    case 4: dy = 1; break;
    case 2: dx = 1; break;
    case 1: dx = -1; break;
    }
    t = F_C2AF(F_C2EC(dx + 9), F_C31C(dy + 4), 1);
    ok = 1;
    lvl = D_2088;
    if (t >= 19 && t <= 47) {
        idx = (t - 19) / 2;
        F_3599(D_37AE[5], D_2046);
        ok = 0;
        if (F_40A8(D_0FD8[106 + idx][0]))
            F_357C('n', D_2046);
        F_357C(' ', D_2046);
        if (t == 47) {
            F_3EED();
            F_352C(1, 24);
        }
        F_3599(D_0FD8[106 + idx], D_2046);
        F_3B46(D_018D);
        F_0476(0);
    } else if (t == 3) {
        ok = F_048A(1);
    } else if (t == 0) {
        if (lvl != 4 && lvl != 3 && lvl != 5)
            ok = F_048A(0);
    } else if (t == 2 && lvl == 5) {
        ok = F_048A(2);
    } else if (lvl == 4 && t != 0) {
        ok = F_048A(3);
    } else if (lvl == 3 && t != 0) {
        ok = F_048A(4);
    } else if (lvl >= 6) {
        ok = F_048A(6);
    }
    return ok;
}

/* movement command in direction c (dungeon/town/space/overworld dispatch) */
F_066F(c)
unsigned char c;
{
    int t;

    if (D_204A == 1 || D_204A == 2)
        F_2E22(c);
    else if (D_204A == 3)
        F_4B73(c);
    else if (F_04AF(c)) {
        t = F_40FE(D_3AAC, D_3AAE);
        if (D_2088 <= 2)
            F_0476(4);
        F_3B46(D_1DC8[c]);
        switch (c) {
        case 3: F_B665(); break;
        case 4: F_B70F(); break;
        case 2: F_B7CD(); break;
        case 1: F_B884(); break;
        }
        D_3B2C[0].x = D_3AAC;
        D_3B2C[0].y = D_3AAE;
        if (F_40FE(D_3AAC, D_3AAE) != t)
            F_86E1(t);
    }
}

/* name of monster type a (a = 19, 21, ...) */
F_0742(a)
int a;
{
    return D_0FD8[106 + (a - 19) / 2];
}

char D_018F[] = "! ";
char D_0192[] = " copper!";
char D_019B[] = "";
char D_019C[] = " silver!";
char D_01A5[] = " gold!";
/* give the player n pence (capped at 9999) and print "Thou dost find: x copper! y silver! z gold!" */
F_0761(n, t)
int n, t;
{
    int ones, tens, thou, ok, f;

    f = 0;
    if (n + D_207C > 9999)
        n = 9999 - D_207C;
    if (t == 0) {
        t = 8;
        f = 1;
    }
    if (n) {
        ones = n % 10;
        tens = (n % 100 - ones) / 10;
        thou = n / 1000;
        if (f)
            ok = 1;
        else if (t > 6) {
            F_3B46(D_018D);
            ok = 0;
        } else {
            F_3599(D_018F, D_2046);
            ok = 1;
        }
        if (ones) {
            F_3C8B(ones, 1, ' ', D_2046, 1);
            F_3599(D_0192, D_2046);
            if (ok) {
                F_3B46(D_019B);
                ok = 0;
            } else
                ok = 1;
        }
        if (tens) {
            F_3C8B(tens, 2, ' ', D_2046, 1);
            F_3599(D_019C, D_2046);
            if (ok) {
                F_3B46(D_019B);
                ok = 0;
            } else
                ok = 1;
        }
        if (thou) {
            F_3C8B(thou, 2, ' ', D_2046, 1);
            F_3599(D_01A5, D_2046);
            if (ok) {
                F_3B46(D_019B);
                ok = 0;
            } else
                ok = 1;
        }
        if (ok)
            F_3B46(D_019B);
        D_207C += n;
        F_4558();
    }
}

/* monster/guard kill reward hook: some objects (9,14,19,24) refresh a counter */
F_08EE(n)
int n;
{
    int v;

    switch (n) {
    case 9: v = 2; break;
    case 14: v = 4; break;
    case 19: v = 6; break;
    case 24: v = 8; break;
    default: v = 0;
    }
    if (v)
        F_5085(8 - v);
}

char D_01AC[] = "Hit! ";
char D_01B2[] = " destroyed!";
char D_01BE[] = " killed!";
char D_01C7[] = "Thou dost find: ";
char D_01D8[] = " damage!";
char D_01E1[] = "Missed!";
/* attack the square (x,y) with sound snd, hit chance hit%, damage dmg (10000 = destroy) */
F_0953(snd, hit, dmg, x, y)
int snd, hit, dmg, x, y;
{
    int r1, r2, ok;

    ok = 1;
    if (D_AD56[D_AD3E][D_AD40].a == 3)
        ok = D_AD56[x][y].a == 0 || D_AD56[x][y].a == 7 || D_AD56[x][y].a == 6;
    if (F_4069(1, 100) <= hit
        && D_AD56[D_AD3E][D_AD40].a != 1
        && D_AD56[D_AD3E][D_AD40].a != 2
        && D_AD56[D_AD3E][D_AD40].a != 8
        && ok) {
        F_0476(snd);
        if (dmg != 10000)
            F_3599(D_01AC, D_2046);
        if (D_AD56[x][y].d <= dmg) {
            F_3C31(D_AD56[x][y].b);
            if (dmg == 10000)
                F_3B46(D_01B2);
            else
                F_3B46(D_01BE);
            F_08EE(D_AD56[x][y].b);
            r1 = F_4069(2, D_AD50 * 3 + 10 + D_AD56[x][y].b);
            F_3599(D_01C7, D_2046);
            F_0761(r1, 0);
            D_B11E[D_AD56[x][y].b % 5] = 0;
            D_AD56[x][y].b = -1;
            D_AD56[x][y].d = 0;
            r2 = F_4069(2, D_AD50 * D_AD50 + 10);
            D_207E += r2;
            D_AD54 += r2 * 2;
            F_450D();
            F_4602();
        } else {
            F_3C8B(dmg, 1, ' ', D_2046, 1);
            F_3B46(D_01D8);
            D_AD56[x][y].d -= dmg;
        }
    } else
        F_3B46(D_01E1);
}

/* how far a shot in the current direction travels (0 if blocked at once) */
F_0BAA(range)
int range;
{
    int i, dx, dy, ox, oy, a, b;

    i = 0;
    dx = F_3C8E();
    dy = F_3CBB();
    do {
        i++;
        ox = dx * i;
        oy = i * dy;
        a = D_AD56[D_AD3E + ox][D_AD40 + oy].a;
        b = D_AD56[D_AD3E + ox][D_AD40 + oy].b;
    } while (i < range && b == -1 && a != 1 && a != 2 && a != 8 && a != 3);
    if (b == -1 || a == 1 || a == 2 || a == 8)
        return 0;
    return i;
}

/* dungeon attack: weapon shot along the facing direction */
F_0C52()
{
    int hit, dmg, r;

    F_3B46(D_019B);
    F_0476(7);
    r = F_0BAA(D_0CD2[D_2082]);
    if (!r) {
        F_3B46("Nothing");
    } else {
        hit = D_3A92 + 50;
        dmg = F_4069(2, D_3A90 + D_2082 * 8);
        F_0953(2, hit, dmg, D_AD3E + r * F_3C8E(), D_AD40 + r * F_3CBB());
    }
}

char D_01F1[] = "Miss!";
char D_01F7[] = "Killed the ";
char D_0203[] = "Hit the ";
char D_020C[] = " damage";
char D_0214[] = "Missed the ";
/* fire a projectile (tile) from the player in direction c; hits monsters in the path */
F_0CF3(c, snd, range, dmg, hit, tile)
unsigned char c;
int snd, range, dmg, hit, tile;
{
    int t, dx, dy, ox, oy, i, id, x, y, r2, r1, cls;

    i = 1;
    if (D_204A == 1 || D_204A == 2)
        F_2B6D(c, range, dmg, hit);
    else {
        switch (c) {
        case 3: dx = 0; dy = -1; break;
        case 4: dx = 0; dy = 1; break;
        case 2: dx = 1; dy = 0; break;
        case 1: dx = -1; dy = 0; break;
        }
        do {
            ox = dx * i;
            oy = i * dy;
            F_355D(tile, ox + 9, oy + 4);
            if (i == 1)
                F_0476(snd);
            else
                F_3E5E(2);
            F_355D(t = F_C2AF(D_3AAC + ox, D_3AAE + oy, 1), ox + 9, oy + 4);
            i++;
        } while (i <= range && t < 19 && t != 3);
        if (t < 19)
            F_3B46(D_01F1);
        else if (F_4069(1, 100) <= hit) {
            cls = F_353A(F_0742(t));
            x = F_C2EC(ox + 9);
            y = F_C31C(oy + 4);
            F_355D(tile, ox + 9, oy + 4);
            id = F_B545(x, y, 1);
            D_3B2C[id].f8 -= dmg;
            if (D_3B2C[id].f8 <= 0)
                F_3599(D_01F7, D_2046);
            else
                F_3599(D_0203, D_2046);
            F_3599(F_0742(t), D_2046);
            if (D_3B2C[id].f8 > 0) {
                if (cls > 6)
                    F_3B46(D_018D);
                else
                    F_3599(D_018F, D_2046);
                F_3C8B(dmg, 2, ' ', D_2046, 1);
                F_3B46(D_020C);
                F_0476(2);
                F_355D(t, ox + 9, oy + 4);
            } else {
                F_B58E(x, y);
                D_8B90--;
                r1 = F_4069(1, D_368C[(t - 19) / 2] * 4) + 10;
                r2 = F_4069(2, D_368C[(t - 19) / 2] * 5);
                D_207E += r2;
                F_0476(2);
                F_355D(F_A363(x, y), ox + 9, oy + 4);
                F_0761(r1, cls);
                F_450D();
            }
        } else {
            F_3599(D_0214, D_2046);
            F_3599(F_0742(t), D_2046);
            F_3B46(D_018D);
        }
    }
}

/* shoot in direction c with weapon (k = 7) or spell: picks range/hit/damage then F_0CF3 */
F_0FEF(c, k)
unsigned char c;
int k;
{
    int dmg, hit, range;

    if (k == 7) {
        range = D_0CD2[D_2082];
        hit = D_3A92 + 50;
        dmg = F_4069(2, D_3A90 + D_2082 * 8);
    } else {
        range = 3;
        hit = 80;
        dmg = F_4069(1, D_2088 * 10) + 30;
    }
    F_0CF3(c, k, range, dmg, hit, 50);
}

char D_0220[] = "nothing";
/* aim: direction a (-1 = ask the player), print it, shoot */
F_1072(a, b)
int a, b;
{
    unsigned char c;

    if (a == -1)
        c = F_4161();
    else
        c = a;
    if (c > 4)
        F_3B46(D_0220);
    else {
        F_3B46(D_1DC8[c]);
        F_0FEF(c, b);
    }
}

char D_0228[] = "Attack with ";
char D_0235[] = "?";
char D_0237[] = ": ";
/* Attack command: weapon name, "?" if it has no range, else aim and shoot */
F_10C2(a)
int a;
{
    F_3599(D_0228, D_2046);
    F_3599(D_0DF8[D_2082], D_2046);
    if (D_0CD2[D_2082] == 0) {
        F_3B46(D_0235);
        F_0476(1);
    } else if (D_204A == 3)
        F_0C52();
    else {
        F_3599(D_0237, D_2046);
        F_1072(a, 7);
    }
}

/* save state and exec the vehicle program (space.exe / mondain.exe) */
F_1135(prog, tile)
char *prog;
int tile;
{
    int r;

    F_355D(tile, 9, 4);
    D_3B22 = D_8BA6;
    D_3B24 = D_8BA8;
    F_3E85("inuse.u1", (int)D_2058, 820);
    for (;;) {
        r = EXECL(prog, prog, D_8B88, D_8B8A, 0);
        F_125C();
    }
}

/* Board command: mount a horse/vehicle, or start the space ship / Mondain's fight */
F_118D()
{
    int t;

    if (D_204A != 0) {
        F_3B46("Board?");
        F_0476(1);
    } else if (D_2088 != 0) {
        F_3B46("X-it thy craft first!");
        F_0476(1);
    } else {
        t = F_C2AF(D_3AAC, D_3AAE, 1);
        if (t < 11 || t > 18) {
            F_3B46("Nothing to Board!");
            F_0476(1);
        } else {
            D_3B2C[0].type = t;
            F_B58E(D_3AAC, D_3AAE);
            D_2088 = t - 11 + 1;
            if (D_2088 > 4)
                D_2088--;
            if (t == 11 || t == 12)
                F_3599("Mount ", D_2046);
            else
                F_3599("Board ", D_2046);
            F_3B46(D_0FA2[D_2088]);
            if (t == 17)
                F_1135("space.exe", t);
            else if (t == 18) {
                if (D_20A4 && D_3ACA && D_3AC8 && D_3AC6)
                    F_1135("mondain.exe", t);
                else {
                    F_21C2(D_0DA8);
                    F_21E7(6, 7, "Thou canst not determine how");
                    F_21E7(6, 8, "to operate the craft at this");
                    F_21E7(6, 9, "           time.");
                    F_2141();
                    F_352C(1, 24);
                }
            }
        }
    }
}

/* spell damage roll, scaled by the selected spell D_2082 */
F_12F2()
{
    int r;

    r = F_4069(1, D_3A9A);
    if (D_2082 == 9)
        r *= 2;
    else if (D_2082 == 8) {
        r *= 3;
        r /= 2;
    } else if (D_2082 == 10 || D_2082 == 11)
        r *= 3;
    return r;
}

/* open a coffin/chest: clears the container and pays out pence */
F_1351()
{
    F_3B46(" opened!");
    D_AD56[D_AD3E][D_AD40].c = 0;
    F_3599(D_01C7, D_2046);
    F_0761(F_4069(3, D_AD50 * D_AD50 + 9), 0);
}

char D_02E1[] = "Coffin";
char D_02E8[] = "Chest";
char D_02EE[] = "Ladder created!";
char D_02FE[] = "Teleported!";
char D_030A[] = "Field created!";
char D_0319[] = "Field destroyed!";
char D_032A[] = "Failed!";
/* cast spell a (dungeon spells 1-10); b = print its name first */
F_13A8(a, b)
int a, b;
{
    int f, i, j;

    f = 0;
    if (b)
        F_3B46(D_0FD8[185 + a]);
    switch (a) {
    case 1:
        if (D_AD56[D_AD3E][D_AD40].c == 5) {
            F_3599(D_02E1, D_2046);
            F_1351();
        } else
            f = 1;
        break;
    case 2:
        if (D_AD56[D_AD3E][D_AD40].c == 4) {
            F_3599(D_02E8, D_2046);
            F_1351();
        } else
            f = 1;
        break;
    case 3:
        i = F_0BAA(5);
        if (i) {
            F_0953(5, 101, F_12F2(), D_AD3E + i * F_3C8E(), D_AD40 + i * F_3CBB());
        } else
            f = 1;
        break;
    case 5:
        if (D_AD50 == 10 || D_AD56[D_AD3E][D_AD40].a == 8)
            f = 1;
        else if ((D_AD3E & 1) || (D_AD40 & 1)) {
            D_AD56[D_AD3E][D_AD40].a = 6;
            F_3B46(D_02EE);
        } else
            f = 1;
        break;
    case 6:
        if (((D_AD3E & 1) || (D_AD40 & 1)) && D_AD56[D_AD3E][D_AD40].a != 8) {
            D_AD56[D_AD3E][D_AD40].a = 7;
            F_3B46(D_02EE);
        } else
            f = 1;
        break;
    case 7:
        do {
            i = F_4069(1, 9);
            j = F_4069(1, 9);
        } while ((i == D_AD3E && j == D_AD40)
                 || (D_AD56[i][j].a == 8 || D_AD56[i][j].a == 1 || D_AD56[i][j].a == 2)
                 || D_AD56[i][j].b != -1);
        D_AD3E = i;
        D_AD40 = j;
        F_3B46(D_02FE);
        break;
    case 8:
        i = D_AD3E + F_3C8E();
        j = D_AD40 + F_3CBB();
        if (D_AD56[i][j].a == 0 && D_AD56[i][j].b == -1 && D_AD56[i][j].c == 0) {
            D_AD56[i][j].a = 8;
            F_3B46(D_030A);
        } else
            f = 1;
        break;
    case 9:
        i = D_AD3E + F_3C8E();
        j = D_AD40 + F_3CBB();
        if (D_AD56[i][j].a == 8) {
            D_AD56[i][j].a = 0;
            F_3B46(D_0319);
        } else
            f = 1;
        break;
    case 10:
        i = D_AD3E + F_3C8E();
        j = D_AD40 + F_3CBB();
        if (D_AD56[i][j].b != -1)
            F_0953(5, 101, 10000, i, j);
        else
            f = 1;
        break;
    }
    if (f) {
        F_0476(6);
        F_3B46(D_032A);
    } else if (a != 10 && a != 3)
        F_0476(5);
}

/* ---- ou_1769_a ---- */
/* first spell (Prayer): heals or feeds when low, else may remove a nearby monster */
F_1769()
{
    int i, done;
    struct mon *m;

    i = 1;
    done = 0;
    F_3B46(D_019B);
    F_3B46(D_0FD8[185]);
    if (D_204A == 3) {
        if ((i = F_4069(1, 10)) == 4)
            i++;
        F_13A8(i, 0);
    } else {
        if (D_206E[0] < 15) {
            D_206E[0] = 15;
            F_3B46(" Shazam!");
            F_4437();
            F_0476(5);
            done = 1;
        } else if (D_2080 < 15) {
            D_2080 = 15;
            F_3B46(" Shazam!");
            F_44A2();
            F_0476(5);
            done = 1;
        } else if (F_4069(1, 100) < 25) {
            for (; i <= D_8BA6 && !done; i++) {
                m = &D_3B2C[i];
                if (m->type >= 19) {
                    if (done = F_8194(m->type, D_3AAC - m->x, D_3AAE - m->y)) {
                        F_355D(0x31, m->x - D_3AAC + 9, m->y - D_3AAE + 4);
                        F_3B46("Monster removed!");
                        F_0476(5);
                        F_355D(F_A363(m->x, m->y), m->x - D_3AAC + 9, m->y - D_3AAE + 4);
                        F_B58E(m->x, m->y);
                    }
                }
            }
        }
        if (!done) {
            F_3B46("Hmmmm... no effect!");
            F_0476(6);
        }
    }
}

/* cast a combat spell (Magic Missile / Kill): direction, then hit roll */
F_18FC()
{
    unsigned char c;
    int miss, dmg, hit;

    miss = 0;
    F_3599(D_0237, D_2046);
    if ((c = F_4161()) > 4) {
        miss++;
        F_3B46("Nothing");
    } else
        F_3B46(D_1DC8[c]);
    if (miss)
        D_20D8[D_2084]++;
    else {
    if (D_2084 == 3) {
        F_3599("\"DELCIO-ERE-UI\" ", D_2046);
        dmg = F_12F2();
    } else {
        F_3599("\"INTERFICIO-NUNC!\" ", D_2046);
        dmg = 9999;
    }
    if (D_206A == 1)
        hit = 1;
    else
        hit = F_4069(1, 100) < D_3A98 + 50;
    if (!hit) {
        F_3B46(D_032A);
        F_0476(6);
    } else {
        F_0476(5);
        F_3B46(D_019B);
        F_0CF3(c, 7, 3, dmg, 0x65, 0x31);
    }
    }
}

/* Cast command: the readied spell D_2084 */
F_19FF()
{
    if (D_204A == 1 || D_204A == 2 || D_204A == 4) {
        F_3599("Cast -- ", D_2046);
        F_3B46("Hmmmm... no effect!");
        F_0476(6);
    } else {
        F_3599("Cast ", D_2046);
        F_3599(D_0F04[D_2084], D_2046);
        if (D_2084 == 0)
            F_1769();
        else if (D_20D8[D_2084] == 0) {
            F_3B46(D_019B);
            F_3B46("You've used up that spell!");
            F_0476(6);
        } else {
            D_20D8[D_2084]--;
            if (D_204A == 3)
                F_13A8(D_2084, 1);
            else if (D_2084 != 3 && D_2084 != 10) {
                F_3B46(D_019B);
                F_3B46("Failed, dungeon spell only!");
                F_0476(6);
            } else
                F_18FC();
        }
    }
}

/* pence offered at a dungeon altar (tiles 33-35): stats, items or food */
F_1AE4(n)
int n;
{
    int t, r;

    t = D_8BAE[D_AD42][D_AD3E][D_AD40];
    if (t != 0x33 && t != 0x34 && t != 0x35) {
        F_3599("Ok!", D_2046);
    } else {
    F_3599("Shazam!", D_2046);
    F_0476(5);
    switch (t) {
    case 0x33:
        r = F_4069(1, 6);
        D_206E[r] += n / 10;
        if (D_206E[r] > 99)
            D_206E[r] = 99;
        break;
    case 0x34:
        if (n >= 10) {
            r = F_4069(1, 15);
            if (D_20B8[r] < 255)
                D_20B8[r]++;
        }
        break;
    case 0x35:
        D_2080 += n * 3;
        F_44A2();
        break;
    }
    }
}

char D_03D7[] = "pence: ";
char D_03DF[] = "none";
char D_03E4[] = "Thou hast not that much!";
char D_03FD[] = "Alakazot!";
/* drop pence: in a dungeon an offering, at a shrine tile hit points and a gem */
F_1BC0()
{
    int n, t;

    F_3599(D_03D7, D_2046);
    if ((n = F_B0B5(12, 24)) == 0) {
        F_352C(13, 24);
        F_3599(D_03DF, D_2046);
    } else {
        F_3B46(D_019B);
        if (n > D_207C) {
            F_3599(D_03E4, D_2046);
            F_0476(1);
        } else {
            D_207C -= n;
            F_4558();
            if (D_204A == 2)
                F_1AE4(n);
            else if (D_8BAE[D_AD42][D_AD3E][D_AD40] != 0x33)
                F_3599("Ok!", D_2046);
            else {
                t = n * 3;
                D_206E[0] += (t = t / 2);
                F_4437();
                if (F_4069(1, 255) > 16)
                    F_3599("Shazam!", D_2046);
                else {
                    if ((n = F_4069(1, 7)) == 3)
                        n++;
                    D_3AFA[n]++;
                    F_3599(D_03FD, D_2046);
                }
                F_0476(5);
            }
        }
    }
}

/* clear the map area for the drop list */
F_1CF6()
{
    F_35CA(8, 8, 0x137, 0x97, 0);
    F_387E(0x10, 0);
    F_3599(" Drop ", D_2046);
    F_3920(0x17, 0);
}

/* draw a frame bar at the top of the screen */
F_1D35()
{
    F_35CA(0x7f, 0, 0xc7, 6, D_0DA8);
    F_39FC(0x7f, 7, 0xc7, 7, D_0DA6);
    D_204E = 1;
}

/* ---- ou_1769_b ---- */
char D_040E[] = "weapon: ";
char D_0417[] = ") ";
/* Weapon drop list: choose a weapon letter and drop one */
F_1D71()
{
    int i, y, n;
    unsigned char c;

    n = 0;
    c = 'b';
    F_3599(D_040E, D_2046);
    for (i = 1; i <= 15; i++)
        if (D_20B8[i])
            n++;
    if (n == 0) {
        F_3599(D_03DF, D_2046);
    } else {
    F_1CF6();
    y = 10 - n / 2;
    n = 0;
    for (i = 1; i <= 15; c++, i++) {
        if (D_20B8[i]) {
            F_352C(15, y + n);
            F_357C(c, D_2046);
            F_3599(D_0417, D_2046);
            F_3599(D_0D72[i], D_2046);
            n++;
        }
    }
    F_352C(14, 24);
    c = F_33BB(F_39C5(D_2046)) - 'a';
    if (c < 1 || c > 15)
        F_3599(D_03DF, D_2046);
    else if (D_20B8[c] == 0)
        F_3599(D_03DF, D_2046);
    else {
        F_3599(D_0D72[c], D_2046);
        D_20B8[c]--;
        if (c == D_2082 && D_20B8[c] == 0)
            D_2082 = 0;
    }
    F_1D35();
    }
}

/* Armour drop list: choose an armour letter and drop one */
F_1EDF()
{
    int i, y, n;
    unsigned char c;

    n = 0;
    c = 'b';
    F_3599("armour: ", D_2046);
    for (i = 1; i <= 5; i++)
        if (D_20AC[i])
            n++;
    if (n == 0) {
        F_3599(D_03DF, D_2046);
    } else {
    F_1CF6();
    y = 10 - n / 2;
    n = 0;
    for (i = 1; i <= 5; c++, i++) {
        if (D_20AC[i]) {
            F_352C(13, y + n);
            F_357C(c, D_2046);
            F_3599(D_0417, D_2046);
            F_3599(D_0F5C[i], D_2046);
            n++;
        }
    }
    F_352C(14, 24);
    c = F_33BB(F_39C5(D_2046)) - 'a';
    if (c < 1 || c > 5)
        F_3599(D_03DF, D_2046);
    else if (D_20AC[c] == 0)
        F_3599(D_03DF, D_2046);
    else {
        F_3599(D_0F5C[c], D_2046);
        D_20AC[c]--;
        if (c == D_2086 && D_20AC[c] == 0)
            D_2086 = 0;
    }
    F_1D35();
    }
}

/* Drop command: only in dungeons/towns; asks pence/weapon/armour */
F_204D()
{
    unsigned char c;

    if (D_204A != 1 && D_204A != 2) {
        F_3599("Drop?", D_2046);
        F_0476(1);
    } else {
        F_3599("Drop Pence,Weapon,Armour:", D_2046);
        c = F_3862(F_39C5(D_2046));
        F_35CA(40, 192, 230, 199, 0);
        F_352C(6, 24);
        switch (c) {
        case 'P':
            F_1BC0();
            break;
        case 'W':
            F_1D71();
            break;
        case 'A':
            F_1EDF();
            break;
        default:
            F_3599(D_0220, D_2046);
        }
    }
    F_3B46(D_019B);
}

/* copy the 15 entries of the current level's table into D_ACC6 */
F_20FC()
{
    int i;

    for (i = 0; i < 15; i++)
        D_ACC6[i] = D_31DC[D_AD42][i];
}

/* wait for a key (up to 10000 polls) */
F_2141()
{
    int i;

    i = 0;
    while (!F_413C(0) && i++ < 10000)
        ;
}

/* draw a frame (outline) in colour c */
F_2168(c)
int c;
{
    F_39FC(34, 26, 284, 26, c);
    F_3812(284, 124, c);
    F_3812(34, 124, c);
    F_3812(34, 26, c);
    F_352C(5, 4);
}

/* clear the frame area and draw the frame */
F_21C2(c)
int c;
{
    F_35CA(31, 23, 287, 127, 0);
    F_2168(c);
}

/* print string s at column x, row y */
F_21E7(x, y, s)
int x, y;
char *s;
{
    F_352C(x, y);
    F_3599(s, D_2046);
}

/* random -1, 0 or 1 */
F_2203()
{
    int r;

    r = F_4069(1, 215) % 3;
    if (r == 2)
        r = -1;
    return r;
}

/* redraw the whole dungeon level map */
F_2231()
{
    int y, x;

    for (y = 0; y < 38; y++)
        for (x = 0; x < 18; x++) {
            if (D_8BAE[D_AD42][y][x] < 0x33)
                F_B34F(D_8BAE[D_AD42][y][x], y, x);
            else
                F_B34F(1, y, x);
        }
    F_B34F(18, D_AD3E, D_AD40);
}

/* redraw all active entities */
F_22B8()
{
    int i;

    for (i = 0; i < 15; i++)
        if (D_ACC6[i].f0 != -1)
            F_B34F(D_ACC6[i].f0, D_ACC6[i].f2, D_ACC6[i].f4);
}

/* find the entity at (x, y) in the D_ACC6 table; -1 if none or dead */
F_22F3(x, y)
int x, y;
{
    int i, r;

    i = 0;
    r = -1;
    do {
        if (D_ACC6[i].f2 == x && D_ACC6[i].f4 == y)
            r = i;
        i++;
    } while (r == -1 && i < 15);
    if (r != -1 && D_ACC6[r].f0 == -1)
        r = -1;
    return r;
}

/* what is at map position (x, y): -1 outside, 18 player, else entity type or -1 */
F_2355(x, y)
int x, y;
{
    int i, r;

    r = i = 0;
    if (x < 0 || x > 37 || y < 0 || y > 17)
        return -1;
    if (D_AD3E == x && D_AD40 == y)
        r = 18;
    else
        do {
            if (D_ACC6[i].f2 == x && D_ACC6[i].f4 == y)
                r = D_ACC6[i].f0;
            i++;
        } while (r == 0 && i < 15);
    if (r == 0)
        r = -1;
    return r;
}

/* classify map position (x, y): 63 outside, entity code, -1 blocked, 0 free */
F_23E7(x, y)
int x, y;
{
    int t, r;

    if (x > 37 || x < 0 || y > 17 || y < 0)
        return 63;
    t = D_8BAE[D_AD42][x][y];
    if ((r = F_2355(x, y)) != -1)
        return r;
    if (t == 63)
        return 63;
    if (t == 1 || t >= 0x33)
        return -1;
    return 0;
}

/* is entity at offset (a, b) from the player a type 22 (guard)? */
F_2470(a, b)
int a, b;
{
    return F_2355(D_AD3E + a, D_AD40 + b) == 22;
}

/* is there a type 22 entity next to the player in any of 8 directions */
F_249D()
{
    return F_2470(-1, 1) || F_2470(-1, 0) || F_2470(-1, -1) || F_2470(0, 1)
        || F_2470(0, -1) || F_2470(-1, 0) || F_2470(1, 1) || F_2470(1, 0);
}

/* thief/jester entity m wanders; a thief at the player steals an item, jester sings */
F_252C(m)
int m;
{
    int dx, dy, r, i = 15, done = 0;

    if (!D_AD46) {
        dx = F_2203();
        dy = F_2203();
        if ((r = F_23E7(D_ACC6[m].f2 + dx, D_ACC6[m].f4 + dy)) == -1) {
            F_B34F(1, D_ACC6[m].f2, D_ACC6[m].f4);
            D_ACC6[m].f2 += dx;
            D_ACC6[m].f4 += dy;
            F_0476(4);
        } else if (r == 18) {
            do {
                if (D_20B8[i] && i != D_2082) {
                    D_20B8[i]--;
                    done = 1;
                }
                i--;
            } while (i > 0 && !done);
            if (F_4069(1, 255) < D_3A92 + 128 && done) {
                if (D_204A == 1)
                    F_3B46("Iolo stole something!");
                else
                    F_3B46(" The jester stole something!");
            }
        }
        if (done == 0) {
            if (F_4069(1, 255) < 15) {
                if (D_204A == 1) {
                    F_3B46("Iolo the Bard sings:");
                    F_3B46("Ho eyoh he hum!");
                } else {
                    F_3B46("Gwino the jester sings:");
                    F_3B46("I've got the key!");
                }
            }
        }
    }
}

/* wandering entity m moves randomly */
F_266C(m)
int m;
{
    int dx, dy;

    if (D_AD46 == 0) {
        dx = F_2203();
        dy = F_2203();
        if (F_23E7(D_ACC6[m].f2 + dx, D_ACC6[m].f4 + dy) == -1) {
            F_B34F(1, D_ACC6[m].f2, D_ACC6[m].f4);
            D_ACC6[m].f2 += dx;
            D_ACC6[m].f4 += dy;
            F_0476(4);
        }
    }
}

/* try to move entity m by (dx, dy); 1 if moved */
F_26E2(m, dx, dy)
int m, dx, dy;
{
    int r, x, y;

    x = D_ACC6[m].f2 + dx;
    y = D_ACC6[m].f4 + dy;
    if (x < 0 || x > 37 || y < 0 || y > 17)
        return 0;
    if ((r = F_23E7(x, y)) == -1 || r == 63) {
        F_B34F(1, D_ACC6[m].f2, D_ACC6[m].f4);
        D_ACC6[m].f2 += dx;
        D_ACC6[m].f4 += dy;
        return 1;
    }
    return 0;
}

/* entity m (jester/guard) moves randomly or away from the player */
F_2776(m)
int m;
{
    int dx, dy, moved = 0;

    if (!D_AD46)
        F_266C(m);
    else {
        dx = D_ACC6[m].f2 - D_AD3E;
        dy = D_ACC6[m].f4 - D_AD40;
        if (dx) {
            if (dx > 0)
                dx = -1;
            else
                dx = 1;
        }
        if (dy) {
            if (dy > 0)
                dy = -1;
            else
                dy = 1;
        }
        if (F_4069(1, 100) < 50) {
            if (dy)
                moved = F_26E2(m, 0, dy);
            if (moved == 0 && dx)
                moved = F_26E2(m, dx, 0);
        } else {
            if (dx)
                moved = F_26E2(m, dx, 0);
            if (moved == 0 && dy)
                moved = F_26E2(m, 0, dy);
        }
        if (moved)
            F_0476(4);
    }
}

/* guard m approaches the player; returns 1 if it moved */
F_286B(m)
int m;
{
    int dx, dy, adx, ady, moved;

    moved = 0;
    dx = D_ACC6[m].f2 - D_AD3E;
    dy = D_ACC6[m].f4 - D_AD40;
    adx = dx < 0 ? -dx : dx;
    ady = dy < 0 ? -dy : dy;
    if (adx < 2 && ady < 2)
        return 0;
    if (adx + ady < 13) {
        if (dx) {
            if (dx > 0)
                dx = -1;
            else
                dx = 1;
        }
        if (dy) {
            if (dy > 0)
                dy = -1;
            else
                dy = 1;
        }
        if (dx)
            moved = F_26E2(m, dx, 0);
        if (moved == 0 && dy)
            moved = F_26E2(m, 0, dy);
    }
    if (moved) {
        F_0476(4);
        return 1;
    }
    return 0;
}

/* guard m next to the player attacks him */
F_294E(m)
int m;
{
    int adx, ady, mx, dmg;

    adx = ABS(D_ACC6[m].f2 - D_AD3E);
    ady = ABS(D_ACC6[m].f4 - D_AD40);
    if (adx < 2 && ady < 2) {
        F_3B46("Attacked by guard!");
        F_0476(7);
        if (F_4069(1, 255) < D_3A94 / 2 + D_2086 * 8 + 56)
            F_3B46(D_01E1);
        else {
            F_3599("Hit!...", D_2046);
            dmg = F_4069(2, mx = D_206E[0] / 128 + 15);
            F_3C8B(dmg, 1, 32, D_2046, 1);
            F_3B46(D_01D8);
            F_0476(2);
            D_206E[0] -= dmg;
            F_4437();
        }
    }
}

/* per-turn update of all entities in the current dungeon level */
F_2A73()
{
    int i, list[10], n;

    n = 0;
    if (D_204E) {
        F_2231();
        D_204E = 0;
    }
    if (D_204A != D_8B9C)
        F_22B8();
    else {
        for (i = 0; i < 15; i++) {
            if (D_ACC6[i].f0 == 19)
                F_252C(i);
            else if (D_ACC6[i].f0 == 50)
                F_266C(i);
            else if (D_ACC6[i].f0 == 22)
                F_2776(i);
            else if (D_ACC6[i].f0 == 17 && D_AD46 && !F_286B(i))
                list[n++] = i;
        }
        F_22B8();
        if (D_AD46 && n)
            for (i = 0; n > i; i++) {
                F_294E(list[i]);
                if (D_206E[0] <= 0)
                    break;
            }
    }
}

/* ---- ou_2b6d ---- */
/* attack in direction dir (1-4) up to range squares: hit chance, damage to the monster met */
F_2B6D(dir, range, damage, chance)
unsigned char dir;
int range, damage, chance;
{
    int m, i, dx, dy, tile, ox, oy, t2;

    i = 1;
    F_0476(7);
    switch (dir) {
    case 3:
        dx = 0;
        dy = -1;
        break;
    case 4:
        dx = 0;
        dy = 1;
        break;
    case 2:
        dx = 1;
        dy = 0;
        break;
    case 1:
        dx = -1;
        dy = 0;
        break;
    }
    do {
        ox = dx * i;
        oy = i * dy;
        tile = F_2355(D_AD3E + ox, D_AD40 + oy);
        i++;
        t2 = F_23E7(D_AD3E + ox, D_AD40 + oy);
        if (t2 == 0x3f)
            t2 = -1;
    } while (i <= range && tile != 0x11 && tile != 0x16 && tile != 0x13 && tile != 0x32
             && tile != 0x14 && tile != 0x15 && t2 == -1);
    m = F_22F3(D_AD3E + ox, D_AD40 + oy);
    if (m == -1 || F_4069(1, 100) > chance) {
        F_3B46(D_01E1);
    } else {
        F_3599("Hit ", D_2046);
        switch (tile) {
        case 0x14:
            F_3599("King! ", D_2046);
            break;
        case 0x11:
            F_3599("Guard! ", D_2046);
            break;
        case 0x15:
            F_3599("Merchant! ", D_2046);
            break;
        case 0x16:
            F_3599("Princess! ", D_2046);
            break;
        case 0x13:
            if (D_204A == 1)
                F_3599("Bard! ", D_2046);
            else
                F_3599("Jester! ", D_2046);
            break;
        case 0x32:
            if (D_206C == 0)
                F_3599("Wench! ", D_2046);
            else
                F_3599("Lecher! ", D_2046);
            break;
        }
        D_AD46 = 1;
        F_0476(2);
        if (damage >= D_ACC6[m].f6) {
            F_3B46("Killed!");
            if (tile != 0x16) {
                D_207E++;
                F_450D();
            } else
                D_8B9E = 0;
            D_ACC6[m].f0 = -1;
            D_ACC6[m].f2 = 0x7f;
            F_B34F(1, D_AD3E + ox, D_AD40 + oy);
            if (tile == 0x11)
                D_207E += 14;
            if (tile == 0x13 && D_204A == 2) {
                F_3B46("Thou hast found a key!");
                D_8BA0 = 1;
            }
        } else {
            if (tile != 0x14 || D_8BA2 == 0)
                D_ACC6[m].f6 -= damage;
            F_3C8B(damage, 2, 0x20, D_2046, 1);
            F_3B46(D_01D8);
        }

    }
}

/* ---- ou_2b6d_b ---- */
/* step the player one square in direction dir (1-4): walk, leave the map, or bump */
F_2E22(dir)
unsigned char dir;
{
    int dx, dy, t;

    switch (dir) {
    case 3:
        dy = -1;
        dx = 0;
        break;
    case 4:
        dy = 1;
        dx = 0;
        break;
    case 2:
        dy = 0;
        dx = 1;
        break;
    case 1:
        dy = 0;
        dx = -1;
        break;
    }
    if (D_AD3E + dx < 0 || D_AD40 + dy < 0 || D_AD3E + dx > 37 || D_AD40 + dy > 17) {
        if (D_204A == 2 && F_249D())
            F_4BD6();
        D_204A = 0;
    } else {
        t = F_23E7(D_AD3E + dx, D_AD40 + dy);
        if (t == -1 || t == 63) {
            F_3B46(D_1DC8[dir]);
            F_B34F(1, D_AD3E, D_AD40);
            D_AD3E += dx;
            D_AD40 += dy;
            F_B34F(18, D_AD3E, D_AD40);
            F_0476(4);
        } else {
            F_3B46("Blocked!");
            F_0476(1);
        }
    }
}

/* transport shop sign number n (shuttle/ship dealer); returns 7 if n is 0 or 7, else 6 */
F_2F3B(n)
unsigned n;
{

    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(10, 4, "Scooter's Super Duper");
        F_21E7(13, 5, "Transport, Inc.");
        break;
    case 3:
        F_21E7(9, 4, "Quality Transport Ltd.");
        break;
    case 5:
        F_21E7(7, 4, "O.K. New and Used Transport");
        break;
    case 7:
        F_21E7(8, 4, "Sly Sam's Transportation");
        F_21E7(15, 5, "Specialists");
        break;
    }
    if (n == 0 || n == 7)
        return 7;
    return 6;
}

/* armour shop sign number n; returns 7 if n is 0 else 6 */
F_2FF6(n)
unsigned n;
{

    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(11, 4, "The Armour Shop of");
        F_21E7(8, 5, "Lord Eldric D'Charbonneux");
        break;
    case 1:
        F_21E7(15, 4, "Max's Armoury");
        break;
    case 2:
        F_21E7(13, 4, "The Iron Fist");
        break;
    case 3:
        F_21E7(10, 4, "The Hammer -n- Anvil");
        break;
    case 5:
        F_21E7(5, 4, "Defense Specialties Unlimited");
        break;
    case 7:
        F_21E7(11, 4, "Custom Armor Works");
        break;
    }
    if (n == 0)
        return 7;
    return 6;
}

/* weapon shop sign number n; returns 7 if n is 0 else 6 */
F_30C0(n)
unsigned n;
{

    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(11, 4, "The Weapon Shop of");
        F_21E7(8, 5, "Lord Eldric D'Charbonneux");
        break;
    case 1:
        F_21E7(11, 4, "The Tempered Steel");
        break;
    case 2:
        F_21E7(12, 4, "The Razor's Edge");
        break;
    case 3:
        F_21E7(10, 4, "Cold Steel Creations");
        break;
    case 4:
        F_21E7(12, 4, "The Bloody Blade");
        break;
    case 5:
        F_21E7(13, 4, "The Duelo Shop");
        break;
    case 6:
        F_21E7(12, 4, "Weaponry Supply");
        break;
    case 7:
        F_21E7(10, 4, "Cold Steel Creations");
        break;
    }
    if (n == 0)
        return 7;
    return 6;
}

/* magic shop sign number n; always returns 6 */
F_31B1(n)
unsigned n;
{
    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(9, 4, "Mad Chad's Magic Shoppe");
        break;
    case 1:
        F_21E7(12, 4, "Mystic Melinda's");
        break;
    case 2:
        F_21E7(13, 4, "Words of Power");
        break;
    case 3:
        F_21E7(5, 4, "Psychic Sam's Magical Emporium");
        break;
    case 7:
        F_21E7(11, 4, "The Unleashed Spell");
        break;
    }
    return 6;
}

/* food shop sign number n; always returns 6 */
F_3240(n)
unsigned n;
{
    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(5, 4, "Li'l Karelia's Finnish Grocery");
        break;
    case 1:
        F_21E7(8, 4, "Adventurer's Supply Post");
        break;
    case 3:
        F_21E7(13, 4, "The Brown Bag");
        break;
    case 4:
        F_21E7(9, 4, "Fresh Food Marketplace");
        break;
    case 5:
        F_21E7(11, 4, "Rations Unlimited");
        break;
    case 6:
        F_21E7(6, 4, "Fastest Freshest Food Market");
        break;
    case 7:
        F_21E7(8, 4, "Exploration Provisioners");
        break;
    }
    return 6;
}
