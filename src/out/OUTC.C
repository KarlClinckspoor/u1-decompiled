/* opts: -ms -v -w -cs */
/*
 * OUT.EXE - the game, part C of 3 (game code 63F2-908B: weapon/armour/spell shops, the king, chests, unlock, monsters, main loop (F_63F2-F_1853)).
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

extern char D_018D[];
extern char D_019B[];
extern char D_01AC[];
extern char D_01C7[];
extern char D_01D8[];
extern char D_01E1[];
extern char D_020C[];
extern char D_0220[];
extern char D_0235[];
extern char D_0237[];
extern char D_03DF[];
extern char D_03E4[];
extern char D_0417[];
extern char D_0CE3[];
extern char D_0D20[];

#define MT ((int (*)[8])D_3B2C)

extern char *D_0172, *D_0180;
extern int D_0182;
/* weapon shop, selling side: list own weapons, sell one */
F_63F2(top)
int top;
{
    int i, n, y;
    unsigned char c, m;

    n = 0;
    y = 0;
    c = 'b';
    i = 1;
    while (i <= 15) {
        if (D_20B8[i])
            n++;
        i++;
    }
    if (n == 0) {
        F_21E7(5, top + 1, "Thou hast no weaponry to sell!");
        F_2141();
        F_352C(16, 24);
        F_3599(D_0220, D_2046);
    } else {
        i = 1;
        do {
            if (D_20B8[i]) {
                F_352C(9, top + y);
                F_357C(c, D_2046);
                F_3599(D_0417, D_2046);
                F_3599(D_0D72[i], D_2046);
                F_352C(26, top + y);
                F_3599("- ", D_2046);
                F_3C8B(F_63D4(i), 3, ' ', D_2046, 1);
                y++;
            }
            c++;
            i++;
        } while (i <= 15 && y < 8);
        m = F_3862(c) - 1;
        F_352C(16, 24);
        c = F_3862(F_39C5(D_2046));
        if (c < 'B' || c > m)
            F_3599(D_0220, D_2046);
        else if (D_20B8[c - 'A'] == 0)
            F_3599(D_0220, D_2046);
        else {
            D_207C += F_63D4(c - 'A');
            D_20B8[c - 'A']--;
            if (D_2082 == c - 'A')
                D_2082 = 0;
            F_3599(D_0D72[c - 'A'], D_2046);
            F_4558();
            F_5E88();
        }
    }
}

/* print one line of the weapons-for-sale list: letter, name, price */
F_65CE(top, i, cnt)
int top, i, cnt;
{
    unsigned char c;

    c = i + 'a';
    F_352C(9, top + cnt);
    F_357C(c, D_2046);
    F_3599(D_0417, D_2046);
    F_3599(D_0D72[i], D_2046);
    F_352C(26, top + cnt);
    F_3599("- ", D_2046);
    F_3C8B(F_63B3(i), 3, ' ', D_2046, 1);
}

/* weapon shop: sell (arg 'S') or buy a weapon */
F_665D(a)
char a;
{
    int r, n;
    unsigned char c, lo, hi;
    int cnt;

    cnt = 0;
    r = F_30C0(D_AD42 - 2);
    if (a == 'S')
        F_63F2(r);
    else {
        lo = ((D_AD42 - 1) & 1) + 'b';
        n = D_8BA8 % 0x7FFFL / 1500L;
        if (n > 3 || D_8BA8 > 3000L)
            n = 3;
        n++;
        hi = lo + n * 2;
        n = lo;
        while (hi >= n) {
            F_65CE(r, n - 'a', cnt);
            cnt++;
            n += 2;
        }
        F_352C(15, 24);
        c = F_33BB(F_39C5(D_2046));
        if (c < lo || c > hi)
            F_3599(D_0220, D_2046);
        else if (((c - 'a') & 1) != (D_AD42 & 1))
            F_3599(D_0220, D_2046);
        else if (F_63B3(c - 'a') > D_207C) {
            F_3599(D_0220, D_2046);
            F_5EA4();
        } else {
            D_207C -= F_63B3(c - 'a');
            if (D_20B8[c - 'a'] < 255)
                D_20B8[c - 'a']++;
            F_3599(D_0D72[c - 'a'], D_2046);
            F_4558();
            F_5E6C();
        }
    }
    F_3B46(D_019B);
}

/* quarter of (200 - D_3A9A) times n */
F_67F1(n)
int n;
{
    return (200 - D_3A9A) / 4 * n;
}

/* quarter of D_3A96 times n */
F_680C(n)
int n;
{
    return D_3A96 / 4 * n;
}

/* ---- ou_6823 ---- */
/* armour shop, sell side: list owned armour, sell the chosen one for its price */
F_6823(y)
int y;
{
    int i, n, row;
    unsigned char c;

    n = 0;
    i = row = 1;
    c = 'b';
    for (i = 1; i <= 5; i++)
        if (D_20AC[i])
            n++;
    if (n == 0) {
        F_21E7(5, y + 1, "  Thou hast no armour to sell!");
        F_2141();
        F_352C(16, 24);
        F_3599(D_0220, D_2046);
    } else {
        for (i = 1; i <= 5; c++, i++) {
            if (D_20AC[i]) {
                F_352C(9, y + row);
                F_357C(c, D_2046);
                F_3599(D_0417, D_2046);
                F_3599(D_0F5C[i], D_2046);
                F_352C(26, y + row);
                F_3599("- ", D_2046);
                F_3C8B(F_680C(i), 3, 32, D_2046, 1);
                row++;
            }
        }
        F_352C(16, 24);
        c = F_3862(F_39C5(D_2046));
        if (c < 'B' || c > 'F') {
            F_3599(D_0220, D_2046);
        } else if (D_20AC[c - 'A'] == 0) {
            F_3599(D_0220, D_2046);
        } else {
            D_207C += F_680C(c - 'A');
            D_20AC[c - 'A']--;
            if (D_2086 == c - 'A')
                D_2086 = 0;
            F_3599(D_0F5C[c - 'A'], D_2046);
            F_4558();
            F_5E88();
        }
    }
}

/* armour shop: print list line i (letter, name, price) at row y+i */
F_69E8(y, i)
int y, i;
{
    unsigned char c;

    c = i + 'a';
    F_352C(9, y + i);
    F_357C(c, D_2046);
    F_3599(D_0417, D_2046);
    F_3599(D_0F5C[i], D_2046);
    F_352C(26, y + i);
    F_3599("- ", D_2046);
    F_3C8B(F_67F1(i), 3, 32, D_2046, 1);
}

/* armour shop (buy 'B' / sell 'S') */
F_6A77(cmd)
unsigned char cmd;
{
    int shop;
    unsigned char c, max;

    max = 'd';
    shop = F_2FF6(D_AD42 - 2);
    if (cmd == 'S') {
        F_6823(shop);
    } else {
        F_69E8(shop, 1);
        F_69E8(shop, 2);
        F_69E8(shop, 3);
        if (D_8BA8 > 3000L) {
            F_69E8(shop, 4);
            F_69E8(shop, 5);
            max = 'f';
        }
        F_352C(15, 24);
        c = F_33BB(F_39C5(D_2046));
        if (c < 'b' || c > max) {
            F_3599(D_0220, D_2046);
        } else if (F_67F1(c - 'a') > D_207C) {
            F_3599(D_0220, D_2046);
            F_5EA4();
        } else {
            D_207C -= F_67F1(c - 'a');
            if (D_20AC[c - 'a'] < 255)
                D_20AC[c - 'a']++;
            F_3599(D_0F5C[c - 'a'], D_2046);
            F_4558();
            F_5E6C();
        }
    }
    F_3B46(D_019B);
}

/* weapon price for item i: (200 - D_3A98) / 32 * i */
F_6BAC(i)
int i;
{
    return (200 - D_3A98) / 32 * i;
}

/* weapon shop: print list line i (letter, name, price) at row y+row */
F_6BC7(y, i, row)
int y, i, row;
{
    unsigned char c;

    F_352C(9, y + row);
    c = i + 'a';
    F_357C(c, D_2046);
    F_3599(D_0417, D_2046);
    F_3599(D_0F04[i], D_2046);
    F_352C(26, y + row);
    F_3599("- ", D_2046);
    F_3C8B(F_6BAC(i), 3, 32, D_2046, 1);
}

/* weapon shop (buy; we do not buy used weapons) */
F_6C53(cmd)
unsigned char cmd;
{
    int shop;
    unsigned i;
    unsigned char c, first, last;
    int row;

    row = 0;
    shop = F_31B1(D_AD42 - 2);
    if (cmd == 'S') {
        F_21E7(5, shop + 2, "  Sorry, we don't buy spells!");
        F_2141();
        F_352C(16, 24);
        F_3599(D_0220, D_2046);
    } else {
        i = D_AD42 & 1;
        first = i + 'b';
        last = i + 'f';
        if (D_206A == 2)
            last = i + 'j';
        for (i = first; last >= i; i += 2) {
            if (i != 'e') {
                F_6BC7(shop, i - 'a', row);
                row++;
            }
        }
        F_352C(15, 24);
        c = F_33BB(F_39C5(D_2046));
        if (c < first || c > last) {
            F_3599(D_0220, D_2046);
        } else if (((c - 'a') & 1) == (D_AD42 & 1)) {
            F_3599(D_0220, D_2046);
        } else if (F_6BAC(c - 'a') > D_207C) {
            F_3599(D_0220, D_2046);
            F_5EA4();
        } else {
            D_207C -= F_6BAC(c - 'a');
            if (D_20D8[c - 'a'] < 255)
                D_20D8[c - 'a']++;
            F_3599(D_0F04[c - 'a'], D_2046);
            F_4558();
            F_5E6C();
        }
    }
    F_3B46(D_019B);
}

/* food shop: buy packs of 10 food (no selling) */
F_6DE3(cmd)
unsigned char cmd;
{
    int n, cost;

    n = F_3240(D_AD42 - 2);
    if (cmd == 'S') {
        F_21E7(9, n + 2, "Used food?  No thanks!");
        F_2141();
        F_352C(15, 24);
        F_3599(D_0220, D_2046);
    } else {
        cost = 5 - D_3A9A / 20;
        F_352C(5, n + 2);
        F_3599(" Packs of 10 food cost ", D_2046);
        F_3C8B(cost, 1, 32, D_2046, 1);
        F_3599(" pence", D_2046);
        F_352C(8, n + 3);
        F_3599("each.  How many dost thou", D_2046);
        F_352C(11, n + 4);
        F_3599("wish to purchase?", D_2046);
        F_352C(15, 24);
        n = F_B0B5(15, 24);
        cost = cost * n;
        if (cost < 0)
            cost = D_207C + 1;
        if (n == 0 || cost > D_207C) {
            F_352C(15, 24);
            F_3599(D_0220, D_2046);
            if (cost > D_207C)
                F_5EA4();
        } else {
            F_3599(" packs food", D_2046);
            D_207C -= cost;
            F_4558();
            D_2080 += n * 10;
            F_44A2();
            F_5E6C();
        }
    }
    F_3B46(D_019B);
}

/* transport bought (k: 1-2 horse/cart, 3-4, 5-6): update counts, redraw one transport on the map */
F_6F58(k)
int k;
{
    int t, a, b, f;

    D_20EE[k]++;
    if (k <= 2 && D_8B98) {
        D_8B98--;
        t = 2;
    } else if (k == 4 || k == 3) {
        D_8B94--;
        t = 0;
    } else {
        t = 1;
        D_8B96--;
    }
    b = -1;
    do {
        a = -1;
        do {
            f = F_C2AF(D_3AAC + a, D_3AAE + b, 1) == t;
            a++;
        } while (a < 2 && !f);
        if (!f)
            b++;
        else
            a--;
    } while (b < 2 && !f);
    F_B502(k + 11 - 1 + (k > 4), t, D_3AAC + a, D_3AAE + b, 0);
}

/* transport price for item i: ((200 - D_3A9A) / 5 * i) * i */
F_7034(i)
int i;
{
    return (200 - D_3A9A) / 5 * i * i;
}

/* transport shop: print list line i (letter c, name, price) at row y */
F_7052(i, y, c)
int i, y;
unsigned char c;
{
    F_352C(12, y);
    F_357C(c, D_2046);
    F_3599(D_0417, D_2046);
    F_3599(D_0FA2[i], D_2046);
    F_352C(23, y);
    F_3599("- ", D_2046);
    F_3C8B(F_7034(i), 4, 32, D_2046, 0);
}

/* transport shop (buy; we do not deal in used stuff) */
F_70CF(cmd)
unsigned char cmd;
{
    int shop, i, cost, flag;
    char x[6];
    unsigned char c;

    shop = F_2F3B(D_AD42 - 2);
    if (cmd == 'S') {
        F_21E7(9, shop + 2, "Sorry, we don't deal in");
        F_21E7(15, shop + 3, "used stuff.");
        F_2141();
        F_352C(16, 24);
        F_3599(D_0220, D_2046);
    } else {
        for (i = 0; i < 6; i++)
            x[i] = 0;
        flag = D_8B96 || D_8B98 || D_8B94;
        if (!flag || (D_3B1A && D_8B92 == 15) || (!D_8B96 && D_8B92 == 15)) {
            F_21E7(11, shop + 2, "Closed for the day.");
            F_2141();
        } else {
            flag = D_3B1A == 0 && D_8B92 == 15;
            if ((D_8B98 || D_8B96) && !flag) {
                F_7052(1, shop, 'a');
                shop++;
                x[0]++;
                F_7052(2, shop, 'b');
                shop++;
                x[1]++;
            }
            if (D_8B94 && !flag) {
                F_7052(3, shop, 'c');
                shop++;
                x[2]++;
                F_7052(4, shop, 'd');
                shop++;
                x[3]++;
            }
            if (D_8BA8 > 3000L && D_8B96) {
                if (!flag) {
                    F_7052(5, shop, 'e');
                    shop++;
                    x[4]++;
                }
                if (D_3B1A == 0)
                    F_7052(6, shop, 'f');
                x[5]++;
            }
            F_352C(15, 24);
            c = F_3862(F_39C5(D_2046));
            if (c < 'A' || c > 'F') {
                F_3599(D_0220, D_2046);
            } else if (!x[c - 'A']) {
                F_3599(D_0220, D_2046);
            } else {
                cost = F_7034(c + 1 - 'A');
                if (cost > D_207C) {
                    F_3599(D_0220, D_2046);
                    F_5EA4();
                } else {
                    F_3599(D_0FA2[c + 1 - 'A'], D_2046);
                    D_207C -= cost;
                    F_4558();
                    D_8B92++;
                    F_5E6C();
                    if (c == 'F')
                        D_3B28 = D_3B2A = 1000;
                    F_6F58(c + 1 - 'A');
                }
            }
        }
    }
    F_3B46(D_019B);
}

/* Transact command in town: at a shop counter (tile 54-62) buy or sell, dispatch by shop type */
F_7377()
{
    int t;
    unsigned char c;

    F_3599("Transact", D_2046);
    if (D_AD46) {
        F_3B46(D_019B);
        F_3B46("None will talk to thee!");
    } else {
        t = D_8BAE[D_AD42][D_AD3E][D_AD40];
        if (t < 54 || t > 62) {
            F_3B46(D_019B);
            F_3B46("Thou art not by a counter!");
            F_0476(1);
        } else {
            F_3599("-Buy, Sell: ", D_2046);
            c = F_3862(F_39C5(D_2046));
            if (c != 'B' && c != 'S') {
                F_352C(9, 24);
                F_3B46(" nothing      ");
            } else {
                if (c == 'B') {
                    F_352C(13, 24);
                    F_3599("         ", D_2046);
                    F_352C(13, 24);
                    F_3599(D_0237, D_2046);
                } else {
                    F_352C(10, 24);
                    F_3599("            ", D_2046);
                    F_352C(10, 24);
                    F_3599("Sell: ", D_2046);
                }
                switch (t) {
                case 54:
                case 55:
                    F_6A77(c);
                    break;
                case 56:
                case 57:
                    F_6DE3(c);
                    break;
                case 58:
                case 59:
                    F_665D(c);
                    break;
                case 60:
                    F_6C53(c);
                    break;
                case 61:
                    F_622C(c);
                    break;
                case 62:
                    F_70CF(c);
                    break;
                }
                D_204E = 1;
            }
        }
    }
}

/* king's audience, quest given: show the quest text by king (D_AD4C) */
F_7526()
{
    F_21C2(D_0DA8);
    D_3AB2[D_AD4C] = -1;
    if (!(D_AD4C & 1)) {
        F_21E7(12, 6, "Go now and kill a");
        F_352C(12, 8);
        switch (D_AD4C) {
        case 2:
            F_3599(" Gelatinous Cube", D_2046);
            break;
        case 4:
            F_3599(" Carrion Creeper", D_2046);
            break;
        case 6:
            F_3599("     Lich", D_2046);
            break;
        case 8:
            F_3599("    Balron", D_2046);
            break;
        }
    } else {
        F_21E7(11, 6, "Go forth and find");
        switch (D_AD4C) {
        case 7:
            F_21E7(9, 8, D_307C[42]);
            break;
        case 5:
            F_21E7(9, 8, D_307C[44]);
            break;
        case 3:
            F_21E7(10, 8, D_307C[46]);
            break;
        case 1:
            F_21E7(8, 8, D_307C[48]);
            break;
        }
    }
    F_21E7(11, 10, "Do not return until");
    F_21E7(11, 11, "thy quest is done!");
    F_3AE2();
    D_204E = 1;
}

/* has the king's quest of king D_AD4C not been given yet (flag -1 set when given) */
F_767D()
{
    return D_3AB2[D_AD4C] == -1;
}

/* quest reward for a monster quest (odd kings): strength points */
F_769D()
{
    int n;

    n = (99 - D_3A90) / 8;
    if (n == 0 && D_3A90 != 99)
        n = 1;
    F_21E7(8, 8, "For this I will give thee");
    F_352C(10, 9);
    F_3C8B(n, 1, 32, D_2046, 1);
    F_3599(" point", D_2046);
    if (n != 1)
        F_357C('s', D_2046);
    F_3599(" of strength.", D_2046);
    D_3A90 += n;
    F_3AE2();
    D_204E = 1;
}

/* quest reward (even kings): a gem and a hint, by king type */
F_7738()
{
    int t;

    t = F_40FE(D_3AAC, D_3AAE);
    F_21E7(7, 8, "For this I shall tell thee");
    switch (t) {
    case 0:
        F_21E7(5, 9, "that it requires all four gems");
        F_21E7(5, 10, "  to launch a time machine!");
        F_21E7(5, 12, "The king gives thee a red gem.");
        D_20A4++;
        break;
    case 1:
        F_21E7(5, 9, "that the time machine must be");
        F_21E7(5, 10, "     used in order to win.");
        F_21E7(5, 12, "     The king gives thee a");
        F_21E7(5, 13, "           green gem!");
        D_3AC6++;
        break;
    case 2:
        F_21E7(5, 9, "   the princess will help a");
        F_21E7(5, 10, "   space ace through time!");
        F_21E7(5, 12, "    The king gives thee a");
        F_21E7(5, 13, "          blue gem!");
        D_3AC8++;
        break;
    case 3:
        F_21E7(5, 9, "   to take nine items from my");
        F_21E7(5, 10, "  storerooms - but only nine!");
        F_21E7(5, 12, "   The king also gives thee a");
        F_21E7(5, 13, "          white gem!");
        D_3ACA++;
        D_8BA4 = 9;
        break;
    }
    F_3E5E(10);
    F_3AE2();
    D_204E = 1;
}

/* quest completed: congratulate the player; returns 1 if it was (reward given), else 0 */
F_78AF()
{
    int n;

    n = 7 - F_353A(D_2058) / 2;
    if (D_3AB2[D_AD4C] != 1)
        return 0;
    D_3AB2[D_AD4C] = 0;
    F_21C2(D_0DA8);
    F_21E7(n + 5, 5, "Ah, ");
    F_3599(D_2058, D_2046);
    F_3599("!  Thou hast", D_2046);
    F_21E7(10, 6, "completed thy quest.");
    if (D_AD4C & 1)
        F_769D();
    else
        F_7738();
    return 1;
}

/* Transact with the king: offer pence (for hit points) or service (a quest) */
F_794E()
{
    unsigned char c;
    int amt, k;

    k = F_40FE(D_3AAC, D_3AAE) * 2 + D_AD42;
    F_3B46("Transact with king");
    if (D_8BAE[D_AD42][D_AD3E][D_AD40] != 62) {
        F_3B46("He is not here!");
    } else if (D_AD46) {
        F_3B46("He rejects thine offer!");
    } else if (F_78AF(k)) {
    } else {
        F_3B46("Dost thou offer pence");
        F_3599("or service: ", D_2046);
        c = F_3862(F_39C5(D_2046));
        if (c != 'P' && c != 'S') {
            F_3B46("neither");
        } else {
            switch (c) {
            case 'P':
                F_3B46("pence");
                F_3599("How much? ", D_2046);
                amt = F_B0B5(11, 24);
                if (amt == 0) {
                    F_352C(11, 24);
                    F_3B46(D_03DF);
                } else if (amt > D_207C) {
                    F_3B46(D_019B);
                    F_3B46(D_03E4);
                    F_0476(1);
                } else {
                    D_207C -= amt;
                    amt *= 3;
                    amt /= 2;
                    F_3B46(D_019B);
                    F_3B46("In return I give unto");
                    F_3599("thee ", D_2046);
                    F_3C8B(amt, 1, 32, D_2046, 1);
                    F_3B46(" hit points");
                    D_206E[0] += amt;
                    F_4437();
                    F_4558();
                }
                break;
            case 'S':
                if (F_767D()) {
                    F_3B46(D_019B);
                    F_3B46("Thou art on a quest");
                    F_3B46("for me already!");
                } else
                    F_7526();
                break;
            }
        }
    }
}

/* Transact command: dispatch by mode (D_204A 1 town, 2 castle) */
F_7B17()
{
    if (D_204A == 1)
        F_7377();
    else if (D_204A == 2)
        F_794E();
    else {
        F_3B46("Transact?");
        F_0476(1);
    }
}

/* View command: not available here */
F_7B4C()
{
    F_3B46("View?");
    F_0476(1);
}

/* open a chest in the dungeon cell: trap (if arg > D_3A92) or treasure */
F_7B63(lvl)
int lvl;
{
    F_3599(" chest", D_2046);
    if (D_AD56[D_AD3E][D_AD40].c != 4) {
        F_3B46(D_0D20);
        F_0476(1);
    } else {
    F_3B46(D_019B);
    if (lvl > D_3A92 && D_206A != 3) {
        F_3599("Thou hast set off a trap!", D_2046);
        F_0476(2);
        F_3B46(D_019B);
        D_206E[0] -= D_AD50;
        F_4437();
    } else {
        D_AD56[D_AD3E][D_AD40].c = 0;
        F_3599(D_01C7, D_2046);
        F_0761(F_4069(3, D_AD50 * D_AD50 + 9), 0);
    }
    }
}

/* ---- ou_7c31 ---- */
/* Unlock a castle door: needs the right key (tile 60/61 ahead), opens the door */
F_7C31()
{
    int t;

    t = D_8BAE[D_AD42][D_AD3E][D_AD40];
    if (t != 60 && t != 61) {
        F_3B46(D_0CE3);
        F_0476(1);
    } else {
        F_3B46(D_019B);
        if (D_8BA0 == 0) {
            F_3B46("Thou hast not a key!");
        } else if (t != D_AD4E) {
            F_3B46("Thou hast not");
            F_3B46("the correct key!");
        } else {
            F_3B46("The door is open!");
            D_8B9E = 1;
            D_8BAE[D_AD42][D_AD3E][D_AD40 - 1] = 1;
            F_B34F(1, D_AD3E, D_AD40 - 1);
        }
    }
}

/* 'U'nlock command: dungeon -> F_7B63(random), castle -> F_7C31, else "?" / " what?" */
F_7CF2()
{
    F_3599("Unlock", D_2046);
    if (D_204A == 3) {
        F_7B63(F_4069(1, 75));
    } else if (D_204A == 2) {
        F_7C31();
    } else if (D_204A == 0 || D_204A == 4) {
        F_3B46(D_0235);
        F_0476(1);
    } else if (D_204A == 1) {
        F_3B46(D_0CE3);
        F_0476(1);
    }
}

/* 'X'-it command: leave the vehicle (ship/aircraft) next to land */
F_7D6B()
{
    int t, i, j, f;

    f = 0;
    if (D_204A != 0) {
        F_3B46("X-it?");
        F_0476(1);
    } else if (D_2088 == 0) {
        F_3B46("X-it what?");
        F_0476(1);
    } else {
        t = F_C2AF(D_3AAC, D_3AAE, 1);
        for (i = -2; i <= 2; i++)
            for (j = -2; j <= 2; j++)
                if (F_A363(D_3AAC + i, D_3AAE + j))
                    f = 1;
        if (t > 2 || f == 0) {
            F_3B46(D_0172);
            F_0476(1);
        } else {
            F_B502(MT[0][0], D_8B31, D_3AAC, D_3AAE, 0);
            D_2088 = 0;
            MT[0][1] = MT[0][0];
            MT[0][0] = 10;
            F_3B46("X-it");
        }
    }
}

/* 'Pass' command */
F_7E5B()
{
    F_3B46("Pass");
}

/* unknown command: "Huh?" */
F_7E69()
{
    F_3B46("Huh?");
    F_0476(1);
}

/* spawn chance by experience (DGROUP 16CC) */
int D_16CC[10] = { 0x23, 0x37, 0x4B, 0x5F, 0x73, 0x87, 0x9B, 0xAF, 0xC3, 0xCD };

/* per-turn food counter on the overworld: eat food when counter reaches vehicle limit */
F_7E80()
{
    if (D_204A == 0) {
        if (D_8B8E >= D_36F8[D_2088]) {
            D_8B8E = 0;
            D_2080--;
            F_44A2();
        }
        D_8B8E++;
    }
}

/* is overworld x coordinate (after wrap) in range 9..158 */
F_7EB3(a)
int a;
{
    if (a > 83)
        a -= 84;
    if (a < 159 && a > 8)
        return 1;
    return 0;
}

/* is overworld y coordinate (after wrap) in range 6..149 */
F_7EE1(a)
int a;
{
    if (a > 77)
        a -= 78;
    if (a < 150 && a > 5)
        return 1;
    return 0;
}

/* maybe spawn a wandering monster near the player on the overworld */
F_7F0F()
{
    int m, r, t, x, y, n, k, lvl, id, hp, ok;

    r = F_4069(1, 255);
    if (r < D_16CC[D_207E / 1000]) {
        n = 0;
        m = F_4069(1, 8) * 2;
        lvl = F_4069(0, 149) / 10;
        r = D_3698[lvl];
        if (m > r)
            m = r;
        if (lvl < 4)
            t = 0;
        else if (lvl < 9)
            t = 2;
        else
            t = 1;
        id = lvl * 2 + 19;
        hp = F_40E9() + 1 + D_3698[lvl] * 10;
        do {
            x = F_4069(10, 15);
            y = F_4069(5, 15);
            if (F_4069(1, 16) & 1)
                x = -x;
            if (F_4069(1, 16) & 1)
                y = -y;
            x = F_C2EC(x + 9);
            y = F_C31C(y + 4);
            k = F_C2AF(x, y, 1);
            ok = F_7EB3(x) && F_7EE1(y);
        } while (n < 5 && k != t && !ok);
        if (k == t && ok) {
            F_B502(id, t, x, y, hp);
            D_8B90++;
        }
    }
}

/* can monster m (type m) move onto the overworld square at (dx,dy) relative to screen centre */
F_80A2(m, dx, dy)
int m, dx, dy;
{
    int a, b, t, k, x, y, res;

    a = 2;
    b = 1;
    res = 0;
    if (F_4069(1, 100) > D_36E8[D_2088])
        return 0;
    t = MT[m][0];
    if (t >= 19 && t <= 25) {
        a = b = 0;
    } else if (t == 31 || t == 35) {
        a = b = 2;
    }
    x = F_C2EC(9 - dx);
    y = F_C31C(4 - dy);
    k = F_C2AF(x, y, 0);
    if (a != b)
        res = k >= 4 && k <= 9;
    if ((a == k || k == b) && F_7EB3(x) && F_7EE1(y) || res)
        return 1;
    return 0;
}

/* distance test: returns reach (1 or 3) if (dx,dy) within it, else 0 */
F_8194(m, dx, dy)
int m, dx, dy;
{
    int a, b, v;

    v = 1;
    if (m == 23 || m == 25 || m == 31 || m == 47)
        v = 3;
    a = dx < 0 ? -dx : dx;
    b = dy < 0 ? -dy : dy;
    if ((a > b ? a : b) <= v)
        return v;
    return 0;
}

/* monster m shoots / attacks along (dx,dy): animates the shot and may hit the player */
F_81FC(m, dx, dy, n)
int m, dx, dy, n;
{
    int dmg, hit, k, x, y, sx, sy, i;

    sy = sx = 0;
    i = 1;
    F_355D(MT[m][0], 9 - dx, 4 - dy);
    if (dx) {
        if (dx > 0)
            sx--;
        else
            sx++;
    }
    if (dy) {
        if (dy > 0)
            sy--;
        else
            sy++;
    }
    F_3599(F_0742(MT[m][0]), D_2046);
    F_3599(" attacks!", D_2046);
    do {
        x = dx + sx * i;
        y = dy + sy * i;
        F_355D(50, 9 - x, 4 - y);
        if (i == 1)
            F_0476(3);
        else
            F_3E5E(2);
        k = F_C2AF(D_3AAC - x, D_3AAE - y, 0);
        F_355D(k, 9 - x, 4 - y);
        i++;
    } while (i <= n && k != 3 && (x != 0 || y != 0));
    hit = F_4069(1, 255) < 200 - D_3A94 / 2 - (D_2086 << 3);
    if (x || y || !hit) {
        F_355D(MT[0][0], 9, 4);
        if (F_353A(F_0742(MT[m][0])) > 8)
            F_3B46(D_019B);
        else
            F_357C(32, D_2046);
        F_3B46(D_01E1);
    } else {
        F_355D(50, 9, 4);
        dmg = F_4069(1, D_368C[(MT[m][0] - 19) / 2] * 2 + 1);
        F_0476(2);
        if (F_353A(F_0742(MT[m][0])) > 10) {
            F_3B46(D_019B);
            F_3599(D_01AC, D_2046);
        } else {
            F_3B46(" Hit!");
        }
        F_3C8B(dmg, 2, 32, D_2046, 1);
        F_3B46(D_020C);
        F_355D(MT[0][0], 9, 4);
        D_206E[0] -= dmg;
        F_4437();
    }
}

/* move monster m to screen offset (dx,dy): update its terrain and position */
F_8479(m, dx, dy)
int m, dx, dy;
{
    int a, b;

    a = F_C2EC(9 - dx);
    b = F_C31C(4 - dy);
    MT[m][1] = F_A363(a, b);
    MT[m][2] = a;
    MT[m][3] = b;
}

/* try to step monster m one square in y toward the player */
F_84D3(m, dx, dy)
int m, dx, dy;
{
    int s;

    s = 0;
    if (dy == 0)
        s = 0;
    else if (dy > 0)
        s = -1;
    else
        s = 1;
    if (s && F_80A2(m, dx, s + dy)) {
        F_8479(m, dx, s + dy);
        return 1;
    }
    return 0;
}

/* try to step monster m one square in x toward the player */
F_8536(m, dx, dy)
int m, dx, dy;
{
    int s;

    if (dx == 0)
        s = 0;
    else if (dx > 0)
        s = -1;
    else
        s = 1;
    if (s && F_80A2(m, dx + s, dy)) {
        F_8479(m, dx + s, dy);
        return 1;
    }
    return 0;
}

/* move monster m toward the player, trying x or y first at random */
F_8598(m, dx, dy)
int m, dx, dy;
{
    int r;

    r = 1;
    if (F_91D4(1, 10) & 1) {
        if (!F_84D3(m, dx, dy))
            r = F_8536(m, dx, dy);
    } else {
        if (!F_8536(m, dx, dy))
            r = F_84D3(m, dx, dy);
    }
}

/* overworld monsters' turn: attackers in range fire, others move toward the player */
F_8600()
{
    int q[10], t, dx, dy, n, i;

    n = 0;
    for (i = 1; i <= D_8BA6; i++) {
        t = MT[i][0];
        if (t >= 19 && t <= 47) {
            dx = D_3AAC - MT[i][2];
            dy = D_3AAE - MT[i][3];
            if (F_8194(t, dx, dy)) {
                q[n] = i;
                n++;
            } else {
                F_8598(i, dx, dy);
            }
        }
    }
    F_C3CA();
    while (n-- && D_206E[0] > 0) {
        i = q[n];
        dx = D_3AAC - MT[i][2];
        dy = D_3AAE - MT[i][3];
        F_81FC(i, dx, dy, F_8194(MT[i][0], dx, dy));
    }
}

/* remove monsters of overworld region a (kill list cleanup) */
F_86E1(a)
int a;
{
    int i;

    for (i = 1; i <= D_8BA6; i++) {
        if (MT[i][0] >= 19) {
            if (F_40FE(MT[i][2], MT[i][3]) == a) {
                F_B58E(MT[i][2], MT[i][3]);
                D_8B90--;
                i--;
            }
        }
    }
}

/* dungeon monster at offset (dx,dy) attacks the player (hit/miss, special effects) */
F_873D(dx, dy)
int dx, dy;
{
    int t, r, dmg, i, f;

    f = 1;
    if (D_AD56[D_AD3E][D_AD40].a != 8) {
        if (D_AD56[D_AD3E - dx][D_AD40 - dy].a != 3 ||
            (D_AD56[D_AD3E][D_AD40].a != 3 && D_AD56[D_AD3E][D_AD40].a != 1 &&
             D_AD56[D_AD3E][D_AD40].a != 2)) {
            t = D_AD56[D_AD3E - dx][D_AD40 - dy].b;
            F_3599("Attacked by ", D_2046);
            F_3C31(t);
            F_3B46(D_018D);
            F_0476(3);
            r = D_3A94 / 2 + (D_2086 << 3) + 56;
            if (F_4069(1, 255) <= r) {
                F_3B46(D_01E1);
                f = 0;
            } else {
                r = F_4069(1, 255);
                dmg = D_AD50 + t * t;
                if (dmg > 255)
                    dmg = F_4069(t + 1, 255);
                if (t == 9 && D_2086) {
                    F_3B46("Armor destroyed!");
                    D_20AC[D_2086]--;
                    f = D_2086 = 0;
                } else if (t == 16) {
                    F_3B46("A gremlin stole some food!");
                    D_2080 = D_2080 / 2;
                    F_44A2();
                    f = 0;
                } else if (t == 21 && r < 128) {
                    F_3B46("Mental attack!");
                    D_3A9A = D_3A9A / 2;
                    D_3A9A += 5;
                    f = 0;
                }
                if (t == 2) {
                    for (i = 1; i <= 15; i++) {
                        if (D_20B8[i] && i != D_2082) {
                            F_3599("Thief stole a", D_2046);
                            if (F_40A8(D_0D72[i][0]))
                                F_3599("n ", D_2046);
                            else
                                F_357C(32, D_2046);
                            if (F_353A(D_0D72[i]) > 11)
                                F_3B46(D_019B);
                            F_3B46(D_0D72[i]);
                            D_20B8[i]--;
                            i = 16;
                        }
                    }
                }
            }
            if (f) {
                F_0476(2);
                F_3599(D_01AC, D_2046);
                F_3C8B(dmg, 1, 32, D_2046, 1);
                F_3B46(D_01D8);
                D_206E[0] -= dmg;
                F_4437();
            }
        }
    }
}

/* dungeon: move monster at (x,y) one step by (dx,dy) if the square is free */
F_8A04(x, y, dx, dy)
int x, y, dx, dy;
{
    int t;

    t = D_AD56[x + dx][y + dy].a;
    if (t != 1 && t != 2 && t != 8 && D_AD56[x + dx][y + dy].b == -1) {
        D_AD56[x + dx][y + dy].b = D_AD56[x][y].b;
        D_AD56[x][y].b = -1;
        D_AD56[x + dx][y + dy].c += 1000;
        return 1;
    }
    return 0;
}

/* dungeon monsters' turn: adjacent ones attack, near ones move toward the player */
F_8AC9()
{
    int i, j, dx, dy, r, d;

    r = 0;
    for (i = 1; i <= 9; i++) {
        for (j = 1; j <= 9; j++) {
            if (D_AD56[i][j].b != -1 && D_AD56[i][j].c < 999) {
                dx = D_AD3E - i;
                dy = D_AD40 - j;
                d = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
                if (d == 1) {
                    F_873D(dx, dy);
                } else if (d < 7) {
                    dx = F_3C64(dx);
                    dy = F_3C64(dy);
                    if (dx)
                        r = F_8A04(i, j, dx, 0);
                    if (r == 0 && dy)
                        r = F_8A04(i, j, 0, dy);
                }
            }
            if (D_206E[0] <= 0 || D_2080 <= 0)
                break;
        }
    }
    for (i = 1; i <= 9; i++)
        for (j = 1; j <= 9; j++)
            if (D_AD56[i][j].c > 999)
                D_AD56[i][j].c -= 1000;
}

/* teleport the player to a random nearby land square of the same region (time-machine style hop) */
F_8C20()
{
    int t, k, x, y;

    t = F_40FE(D_3AAC, D_3AAE);
    do {
        do {
            x = D_3AAC + F_4069(3, 23) - 13;
            y = D_3AAE + F_4069(3, 23) - 13;
            if (x < 2 || x > 165)
                x = D_3AAC;
            if (y < 2 || y > 153)
                y = D_3AAE;
        } while (t != F_40FE(x, y));
        k = F_C2AF(x, y, 1);
    } while (k != 1 && k != 2);
    D_3AAC = x;
    D_3AAE = y;
    MT[0][0] = 10;
    MT[0][1] = k;
    MT[0][2] = D_3AAC;
    MT[0][3] = D_3AAE;
}

/* monsters' turn dispatch on game mode (0 world, 3 dungeon, 1/2 town/castle) */
F_8CEB()
{
    if (D_204A == 0)
        F_8600();
    else if (D_204A == 3)
        F_8AC9();
    else if (D_204A == 1 || D_204A == 2)
        F_2A73();
}

/* "Insert Ultima I diskette / Hit space to continue..." prompt */
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

/* OUT.EXE main: load map.bin/tcd.bin/data, init state, command loop */
F_1853(argc, argv)
int argc;
unsigned char **argv;
{
    unsigned char c;
    int cnt;

    F_1D0D(1);
    D_2048 = F_3862(argv[1][0]) == 'C';
    D_8B88 = argv[1];
    D_8B8A = argv[2];
    D_8B8C = argv[3];
    F_3F58(0);
    F_03E9(argv[2][0]);
    F_0441();
    F_3E2F("map.bin", D_3DAC, 13104);
    F_3E2F("tcd.bin", (char *)D_8BAE, 6840);
    F_3E2F(D_1DC8[D_082C + 23], 0xA666, 1632);
    D_8BA6 = D_3B22;
    D_8BA8 = D_3B24;
    D_8B8E = 1;
    D_204E = D_204A = D_8B9C = 0;
    D_8B90 = F_B5E1();
    D_8B92 = F_B617();
    F_37FD(D_3AAA + F_3EC4() % 999);
    if (D_206E[0] <= 0 || D_2080 <= 0)
        F_C47C();
    else
        F_C3CA();
    F_45A3();
    for (;;) {
        if (D_206E[0] <= 0 || D_2080 <= 0)
            F_C47C();
        cnt = 0;
        F_387E(0, 24);
        do {
            c = F_4127();
            cnt++;
            if (D_204A == 0)
                F_AF57(cnt);
        } while (c == 0 && cnt < 1500);
        if (c == 0)
            c = ' ';
        switch (c) {
        case 3:
        case 4:
        case 2:
        case 1:
            F_066F(c);
            break;
        case 'A':
            F_10C2(-1);
            break;
        case '8':
            F_10C2(3);
            break;
        case '2':
            F_10C2(4);
            break;
        case '6':
            F_10C2(2);
            break;
        case '4':
            F_10C2(1);
            break;
        case 'B':
            F_118D();
            break;
        case 'C':
            F_19FF();
            break;
        case 'D':
            F_204D();
            break;
        case 'E':
            F_52AB();
            break;
        case 'F':
            F_53DA();
            break;
        case 'G':
            F_5550();
            break;
        case 'H':
            F_559A();
            break;
        case 'I':
            F_564B();
            break;
        case 'K':
            F_581A();
            break;
        case 'N':
            F_58BD();
            break;
        case 'O':
            F_59D7();
            break;
        case 'Q':
            F_5B54();
            break;
        case 'R':
            F_43C8();
            break;
        case 'S':
            F_5DB1();
            break;
        case 'T':
            F_7B17();
            break;
        case 'U':
            F_7CF2();
            break;
        case 'V':
            F_7B4C();
            break;
        case 'X':
            F_7D6B();
            break;
        case 'Z':
            F_478E();
            D_204E = 1;
            F_387E(0, 24);
            break;
        case ' ':
            F_7E5B();
            break;
        case 27:
            D_0182 = 1;
        default:
            F_7E69();
        }
        if (D_204A == 0) {
            F_A554();
            if (D_8B90 <= 10)
                F_7F0F();
        }
        D_8BA8++;
        F_8CEB();
        F_7E80();
        if (D_204A == 3)
            F_414A();
        D_8B9C = D_204A;
    }
}
