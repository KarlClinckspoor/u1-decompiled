/* opts: -ms -v -w -cs */
/*
 * OUT.EXE code 4013-5469: dungeon-view / town-castle entry helpers.
 */
struct tl { int a, b, c, d; };
extern struct tl D_AD56[][11];
extern int D_AD3E, D_AD40, D_AD42, D_AD46, D_AD48, D_AD4A, D_AD4C, D_AD4E, D_AD50, D_AD52, D_AD54;
extern int D_204A, D_204E, D_2046, D_0DA6, D_0DA8, D_2088;
extern int D_3AAA, D_3AAC, D_3AAE, D_3AB2[], D_3B1C, D_3B20, D_3708[], D_307C[], D_31CC[];
extern int D_207C, D_207E, D_206E[], D_20B8[], D_20FE;
extern int D_8B94, D_8B96, D_8B98, D_8B9E, D_8BA0, D_8BA2, D_8BA4;
extern int D_ADB6, D_ADBE, D_AFD6, D_AE96;
extern int D_1DC8[];
extern char *D_0D72[], *D_0E9E[];
extern unsigned char *D_0DF8[];
extern unsigned char D_B11E[];
extern unsigned char D_8BAE[][38][18];
extern F_3C64(), F_456D(), F_3C8E(), F_3CBB(), F_3CE8(), F_35CA(), F_34B0(), F_3A54();
extern F_3F4C(), F_3F19(), F_3B25(), F_3B65(), F_3DF5(), F_3BF5(), F_3C13(), F_4069();
extern F_387E(), F_3599(), F_3920(), F_352C(), F_3C8B(), F_45C8(), F_37FD(), F_3EC4();
extern F_4602(), F_4761(), F_3B46(), F_3F7F_O(), F_0476(), F_21C2(), F_21E7(), F_4558();
extern F_4437(), F_450D(), F_40FE(), F_C2AF(), F_B502(), F_3E5E(), F_2141(), F_20FC();
extern F_2231(), F_4EA2(), F_5085(), F_357C(), F_40A8(), F_2168(), F_A3BA(), F_4E63();
extern F_4DBF(), F_50C6(), F_4BB0(), F_1072(), F_4A38(), F_4A82(), F_4ACC(), F_4B16();
extern char F_411C();

/* passable test for map tile (x,y): 0 if blocked */
F_4013(x, y)
int x, y;
{
    if (D_AD56[x][y].b == -1 || D_AD56[x][y].b == 20 || D_AD56[x][y].b == 11 || D_AD56[x][y].b == 9)
        return 0;
    return 1;
}

/* is square at offset (dx,dy) from player walkable */
F_407E(dx, dy)
int dx, dy;
{
    int t;

    t = D_AD56[D_AD3E + dx][D_AD40 + dy].a;
    if (t == 1 || t == 2 || t == 3)
        return 1;
    return F_4013(D_AD3E + dx, D_AD40 + dy);
}

/* distance (1..5) walking along direction (a,b) until blocked */
F_40D4(a, b)
int a, b;
{
    int n;

    n = 1;
    while (!F_407E(a, b)) {
        n++;
        a += F_3C64(a);
        b += F_3C64(b);
    }
    if (n > 5)
        n = 5;
    return n;
}

/* tile type is not 0/6/7/8 */
char F_411C(c)
unsigned char c;
{
    if (c == 0 || c == 7 || c == 6 || c == 8)
        return 0;
    return 1;
}

/* draw the first-person dungeon view in the facing direction D_AD52 */
F_414A()
{
    int a, b, c, d, ex, ey, dx, dy, n, dist, fx, fy, f1, f2;
    unsigned char pl, pr, l, r, fl, fr;

    d = c = b = a = 0;
    if (D_204E) {
        F_456D();
        D_204E = 0;
    }
    fl = fr = pl = pr = 100;
    dx = F_3C8E();
    dy = F_3CBB();
    switch (D_AD52) {
    case 3:
        a = -1;
        b = 1;
        break;
    case 4:
        a = 1;
        b = -1;
        break;
    case 2:
        c = -1;
        d = 1;
        break;
    case 1:
        c = 1;
        d = -1;
        break;
    }
    F_35CA(8, 8, 0x137, 0x97, 0);
    F_3CE8();
    f1 = D_AD56[D_AD3E][D_AD40].a == 3;
    f2 = D_AD56[D_AD3E][D_AD40].a == 2 || D_AD56[D_AD3E][D_AD40].a == 1;
    dist = F_40D4(dx, dy);
    if (f2)
        F_34B0(0);
    else {
        n = dist;
        fx = ex = F_3C64(dx) * n;
        fy = ey = F_3C64(dy) * n;
        if (f1 && (D_AD56[D_AD3E + dx][D_AD40 + dy].a == 1 || D_AD56[D_AD3E + dx][D_AD40 + dy].a == 3
                   || D_AD56[D_AD3E + dx][D_AD40 + dy].a == 2))
            F_34B0(0);
        else {
            if (f1)
                F_3A54(0);
            while (n) {
                ex -= dx;
                ey -= dy;
                if (!f1 || n > 1) {
                    l = D_AD56[D_AD3E + ex + a][D_AD40 + ey + c].a;
                    r = D_AD56[D_AD3E + ex + b][D_AD40 + ey + d].a;
                    F_3F4C(n, l);
                    F_3F19(n, r);
                    l = F_411C(l);
                    r = F_411C(r);
                    if (n == dist) {
                        fl = l;
                        fr = r;
                    }
                    if (n != dist) {
                        if (pl != l)
                            F_3B25(n);
                        if (pr != r)
                            F_3B65(n);
                    }
                }
                F_3DF5(n, D_AD3E + ex + dx, D_AD40 + ey + dy);
                pl = l;
                pr = r;
                n--;
            }
            if (!f1)
                F_3DF5(0, D_AD3E, D_AD40);
            if (F_4013(D_AD3E + fx, D_AD40 + fy) && D_AD56[D_AD3E + fx][D_AD40 + fy].a != 3 && dist < 5) {
                l = D_AD56[a + D_AD3E + fx][c + D_AD40 + fy].a;
                r = D_AD56[D_AD3E + b + fx][D_AD40 + d + fy].a;
                F_3F4C(dist + 1, l);
                F_3F19(dist + 1, r);
                if (!F_411C(l))
                    F_3B25(dist);
                if (!F_411C(r))
                    F_3B65(dist);
            } else {
                if (fl == 1)
                    F_3B25(dist);
                if (fr == 1)
                    F_3B65(dist);
            }
        }
        if (f1) {
            if (D_AD56[D_AD3E][D_AD40].c == 5)
                F_3BF5(1);
            else if (D_AD56[D_AD3E][D_AD40].c == 4)
                F_3C13(1);
        }
    }
}

/* clear the screen area and print the status line with the dungeon level */
F_456D()
{
    F_387E(15, 0);
    F_3599(" Level    ", D_2046);
    F_3920(26, 0);
    F_352C(23, 0);
    F_3C8B(D_AD50, 2, 32, D_2046, 0);
    F_352C(1, 24);
}

/* set the cell (D_3AAA + 3*D_3AAE + 5*D_3AAC + 17*D_AD50) */
F_45C8()
{
    int t;

    t = D_3AAC * 5 + D_3AAE * 3 + D_AD50 * 17;
    F_37FD(t + D_3AAA);
}

/* place one monster/ladder on a random free square of the dungeon level */
F_4602()
{
    int x, y, q, ok, cnt;

    cnt = 0;
    do {
        x = F_4069(0, 242) % 9 + 1;
        y = F_4069(0, 242) % 9 + 1;
        ok = D_AD56[x][y].b == -1 && D_AD56[x][y].a != 1 && D_AD56[x][y].a != 2
             && D_AD56[x][y].a != 8 && D_AD56[x][y].c != 4;
    } while (!ok && ++cnt <= 500);
    if (cnt < 500 && !(x == D_AD3E && y == D_AD40)) {
        do {
            cnt = F_4069(0, 49) / 10;
        } while (D_B11E[cnt]);
        D_B11E[cnt] = 1;
        q = (D_AD50 - 1) / 2 * 5 + cnt;
        D_AD56[x][y].b = q;
        D_AD56[x][y].d = F_4069(1, D_AD50 * D_AD50 + 1) + 10 + q;
    }
}

/* generate a new dungeon level: walls, doors, traps, monsters */
F_4761()
{
    int i, j, k, m, d;

    F_456D();
    F_45C8();
    for (i = 0; i <= 10; i++)
        for (j = 0; j <= 10; j++) {
            D_AD56[i][j].a = 0;
            D_AD56[i][j].c = 0;
            D_AD56[i][j].b = -1;
            D_AD56[i][j].d = 0;
        }
    for (i = 0; i <= 10; i++) {
        D_AD56[i][0].a = 1;
        D_AD56[0][i].a = 1;
        D_AD56[10][i].a = 1;
        D_AD56[i][10].a = 1;
    }
    for (i = 2; i <= 8; i += 2) {
        D_AD56[i][2].a = 1;
        D_AD56[i][4].a = 1;
        D_AD56[i][6].a = 1;
        D_AD56[i][8].a = 1;
    }
    for (i = 2; i <= 8; i += 2)
        for (j = 1; j <= 9; j++)
            D_AD56[i][j].a = F_4069(1, 4) - 1;
    m = D_AD50 << 1;
    k = 0;
    while (k <= m) {
        if (F_4069(1, 255) > 160)
            d = 8;
        else
            d = 1;
        i = F_4069(1, 16) - 1;
        if (D_AD50 & 1)
            D_AD56[D_1DC8[38 + i]][D_1DC8[54 + i]].a = d;
        else
            D_AD56[D_1DC8[54 + i]][D_1DC8[38 + i]].a = d;
        k++;
    }
    F_37FD(F_3EC4() + 0x6F1);
    k = 0;
    while (k <= D_AD50) {
        d = (F_4069(1, 100) & 1) + 4;
        i = F_4069(10, 99) / 10;
        j = F_4069(10, 99) / 10;
        if (D_AD56[i][j].a != 1 && D_AD56[i][j].a != 2 && D_AD56[i][j].a != 8)
            D_AD56[i][j].c = d;
        k++;
    }
    D_ADBE = 0;
    if (D_AD50 & 1) {
        D_AFD6 = 7;
        D_AE96 = 6;
    } else {
        D_AFD6 = 6;
        D_AE96 = 7;
    }
    if (D_AD50 == 10)
        D_AFD6 = 0;
    if (D_AD50 == 1) {
        D_ADB6 = 7;
        D_AFD6 = 0;
    }
    for (i = 0; i < 5; i++)
        D_B11E[i] = 0;
    F_4602();
    F_4602();
    F_4602();
}

/* Turn around command */
F_4A38()
{
    F_3B46("Turn around");
    switch (D_AD52) {
    case 3:
        D_AD52 = 4;
        break;
    case 4:
        D_AD52 = 3;
        break;
    case 2:
        D_AD52 = 1;
        break;
    case 1:
        D_AD52 = 2;
        break;
    }
}

/* Turn left command */
F_4A82()
{
    F_3B46("Turn left");
    switch (D_AD52) {
    case 3:
        D_AD52 = 1;
        break;
    case 4:
        D_AD52 = 2;
        break;
    case 2:
        D_AD52 = 3;
        break;
    case 1:
        D_AD52 = 4;
        break;
    }
}

/* Turn right command */
F_4ACC()
{
    F_3B46("Turn right");
    switch (D_AD52) {
    case 3:
        D_AD52 = 2;
        break;
    case 4:
        D_AD52 = 1;
        break;
    case 2:
        D_AD52 = 4;
        break;
    case 1:
        D_AD52 = 3;
        break;
    }
}

/* Forward command: step one square unless blocked */
F_4B16()
{
    int dx, dy;

    dx = F_3C8E();
    dy = F_3CBB();
    F_3599("Forward", D_2046);
    if (F_3F7F_O(dx, dy) == 0) {
        D_AD3E += dx;
        D_AD40 += dy;
        F_3B46("");
    } else {
        F_0476(0);
        F_3B46(" - path blocked!");
    }
}

/* dungeon command dispatch: 1 left, 2 right, 3 forward, 4 turn around */
F_4B73(c)
unsigned char c;
{
    switch (c) {
    case 4:
        F_4A38();
        break;
    case 3:
        F_4B16();
        break;
    case 1:
        F_4A82();
        break;
    case 2:
        F_4ACC();
        break;
    }
}

/* enter the first dungeon level (start state) */
F_4BB0()
{
    D_204A = 3;
    D_AD50 = D_AD3E = D_AD40 = 1;
    D_AD52 = 4;
    D_AD54 = 0;
    F_4761();
}

/* princess rescued: rewards and the time-machine message */
F_4BD6()
{
    int rx, ry, x, y, flg, cnt, off;

    off = cnt = 0;
    flg = D_20FE >= 20 && D_207E >= 7000;
    if (!flg)
        off = 2;
    F_21C2(D_0DA8);
    F_21E7(5, off + 5, " Thou hast saved the princess");
    F_21E7(5, off + 6, D_31CC[D_AD4A]);
    F_3599(".  She giveth thee", D_2046);
    F_21E7(8, off + 7, "500 hit points, pence and");
    F_21E7(12, off + 8, "experience points.");
    D_207C += 500;
    D_206E[0] += 500;
    D_207E += 500;
    F_4558();
    F_4437();
    F_450D();
    if (flg) {
        F_21E7(5, 10, "  She informs thee that thou");
        F_21E7(5, 11, "art now ready for time travel,");
        F_21E7(5, 12, "    and that there is a time");
        F_21E7(5, 13, " machine far to the northwest!");
        if (D_3B1C < 9) {
            D_3B1C++;
            switch (F_40FE(D_3AAC, D_3AAE)) {
            case 0:
                x = y = 0;
                break;
            case 1:
                x = 83;
                y = 0;
                break;
            case 2:
                x = 0;
                y = 77;
                break;
            case 3:
                x = 83;
                y = 77;
                break;
            }
            do {
                rx = F_4069(12, 21);
                ry = F_4069(11, 15);
                if (F_C2AF(x + rx, y + ry, 1) == 2) {
                    cnt = 500;
                    F_B502(18, 2, x + rx, y + ry, 0);
                }
            } while (cnt++ < 499);
        }
    }
    F_3E5E(5);
    F_2141();
    F_352C(1, 24);
}

/* enter a castle (c = 0x21..0x28): set up the level, player position */
F_4DBF(c)
int c;
{
    D_AD4A = c - 33;
    D_AD42 = (c - 1) % 2;
    D_AD4C = D_AD4A + 1;
    D_204A = 2;
    F_20FC();
    D_AD3E = 0;
    D_AD40 = 9;
    D_AD46 = 0;
    D_8BA0 = 0;
    D_8B9E = 0;
    D_8BA4 = 0;
    D_8BAE[D_AD42][35][(D_AD42 == 0) * 10 + 4] = 11;
    D_8BAE[D_AD42][31][(D_AD42 == 0) * 10 + 4] = 11;
    if (F_4069(1, 255) & 1)
        D_AD4E = 61;
    else
        D_AD4E = 60;
}

/* enter a town (c = 1..0x20): set up the level and position */
F_4E63(c)
int c;
{
    D_AD48 = 0;
    D_AD42 = (c - 1) % 8 + 2;
    D_204A = 1;
    F_20FC();
    D_AD3E = 19;
    D_AD40 = 17;
    F_2231();
    D_204E = 0;
}

/* show the sign / inscription text for a place (c = 0..7) */
F_4EA2(c)
unsigned c;
{
    F_21C2(D_0DA8);
    switch (c) {
    case 0:
        F_21E7(8, 8, "You feel a strong magic");
        F_21E7(12, 9, "surrounding you!");
        break;
    case 1:
        F_21E7(10, 7, "You hear someone say,");
        F_21E7(6, 9, " \"TURIS-SCIENTIA-MAGNOPERE\"");
        break;
    case 2:
        F_21E7(13, 7, "A sign reads:");
        F_21E7(8, 9, "\"FORTES FORTUNA ADIUVAT!\"");
        break;
    case 3:
        F_21E7(13, 5, "On a pedestal,");
        F_21E7(10, 6, "these words appear:");
        F_21E7(8, 7, "\"MY NAME IS OZYMANDIAS,");
        F_21E7(13, 8, "KING OF KINGS:");
        F_21E7(11, 9, "LOOK AT MY WORKS,");
        F_21E7(8, 10, "YE MIGHTY, AND DESPAIR!\"");
        F_21E7(8, 12, "Nothing beside remains.");
        F_21E7(7, 13, "You feel a strange force!");
        break;
    case 4:
        F_21E7(12, 6, "The sign reads:");
        F_21E7(12, 8, "\"ULTIMA THULE!\"");
        F_21E7(7, 10, "The sky grows dark, and a");
        F_21E7(7, 11, "strong magic engulfs you!");
        break;
    case 5:
        F_21E7(12, 7, "The sign reads:");
        F_21E7(11, 9, "\"OMNIA MUTANTUR!\"");
        break;
    case 6:
        F_21E7(12, 7, "The sign reads:");
        F_21E7(9, 9, "\"GO EAST TO GO EAST!\"");
        break;
    case 7:
        F_21E7(10, 7, "The grave is marked:");
        F_21E7(14, 9, "\"VAE VICTIS\"");
        break;
    }
    F_352C(1, 24);
}

/* first visit message "A quest has been completed!" for an odd place */
F_5085(c)
int c;
{
    if (D_3AB2[8 - c] == -1) {
        D_3AB2[8 - c] = 1;
        F_3599("A quest has been completed!", D_2046);
        F_0476(5);
        F_3B46("");
    }
}

/* enter a sign/shrine place (c = 0x29..0x30): text, gifts, stat gain */
F_50C6(c)
int c;
{
    int v, hp, gain, ok;

    ok = 1;
    c -= 41;
    F_4EA2(c);
    if (c & 1)
        F_5085(c);
    if (c == D_3B20)
        ok = 0;
    else {
        D_3B20 = c;
        v = D_3708[c];
        if (v == 0 && c == 2) {
            v = 0;
            do
                v++;
            while (D_20B8[v] != 0);
            if (v > 15)
                ok = 0;
            else {
                D_20B8[v]++;
                F_3599("You find a", D_2046);
                if (F_40A8(D_0DF8[v][0]))
                    F_357C('n', D_2046);
                F_357C(' ', D_2046);
                F_3599(D_0D72[v], D_2046);
                F_3599("!", D_2046);
            }
        } else if (c != 6) {
            hp = D_206E[v];
            if (hp == 99)
                ok = 0;
            else {
                gain = (hp + 8) / 10;
                if (gain + hp > 99)
                    gain = 99 - hp;
                F_3599("Thou dost gain ", D_2046);
                F_3C8B(gain, 1, 32, D_2046, 1);
                F_357C(' ', D_2046);
                F_3599(D_0E9E[v], D_2046);
                D_206E[v] += gain;
            }
        }
    }
    if (c == 6)
        ok = 0;
    if (ok) {
        F_2168(D_0DA6);
        F_0476(5);
        F_2168(D_0DA8);
    } else if (c != 6) {
        F_3599("Hmmmm...no effect!", D_2046);
        F_0476(6);
    }
    F_2141();
    if (c != 6)
        F_3B46("");
}

/* Enter command: enter the town/castle/dungeon/shrine at the player's spot */
F_52AB()
{
    int t, i, j, k;

    F_3599("Enter", D_2046);
    t = F_A3BA(D_3AAC, D_3AAE);
    if (!t || D_204A) {
        F_3B46("?");
        F_0476(1);
    } else {
        F_3B46("ing...");
        D_AD46 = D_8B94 = D_8B96 = D_8B98 = 0;
        D_204E = 1;
        for (i = -1; i <= 1; i++)
            for (j = -1; j <= 1; j++) {
                k = F_C2AF(D_3AAC + i, D_3AAE + j, 1);
                if (k == 0)
                    D_8B94++;
                else if (k == 1)
                    D_8B96++;
                else if (k == 2)
                    D_8B98++;
            }
        D_8BA2 = t == 33;
        if (t < 33)
            F_3599("The city of ", D_2046);
        F_3B46(D_307C[t]);
        if (t < 33)
            F_4E63(t);
        else if (t < 41)
            F_4DBF(t);
        else if (t < 49)
            F_50C6(t);
        else
            F_4BB0(t);
    }
}

/* Fire command (ship cannons / lasers) */
F_53DA()
{
    if (D_204A == 1 || D_204A == 2 || D_204A == 3) {
        F_3B46("Fire?");
        F_0476(1);
    } else {
        F_3599("Fire ", D_2046);
        if (D_2088 != 4 && D_2088 != 5) {
            F_3B46("what?");
            F_0476(1);
        } else {
            if (D_2088 == 4)
                F_3599("cannons: ", D_2046);
            else
                F_3599("lasers: ", D_2046);
            F_1072(-1, 8);
        }
    }
}
