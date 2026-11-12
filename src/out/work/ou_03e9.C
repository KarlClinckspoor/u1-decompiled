/* opts: -ms -v -w -cs */
/*
 * OUT.EXE code 03E9-1769: save/load of the player file, sound effects, movement into
 * monsters/obstacles, pence, attack and spell commands, boarding vehicles.
 */

/* Origin library */
extern F_352C(), F_355D(), F_357C(), F_3599(), F_3B46(), F_3C8B(), F_3E2F(), F_3E85();
extern F_3E5E(), F_3EED(), F_3F62(), F_4558(), F_450D(), F_4602(), F_5085(), F_5643();
extern F_86E1(), F_4B73(), F_2E22(), F_2B6D();
extern F_B58E(), F_B665(), F_B70F(), F_B7CD(), F_B884();
extern int F_353A(), F_3C31(), F_3C8E(), F_3CBB(), F_4069(), F_40A8(), F_40FE();
extern int F_A363(), F_B545(), F_C2AF(), F_C2EC(), F_C31C();
extern unsigned char F_4161();
/* other game code (OUT3) and Lattice */
extern F_125C(), F_21C2(), F_21E7(), F_2141();
extern EXECL(), EXIT();

struct cell { int a, b, c, d; };

extern char *D_0180, *D_014E[];
extern int D_082C;
extern char D_238C[], D_2058[];
extern int D_0DA8, D_2046, D_204A, D_2082, D_2088, D_207C, D_207E;
extern int D_3A90, D_3A92, D_3A9A, D_3AAC, D_3AAE, D_3AB0;
extern int D_3AC6, D_3AC8, D_3ACA, D_20A4;
struct mon { int type, f2, x, y, f8, fa, fc, fe; };
extern struct mon D_3B2C[];
extern int D_3B22;
extern long D_3B24, D_8BA8;
extern int D_8B88, D_8B8A, D_8B8C, D_8B90, D_8BA6;
extern int D_AD3E, D_AD40, D_AD50, D_AD54;
extern char D_B11E[];
extern struct cell D_AD56[][11];
extern int D_0CD2[];
extern char *D_0DF8[], *D_0FA2[], *D_1DC8[];
extern unsigned char *D_0FD8[];

/* "blocked" messages (DATA 3712-37B9), indexed by F_048A */
static char *D_37AE[] = {
    "You can't walk on water!", "Mountains are impassable!",
    "Aircars can't pass woods!", "Frigates like water!", "Rafts like water!",
    "Blocked by a", "Can't move on land!"
};

/* per monster class damage/gold scale (DATA 368C) */
static int D_368C[] = { 10, 5, 4, 3, 2, 1, 4, 6, 8, 10, 1, 2, 4, 6, 8 };

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
        F_3B46("!");
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
            F_3B46("!");
            ok = 0;
        } else {
            F_3599("! ", D_2046);
            ok = 1;
        }
        if (ones) {
            F_3C8B(ones, 1, ' ', D_2046, 1);
            F_3599(" copper!", D_2046);
            if (ok) {
                F_3B46("");
                ok = 0;
            } else
                ok = 1;
        }
        if (tens) {
            F_3C8B(tens, 2, ' ', D_2046, 1);
            F_3599(" silver!", D_2046);
            if (ok) {
                F_3B46("");
                ok = 0;
            } else
                ok = 1;
        }
        if (thou) {
            F_3C8B(thou, 2, ' ', D_2046, 1);
            F_3599(" gold!", D_2046);
            if (ok) {
                F_3B46("");
                ok = 0;
            } else
                ok = 1;
        }
        if (ok)
            F_3B46("");
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
            F_3599("Hit! ", D_2046);
        if (D_AD56[x][y].d <= dmg) {
            F_3C31(D_AD56[x][y].b);
            if (dmg == 10000)
                F_3B46(" destroyed!");
            else
                F_3B46(" killed!");
            F_08EE(D_AD56[x][y].b);
            r1 = F_4069(2, D_AD50 * 3 + 10 + D_AD56[x][y].b);
            F_3599("Thou dost find: ", D_2046);
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
            F_3B46(" damage!");
            D_AD56[x][y].d -= dmg;
        }
    } else
        F_3B46("Missed!");
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

    F_3B46("");
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
            F_3B46("Miss!");
        else if (F_4069(1, 100) <= hit) {
            cls = F_353A(F_0742(t));
            x = F_C2EC(ox + 9);
            y = F_C31C(oy + 4);
            F_355D(tile, ox + 9, oy + 4);
            id = F_B545(x, y, 1);
            D_3B2C[id].f8 -= dmg;
            if (D_3B2C[id].f8 <= 0)
                F_3599("Killed the ", D_2046);
            else
                F_3599("Hit the ", D_2046);
            F_3599(F_0742(t), D_2046);
            if (D_3B2C[id].f8 > 0) {
                if (cls > 6)
                    F_3B46("!");
                else
                    F_3599("! ", D_2046);
                F_3C8B(dmg, 2, ' ', D_2046, 1);
                F_3B46(" damage");
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
            F_3599("Missed the ", D_2046);
            F_3599(F_0742(t), D_2046);
            F_3B46("!");
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
        F_3B46("nothing");
    else {
        F_3B46(D_1DC8[c]);
        F_0FEF(c, b);
    }
}

/* Attack command: weapon name, "?" if it has no range, else aim and shoot */
F_10C2(a)
int a;
{
    F_3599("Attack with ", D_2046);
    F_3599(D_0DF8[D_2082], D_2046);
    if (D_0CD2[D_2082] == 0) {
        F_3B46("?");
        F_0476(1);
    } else if (D_204A == 3)
        F_0C52();
    else {
        F_3599(": ", D_2046);
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
    F_3599("Thou dost find: ", D_2046);
    F_0761(F_4069(3, D_AD50 * D_AD50 + 9), 0);
}

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
            F_3599("Coffin", D_2046);
            F_1351();
        } else
            f = 1;
        break;
    case 2:
        if (D_AD56[D_AD3E][D_AD40].c == 4) {
            F_3599("Chest", D_2046);
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
            F_3B46("Ladder created!");
        } else
            f = 1;
        break;
    case 6:
        if (((D_AD3E & 1) || (D_AD40 & 1)) && D_AD56[D_AD3E][D_AD40].a != 8) {
            D_AD56[D_AD3E][D_AD40].a = 7;
            F_3B46("Ladder created!");
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
        F_3B46("Teleported!");
        break;
    case 8:
        i = D_AD3E + F_3C8E();
        j = D_AD40 + F_3CBB();
        if (D_AD56[i][j].a == 0 && D_AD56[i][j].b == -1 && D_AD56[i][j].c == 0) {
            D_AD56[i][j].a = 8;
            F_3B46("Field created!");
        } else
            f = 1;
        break;
    case 9:
        i = D_AD3E + F_3C8E();
        j = D_AD40 + F_3CBB();
        if (D_AD56[i][j].a == 8) {
            D_AD56[i][j].a = 0;
            F_3B46("Field destroyed!");
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
        F_3B46("Failed!");
    } else if (a != 10 && a != 3)
        F_0476(5);
}
