/* opts: -ms -v -w -cs */
/*
 * OUT.EXE code 7C31-908B: Unlock/X-it/Pass commands, wandering-monster spawn,
 * dungeon/world monster movement and attacks, main game loop (F_1853).
 */
struct tl { int a, b, c, d; };
extern struct tl D_AD56[][11];
extern int D_AD3E, D_AD40, D_AD42, D_AD4E, D_AD50;
extern int D_204A, D_204E, D_2046, D_2048, D_2080, D_2082, D_2086, D_2088, D_206E, D_207E;
extern int D_3AAA, D_3AAC, D_3AAE, D_3A94, D_3A9A, D_3B22;
extern long D_3B24;
extern int D_3B2C[][8];
extern int D_16CC[], D_0FD8[], D_20AC[], D_20B8[], D_1DC8[];
#define D_368C (D_0FD8 + 0x654)
#define D_3698 (D_0FD8 + 0x65A)
#define D_36E8 (D_0FD8 + 0x682)
#define D_36F8 (D_0FD8 + 0x68A)
extern int D_8B8E, D_8B90, D_8B92, D_8B9C, D_8B9E, D_8BA0, D_8BA6, D_082C, D_0180[];
extern long D_8BA8;
extern unsigned char *D_8B88, *D_8B8A, *D_8B8C;
extern unsigned char D_8B31;
extern unsigned char D_8BAE[][38][18];
extern unsigned char *D_0D72[];
extern char *D_0172;
extern char D_3DAC[];
extern F_3B46(), F_0476(), F_B34F(), F_3599(), F_4069(), F_7B63(), F_C2AF(), F_A363();
extern F_B502(), F_44A2(), F_40E9(), F_C2EC(), F_C31C(), F_355D(), F_0742(), F_3E5E();
extern F_353A(), F_357C(), F_3C8B(), F_4437(), F_91D4(), F_C3CA(), F_40FE(), F_B58E();
extern F_3C31(), F_3C64(), F_40A8(), F_2A73(), F_3EED(), F_352C(), F_1D0D(), F_3F58();
extern F_03E9(), F_0441(), F_3E2F(), F_B5E1(), F_B617(), F_3EC4(), F_37FD(), F_C47C();
extern F_45A3(), F_387E(), F_AF57(), F_066F(), F_10C2(), F_118D(), F_19FF(), F_204D();
extern F_52AB(), F_53DA(), F_5550(), F_559A(), F_564B(), F_581A(), F_58BD(), F_59D7();
extern F_5B54(), F_43C8(), F_5DB1(), F_7B17(), F_7B4C(), F_478E(), F_414A(), F_A554();
extern F_7C31(), F_7CF2(), F_7D6B(), F_7E5B(), F_7E69(), F_7E80(), F_7EB3(), F_7EE1();
extern F_7F0F(), F_80A2(), F_8194(), F_81FC(), F_8479(), F_84D3(), F_8536(), F_8598();
extern F_8600(), F_86E1(), F_873D(), F_8A04(), F_8AC9(), F_8C20(), F_8CEB();
extern char F_39C5(), F_3862();
extern unsigned char F_4127();

/* Unlock a castle door: needs the right key (tile 60/61 ahead), opens the door */
F_7C31()
{
    int t;

    t = D_8BAE[D_AD42][D_AD3E][D_AD40];
    if (t != 60 && t != 61) {
        F_3B46(" what?");
        F_0476(1);
    } else {
        F_3B46("");
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
        F_3B46("?");
        F_0476(1);
    } else if (D_204A == 1) {
        F_3B46(" what?");
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
            F_B502(D_3B2C[0][0], D_8B31, D_3AAC, D_3AAE, 0);
            D_2088 = 0;
            D_3B2C[0][1] = D_3B2C[0][0];
            D_3B2C[0][0] = 10;
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
    t = D_3B2C[m][0];
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
    F_355D(D_3B2C[m][0], 9 - dx, 4 - dy);
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
    F_3599(F_0742(D_3B2C[m][0]), D_2046);
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
        F_355D(D_3B2C[0][0], 9, 4);
        if (F_353A(F_0742(D_3B2C[m][0])) > 8)
            F_3B46("");
        else
            F_357C(32, D_2046);
        F_3B46("Missed!");
    } else {
        F_355D(50, 9, 4);
        dmg = F_4069(1, D_368C[(D_3B2C[m][0] - 19) / 2] * 2 + 1);
        F_0476(2);
        if (F_353A(F_0742(D_3B2C[m][0])) > 10) {
            F_3B46("");
            F_3599("Hit! ", D_2046);
        } else {
            F_3B46(" Hit!");
        }
        F_3C8B(dmg, 2, 32, D_2046, 1);
        F_3B46(" damage");
        F_355D(D_3B2C[0][0], 9, 4);
        D_206E -= dmg;
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
    D_3B2C[m][1] = F_A363(a, b);
    D_3B2C[m][2] = a;
    D_3B2C[m][3] = b;
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
        t = D_3B2C[i][0];
        if (t >= 19 && t <= 47) {
            dx = D_3AAC - D_3B2C[i][2];
            dy = D_3AAE - D_3B2C[i][3];
            if (F_8194(t, dx, dy)) {
                q[n] = i;
                n++;
            } else {
                F_8598(i, dx, dy);
            }
        }
    }
    F_C3CA();
    while (n-- && D_206E > 0) {
        i = q[n];
        dx = D_3AAC - D_3B2C[i][2];
        dy = D_3AAE - D_3B2C[i][3];
        F_81FC(i, dx, dy, F_8194(D_3B2C[i][0], dx, dy));
    }
}

/* remove monsters of overworld region a (kill list cleanup) */
F_86E1(a)
int a;
{
    int i;

    for (i = 1; i <= D_8BA6; i++) {
        if (D_3B2C[i][0] >= 19) {
            if (F_40FE(D_3B2C[i][2], D_3B2C[i][3]) == a) {
                F_B58E(D_3B2C[i][2], D_3B2C[i][3]);
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
            F_3B46("!");
            F_0476(3);
            r = D_3A94 / 2 + (D_2086 << 3) + 56;
            if (F_4069(1, 255) <= r) {
                F_3B46("Missed!");
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
                                F_3B46("");
                            F_3B46(D_0D72[i]);
                            D_20B8[i]--;
                            i = 16;
                        }
                    }
                }
            }
            if (f) {
                F_0476(2);
                F_3599("Hit! ", D_2046);
                F_3C8B(dmg, 1, 32, D_2046, 1);
                F_3B46(" damage!");
                D_206E -= dmg;
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
            if (D_206E <= 0 || D_2080 <= 0)
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
    D_3B2C[0][0] = 10;
    D_3B2C[0][1] = k;
    D_3B2C[0][2] = D_3AAC;
    D_3B2C[0][3] = D_3AAE;
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
    if (D_206E <= 0 || D_2080 <= 0)
        F_C47C();
    else
        F_C3CA();
    F_45A3();
    for (;;) {
        if (D_206E <= 0 || D_2080 <= 0)
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
            D_0180[1] = 1;
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
