/* opts: -ms -v -w -cs */
/*
 * OUT.EXE - the game, part B of 3 (game code 32F8-63F2: dungeon view, dungeon generation, enter/Get/Inform/Steal commands, shops, story pages (signpub-sellpric)).
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
extern char sfind[];
extern char snothing[];
extern char squest[];
extern char scoffin[];
extern char schest[];

#define MT ((int (*)[8])objtab)

extern char *msgleave, *plfile;
extern int escflag;
/* pub sign number n; always returns 6 */
signpub(n)
unsigned n;
{
    boxclr(colfill);
    switch (n) {
    case 0:
        boxtext(14, 4, "Pub de Varg");
        break;
    case 1:
        boxtext(11, 4, "Ye Olde Local Pub");
        break;
    case 2:
        boxtext(13, 4, "Dr. Cat's Lair");
        break;
    case 3:
        boxtext(13, 4, "Teaser's House");
        break;
    case 5:
        boxtext(12, 4, "Mary's Midway");
        break;
    case 6:
        boxtext(10, 4, "Kurtstable's Korner");
        break;
    case 7:
        boxtext(11, 4, "Dav's House-o-Suds");
        break;
    }
    return 6;
}

/* ---- ou_2b6d_c ---- */
/* draw the left wall edges (top and bottom) of dungeon depth step i */
edgel(i)
int i;
{
    if (i <= 5) {
        drawline(persp1[i - 1] + 16, persp2[i - 1] + 8, persp1[i] + 16, persp2[i] + 8, colline);
        drawline(persp1[i - 1] + 16, 151 - persp2[i - 1], persp1[i] + 16, 151 - persp2[i], colline);
    }
}

/* draw the right wall edges (top and bottom) of dungeon depth step i */
edger(i)
int i;
{
    if (i <= 5) {
        drawline(303 - persp1[i - 1], persp2[i - 1] + 8, 303 - persp1[i], persp2[i] + 8, colline);
        drawline(303 - persp1[i - 1], 151 - persp2[i - 1], 303 - persp1[i], 151 - persp2[i], colline);
    }
}

/* draw the front wall rectangle of depth step i (inset by 8 pixels when i is 0) */
frontrct(i)
int i;
{
    int dx, dy;

    dy = dx = 0;
    if (i == 0)
        dx = dy = 8;
    if (i <= 5) {
        drawline(persp1[i] + 16 + dx, persp2[i] + 8 + dy, 303 - persp1[i] - dx, persp2[i] + 8 + dy, colline);
        drawline(persp1[i] + 16 + dx, 151 - persp2[i] - dy, 303 - persp1[i] - dx, 151 - persp2[i] - dy, colline);
    }
}

/* draw the left wall's outline at depth step i */
outlinel(i)
int i;
{
    if (i <= 5) {
        drawline(persp1[i - 1] + 16, persp2[i] + 8, persp1[i] + 16, persp2[i] + 8, colline);
        drawline(persp1[i - 1] + 16, 151 - persp2[i], persp1[i] + 16, 151 - persp2[i], colline);
    }
}

/* draw the right wall's outline at depth step i */
outliner(i)
int i;
{
    if (i <= 5) {
        drawline(303 - persp1[i - 1], persp2[i] + 8, 303 - persp1[i], persp2[i] + 8, colline);
        drawline(303 - persp1[i - 1], 151 - persp2[i], 303 - persp1[i], 151 - persp2[i], colline);
    }
}

/* vertical lines at x=26 and x=28 spaced by step i */
sidelns(i)
int i;
{
    if (i <= 5) {
        drawdmon(26, i, colline);
        drawdmon(28, i, colline);
    }
}

deco1(i)
int i;
{
    if (i <= 5) {
        drawdmon(27, i, colline);
        drawdmon(28, i, colline);
    }
}

deco2(i)
int i;
{
    if (i <= 5) {
        drawdmon(27, i, colline);
        drawdmon(29, i, colline);
    }
}

deco3(i)
int i;
{
    if (i <= 5) {
        drawdmon(26, i, colline);
        drawdmon(29, i, colline);
    }
}

/* draw three horizontal rungs (a ladder) across the wall at depth step i */
ladder(i)
int i;
{
    int k, d;

    if (i <= 5) {
        d = persp2[i] + 8 - (151 - persp2[i]);
        d = ABS(d / 4);
        for (k = 1; k <= 3; k++)
            drawline(persp1[i] + 16, persp2[i] + 8 + k * d + i / 2, 303 - persp1[i],
                   persp2[i] + 8 + k * d + i / 2, colhigh);
    }
}

/* draw the left side door at depth step i: wall lines plus door frame */
doorl(i)
int i;
{
    int a, b;

    if (i <= 5) {
        edgel(i);
        a = persp1[i - 1] - persp1[i];
        b = persp2[i - 1] - persp2[i];
        a = ABS(a / 9);
        b = ABS(b / 5);
        drawline(a + 16 + a + persp1[i - 1], 151 - b - persp2[i - 1] - 1, a + 16 + a + persp1[i - 1],
               persp2[i] + 8 - b, colline);
        lineto(a * 6 + 16 + persp1[i - 1], persp2[i] + 8 + b, colline);
        lineto(a * 6 + 16 + persp1[i - 1], 151 - persp2[i] + b + b - (i == 1) * 2, colline);
    }
}

/* draw the right side door at depth step i */
doorr(i)
int i;
{
    int a, b;

    if (i <= 5) {
        edger(i);
        a = persp1[i - 1] - persp1[i];
        b = persp2[i - 1] - persp2[i];
        a = ABS(a / 9);
        b = ABS(b / 5);
        drawline(303 - (a + a + persp1[i - 1]), 151 - b - persp2[i - 1] - 1, 303 - (a + a + persp1[i - 1]),
               persp2[i] + 8 - b, colline);
        lineto(303 - (a * 6 + persp1[i - 1]), persp2[i] + 8 + b, colline);
        lineto(303 - (a * 6 + persp1[i - 1]), 151 - persp2[i] + b + b - (i == 1) * 2, colline);
    }
}

/* draw the front wall at depth step i with a doorway */
frontdr(i)
int i;
{
    int col, dy;

    dy = col = 0;
    if (i)
        col = colline;
    else
        dy = 8;
    if (i < 5) {
        frontrct(i);
        drawline(303 - persp1[i + 1], 151 - persp2[i] - dy, persp1[i + 1] + 16, 151 - persp2[i] - dy, col);
        lineto(persp1[i + 1] + 16, persp2[i + 1] + 8, colline);
        lineto(303 - persp1[i + 1], persp2[i + 1] + 8, colline);
        lineto(303 - persp1[i + 1], 151 - persp2[i] - dy, colline);
    }
}

/* vertical line at the left edge of depth step i */
vedgel(i)
int i;
{
    if (i <= 5)
        drawline(persp1[i] + 16, persp2[i] + 8, persp1[i] + 16, 151 - persp2[i], colline);
}

/* vertical line at the right edge of depth step i */
vedger(i)
int i;
{
    if (i <= 5)
        drawline(303 - persp1[i], persp2[i] + 8, 303 - persp1[i], 151 - persp2[i], colline);
}

/* name the dungeon feature kind y and draw it at depth x (kind 9 = front wall outline) */
drawfeat(x, y)
int x, y;
{
    if (x < 5) {
        msgline(dmonname[y]);
        if (y != 9)
            drawdmon(y, x, colline);
        else {
            frontrct(x);
            vedgel(x);
            vedger(x);
        }
    }
}

/* coffin at depth x */
drawcoff(x)
int x;
{
    msgline(scoffin);
    drawdmon(25, x, colline);
}

/* chest at depth x */
drawchst(x)
int x;
{
    msgline(schest);
    drawdmon(11, x, colline);
}

/* print the name of feature kind k ("Mimic" for 11) */
featname(k)
int k;
{
    if (k != 11)
        putstr(dmonname[k], coltext);
    else
        putstr("Mimic", coltext);
}

/* sign of x: -1, 0 or 1 */
sign(x)
int x;
{
    if (x < 0)
        return -1;
    if (x > 0)
        return 1;
    return 0;
}

/* dx of the facing direction dungdir: east 1, west -1 */
facedx()
{
    int r;

    r = 0;
    if (dungdir == 2)
        r = 1;
    else if (dungdir == 1)
        r = -1;
    return r;
}

/* dy of the facing direction dungdir: north -1, south 1 */
facedy()
{
    int r;

    r = 0;
    if (dungdir == 3)
        r = -1;
    else if (dungdir == 4)
        r = 1;
    return r;
}

/* print the compass direction at the status line */
showdir()
{
    arrowr(16, 19);
    switch (dungdir) {
    case 3:
        putstr(" North ", coltext);
        break;
    case 4:
        putstr(" South ", coltext);
        break;
    case 2:
        putstr(" East  ", coltext);
        break;
    case 1:
        putstr(" West  ", coltext);
        break;
    }
    arrowl(24, 19);
    gotoxy(1, 24);
}

/* draw the dungeon cell (x, y) at depth step depth */
drawcell(depth, x, y)
int depth, x, y;
{
    int t;

    t = dungmap[x][y].a;
    if (t == 3 || t == 2 || t == 1)
        frontrct(depth);
    if (t == 6)
        drawdmon(27, depth + 1, colline);
    if (t == 7)
        drawdmon(26, depth + 1, colline);
    drawfeat(depth, dungmap[x][y].b);
}

/* draw the dungeon cell (x, y) seen at depth step depth */
drawcel2(depth, x, y)
int depth, x, y;
{
    if (dungmap[x][y].b != -1)
        drawcell(depth, x, y);
    else {
        switch (dungmap[x][y].a) {
        case 1:
        case 2:
            frontrct(depth);
            break;
        case 3:
            frontdr(depth);
            break;
        case 8:
            ladder(depth);
            break;
        case 6:
            if (dungdir == 3 || dungdir == 4)
                deco1(depth + 1);
            else
                deco2(depth + 1);
            break;
        case 7:
            if (dungdir == 3 || dungdir == 4)
                sidelns(depth + 1);
            else
                deco3(depth + 1);
            break;
        }
    }
    if (dungmap[x][y].c == 5)
        drawcoff(depth + 1);
    else if (dungmap[x][y].c == 4)
        drawchst(depth + 1);
}

/* draw the wall piece at depth step depth, type side: 1/2 left-ish, 3 door, else right */
wallpc1(depth, side)
int depth, side;
{
    switch (side) {
    case 2:
    case 1:
        edger(depth);
        break;
    case 3:
        doorr(depth);
        break;
    default:
        outliner(depth);
    }
}

/* same for the other pair of wall lines */
wallpc2(depth, side)
int depth, side;
{
    switch (side) {
    case 2:
    case 1:
        edgel(depth);
        break;
    case 3:
        doorl(depth);
        break;
    default:
        outlinel(depth);
    }
}

/* is the square at (townx + a, towny + b) walkable? 1 yes, 0 no */
walkdng(a, b)
int a, b;
{
    int v;

    v = dungmap[townx + a][towny + b].a;
    if (v == 1 || v == 2 || v == 8)
        return 1;
    if (v == 3 && dungmap[townx][towny].a == 3)
        return 1;
    if (dungmap[townx + a][towny + b].b == -1)
        return 0;
    return 1;
}

/* ---- ou_4013 ---- */
/* passable test for map tile (x,y): 0 if blocked */
passable(x, y)
int x, y;
{
    if (dungmap[x][y].b == -1 || dungmap[x][y].b == 20 || dungmap[x][y].b == 11 || dungmap[x][y].b == 9)
        return 0;
    return 1;
}

/* is square at offset (dx,dy) from player walkable */
walkable(dx, dy)
int dx, dy;
{
    int t;

    t = dungmap[townx + dx][towny + dy].a;
    if (t == 1 || t == 2 || t == 3)
        return 1;
    return passable(townx + dx, towny + dy);
}

/* distance (1..5) walking along direction (a,b) until blocked */
viewdist(a, b)
int a, b;
{
    int n;

    n = 1;
    while (!walkable(a, b)) {
        n++;
        a += sign(a);
        b += sign(b);
    }
    if (n > 5)
        n = 5;
    return n;
}

/* tile type is not 0/6/7/8 */
char drawable(c)
unsigned char c;
{
    if (c == 0 || c == 7 || c == 6 || c == 8)
        return 0;
    return 1;
}

/* draw the first-person dungeon view in the facing direction dungdir */
drawdung()
{
    int a, b, c, d, ex, ey, dx, dy, n, dist, fx, fy, f1, f2;
    unsigned char pl, pr, l, r, fl, fr;

    d = c = b = a = 0;
    if (redraw) {
        dungscrn();
        redraw = 0;
    }
    fl = fr = pl = pr = 100;
    dx = facedx();
    dy = facedy();
    switch (dungdir) {
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
    fillrect(8, 8, 0x137, 0x97, 0);
    showdir();
    f1 = dungmap[townx][towny].a == 3;
    f2 = dungmap[townx][towny].a == 2 || dungmap[townx][towny].a == 1;
    dist = viewdist(dx, dy);
    if (f2)
        frontrct(0);
    else {
        n = dist;
        fx = ex = sign(dx) * n;
        fy = ey = sign(dy) * n;
        if (f1 && (dungmap[townx + dx][towny + dy].a == 1 || dungmap[townx + dx][towny + dy].a == 3
                   || dungmap[townx + dx][towny + dy].a == 2))
            frontrct(0);
        else {
            if (f1)
                frontdr(0);
            while (n) {
                ex -= dx;
                ey -= dy;
                if (!f1 || n > 1) {
                    l = dungmap[townx + ex + a][towny + ey + c].a;
                    r = dungmap[townx + ex + b][towny + ey + d].a;
                    wallpc2(n, l);
                    wallpc1(n, r);
                    l = drawable(l);
                    r = drawable(r);
                    if (n == dist) {
                        fl = l;
                        fr = r;
                    }
                    if (n != dist) {
                        if (pl != l)
                            vedgel(n);
                        if (pr != r)
                            vedger(n);
                    }
                }
                drawcel2(n, townx + ex + dx, towny + ey + dy);
                pl = l;
                pr = r;
                n--;
            }
            if (!f1)
                drawcel2(0, townx, towny);
            if (passable(townx + fx, towny + fy) && dungmap[townx + fx][towny + fy].a != 3 && dist < 5) {
                l = dungmap[a + townx + fx][c + towny + fy].a;
                r = dungmap[townx + b + fx][towny + d + fy].a;
                wallpc2(dist + 1, l);
                wallpc1(dist + 1, r);
                if (!drawable(l))
                    vedgel(dist);
                if (!drawable(r))
                    vedger(dist);
            } else {
                if (fl == 1)
                    vedgel(dist);
                if (fr == 1)
                    vedger(dist);
            }
        }
        if (f1) {
            if (dungmap[townx][towny].c == 5)
                drawcoff(1);
            else if (dungmap[townx][towny].c == 4)
                drawchst(1);
        }
    }
}

/* clear the screen area and print the status line with the dungeon level */
dungscrn()
{
    arrowr(15, 0);
    putstr(" Level    ", coltext);
    arrowl(26, 0);
    gotoxy(23, 0);
    putnum(dunglvl, 2, 32, coltext, 0);
    gotoxy(1, 24);
}

/* set the cell (plseed + 3*worldy + 5*worldx + 17*dunglvl) */
dungseed()
{
    int t;

    t = worldx * 5 + worldy * 3 + dunglvl * 17;
    seedrnd(t + plseed);
}

/* place one monster/ladder on a random free square of the dungeon level */
dungmon()
{
    int x, y, q, ok, cnt;

    cnt = 0;
    do {
        x = rndrange(0, 242) % 9 + 1;
        y = rndrange(0, 242) % 9 + 1;
        ok = dungmap[x][y].b == -1 && dungmap[x][y].a != 1 && dungmap[x][y].a != 2
             && dungmap[x][y].a != 8 && dungmap[x][y].c != 4;
    } while (!ok && ++cnt <= 500);
    if (cnt < 500 && !(x == townx && y == towny)) {
        do {
            cnt = rndrange(0, 49) / 10;
        } while (dmontype[cnt]);
        dmontype[cnt] = 1;
        q = (dunglvl - 1) / 2 * 5 + cnt;
        dungmap[x][y].b = q;
        dungmap[x][y].d = rndrange(1, dunglvl * dunglvl + 1) + 10 + q;
    }
}

/* generate a new dungeon level: walls, doors, traps, monsters */
mkdung()
{
    int i, j, k, m, d;

    dungscrn();
    dungseed();
    for (i = 0; i <= 10; i++)
        for (j = 0; j <= 10; j++) {
            dungmap[i][j].a = 0;
            dungmap[i][j].c = 0;
            dungmap[i][j].b = -1;
            dungmap[i][j].d = 0;
        }
    for (i = 0; i <= 10; i++) {
        dungmap[i][0].a = 1;
        dungmap[0][i].a = 1;
        dungmap[10][i].a = 1;
        dungmap[i][10].a = 1;
    }
    for (i = 2; i <= 8; i += 2) {
        dungmap[i][2].a = 1;
        dungmap[i][4].a = 1;
        dungmap[i][6].a = 1;
        dungmap[i][8].a = 1;
    }
    for (i = 2; i <= 8; i += 2)
        for (j = 1; j <= 9; j++)
            dungmap[i][j].a = rndrange(1, 4) - 1;
    m = dunglvl << 1;
    k = 0;
    while (k <= m) {
        if (rndrange(1, 255) > 160)
            d = 8;
        else
            d = 1;
        i = rndrange(1, 16) - 1;
        if (dunglvl & 1)
            dungmap[dirtab[38 + i]][dirtab[54 + i]].a = d;
        else
            dungmap[dirtab[54 + i]][dirtab[38 + i]].a = d;
        k++;
    }
    seedrnd(clockrnd() + 0x6F1);
    k = 0;
    while (k <= dunglvl) {
        d = (rndrange(1, 100) & 1) + 4;
        i = rndrange(10, 99) / 10;
        j = rndrange(10, 99) / 10;
        if (dungmap[i][j].a != 1 && dungmap[i][j].a != 2 && dungmap[i][j].a != 8)
            dungmap[i][j].c = d;
        k++;
    }
    D_ADBE = 0;
    if (dunglvl & 1) {
        D_AFD6 = 7;
        D_AE96 = 6;
    } else {
        D_AFD6 = 6;
        D_AE96 = 7;
    }
    if (dunglvl == 10)
        D_AFD6 = 0;
    if (dunglvl == 1) {
        D_ADB6 = 7;
        D_AFD6 = 0;
    }
    for (i = 0; i < 5; i++)
        dmontype[i] = 0;
    dungmon();
    dungmon();
    dungmon();
}

/* Turn around command */
cmdturna()
{
    msgline("Turn around");
    switch (dungdir) {
    case 3:
        dungdir = 4;
        break;
    case 4:
        dungdir = 3;
        break;
    case 2:
        dungdir = 1;
        break;
    case 1:
        dungdir = 2;
        break;
    }
}

/* Turn left command */
cmdturnl()
{
    msgline("Turn left");
    switch (dungdir) {
    case 3:
        dungdir = 1;
        break;
    case 4:
        dungdir = 2;
        break;
    case 2:
        dungdir = 3;
        break;
    case 1:
        dungdir = 4;
        break;
    }
}

/* Turn right command */
cmdturnr()
{
    msgline("Turn right");
    switch (dungdir) {
    case 3:
        dungdir = 2;
        break;
    case 4:
        dungdir = 1;
        break;
    case 2:
        dungdir = 4;
        break;
    case 1:
        dungdir = 3;
        break;
    }
}

/* Forward command: step one square unless blocked */
cmdfwd()
{
    int dx, dy;

    dx = facedx();
    dy = facedy();
    putstr("Forward", coltext);
    if (walkdng(dx, dy) == 0) {
        townx += dx;
        towny += dy;
        msgline(sempty);
    } else {
        playif(0);
        msgline(" - path blocked!");
    }
}

/* dungeon command dispatch: 1 left, 2 right, 3 forward, 4 turn around */
dungcmd(c)
unsigned char c;
{
    switch (c) {
    case 4:
        cmdturna();
        break;
    case 3:
        cmdfwd();
        break;
    case 1:
        cmdturnl();
        break;
    case 2:
        cmdturnr();
        break;
    }
}

/* enter the first dungeon level (start state) */
startdng()
{
    gmode = 3;
    dunglvl = townx = towny = 1;
    dungdir = 4;
    dungbon = 0;
    mkdung();
}

/* princess rescued: rewards and the time-machine message */
princess()
{
    int rx, ry, x, y, flg, cnt, off;

    off = cnt = 0;
    flg = plkills >= 20 && plexp >= 7000;
    if (!flg)
        off = 2;
    boxclr(colfill);
    boxtext(5, off + 5, " Thou hast saved the princess");
    boxtext(5, off + 6, princnam[castlenr]);
    putstr(".  She giveth thee", coltext);
    boxtext(8, off + 7, "500 hit points, pence and");
    boxtext(12, off + 8, "experience points.");
    plcoin += 500;
    plstat[0] += 500;
    plexp += 500;
    showcoin();
    showhp();
    showexp();
    if (flg) {
        boxtext(5, 10, "  She informs thee that thou");
        boxtext(5, 11, "art now ready for time travel,");
        boxtext(5, 12, "    and that there is a time");
        boxtext(5, 13, " machine far to the northwest!");
        if (owntimem < 9) {
            owntimem++;
            switch (quadrant(worldx, worldy)) {
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
                rx = rndrange(12, 21);
                ry = rndrange(11, 15);
                if (objat(x + rx, y + ry, 1) == 2) {
                    cnt = 500;
                    addmon(18, 2, x + rx, y + ry, 0);
                }
            } while (cnt++ < 499);
        }
    }
    waittick(5);
    waitpoll();
    gotoxy(1, 24);
}

/* enter a castle (c = 0x21..0x28): set up the level, player position */
entcastl(c)
int c;
{
    castlenr = c - 33;
    townmap = (c - 1) % 2;
    kingnr = castlenr + 1;
    gmode = 2;
    loadents();
    townx = 0;
    towny = 9;
    hostile = 0;
    hasjkey = 0;
    doorunl = 0;
    takeleft = 0;
    tcdbin[townmap][35][(townmap == 0) * 10 + 4] = 11;
    tcdbin[townmap][31][(townmap == 0) * 10 + 4] = 11;
    if (rndrange(1, 255) & 1)
        lockkey = 61;
    else
        lockkey = 60;
}

/* enter a town (c = 1..0x20): set up the level and position */
enttown(c)
int c;
{
    alesdrnk = 0;
    townmap = (c - 1) % 8 + 2;
    gmode = 1;
    loadents();
    townx = 19;
    towny = 17;
    drawlvl();
    redraw = 0;
}

/* show the sign / inscription text for a place (c = 0..7) */
placetxt(c)
unsigned c;
{
    boxclr(colfill);
    switch (c) {
    case 0:
        boxtext(8, 8, "You feel a strong magic");
        boxtext(12, 9, "surrounding you!");
        break;
    case 1:
        boxtext(10, 7, "You hear someone say,");
        boxtext(6, 9, " \"TURIS-SCIENTIA-MAGNOPERE\"");
        break;
    case 2:
        boxtext(13, 7, "A sign reads:");
        boxtext(8, 9, "\"FORTES FORTUNA ADIUVAT!\"");
        break;
    case 3:
        boxtext(13, 5, "On a pedestal,");
        boxtext(10, 6, "these words appear:");
        boxtext(8, 7, "\"MY NAME IS OZYMANDIAS,");
        boxtext(13, 8, "KING OF KINGS:");
        boxtext(11, 9, "LOOK AT MY WORKS,");
        boxtext(8, 10, "YE MIGHTY, AND DESPAIR!\"");
        boxtext(8, 12, "Nothing beside remains.");
        boxtext(7, 13, "You feel a strange force!");
        break;
    case 4:
        boxtext(12, 6, "The sign reads:");
        boxtext(12, 8, "\"ULTIMA THULE!\"");
        boxtext(7, 10, "The sky grows dark, and a");
        boxtext(7, 11, "strong magic engulfs you!");
        break;
    case 5:
        boxtext(12, 7, "The sign reads:");
        boxtext(11, 9, "\"OMNIA MUTANTUR!\"");
        break;
    case 6:
        boxtext(12, 7, "The sign reads:");
        boxtext(9, 9, "\"GO EAST TO GO EAST!\"");
        break;
    case 7:
        boxtext(10, 7, "The grave is marked:");
        boxtext(14, 9, "\"VAE VICTIS\"");
        break;
    }
    gotoxy(1, 24);
}

/* first visit message "A quest has been completed!" for an odd place */
firstvis(c)
int c;
{
    if (plquest[8 - c] == -1) {
        plquest[8 - c] = 1;
        putstr("A quest has been completed!", coltext);
        playif(5);
        msgline(sempty);
    }
}

/* enter a sign/shrine place (c = 0x29..0x30): text, gifts, stat gain */
monument(c)
int c;
{
    int v, hp, gain, ok;

    ok = 1;
    c -= 41;
    placetxt(c);
    if (c & 1)
        firstvis(c);
    if (c == lastmnmt)
        ok = 0;
    else {
        lastmnmt = c;
        v = mntattr[c];
        if (v == 0 && c == 2) {
            v = 0;
            do
                v++;
            while (ownweap[v] != 0);
            if (v > 15)
                ok = 0;
            else {
                ownweap[v]++;
                putstr("You find a", coltext);
                if (isvowel(wnames[v][0]))
                    putch('n', coltext);
                putch(' ', coltext);
                putstr(wnamelng[v], coltext);
                putstr(sbang, coltext);
            }
        } else if (c != 6) {
            hp = plstat[v];
            if (hp == 99)
                ok = 0;
            else {
                gain = (hp + 8) / 10;
                if (gain + hp > 99)
                    gain = 99 - hp;
                putstr("Thou dost gain ", coltext);
                putnum(gain, 1, 32, coltext, 1);
                putch(' ', coltext);
                putstr(attrnam[v], coltext);
                plstat[v] += gain;
            }
        }
    }
    if (c == 6)
        ok = 0;
    if (ok) {
        drawfrm(colline);
        playif(5);
        drawfrm(colfill);
    } else if (c != 6) {
        putstr("Hmmmm...no effect!", coltext);
        playif(6);
    }
    waitpoll();
    if (c != 6)
        msgline(sempty);
}

/* Enter command: enter the town/castle/dungeon/shrine at the player's spot */
cmdenter()
{
    int t, i, j, k;

    putstr("Enter", coltext);
    t = placeat(worldx, worldy);
    if (!t || gmode) {
        msgline(squest);
        playif(1);
    } else {
        msgline("ing...");
        hostile = nwater = ngrass = nwoods = 0;
        redraw = 1;
        for (i = -1; i <= 1; i++)
            for (j = -1; j <= 1; j++) {
                k = objat(worldx + i, worldy + j, 1);
                if (k == 0)
                    nwater++;
                else if (k == 1)
                    ngrass++;
                else if (k == 2)
                    nwoods++;
            }
        isbrit = t == 33;
        if (t < 33)
            putstr("The city of ", coltext);
        msgline(placenam[t]);
        if (t < 33)
            enttown(t);
        else if (t < 41)
            entcastl(t);
        else if (t < 49)
            monument(t);
        else
            startdng(t);
    }
}

/* Fire command (ship cannons / lasers) */
cmdfire()
{
    if (gmode == 1 || gmode == 2 || gmode == 3) {
        msgline("Fire?");
        playif(1);
    } else {
        putstr("Fire ", coltext);
        if (plveh != 4 && plveh != 5) {
            msgline("what?");
            playif(1);
        } else {
            if (plveh == 4)
                putstr("cannons: ", coltext);
            else
                putstr("lasers: ", coltext);
            aimshot(-1, 8);
        }
    }
}

/* ---- ou_5469 ---- */
/* "not the king's permission" message and sound */
kingperm()
{
    msgline(sempty);
    msgline("Thou hast not the king's");
    msgline("permission!");
    playif(1);
}

/* shop counter type 0x37: needs a turn counter, then stealarm (armour) */
cntrarm()
{
    if (takeleft == 0)
        kingperm();
    else {
        takeleft--;
        stealarm(0);
    }
}

/* shop counter type 0x3B: stealwpn (weapons) */
cntrweap()
{
    if (takeleft == 0)
        kingperm();
    else {
        takeleft--;
        stealwpn(0);
    }
}

/* shop counter type 0x39: stealfod (food) */
cntrfood()
{
    if (takeleft == 0)
        kingperm();
    else {
        takeleft--;
        stealfod(0);
    }
}

/* dispatch on the counter tile the player stands on */
cntrdisp()
{
    int c;

    c = tcdbin[townmap][townx][towny];
    switch (c) {
    case 0x39:
        cntrfood();
        break;
    case 0x37:
        cntrarm();
        break;
    case 0x3B:
        cntrweap();
        break;
    default:
        msgline(" - nothing here!");
    }
}

/* Get command */
cmdget()
{
    putstr("Get", coltext);
    if (gmode == 2)
        cntrdisp();
    else {
        if (gmode == 1)
            putstr(" what", coltext);
        msgline(squest);
        playif(1);
    }
}

/* HyperJump? message */
cmdhyper()
{
    msgline("HyperJump?");
    playif(1);
}

/* dungeon: look at the cell in the facing direction (secret door -> ladder) */
dunglook()
{
    int a, dx, dy;

    dx = facedx();
    dy = facedy();
    a = dungmap[townx + dx][towny + dy].a;
    putstr("Thou dost find ", coltext);
    if (a == 2 && dungmap[townx][towny].a != 3) {
        msgline("a secret door!");
        dungmap[townx + dx][towny + dy].a = 3;
    } else
        msgline(snothing);
}

/* Inform command: names the place you are on or looking at */
cmdinfo()
{
    int m;

    msgline("Inform and search");
    if (gmode == 3)
        dunglook();
    else {
        m = placeat(worldx, worldy);
        if (m) {
            if (m < 33)
                putstr("the city of ", coltext);
            msgline(placenam[m]);
        } else {
            m = terrain(worldx, worldy);
            if (m == 0)
                msgline("You are at sea");
            else if (m == 2)
                msgline("You are in the woods");
            else {
                msgline("You are in the lands");
                msgline(landnam[quadrant(worldx, worldy)]);
            }
        }
    }
}

/* Climb up a dungeon ladder: out of the dungeon on level 0 (hit point bonus) */
climbup()
{
    dunglvl--;
    putstr(" up to level ", coltext);
    putnum(dunglvl, 1, ' ', coltext, 1);
    msgline(sempty);
    if (dunglvl == 0) {
        if (dungbon + plstat[0] > 9999)
            dungbon = 9999 - plstat[0];
        if (dungbon) {
            putstr("Thou dost gain ", coltext);
            putnum(dungbon, 1, ' ', coltext, 1);
            putstr(" hit ", coltext);
            if (dungbon > 999)
                msgline(sempty);
            msgline("points");
            plstat[0] += dungbon;
            showhp();
        }
        gmode = 0;
        topbar();
        botbar();
    } else {
        mkdung();
        dungmap[townx][towny].b = -1;
    }
}

/* Climb down a dungeon ladder */
climbdwn()
{
    dunglvl++;
    putstr(" down to level ", coltext);
    putnum(dunglvl, 1, ' ', coltext, 1);
    msgline(sempty);
    mkdung();
}

char sklimb[] = "K-Limb";
char swhat[] = " what?";
char sfacelad[] = "Thou must face the ladder!";
/* K-Limb command (dungeon ladders only) */
cmdklimb()
{
    unsigned char c;

    putstr(sklimb, coltext);
    if (gmode != 3) {
        msgline(squest);
        playif(1);
    } else {
        c = dungmap[townx][towny].a;
        if (c != 7 && c != 6) {
            msgline(swhat);
            playif(1);
        } else if (dungdir == 2 || dungdir == 1) {
            msgline(sempty);
            msgline(sfacelad);
            playif(1);
        } else if (c == 7)
            climbup();
        else
            climbdwn();
    }
}

/* Noise command: toggle sound */
cmdnoise()
{
    putstr("Noise ", coltext);
    if (plnoise = !plnoise)
        msgline("on");
    else
        msgline("off");
}

/* put a chest-kept monster/treasure number at (x,y) in the dungeon level */
chestmon(x, y)
int x, y;
{
    int t, r, n;

    n = 0;
    t = dungmap[x][y].a;
    if (t == 0 || t == 7 || t == 6) {
        if (dungmap[x][y].b == -1) {
            do {
                t = rndrange(0, 49) / 10;
            } while (dmontype[t] && ++n < 50);
            dmontype[t] = 1;
            r = (dunglvl - 1) / 2 * 5 + t;
            dungmap[x][y].b = r;
            dungmap[x][y].d = rndrange(1, dunglvl * dunglvl + 1) + 10 + r;
        }
    }
}

char sopen[] = "Open";
char scoffin2[] = " coffin";
char snonehr[] = " - none here!";
/* Open coffin command (dungeon): monster or pence */
cmdopen()
{
    int x, y;

    x = facedx() + townx;
    y = facedy() + towny;
    putstr(sopen, coltext);
    if (gmode != 3) {
        msgline(squest);
        playif(1);
    } else {
        putstr(scoffin2, coltext);
        if (dungmap[townx][towny].c != 5) {
            msgline(snonehr);
            playif(1);
        } else {
            msgline(sempty);
            if (rndrange(1, 255) < 104 && dungmap[x][y].a != 1 && dungmap[x][y].a != 2 && dungmap[x][y].b == -1)
                chestmon(x, y);
            else {
                dungmap[townx][towny].c = 0;
                putstr(sfind, coltext);
                givecoin(rndrange(3, dunglvl * dunglvl + 9), 0);
            }
        }
    }
}

/* save the game to the player file */
savegame()
{
    putstr("saving game...", coltext);
    savlast = lastmon;
    savturns = turns;
    writfile(plfile, (int)plname, 820);
    msgline("saved.");
}

/* Quit command (outdoors only) */
cmdquit()
{
    putstr("Quit - ", coltext);
    if (gmode != 0) {
        msgline("only allowed outdoors!");
        playif(1);
    } else
        savegame();
}

/* did the thief get caught? (returns 1 if not caught) */
notcaugt()
{
    int r, flag;

    flag = 0;
    r = rndrange(1, 255);
    if (hostile || r < 38)
        flag = 1;
    if (!flag && plclass == 3)
        return 0;
    if (!flag && r > 77)
        return 0;
    msgline(sempty);
    msgline("Oh no!  Thou wert caught!");
    return hostile = 1;
}

/* steal a weapon (arg: check for being caught) */
stealwpn(chk)
int chk;
{
    int i, r;

    r = 0;
    if (chk)
        r = notcaugt();
    if (r == 0) {
        msgline(sempty);
        putstr("Thou dost find a", coltext);
        i = rndrange(1, 15);
        if (isvowel(wnamelng[i][0]))
            putch('n', coltext);
        if (strlen2(wnamelng[i]) > 8)
            msgline(sempty);
        else
            putch(' ', coltext);
        msgline(wnamelng[i]);
        if (ownweap[i] < 255)
            ownweap[i]++;
    }
}

/* steal food */
stealfod(chk)
int chk;
{
    int n, r;

    r = 0;
    if (chk)
        r = notcaugt();
    if (r == 0) {
        msgline(sempty);
        putstr("Thou dost find ", coltext);
        n = rndrange(2, 31);
        putnum(n, 1, ' ', coltext, 1);
        msgline(" bags of");
        msgline("food!");
        plfood += n;
        showfood();
    }
}

/* steal armour */
stealarm(chk)
int chk;
{
    int i, r;

    r = 0;
    if (chk)
        r = notcaugt();
    if (r == 0) {
        msgline(sempty);
        putstr("Thou dost find ", coltext);
        i = rndrange(1, 5);
        if (i > 4)
            putstr("a ", coltext);
        msgline(armornam[i]);
        if (ownarm[i] < 255)
            ownarm[i]++;
    }
}

/* Steal command */
cmdsteal()
{
    int c;

    putstr("Steal", coltext);
    if (gmode != 1 && gmode != 2) {
        msgline(squest);
        playif(1);
    } else {
        c = tcdbin[townmap][townx][towny];
        switch (c) {
        case 0x37:
            stealarm(1);
            break;
        case 0x3B:
            stealwpn(1);
            break;
        case 0x39:
            stealfod(1);
            break;
        default:
            msgline(" - nothing here!");
            playif(1);
        }
    }
}

/* clear the shop message box */
clrshop()
{
    fillrect(39, 48, 280, 120, 0);
}

/* "Sold!" message */
msgsold()
{
    clrshop();
    boxtext(18, 9, "Sold!");
    waitpoll();
}

/* "Done!" message */
msgdone()
{
    clrshop();
    boxtext(18, 9, "Done!");
    waitpoll();
}

/* "Thou canst not afford it!" message */
msgpoor()
{
    clrshop();
    boxtext(8, 9, "Thou canst not afford it!");
    playif(1);
    waitpoll();
}

/* story pages 0-8 (tavern seduction, advice from the king / castle people) */
gossip(n)
int n;
{
    clrshop();
    if (n)
        boxtext(11, 7, "Thou had best know");
    switch (n) {
    case 0:
        boxtext(9, 8, "Thou hast been seduced!");
        boxtext(11, 10, "After a long night,");
        boxtext(13, 11, "thou art back.");
        break;
    case 1:
        boxtext(10, 8, "about space travel!");
        boxtext(10, 9, "Thou must destroy at");
        boxtext(9, 10, "least 20 enemy vessels");
        boxtext(11, 11, "to become an ace!");
        break;
    case 2:
        boxtext(11, 8, "to watch the ");
        if (plsex == 0)
            putstr("wench.", coltext);
        else
            putstr("lecher.", coltext);
        break;
    case 3:
        boxtext(5, 8, " that the princess will give");
        boxtext(5, 9, " great reward to the one who");
        boxtext(5, 10, "rescues her, and an extra gift");
        boxtext(10, 11, "to an 8th level ace!");
        break;
    case 4:
        boxtext(7, 8, "thou must go back in time.");
        break;
    case 5:
        boxtext(10, 8, "thou should destroy");
        boxtext(13, 9, "the evil gem!");
        break;
    case 6:
        boxtext(5, 8, "   that many lakes and ponds");
        boxtext(5, 9, "  have strong magical powers!");
        break;
    case 7:
        boxtext(10, 8, "this is a great game!");
        break;
    case 8:
        boxtext(5, 8, "   that over 1000 years ago,");
        boxtext(5, 9, "Mondain the Wizard created an");
        boxtext(5, 10, " evil gem.  With this gem, he");
        boxtext(5, 11, "   is immortal and cannot be");
        boxtext(15, 12, "defeated.");
        while (!getkey(0))
            ;
        clrshop();
        boxtext(5, 7, "The quest of --Ultima-- is to");
        boxtext(5, 8, " traverse the lands in search");
        boxtext(8, 9, "of a time machine.  Upon");
        boxtext(5, 10, " finding such a device, thou");
        boxtext(5, 11, "should go back in time to the");
        boxtext(5, 12, " days before Mondain created");
        boxtext(5, 13, "the evil gem and destroy him!");
        while (!getkey(0))
            ;
    }
    if (n != 8)
        waitpoll();
}

/* is the cell at offset (dx,dy) from the player a type-50 entity? */
isent50(dx, dy)
int dx, dy;
{
    return whatat(townx + dx, towny + dy) == 50;
}

/* is a type-50 entity adjacent (8 directions)? */
ent50nr()
{
    return isent50(1, 1) || isent50(1, 0) || isent50(1, -1) || isent50(0, 1) ||
           isent50(0, -1) || isent50(-1, 1) || isent50(-1, 0) || isent50(-1, -1);
}

/* tavern: order ale (arg 'B') or refuse */
tavern(a)
char a;
{
    int r;

    r = signpub(townmap - 2);
    if (a == 'B') {
        if (plcoin == 0) {
            boxtext(12, r + 1, "Thou art broke!");
            boxtext(8, r + 4, "Come back when thou hast");
            boxtext(10, r + 5, "some money to spend.");
            waitpoll();
            gotoxy(15, 24);
            putstr(snothing, coltext);
        } else {
            plcoin--;
            showcoin();
            boxtext(8, r + 1, "The tavern keeper sayeth:");
            boxtext(9, r + 4, "Here, have a cold one!");
            gotoxy(15, 24);
            putstr("ale", coltext);
            alesdrnk++;
            waitpoll();
            if (alesdrnk > plsta / 4 && ent50nr()) {
                gossip(0);
                plcoin = plcoin / 2;
                showcoin();
                plwis--;
                if (plwis == 4)
                    plwis++;
            }
            if (rndrange(1, 255) < 75)
                gossip(rndrange(11, 89) / 10);
        }
    } else {
        boxtext(8, r + 2, "We have plenty of booze");
        boxtext(17, r + 3, "already!");
        waitpoll();
        gotoxy(16, 24);
        putstr(snothing, coltext);
    }
    msgline(sempty);
}

/* armour/weapon buying price for item n (charisma plint) */
buyprice(n)
int n;
{
    return (255 - plint) * n * n / 256 + 5;
}

/* selling price for item n (plcha) */
sellpric(n)
int n;
{
    return (plcha + 40) * n * n / 256 + 1;
}
