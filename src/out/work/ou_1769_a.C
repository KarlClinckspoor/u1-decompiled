/* opts: -ms -v -w -cs */
/*
 * OUT.EXE code 1769-1D71: spells, offering pence, the drop menu frame.
 */

/* Origin library */
extern F_352C(), F_355D(), F_357C(), F_3599(), F_35CA(), F_387E(), F_3920();
extern F_39FC(), F_3B46(), F_4437(), F_44A2(), F_4558();
extern int F_4069();
extern unsigned char F_4161();
extern int F_A363(), F_B0B5();
extern F_B58E();
extern int F_8194();
/* other game code */
extern F_0476(), F_0CF3(), F_13A8();
extern int F_12F2();

struct mon { int type, f2, x, y, f8, fa, fc, fe; };

extern int D_0DA6, D_0DA8, D_2046;
extern int D_204A, D_204E, D_206A, D_206E[], D_2080, D_2084, D_207C;
extern int D_20B8[], D_20D8[], D_3AFA[];
extern int D_3A98, D_3AAC, D_3AAE, D_8BA6;
extern int D_AD3E, D_AD40, D_AD42;
extern unsigned char D_8BAE[][38][18];
extern struct mon D_3B2C[];
extern char *D_0FD8[], *D_1DC8[], *D_0F04[];

/* first spell (Prayer): heals or feeds when low, else may remove a nearby monster */
F_1769()
{
    int i, done;
    struct mon *m;

    i = 1;
    done = 0;
    F_3B46("");
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
    F_3599(": ", D_2046);
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
        F_3B46("Failed!");
        F_0476(6);
    } else {
        F_0476(5);
        F_3B46("");
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
            F_3B46("");
            F_3B46("You've used up that spell!");
            F_0476(6);
        } else {
            D_20D8[D_2084]--;
            if (D_204A == 3)
                F_13A8(D_2084, 1);
            else if (D_2084 != 3 && D_2084 != 10) {
                F_3B46("");
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

/* drop pence: in a dungeon an offering, at a shrine tile hit points and a gem */
F_1BC0()
{
    int n, t;

    F_3599("pence: ", D_2046);
    if ((n = F_B0B5(12, 24)) == 0) {
        F_352C(13, 24);
        F_3599("none", D_2046);
    } else {
        F_3B46("");
        if (n > D_207C) {
            F_3599("Thou hast not that much!", D_2046);
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
                    F_3599("Alakazot!", D_2046);
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
