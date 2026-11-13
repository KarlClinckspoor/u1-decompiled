/* opts: -ms -v -w -cs */
/*
 * OUT.EXE - the game, part C of 3 (game code 63F2-908B: weapon/armour/spell shops, the king, chests, unlock, monsters, main loop (sellweap-main)).
 * The three parts are one program split for the compiler's memory (LC1 stops at about 2200 lines);
 * string literals used in more than one part are named arrays (D_xxxx = their DGROUP address).
 * Builds byte-identical with Lattice C 3.10 (lc -ms -v -w -cs).
 * Names are still the addresses they had in the original (F_xxxx code, D_xxxx DGROUP data).
 */

/* Origin library and Lattice library */
extern erron(), gotoxy(), strlen2(), drawtile(), putch(), putstr(), fillrect();
extern seedrnd(), lineto(), arrowr(), arrowl(), drawline(), presskey(), msgline();
extern botbar(), topbar(), putnum(), readfile(), waittick(), writfile(), clockrnd(), scrlmsg();
extern vidinit(), playsnd(), rndrange(), isvowel(), getlevel(), quadrant(), readycmd(), showhp();
extern showfood(), showexp(), showcoin(), showstat(), ztats(), textmode(), lcrand(), terrain();
extern placeat(), animwat(), drawdmon(), drawview(), readnum(), towntile(), addmon(), findmon();
extern delmon(), cntmon(), cntveh(), scrolln(), scrolls(), scrolle(), scrollw(), objat();
extern viewx(), viewy(), drawwrld(), death();
extern EXECL(), EXIT();
extern unsigned char lowcase(), upcase(), waitkey(), keyup(), getkey(), keywait();
extern char drawable();

#define ABS(x) ((x) < 0 ? -(x) : (x))

struct cell { int a, b, c, d; };
struct mon { int type, f2, x, y, f8, fa, fc, fe; };
struct ent { int f0, f2, f4, f6; };

/* data of the Origin text module (OM4437): tables of names and messages */
extern int vidmode;
extern int wrange[];
extern unsigned char *wnamelng[], *wnames[], *strtab[];
extern char *attrnam[], *spellnam[], *armornam[], *vehnam[];
extern int dirtab[];
/* tables inside that module that have no label of their own (absolute names in OUTSYM.ASM) */
extern struct ent enttmpl[][15];
extern int dmgfact[], hpfact[], movechan[], foodper[], princnam[], mntattr[], persp1[], persp2[];
extern char *placenam[], *landnam[], *blockmsg[], *dmonname[];

/* BSS (defined in OUTBSS.C) */
extern int colline, colfill, colhigh, coltext, forcecga, gmode, redraw;
extern char plname[];
extern int plclass, plsex, plstat[];
extern int plcoin, plexp, plfood, rdyweap, rdyspel, rdyarm, plveh;
extern int gemred, ownarm[], ownweap[], ownspel[], ownveh[], plkills;
extern int plstr, plagi, plsta, plcha, plwis, plint;
extern int plseed, worldx, worldy, plnoise, plquest[];
extern int gemgreen, gemblue, gemwhite, ownspl1[];
extern int ownshutl, owntimem, lastmnmt, savlast, shutfuel, shutshld;
extern long savturns;
extern struct mon objtab[];
extern char mapbin[], tilegfx[];
extern int argv1, argv2, argv3, foodcnt, nmons, nvehs, nwater, ngrass, nwoods;
extern int prevmode, doorunl, hasjkey, isbrit, takeleft, lastmon;
extern long turns;
extern unsigned char viewctr, tcdbin[][38][18];
extern struct ent ents[];
extern int townx, towny, townmap, hostile, alesdrnk, castlenr, kingnr, lockkey;
extern int dunglvl, dungdir, dungbon;
extern struct cell dungmap[][11];
extern int D_ADB6, D_ADBE, D_AE96, D_AFD6;
extern char dmontype[];

extern char sbang[];
extern char sempty[];
extern char shit[];
extern char sfind[];
extern char sdamagex[];
extern char smissed[];
extern char sdamage[];
extern char snothing[];
extern char squest[];
extern char scolon[];
extern char snone[];
extern char snotmuch[];
extern char sparen[];
extern char swhat[];
extern char snonehr[];

#define MT ((int (*)[8])objtab)

extern char *msgleave, *plfile;
extern int escflag;
/* weapon shop, selling side: list own weapons, sell one */
sellweap(top)
int top;
{
    int i, n, y;
    unsigned char c, m;

    n = 0;
    y = 0;
    c = 'b';
    i = 1;
    while (i <= 15) {
        if (ownweap[i])
            n++;
        i++;
    }
    if (n == 0) {
        boxtext(5, top + 1, "Thou hast no weaponry to sell!");
        waitpoll();
        gotoxy(16, 24);
        putstr(snothing, coltext);
    } else {
        i = 1;
        do {
            if (ownweap[i]) {
                gotoxy(9, top + y);
                putch(c, coltext);
                putstr(sparen, coltext);
                putstr(wnamelng[i], coltext);
                gotoxy(26, top + y);
                putstr("- ", coltext);
                putnum(sellpric(i), 3, ' ', coltext, 1);
                y++;
            }
            c++;
            i++;
        } while (i <= 15 && y < 8);
        m = upcase(c) - 1;
        gotoxy(16, 24);
        c = upcase(waitkey(coltext));
        if (c < 'B' || c > m)
            putstr(snothing, coltext);
        else if (ownweap[c - 'A'] == 0)
            putstr(snothing, coltext);
        else {
            plcoin += sellpric(c - 'A');
            ownweap[c - 'A']--;
            if (rdyweap == c - 'A')
                rdyweap = 0;
            putstr(wnamelng[c - 'A'], coltext);
            showcoin();
            msgdone();
        }
    }
}

/* print one line of the weapons-for-sale list: letter, name, price */
weapln1(top, i, cnt)
int top, i, cnt;
{
    unsigned char c;

    c = i + 'a';
    gotoxy(9, top + cnt);
    putch(c, coltext);
    putstr(sparen, coltext);
    putstr(wnamelng[i], coltext);
    gotoxy(26, top + cnt);
    putstr("- ", coltext);
    putnum(buyprice(i), 3, ' ', coltext, 1);
}

/* weapon shop: sell (arg 'S') or buy a weapon */
weapshop(a)
char a;
{
    int r, n;
    unsigned char c, lo, hi;
    int cnt;

    cnt = 0;
    r = signweap(townmap - 2);
    if (a == 'S')
        sellweap(r);
    else {
        lo = ((townmap - 1) & 1) + 'b';
        n = turns % 0x7FFFL / 1500L;
        if (n > 3 || turns > 3000L)
            n = 3;
        n++;
        hi = lo + n * 2;
        n = lo;
        while (hi >= n) {
            weapln1(r, n - 'a', cnt);
            cnt++;
            n += 2;
        }
        gotoxy(15, 24);
        c = lowcase(waitkey(coltext));
        if (c < lo || c > hi)
            putstr(snothing, coltext);
        else if (((c - 'a') & 1) != (townmap & 1))
            putstr(snothing, coltext);
        else if (buyprice(c - 'a') > plcoin) {
            putstr(snothing, coltext);
            msgpoor();
        } else {
            plcoin -= buyprice(c - 'a');
            if (ownweap[c - 'a'] < 255)
                ownweap[c - 'a']++;
            putstr(wnamelng[c - 'a'], coltext);
            showcoin();
            msgsold();
        }
    }
    msgline(sempty);
}

/* quarter of (200 - plint) times n */
price1(n)
int n;
{
    return (200 - plint) / 4 * n;
}

/* quarter of plcha times n */
price2(n)
int n;
{
    return plcha / 4 * n;
}

/* ---- ou_6823 ---- */
/* armour shop, sell side: list owned armour, sell the chosen one for its price */
armsell(y)
int y;
{
    int i, n, row;
    unsigned char c;

    n = 0;
    i = row = 1;
    c = 'b';
    for (i = 1; i <= 5; i++)
        if (ownarm[i])
            n++;
    if (n == 0) {
        boxtext(5, y + 1, "  Thou hast no armour to sell!");
        waitpoll();
        gotoxy(16, 24);
        putstr(snothing, coltext);
    } else {
        for (i = 1; i <= 5; c++, i++) {
            if (ownarm[i]) {
                gotoxy(9, y + row);
                putch(c, coltext);
                putstr(sparen, coltext);
                putstr(armornam[i], coltext);
                gotoxy(26, y + row);
                putstr("- ", coltext);
                putnum(price2(i), 3, 32, coltext, 1);
                row++;
            }
        }
        gotoxy(16, 24);
        c = upcase(waitkey(coltext));
        if (c < 'B' || c > 'F') {
            putstr(snothing, coltext);
        } else if (ownarm[c - 'A'] == 0) {
            putstr(snothing, coltext);
        } else {
            plcoin += price2(c - 'A');
            ownarm[c - 'A']--;
            if (rdyarm == c - 'A')
                rdyarm = 0;
            putstr(armornam[c - 'A'], coltext);
            showcoin();
            msgdone();
        }
    }
}

/* armour shop: print list line i (letter, name, price) at row y+i */
armline(y, i)
int y, i;
{
    unsigned char c;

    c = i + 'a';
    gotoxy(9, y + i);
    putch(c, coltext);
    putstr(sparen, coltext);
    putstr(armornam[i], coltext);
    gotoxy(26, y + i);
    putstr("- ", coltext);
    putnum(price1(i), 3, 32, coltext, 1);
}

/* armour shop (buy 'B' / sell 'S') */
armshop(cmd)
unsigned char cmd;
{
    int shop;
    unsigned char c, max;

    max = 'd';
    shop = signarm(townmap - 2);
    if (cmd == 'S') {
        armsell(shop);
    } else {
        armline(shop, 1);
        armline(shop, 2);
        armline(shop, 3);
        if (turns > 3000L) {
            armline(shop, 4);
            armline(shop, 5);
            max = 'f';
        }
        gotoxy(15, 24);
        c = lowcase(waitkey(coltext));
        if (c < 'b' || c > max) {
            putstr(snothing, coltext);
        } else if (price1(c - 'a') > plcoin) {
            putstr(snothing, coltext);
            msgpoor();
        } else {
            plcoin -= price1(c - 'a');
            if (ownarm[c - 'a'] < 255)
                ownarm[c - 'a']++;
            putstr(armornam[c - 'a'], coltext);
            showcoin();
            msgsold();
        }
    }
    msgline(sempty);
}

/* weapon price for item i: (200 - plwis) / 32 * i */
weapprc(i)
int i;
{
    return (200 - plwis) / 32 * i;
}

/* weapon shop: print list line i (letter, name, price) at row y+row */
weapln2(y, i, row)
int y, i, row;
{
    unsigned char c;

    gotoxy(9, y + row);
    c = i + 'a';
    putch(c, coltext);
    putstr(sparen, coltext);
    putstr(spellnam[i], coltext);
    gotoxy(26, y + row);
    putstr("- ", coltext);
    putnum(weapprc(i), 3, 32, coltext, 1);
}

/* weapon shop (buy; we do not buy used weapons) */
weapbuy(cmd)
unsigned char cmd;
{
    int shop;
    unsigned i;
    unsigned char c, first, last;
    int row;

    row = 0;
    shop = signmagi(townmap - 2);
    if (cmd == 'S') {
        boxtext(5, shop + 2, "  Sorry, we don't buy spells!");
        waitpoll();
        gotoxy(16, 24);
        putstr(snothing, coltext);
    } else {
        i = townmap & 1;
        first = i + 'b';
        last = i + 'f';
        if (plclass == 2)
            last = i + 'j';
        for (i = first; last >= i; i += 2) {
            if (i != 'e') {
                weapln2(shop, i - 'a', row);
                row++;
            }
        }
        gotoxy(15, 24);
        c = lowcase(waitkey(coltext));
        if (c < first || c > last) {
            putstr(snothing, coltext);
        } else if (((c - 'a') & 1) == (townmap & 1)) {
            putstr(snothing, coltext);
        } else if (weapprc(c - 'a') > plcoin) {
            putstr(snothing, coltext);
            msgpoor();
        } else {
            plcoin -= weapprc(c - 'a');
            if (ownspel[c - 'a'] < 255)
                ownspel[c - 'a']++;
            putstr(spellnam[c - 'a'], coltext);
            showcoin();
            msgsold();
        }
    }
    msgline(sempty);
}

/* food shop: buy packs of 10 food (no selling) */
foodshop(cmd)
unsigned char cmd;
{
    int n, cost;

    n = signfood(townmap - 2);
    if (cmd == 'S') {
        boxtext(9, n + 2, "Used food?  No thanks!");
        waitpoll();
        gotoxy(15, 24);
        putstr(snothing, coltext);
    } else {
        cost = 5 - plint / 20;
        gotoxy(5, n + 2);
        putstr(" Packs of 10 food cost ", coltext);
        putnum(cost, 1, 32, coltext, 1);
        putstr(" pence", coltext);
        gotoxy(8, n + 3);
        putstr("each.  How many dost thou", coltext);
        gotoxy(11, n + 4);
        putstr("wish to purchase?", coltext);
        gotoxy(15, 24);
        n = readnum(15, 24);
        cost = cost * n;
        if (cost < 0)
            cost = plcoin + 1;
        if (n == 0 || cost > plcoin) {
            gotoxy(15, 24);
            putstr(snothing, coltext);
            if (cost > plcoin)
                msgpoor();
        } else {
            putstr(" packs food", coltext);
            plcoin -= cost;
            showcoin();
            plfood += n * 10;
            showfood();
            msgsold();
        }
    }
    msgline(sempty);
}

/* transport bought (k: 1-2 horse/cart, 3-4, 5-6): update counts, redraw one transport on the map */
transbgt(k)
int k;
{
    int t, a, b, f;

    ownveh[k]++;
    if (k <= 2 && nwoods) {
        nwoods--;
        t = 2;
    } else if (k == 4 || k == 3) {
        nwater--;
        t = 0;
    } else {
        t = 1;
        ngrass--;
    }
    b = -1;
    do {
        a = -1;
        do {
            f = objat(worldx + a, worldy + b, 1) == t;
            a++;
        } while (a < 2 && !f);
        if (!f)
            b++;
        else
            a--;
    } while (b < 2 && !f);
    addmon(k + 11 - 1 + (k > 4), t, worldx + a, worldy + b, 0);
}

/* transport price for item i: ((200 - plint) / 5 * i) * i */
transprc(i)
int i;
{
    return (200 - plint) / 5 * i * i;
}

/* transport shop: print list line i (letter c, name, price) at row y */
transln(i, y, c)
int i, y;
unsigned char c;
{
    gotoxy(12, y);
    putch(c, coltext);
    putstr(sparen, coltext);
    putstr(vehnam[i], coltext);
    gotoxy(23, y);
    putstr("- ", coltext);
    putnum(transprc(i), 4, 32, coltext, 0);
}

/* transport shop (buy; we do not deal in used stuff) */
transshp(cmd)
unsigned char cmd;
{
    int shop, i, cost, flag;
    char x[6];
    unsigned char c;

    shop = signship(townmap - 2);
    if (cmd == 'S') {
        boxtext(9, shop + 2, "Sorry, we don't deal in");
        boxtext(15, shop + 3, "used stuff.");
        waitpoll();
        gotoxy(16, 24);
        putstr(snothing, coltext);
    } else {
        for (i = 0; i < 6; i++)
            x[i] = 0;
        flag = ngrass || nwoods || nwater;
        if (!flag || (ownshutl && nvehs == 15) || (!ngrass && nvehs == 15)) {
            boxtext(11, shop + 2, "Closed for the day.");
            waitpoll();
        } else {
            flag = ownshutl == 0 && nvehs == 15;
            if ((nwoods || ngrass) && !flag) {
                transln(1, shop, 'a');
                shop++;
                x[0]++;
                transln(2, shop, 'b');
                shop++;
                x[1]++;
            }
            if (nwater && !flag) {
                transln(3, shop, 'c');
                shop++;
                x[2]++;
                transln(4, shop, 'd');
                shop++;
                x[3]++;
            }
            if (turns > 3000L && ngrass) {
                if (!flag) {
                    transln(5, shop, 'e');
                    shop++;
                    x[4]++;
                }
                if (ownshutl == 0)
                    transln(6, shop, 'f');
                x[5]++;
            }
            gotoxy(15, 24);
            c = upcase(waitkey(coltext));
            if (c < 'A' || c > 'F') {
                putstr(snothing, coltext);
            } else if (!x[c - 'A']) {
                putstr(snothing, coltext);
            } else {
                cost = transprc(c + 1 - 'A');
                if (cost > plcoin) {
                    putstr(snothing, coltext);
                    msgpoor();
                } else {
                    putstr(vehnam[c + 1 - 'A'], coltext);
                    plcoin -= cost;
                    showcoin();
                    nvehs++;
                    msgsold();
                    if (c == 'F')
                        shutfuel = shutshld = 1000;
                    transbgt(c + 1 - 'A');
                }
            }
        }
    }
    msgline(sempty);
}

/* Transact command in town: at a shop counter (tile 54-62) buy or sell, dispatch by shop type */
shopdisp()
{
    int t;
    unsigned char c;

    putstr("Transact", coltext);
    if (hostile) {
        msgline(sempty);
        msgline("None will talk to thee!");
    } else {
        t = tcdbin[townmap][townx][towny];
        if (t < 54 || t > 62) {
            msgline(sempty);
            msgline("Thou art not by a counter!");
            playif(1);
        } else {
            putstr("-Buy, Sell: ", coltext);
            c = upcase(waitkey(coltext));
            if (c != 'B' && c != 'S') {
                gotoxy(9, 24);
                msgline(" nothing      ");
            } else {
                if (c == 'B') {
                    gotoxy(13, 24);
                    putstr("         ", coltext);
                    gotoxy(13, 24);
                    putstr(scolon, coltext);
                } else {
                    gotoxy(10, 24);
                    putstr("            ", coltext);
                    gotoxy(10, 24);
                    putstr("Sell: ", coltext);
                }
                switch (t) {
                case 54:
                case 55:
                    armshop(c);
                    break;
                case 56:
                case 57:
                    foodshop(c);
                    break;
                case 58:
                case 59:
                    weapshop(c);
                    break;
                case 60:
                    weapbuy(c);
                    break;
                case 61:
                    tavern(c);
                    break;
                case 62:
                    transshp(c);
                    break;
                }
                redraw = 1;
            }
        }
    }
}

/* king's audience, quest given: show the quest text by king (kingnr) */
questtxt()
{
    boxclr(colfill);
    plquest[kingnr] = -1;
    if (!(kingnr & 1)) {
        boxtext(12, 6, "Go now and kill a");
        gotoxy(12, 8);
        switch (kingnr) {
        case 2:
            putstr(" Gelatinous Cube", coltext);
            break;
        case 4:
            putstr(" Carrion Creeper", coltext);
            break;
        case 6:
            putstr("     Lich", coltext);
            break;
        case 8:
            putstr("    Balron", coltext);
            break;
        }
    } else {
        boxtext(11, 6, "Go forth and find");
        switch (kingnr) {
        case 7:
            boxtext(9, 8, placenam[42]);
            break;
        case 5:
            boxtext(9, 8, placenam[44]);
            break;
        case 3:
            boxtext(10, 8, placenam[46]);
            break;
        case 1:
            boxtext(8, 8, placenam[48]);
            break;
        }
    }
    boxtext(11, 10, "Do not return until");
    boxtext(11, 11, "thy quest is done!");
    presskey();
    redraw = 1;
}

/* has the king's quest of king kingnr not been given yet (flag -1 set when given) */
questnew()
{
    return plquest[kingnr] == -1;
}

/* quest reward for a monster quest (odd kings): strength points */
rewardst()
{
    int n;

    n = (99 - plstr) / 8;
    if (n == 0 && plstr != 99)
        n = 1;
    boxtext(8, 8, "For this I will give thee");
    gotoxy(10, 9);
    putnum(n, 1, 32, coltext, 1);
    putstr(" point", coltext);
    if (n != 1)
        putch('s', coltext);
    putstr(" of strength.", coltext);
    plstr += n;
    presskey();
    redraw = 1;
}

/* quest reward (even kings): a gem and a hint, by king type */
rewardgm()
{
    int t;

    t = quadrant(worldx, worldy);
    boxtext(7, 8, "For this I shall tell thee");
    switch (t) {
    case 0:
        boxtext(5, 9, "that it requires all four gems");
        boxtext(5, 10, "  to launch a time machine!");
        boxtext(5, 12, "The king gives thee a red gem.");
        gemred++;
        break;
    case 1:
        boxtext(5, 9, "that the time machine must be");
        boxtext(5, 10, "     used in order to win.");
        boxtext(5, 12, "     The king gives thee a");
        boxtext(5, 13, "           green gem!");
        gemgreen++;
        break;
    case 2:
        boxtext(5, 9, "   the princess will help a");
        boxtext(5, 10, "   space ace through time!");
        boxtext(5, 12, "    The king gives thee a");
        boxtext(5, 13, "          blue gem!");
        gemblue++;
        break;
    case 3:
        boxtext(5, 9, "   to take nine items from my");
        boxtext(5, 10, "  storerooms - but only nine!");
        boxtext(5, 12, "   The king also gives thee a");
        boxtext(5, 13, "          white gem!");
        gemwhite++;
        takeleft = 9;
        break;
    }
    waittick(10);
    presskey();
    redraw = 1;
}

/* quest completed: congratulate the player; returns 1 if it was (reward given), else 0 */
questdon()
{
    int n;

    n = 7 - strlen2(plname) / 2;
    if (plquest[kingnr] != 1)
        return 0;
    plquest[kingnr] = 0;
    boxclr(colfill);
    boxtext(n + 5, 5, "Ah, ");
    putstr(plname, coltext);
    putstr("!  Thou hast", coltext);
    boxtext(10, 6, "completed thy quest.");
    if (kingnr & 1)
        rewardst();
    else
        rewardgm();
    return 1;
}

/* Transact with the king: offer pence (for hit points) or service (a quest) */
kingtrns()
{
    unsigned char c;
    int amt, k;

    k = quadrant(worldx, worldy) * 2 + townmap;
    msgline("Transact with king");
    if (tcdbin[townmap][townx][towny] != 62) {
        msgline("He is not here!");
    } else if (hostile) {
        msgline("He rejects thine offer!");
    } else if (questdon(k)) {
    } else {
        msgline("Dost thou offer pence");
        putstr("or service: ", coltext);
        c = upcase(waitkey(coltext));
        if (c != 'P' && c != 'S') {
            msgline("neither");
        } else {
            switch (c) {
            case 'P':
                msgline("pence");
                putstr("How much? ", coltext);
                amt = readnum(11, 24);
                if (amt == 0) {
                    gotoxy(11, 24);
                    msgline(snone);
                } else if (amt > plcoin) {
                    msgline(sempty);
                    msgline(snotmuch);
                    playif(1);
                } else {
                    plcoin -= amt;
                    amt *= 3;
                    amt /= 2;
                    msgline(sempty);
                    msgline("In return I give unto");
                    putstr("thee ", coltext);
                    putnum(amt, 1, 32, coltext, 1);
                    msgline(" hit points");
                    plstat[0] += amt;
                    showhp();
                    showcoin();
                }
                break;
            case 'S':
                if (questnew()) {
                    msgline(sempty);
                    msgline("Thou art on a quest");
                    msgline("for me already!");
                } else
                    questtxt();
                break;
            }
        }
    }
}

/* Transact command: dispatch by mode (gmode 1 town, 2 castle) */
cmdtrans()
{
    if (gmode == 1)
        shopdisp();
    else if (gmode == 2)
        kingtrns();
    else {
        msgline("Transact?");
        playif(1);
    }
}

/* View command: not available here */
cmdview()
{
    msgline("View?");
    playif(1);
}

/* open a chest in the dungeon cell: trap (if arg > plagi) or treasure */
dungchst(lvl)
int lvl;
{
    putstr(" chest", coltext);
    if (dungmap[townx][towny].c != 4) {
        msgline(snonehr);
        playif(1);
    } else {
    msgline(sempty);
    if (lvl > plagi && plclass != 3) {
        putstr("Thou hast set off a trap!", coltext);
        playif(2);
        msgline(sempty);
        plstat[0] -= dunglvl;
        showhp();
    } else {
        dungmap[townx][towny].c = 0;
        putstr(sfind, coltext);
        givecoin(rndrange(3, dunglvl * dunglvl + 9), 0);
    }
    }
}

/* ---- ou_7c31 ---- */
/* Unlock a castle door: needs the right key (tile 60/61 ahead), opens the door */
unlkdoor()
{
    int t;

    t = tcdbin[townmap][townx][towny];
    if (t != 60 && t != 61) {
        msgline(swhat);
        playif(1);
    } else {
        msgline(sempty);
        if (hasjkey == 0) {
            msgline("Thou hast not a key!");
        } else if (t != lockkey) {
            msgline("Thou hast not");
            msgline("the correct key!");
        } else {
            msgline("The door is open!");
            doorunl = 1;
            tcdbin[townmap][townx][towny - 1] = 1;
            towntile(1, townx, towny - 1);
        }
    }
}

/* 'U'nlock command: dungeon -> dungchst(random), castle -> unlkdoor, else "?" / " what?" */
cmdunlk()
{
    putstr("Unlock", coltext);
    if (gmode == 3) {
        dungchst(rndrange(1, 75));
    } else if (gmode == 2) {
        unlkdoor();
    } else if (gmode == 0 || gmode == 4) {
        msgline(squest);
        playif(1);
    } else if (gmode == 1) {
        msgline(swhat);
        playif(1);
    }
}

/* 'X'-it command: leave the vehicle (ship/aircraft) next to land */
cmdxit()
{
    int t, i, j, f;

    f = 0;
    if (gmode != 0) {
        msgline("X-it?");
        playif(1);
    } else if (plveh == 0) {
        msgline("X-it what?");
        playif(1);
    } else {
        t = objat(worldx, worldy, 1);
        for (i = -2; i <= 2; i++)
            for (j = -2; j <= 2; j++)
                if (terrain(worldx + i, worldy + j))
                    f = 1;
        if (t > 2 || f == 0) {
            msgline(msgleave);
            playif(1);
        } else {
            addmon(MT[0][0], viewctr, worldx, worldy, 0);
            plveh = 0;
            MT[0][1] = MT[0][0];
            MT[0][0] = 10;
            msgline("X-it");
        }
    }
}

/* 'Pass' command */
cmdpass()
{
    msgline("Pass");
}

/* unknown command: "Huh?" */
cmdhuh()
{
    msgline("Huh?");
    playif(1);
}

/* spawn chance by experience (DGROUP 16CC) */
int spawnthr[10] = { 0x23, 0x37, 0x4B, 0x5F, 0x73, 0x87, 0x9B, 0xAF, 0xC3, 0xCD };

/* per-turn food counter on the overworld: eat food when counter reaches vehicle limit */
foodtick()
{
    if (gmode == 0) {
        if (foodcnt >= foodper[plveh]) {
            foodcnt = 0;
            plfood--;
            showfood();
        }
        foodcnt++;
    }
}

/* is overworld x coordinate (after wrap) in range 9..158 */
xinrange(a)
int a;
{
    if (a > 83)
        a -= 84;
    if (a < 159 && a > 8)
        return 1;
    return 0;
}

/* is overworld y coordinate (after wrap) in range 6..149 */
yinrange(a)
int a;
{
    if (a > 77)
        a -= 78;
    if (a < 150 && a > 5)
        return 1;
    return 0;
}

/* maybe spawn a wandering monster near the player on the overworld */
spawnmon()
{
    int m, r, t, x, y, n, k, lvl, id, hp, ok;

    r = rndrange(1, 255);
    if (r < spawnthr[plexp / 1000]) {
        n = 0;
        m = rndrange(1, 8) * 2;
        lvl = rndrange(0, 149) / 10;
        r = hpfact[lvl];
        if (m > r)
            m = r;
        if (lvl < 4)
            t = 0;
        else if (lvl < 9)
            t = 2;
        else
            t = 1;
        id = lvl * 2 + 19;
        hp = getlevel() + 1 + hpfact[lvl] * 10;
        do {
            x = rndrange(10, 15);
            y = rndrange(5, 15);
            if (rndrange(1, 16) & 1)
                x = -x;
            if (rndrange(1, 16) & 1)
                y = -y;
            x = viewx(x + 9);
            y = viewy(y + 4);
            k = objat(x, y, 1);
            ok = xinrange(x) && yinrange(y);
        } while (n < 5 && k != t && !ok);
        if (k == t && ok) {
            addmon(id, t, x, y, hp);
            nmons++;
        }
    }
}

/* can monster m (type m) move onto the overworld square at (dx,dy) relative to screen centre */
moncango(m, dx, dy)
int m, dx, dy;
{
    int a, b, t, k, x, y, res;

    a = 2;
    b = 1;
    res = 0;
    if (rndrange(1, 100) > movechan[plveh])
        return 0;
    t = MT[m][0];
    if (t >= 19 && t <= 25) {
        a = b = 0;
    } else if (t == 31 || t == 35) {
        a = b = 2;
    }
    x = viewx(9 - dx);
    y = viewy(4 - dy);
    k = objat(x, y, 0);
    if (a != b)
        res = k >= 4 && k <= 9;
    if ((a == k || k == b) && xinrange(x) && yinrange(y) || res)
        return 1;
    return 0;
}

/* distance test: returns reach (1 or 3) if (dx,dy) within it, else 0 */
monreach(m, dx, dy)
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
monshoot(m, dx, dy, n)
int m, dx, dy, n;
{
    int dmg, hit, k, x, y, sx, sy, i;

    sy = sx = 0;
    i = 1;
    drawtile(MT[m][0], 9 - dx, 4 - dy);
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
    putstr(monname(MT[m][0]), coltext);
    putstr(" attacks!", coltext);
    do {
        x = dx + sx * i;
        y = dy + sy * i;
        drawtile(50, 9 - x, 4 - y);
        if (i == 1)
            playif(3);
        else
            waittick(2);
        k = objat(worldx - x, worldy - y, 0);
        drawtile(k, 9 - x, 4 - y);
        i++;
    } while (i <= n && k != 3 && (x != 0 || y != 0));
    hit = rndrange(1, 255) < 200 - plsta / 2 - (rdyarm << 3);
    if (x || y || !hit) {
        drawtile(MT[0][0], 9, 4);
        if (strlen2(monname(MT[m][0])) > 8)
            msgline(sempty);
        else
            putch(32, coltext);
        msgline(smissed);
    } else {
        drawtile(50, 9, 4);
        dmg = rndrange(1, dmgfact[(MT[m][0] - 19) / 2] * 2 + 1);
        playif(2);
        if (strlen2(monname(MT[m][0])) > 10) {
            msgline(sempty);
            putstr(shit, coltext);
        } else {
            msgline(" Hit!");
        }
        putnum(dmg, 2, 32, coltext, 1);
        msgline(sdamage);
        drawtile(MT[0][0], 9, 4);
        plstat[0] -= dmg;
        showhp();
    }
}

/* move monster m to screen offset (dx,dy): update its terrain and position */
monplace(m, dx, dy)
int m, dx, dy;
{
    int a, b;

    a = viewx(9 - dx);
    b = viewy(4 - dy);
    MT[m][1] = terrain(a, b);
    MT[m][2] = a;
    MT[m][3] = b;
}

/* try to step monster m one square in y toward the player */
monstepy(m, dx, dy)
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
    if (s && moncango(m, dx, s + dy)) {
        monplace(m, dx, s + dy);
        return 1;
    }
    return 0;
}

/* try to step monster m one square in x toward the player */
monstepx(m, dx, dy)
int m, dx, dy;
{
    int s;

    if (dx == 0)
        s = 0;
    else if (dx > 0)
        s = -1;
    else
        s = 1;
    if (s && moncango(m, dx + s, dy)) {
        monplace(m, dx + s, dy);
        return 1;
    }
    return 0;
}

/* move monster m toward the player, trying x or y first at random */
monchase(m, dx, dy)
int m, dx, dy;
{
    int r;

    r = 1;
    if (lcrand(1, 10) & 1) {
        if (!monstepy(m, dx, dy))
            r = monstepx(m, dx, dy);
    } else {
        if (!monstepx(m, dx, dy))
            r = monstepy(m, dx, dy);
    }
}

/* overworld monsters' turn: attackers in range fire, others move toward the player */
worldmon()
{
    int q[10], t, dx, dy, n, i;

    n = 0;
    for (i = 1; i <= lastmon; i++) {
        t = MT[i][0];
        if (t >= 19 && t <= 47) {
            dx = worldx - MT[i][2];
            dy = worldy - MT[i][3];
            if (monreach(t, dx, dy)) {
                q[n] = i;
                n++;
            } else {
                monchase(i, dx, dy);
            }
        }
    }
    drawwrld();
    while (n-- && plstat[0] > 0) {
        i = q[n];
        dx = worldx - MT[i][2];
        dy = worldy - MT[i][3];
        monshoot(i, dx, dy, monreach(MT[i][0], dx, dy));
    }
}

/* remove monsters of overworld region a (kill list cleanup) */
rmregion(a)
int a;
{
    int i;

    for (i = 1; i <= lastmon; i++) {
        if (MT[i][0] >= 19) {
            if (quadrant(MT[i][2], MT[i][3]) == a) {
                delmon(MT[i][2], MT[i][3]);
                nmons--;
                i--;
            }
        }
    }
}

/* dungeon monster at offset (dx,dy) attacks the player (hit/miss, special effects) */
dungmatk(dx, dy)
int dx, dy;
{
    int t, r, dmg, i, f;

    f = 1;
    if (dungmap[townx][towny].a != 8) {
        if (dungmap[townx - dx][towny - dy].a != 3 ||
            (dungmap[townx][towny].a != 3 && dungmap[townx][towny].a != 1 &&
             dungmap[townx][towny].a != 2)) {
            t = dungmap[townx - dx][towny - dy].b;
            putstr("Attacked by ", coltext);
            featname(t);
            msgline(sbang);
            playif(3);
            r = plsta / 2 + (rdyarm << 3) + 56;
            if (rndrange(1, 255) <= r) {
                msgline(smissed);
                f = 0;
            } else {
                r = rndrange(1, 255);
                dmg = dunglvl + t * t;
                if (dmg > 255)
                    dmg = rndrange(t + 1, 255);
                if (t == 9 && rdyarm) {
                    msgline("Armor destroyed!");
                    ownarm[rdyarm]--;
                    f = rdyarm = 0;
                } else if (t == 16) {
                    msgline("A gremlin stole some food!");
                    plfood = plfood / 2;
                    showfood();
                    f = 0;
                } else if (t == 21 && r < 128) {
                    msgline("Mental attack!");
                    plint = plint / 2;
                    plint += 5;
                    f = 0;
                }
                if (t == 2) {
                    for (i = 1; i <= 15; i++) {
                        if (ownweap[i] && i != rdyweap) {
                            putstr("Thief stole a", coltext);
                            if (isvowel(wnamelng[i][0]))
                                putstr("n ", coltext);
                            else
                                putch(32, coltext);
                            if (strlen2(wnamelng[i]) > 11)
                                msgline(sempty);
                            msgline(wnamelng[i]);
                            ownweap[i]--;
                            i = 16;
                        }
                    }
                }
            }
            if (f) {
                playif(2);
                putstr(shit, coltext);
                putnum(dmg, 1, 32, coltext, 1);
                msgline(sdamagex);
                plstat[0] -= dmg;
                showhp();
            }
        }
    }
}

/* dungeon: move monster at (x,y) one step by (dx,dy) if the square is free */
dungmmov(x, y, dx, dy)
int x, y, dx, dy;
{
    int t;

    t = dungmap[x + dx][y + dy].a;
    if (t != 1 && t != 2 && t != 8 && dungmap[x + dx][y + dy].b == -1) {
        dungmap[x + dx][y + dy].b = dungmap[x][y].b;
        dungmap[x][y].b = -1;
        dungmap[x + dx][y + dy].c += 1000;
        return 1;
    }
    return 0;
}

/* dungeon monsters' turn: adjacent ones attack, near ones move toward the player */
dungmons()
{
    int i, j, dx, dy, r, d;

    r = 0;
    for (i = 1; i <= 9; i++) {
        for (j = 1; j <= 9; j++) {
            if (dungmap[i][j].b != -1 && dungmap[i][j].c < 999) {
                dx = townx - i;
                dy = towny - j;
                d = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
                if (d == 1) {
                    dungmatk(dx, dy);
                } else if (d < 7) {
                    dx = sign(dx);
                    dy = sign(dy);
                    if (dx)
                        r = dungmmov(i, j, dx, 0);
                    if (r == 0 && dy)
                        r = dungmmov(i, j, 0, dy);
                }
            }
            if (plstat[0] <= 0 || plfood <= 0)
                break;
        }
    }
    for (i = 1; i <= 9; i++)
        for (j = 1; j <= 9; j++)
            if (dungmap[i][j].c > 999)
                dungmap[i][j].c -= 1000;
}

/* teleport the player to a random nearby land square of the same region (time-machine style hop) */
randhop()
{
    int t, k, x, y;

    t = quadrant(worldx, worldy);
    do {
        do {
            x = worldx + rndrange(3, 23) - 13;
            y = worldy + rndrange(3, 23) - 13;
            if (x < 2 || x > 165)
                x = worldx;
            if (y < 2 || y > 153)
                y = worldy;
        } while (t != quadrant(x, y));
        k = objat(x, y, 1);
    } while (k != 1 && k != 2);
    worldx = x;
    worldy = y;
    MT[0][0] = 10;
    MT[0][1] = k;
    MT[0][2] = worldx;
    MT[0][3] = worldy;
}

/* monsters' turn dispatch on game mode (0 world, 3 dungeon, 1/2 town/castle) */
montrn()
{
    if (gmode == 0)
        worldmon();
    else if (gmode == 3)
        dungmons();
    else if (gmode == 1 || gmode == 2)
        entturn();
}

/* "Insert Ultima I diskette / Hit space to continue..." prompt */
diskprmt()
{
    scrlmsg();
    gotoxy(1, 23);
    putstr("Insert Ultima I diskette", coltext);
    gotoxy(1, 24);
    putstr("Hit space to continue...", coltext);
    while (waitkey(coltext) != 32)
        ;
    gotoxy(1, 23);
    putstr("                        ", coltext);
    gotoxy(1, 24);
    putstr("                        ", coltext);
    gotoxy(1, 23);
}

/* OUT.EXE main: load map.bin/tcd.bin/data, init state, command loop */
main(argc, argv)
int argc;
unsigned char **argv;
{
    unsigned char c;
    int cnt;

    erron(1);
    forcecga = upcase(argv[1][0]) == 'C';
    argv1 = argv[1];
    argv2 = argv[2];
    argv3 = argv[3];
    vidinit(0);
    loadchar(argv[2][0]);
    loadgfx();
    readfile("map.bin", mapbin, 13104);
    readfile("tcd.bin", (char *)tcdbin, 6840);
    readfile(dirtab[vidmode + 23], 0xA666, 1632);
    lastmon = savlast;
    turns = savturns;
    foodcnt = 1;
    redraw = gmode = prevmode = 0;
    nmons = cntmon();
    nvehs = cntveh();
    seedrnd(plseed + clockrnd() % 999);
    if (plstat[0] <= 0 || plfood <= 0)
        death();
    else
        drawwrld();
    showstat();
    for (;;) {
        if (plstat[0] <= 0 || plfood <= 0)
            death();
        cnt = 0;
        arrowr(0, 24);
        do {
            c = keyup();
            cnt++;
            if (gmode == 0)
                drawview(cnt);
        } while (c == 0 && cnt < 1500);
        if (c == 0)
            c = ' ';
        switch (c) {
        case 3:
        case 4:
        case 2:
        case 1:
            domove(c);
            break;
        case 'A':
            cmdattk(-1);
            break;
        case '8':
            cmdattk(3);
            break;
        case '2':
            cmdattk(4);
            break;
        case '6':
            cmdattk(2);
            break;
        case '4':
            cmdattk(1);
            break;
        case 'B':
            cmdboard();
            break;
        case 'C':
            cmdcast();
            break;
        case 'D':
            cmddrop();
            break;
        case 'E':
            cmdenter();
            break;
        case 'F':
            cmdfire();
            break;
        case 'G':
            cmdget();
            break;
        case 'H':
            cmdhyper();
            break;
        case 'I':
            cmdinfo();
            break;
        case 'K':
            cmdklimb();
            break;
        case 'N':
            cmdnoise();
            break;
        case 'O':
            cmdopen();
            break;
        case 'Q':
            cmdquit();
            break;
        case 'R':
            readycmd();
            break;
        case 'S':
            cmdsteal();
            break;
        case 'T':
            cmdtrans();
            break;
        case 'U':
            cmdunlk();
            break;
        case 'V':
            cmdview();
            break;
        case 'X':
            cmdxit();
            break;
        case 'Z':
            ztats();
            redraw = 1;
            arrowr(0, 24);
            break;
        case ' ':
            cmdpass();
            break;
        case 27:
            escflag = 1;
        default:
            cmdhuh();
        }
        if (gmode == 0) {
            animwat();
            if (nmons <= 10)
                spawnmon();
        }
        turns++;
        montrn();
        foodtick();
        if (gmode == 3)
            drawdung();
        prevmode = gmode;
    }
}
