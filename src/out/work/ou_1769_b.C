/* opts: -ms -v -w -cs */
/*
 * OUT.EXE code 1D71-2B6D (part 1): drop lists, map/entity helpers.
 */

/* Origin library */
extern F_352C(), F_357C(), F_3599(), F_35CA(), F_3812(), F_39FC(), F_B34F();
#define ABS(x) ((x) < 0 ? -(x) : (x))
extern F_3C8B(), F_4437(), F_3B46(), F_0476(), F_1CF6(), F_1D35(), F_1BC0();
extern int F_4069();
extern unsigned char F_39C5(), F_3862(), F_33BB();
extern unsigned char F_413C();

struct ent { int f0, f2, f4, f6; };

extern int D_204E, D_8B9C, D_AD46, D_3A92, D_3A94, D_206E, D_2046, D_204A, D_2082, D_2086, D_AD3E, D_AD40, D_AD42;
extern int D_20B8[], D_20AC[];
extern struct ent D_ACC6[], D_31DC[][15];
extern unsigned char D_8BAE[][38][18];
extern char *D_0D72[], *D_0F5C[];

/* Weapon drop list: choose a weapon letter and drop one */
F_1D71()
{
    int i, y, n;
    unsigned char c;

    n = 0;
    c = 'b';
    F_3599("weapon: ", D_2046);
    for (i = 1; i <= 15; i++)
        if (D_20B8[i])
            n++;
    if (n == 0) {
        F_3599("none", D_2046);
    } else {
    F_1CF6();
    y = 10 - n / 2;
    n = 0;
    for (i = 1; i <= 15; c++, i++) {
        if (D_20B8[i]) {
            F_352C(15, y + n);
            F_357C(c, D_2046);
            F_3599(") ", D_2046);
            F_3599(D_0D72[i], D_2046);
            n++;
        }
    }
    F_352C(14, 24);
    c = F_33BB(F_39C5(D_2046)) - 'a';
    if (c < 1 || c > 15)
        F_3599("none", D_2046);
    else if (D_20B8[c] == 0)
        F_3599("none", D_2046);
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
        F_3599("none", D_2046);
    } else {
    F_1CF6();
    y = 10 - n / 2;
    n = 0;
    for (i = 1; i <= 5; c++, i++) {
        if (D_20AC[i]) {
            F_352C(13, y + n);
            F_357C(c, D_2046);
            F_3599(") ", D_2046);
            F_3599(D_0F5C[i], D_2046);
            n++;
        }
    }
    F_352C(14, 24);
    c = F_33BB(F_39C5(D_2046)) - 'a';
    if (c < 1 || c > 5)
        F_3599("none", D_2046);
    else if (D_20AC[c] == 0)
        F_3599("none", D_2046);
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
            F_3599("nothing", D_2046);
        }
    }
    F_3B46("");
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
            F_3B46("Missed!");
        else {
            F_3599("Hit!...", D_2046);
            dmg = F_4069(2, mx = D_206E / 128 + 15);
            F_3C8B(dmg, 1, 32, D_2046, 1);
            F_3B46(" damage!");
            F_0476(2);
            D_206E -= dmg;
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
                if (D_206E <= 0)
                    break;
            }
    }
}
