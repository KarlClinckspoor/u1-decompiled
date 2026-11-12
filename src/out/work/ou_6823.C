/* opts: -ms -v -w -cs */
/*
 * OUT.EXE code 6823-7C31 (agent ou_6823): shop sell/buy lists (armour, weapons, spells...),
 * king's quest, chest.
 */
extern F_352C(), F_357C(), F_3599(), F_3B46(), F_3C8B(), F_21C2(), F_21E7(), F_2141();
extern F_4558(), F_5E88(), F_5E6C(), F_5EA4(), F_44A2(), F_4437(), F_0476(), F_0761();
extern F_3AE2(), F_3E5E(), F_B502();
extern F_6823(), F_69E8(), F_6BC7(), F_7052(), F_7526(), F_7738(), F_769D();
extern F_6A77(), F_6DE3(), F_665D(), F_6C53(), F_622C(), F_70CF(), F_7377(), F_794E();
extern F_67F1(), F_680C(), F_6BAC(), F_7034(), F_78AF(), F_767D();
extern F_31B1(), F_2FF6(), F_3240(), F_2F3B();
extern int F_B0B5(), F_C2AF(), F_353A(), F_4069(), F_40FE();
extern unsigned char F_39C5(), F_3862(), F_33BB();

struct cell { int a, b, c, d; };

extern int D_0DA8, D_2046, D_204A, D_204E, D_206A, D_206E, D_207C, D_2080;
extern int D_20AC[], D_20D8[], D_20EE[], D_2086;
extern int D_3A90, D_3A92, D_3A96, D_3A98, D_3A9A, D_3AAC, D_3AAE, D_3AB2[];
extern int D_20A4, D_3AC6, D_3AC8, D_3ACA, D_3B1A, D_3B28, D_3B2A;
extern int D_8B92, D_8B94, D_8B96, D_8B98, D_8BA4;
extern long D_8BA8;
extern int D_AD3E, D_AD40, D_AD42, D_AD46, D_AD4C, D_AD50;
extern char D_2058[];
extern char *D_0F5C[], *D_0F04[], *D_0FA2[];
extern char *D_30D0, *D_30D4, *D_30D8, *D_30DC;
extern unsigned char D_8BAE[][38][18];
extern struct cell D_AD56[][11];

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
        F_3599("nothing", D_2046);
    } else {
        for (i = 1; i <= 5; c++, i++) {
            if (D_20AC[i]) {
                F_352C(9, y + row);
                F_357C(c, D_2046);
                F_3599(") ", D_2046);
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
            F_3599("nothing", D_2046);
        } else if (D_20AC[c - 'A'] == 0) {
            F_3599("nothing", D_2046);
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
    F_3599(") ", D_2046);
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
            F_3599("nothing", D_2046);
        } else if (F_67F1(c - 'a') > D_207C) {
            F_3599("nothing", D_2046);
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
    F_3B46("");
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
    F_3599(") ", D_2046);
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
        F_3599("nothing", D_2046);
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
            F_3599("nothing", D_2046);
        } else if (((c - 'a') & 1) == (D_AD42 & 1)) {
            F_3599("nothing", D_2046);
        } else if (F_6BAC(c - 'a') > D_207C) {
            F_3599("nothing", D_2046);
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
    F_3B46("");
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
        F_3599("nothing", D_2046);
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
            F_3599("nothing", D_2046);
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
    F_3B46("");
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
    F_3599(") ", D_2046);
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
    unsigned char x[6];
    unsigned char c;

    shop = F_2F3B(D_AD42 - 2);
    if (cmd == 'S') {
        F_21E7(9, shop + 2, "Sorry, we don't deal in");
        F_21E7(15, shop + 3, "used stuff.");
        F_2141();
        F_352C(16, 24);
        F_3599("nothing", D_2046);
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
                F_3599("nothing", D_2046);
            } else if (x[c - 'A'] == 0) {
                F_3599("nothing", D_2046);
            } else {
                cost = F_7034(c + 1 - 'A');
                if (cost > D_207C) {
                    F_3599("nothing", D_2046);
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
    F_3B46("");
}

/* Transact command in town: at a shop counter (tile 54-62) buy or sell, dispatch by shop type */
F_7377()
{
    int t;
    unsigned char c;

    F_3599("Transact", D_2046);
    if (D_AD46) {
        F_3B46("");
        F_3B46("None will talk to thee!");
        return;
    }
    t = D_8BAE[D_AD42][D_AD3E][D_AD40];
    if (t < 54 || t > 62) {
        F_3B46("");
        F_3B46("Thou art not by a counter!");
        F_0476(1);
        return;
    }
    F_3599("-Buy, Sell: ", D_2046);
    c = F_3862(F_39C5(D_2046));
    if (c != 'B' && c != 'S') {
        F_352C(9, 24);
        F_3B46(" nothing      ");
        return;
    }
    if (c == 'B') {
        F_352C(13, 24);
        F_3599("         ", D_2046);
        F_352C(13, 24);
        F_3599(": ", D_2046);
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
            F_21E7(9, 8, D_30D0);
            break;
        case 5:
            F_21E7(9, 8, D_30D4);
            break;
        case 3:
            F_21E7(10, 8, D_30D8);
            break;
        case 1:
            F_21E7(8, 8, D_30DC);
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
        return;
    }
    if (D_AD46) {
        F_3B46("He rejects thine offer!");
        return;
    }
    if (F_78AF(k))
        return;
    F_3B46("Dost thou offer pence");
    F_3599("or service: ", D_2046);
    c = F_3862(F_39C5(D_2046));
    if (c != 'P' && c != 'S') {
        F_3B46("neither");
        return;
    }
    switch (c) {
    case 'P':
        F_3B46("pence");
        F_3599("How much? ", D_2046);
        amt = F_B0B5(11, 24);
        if (amt == 0) {
            F_352C(11, 24);
            F_3B46("none");
            return;
        }
        if (amt > D_207C) {
            F_3B46("");
            F_3B46("Thou hast not that much!");
            F_0476(1);
            return;
        }
        D_207C -= amt;
        amt *= 3;
        amt /= 2;
        F_3B46("");
        F_3B46("In return I give unto");
        F_3599("thee ", D_2046);
        F_3C8B(amt, 1, 32, D_2046, 1);
        F_3B46(" hit points");
        D_206E += amt;
        F_4437();
        F_4558();
        break;
    case 'S':
        if (F_767D()) {
            F_3B46("");
            F_3B46("Thou art on a quest");
            F_3B46("for me already!");
        } else
            F_7526();
        break;
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
        F_3B46(" - none here!");
        F_0476(1);
        return;
    }
    F_3B46("");
    if (lvl > D_3A92 && D_206A != 3) {
        F_3599("Thou hast set off a trap!", D_2046);
        F_0476(2);
        F_3B46("");
        D_206E -= D_AD50;
        F_4437();
    } else {
        D_AD56[D_AD3E][D_AD40].c = 0;
        F_3599("Thou dost find: ", D_2046);
        F_0761(F_4069(3, D_AD50 * D_AD50 + 9), 0);
    }
}
