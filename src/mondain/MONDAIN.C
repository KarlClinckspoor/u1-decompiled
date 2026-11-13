/* opts: -ms -v -w -cs */
/*
 * MONDAIN.EXE - the final battle against Mondain (game code 03E9-2230).
 * Builds byte-identical with Lattice C 3.10 (lc -ms -v -w -cs).
 * Names are still the addresses they had in the original (F_xxxx code, D_xxxx DGROUP data).
 */

/* Origin support routines and their data */
extern int vidmode;
extern int wrange[];
extern char *wnames[], *spellnam[], *dirtab[];
extern drawline(), lineto(), gotoxy(), fillrect(), putstr(), playsnd(), msgline();
extern waittick(), drawtile(), F_4031(), putnum(), showhp(), F_3FF9();
extern writfile(), showfood(), scrlmsg(), putch(), presskey(), vwait(), pixel();
extern readfile(), clockrnd(), vidinit(), seedrnd(), arrowr(), flushkey(), readycmd();
extern showstat(), ztats();
extern int rndrange();
extern char upcase(), waitkey();
extern unsigned char getkey();
/* Lattice library */
extern EXECL(), F_1D0D();

/*
 * Initialised data (DGROUP 0124-0256)
 */
int _stack = 3000;                      /* stack size */
int plx = 3, ply = 4;             /* player position */
int mondx = 15, mondy = 4;            /* Mondain position */
int mondhp = 1000;                      /* Mondain hit points */
int fxfrm = -1;                        /* frame of the spell effect */
char *mondfile[3] = { "CgaMond.Bin", "EgaMond.Bin", "T1KMond.Bin" };
char *tilefile[3] = { "CgaTiles.Bin", "EgaTiles.Bin", "T1KTiles.Bin" };
int tilesize[3] = { 0x2c0, 0x580, 0x580 };
int gemfrm = 0, gemdir = 0;             /* bouncing gem */
unsigned char field[19][9] = {         /* the battle field */
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 20, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 5, 0, 0, 0, 5, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 5, 5, 5, 0, 5, 5, 5, 0},
    {0, 5, 0, 0, 0, 0, 0, 5, 0},
    {0, 5, 0, 0, 0, 0, 0, 5, 0},
    {0, 0, 0, 0, 6, 0, 0, 0, 0},
    {0, 5, 0, 0, 0, 0, 0, 5, 0},
    {0, 5, 0, 0, 0, 0, 0, 5, 0},
    {0, 5, 5, 5, 0, 5, 5, 5, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0}
};
int mondtile = 2, mondfram = 0, mondmode = 20;        /* Mondain's tile, frame, state */
int fighton = 0, gemgone = 0, wongame = 0;         /* fight on, gem destroyed, victory */

/*
 * Uninitialised data (DGROUP 2040-2E10 + 1A90)
 */
int colline, colfill;                     /* colours */
int colhigh, coltext, forcecga, gmode, D_204C;
int redraw, U_2050, U_2052;
int magx, magy;
char plname[16];
int plrace, plclass, plsex;
int plstat, plstr, plagi, plsta, plcha, plwis, plint, plcoin, plexp;
int plfood, rdyweap, rdyspel, rdyarm, plveh;
int plseed, worldx, worldy;
int plnoise, plquest[9];
char gemred[8];
char ownarm[12];
char ownweap[32];
int ownspel[11];
char ownveh[16];
char plkills[0x28e];
char tilegfx[0x78d];
char D_2B19[0x82];
char D_2B9B[0x45];
char D_2BE0[0x22c];
char *argv1, *argv2;
unsigned char bmpbuf[0x1a90];

/* wait about a while, or until a key is pressed */
idlewait() { long n; for (n = 0; !getkey(0) && ++n < 0x124F8; ) ; }

/* draw the frame of the big window in colour c and home the text cursor */
bigframe(c) int c; { drawline(0x19, 0x12, 0x129, 0x12, c); lineto(0x129, 0x8c, c); lineto(0x19, 0x8c, c); lineto(0x19, 0x12, c); gotoxy(4, 3); }

/* clear the big window and frame it */
clrbig(c) int c; { fillrect(0x16, 0xf, 0x12c, 0x8f, 0); bigframe(c); }

/* print string s at text position (a, b) */
printat(a, b, s) int a, b, s; { gotoxy(a, b); putstr(s, coltext); }

/* draw the frame of the smaller window in colour c */
smframe(c)
int c;
{
    drawline(0x22, 0x1a, 0x11c, 0x1a, c);
    lineto(0x11c, 0x7c, c);
    lineto(0x22, 0x7c, c);
    lineto(0x22, 0x1a, c);
    gotoxy(5, 4);
}

/* clear the smaller window and frame it */
clrsmall(c)
int c;
{
    fillrect(0x1f, 0x17, 0x11f, 0x7f, 0);
    smframe(c);
}

/* play sound n if sound is on */
playif(n)
int n;
{
    if (plnoise)
        playsnd(n);
}

/* "Pass" */
cmdpass()
{
    msgline("Pass");
}

/* "Huh?" and a beep */
cmdhuh()
{
    msgline("Huh?");
    playif(1);
}

/* toggle the sound */
cmdnoise()
{
    putstr("Noise ", coltext);
    if (plnoise = !plnoise)
        msgline("on");
    else
        msgline("off");
}

/* sign of x */
sign(x)
int x;
{
    if (x < 0)
        return -1;
    if (x > 0)
        return 1;
    return 0;
}

/* wait for a key, animating meanwhile */
unsigned char waitanim()
{
    char c, k;

    do {
        c = getkey(coltext);
        animate();
        if (c)
            waittick(1);
        else
            waittick(3);
    } while (c == 0);
    return c;
}

/* random check against the player's agility (or plclass == 1) */
agilchk()
{
    return rndrange(1, 0xf9) < plint + 0xa0 || plclass == 1;
}

#define abs(x) ((x) < 0 ? -(x) : (x))

/* player within 7 steps of Mondain */
nearmond()
{
    return abs(plx - mondx) + abs(ply - mondy) < 7;
}

/* move the player one step in direction dir */
moveplr(dir)
unsigned char dir;
{
    int t, x, y;

    x = plx + stepx(dir);
    y = ply + stepy(dir);
    if (x < 0 || x >= 19 || y < 0 || y >= 9)
        t = 2;
    else if (x == mondx && y == mondy)
        t = 2;
    else
        t = field[x][y];
    if (t == 0) {
        msgline(dirtab[dir]);
        drawtile(0, plx, ply);
        field[plx][ply] = 0;
        plx += stepx(dir);
        ply += stepy(dir);
        field[plx][ply] = 0x14;
        drawtile(0x14, plx, ply);
        playif(4);
    } else if (t == 1) {
        msgline("Blocked!");
        playif(0);
    } else {
        playif(0);
        F_4031();
        putstr("Burned! ", coltext);
        t = plstat / 10;
        if (t == 0)
            t++;
        putnum(t, 1, 0x20, coltext, 1);
        msgline(" damage!");
        playif(2);
        plstat -= t;
        showhp();
        F_3FF9();
    }
    if (gemnear() && gemfrm == 0)
        fighton = gemfrm = 1;
}

/* fire a shot in direction dir: range, damage and hit chance given */
fireshot(dir, snd, range, damage, chance)
int dir, snd, range, damage, chance;
{
    int t, dx, dy, x, y, i;

    dy = dx = 0;
    i = 1;
    playif(snd);
    dx = stepx(dir);
    dy = stepy(dir);
    do {
        x = dx * i + plx;
        y = i * dy + ply;
        if ((t = field[x][y]) == 0)
            if (x == mondx && y == mondy)
                t = 2;
        i++;
    } while (i <= range && t != 2 && x > 0 && y > 0 && x < 19 && y < 9);
    if (t != 2)
        msgline("Missed!");
    else if (rndrange(1, 100) <= chance) {
        mondhp -= damage;
        putstr("Hit Mondain! ", coltext);
        putnum(damage, 1, 0x20, coltext, 1);
        msgline(" damage!");
        playif(2);
    } else
        msgline("Missed!");
}

/* attack in direction dir with weapon w (7: ranged) or spell */
attackdr(dir, w)
int dir, w;
{
    int damage, chance, range;

    if (w == 7) {
        range = wrange[rdyweap];
        chance = plagi + 50;
        damage = rndrange(2, plstr + rdyweap * 3);
    } else {
        range = 3;
        chance = 0x50;
        damage = rndrange(1, plveh * 10) + 30;
    }
    fireshot(dir, w, range, damage, chance);
}

/* aim: read a direction (or use dir), say it, and attack with weapon w */
aimattk(dir, w)
int dir, w;
{
    int d;

    if (dir == -1)
        d = waitanim();
    else
        d = dir;
    if (d > 4)
        msgline("nothing");
    else {
        msgline(dirtab[d]);
        attackdr(d, w);
    }
}

/* Attack: choose weapon, ask for a direction */
cmdattk(dir)
int dir;
{
    fighton = 1;
    putstr("Attack with ", coltext);
    putstr(wnames[rdyweap], coltext);
    if (wrange[rdyweap] == 0) {
        msgline("?");
        playif(1);
    } else {
        putstr(": ", coltext);
        aimattk(dir, 7);
    }
}

/* damage of the spell: random up to plint, scaled by the spell */
spldamg()
{
    int t;

    t = rndrange(2, plint);
    if (rdyweap == 9)
        t <<= 1;
    else if (rdyweap == 8) {
        t *= 3;
        t /= 2;
    } else if (rdyweap == 10 || rdyweap == 11)
        t *= 3;
    return t;
}

/* spell with no effect (D_0618 test) */
splnone()
{
    msgline("");
    if (agilchk) {
        msgline("Hmmm...no effect!");
        playif(6);
        return 1;
    }
    return 0;
}

/* spell: ask a direction, put a block (5) in the empty square there */
splblock()
{
    unsigned char c;
    int x, y;

    putstr(": ", coltext);
    c = waitanim();
    if (c > 4) {
        msgline("nothing");
        playif(6);
        return 1;
    }
    msgline(dirtab[c]);
    if (!agilchk())
        return 0;
    x = plx + stepx(c);
    y = ply + stepy(c);
    if (field[x][y] == 0 && x != mondx && y != mondy) {
        field[x][y] = 5;
        drawtile(5, x, y);
        playif(5);
        msgline("Done.");
        return 1;
    }
    return 0;
}

/* spell: ask a direction, remove a block (5) from the square there */
splunblk()
{
    unsigned char c;
    int x, y;

    putstr(": ", coltext);
    c = waitanim();
    if (c > 4) {
        msgline("nothing");
        playif(6);
        return 1;
    }
    msgline(dirtab[c]);
    if (!agilchk())
        return 0;
    x = plx + stepx(c);
    y = ply + stepy(c);
    if (field[x][y] == 5) {
        field[x][y] = 0;
        drawtile(0, x, y);
        playif(5);
        msgline("Done.");
        return 1;
    }
    return 0;
}

/* spell: strengthen Mondain (doubles his hit points below 15000) */
splboost()
{
    msgline("");
    if (agilchk() && nearmond()) {
        msgline("\"INTERFICIO-NUNC!\"");
        msgline("The spell doth seem");
        msgline("to make him stronger!");
        if (mondhp < 15000)
            mondhp <<= 1;
        playif(5);
        return 1;
    }
    return 0;
}

/* spell: ask a direction and fire a bolt of spell damage */
splbolt()
{
    unsigned char c;
    int t;

    putstr(": ", coltext);
    c = waitanim();
    if (c > 4) {
        msgline("nothing");
        playif(6);
        return 0;
    }
    msgline(dirtab[c]);
    if (agilchk() && nearmond()) {
        t = spldamg();
        fireshot(c, 5, 5, t, 500);
        return 1;
    }
    return 0;
}

/* spell: teleport the player to a random empty square */
splblink()
{
    int x, y, r;

    r = 0;
    msgline("");
    if (agilchk()) {
        do {
            x = rndrange(1, 255) % 19;
            y = rndrange(1, 255) % 9;
        } while (field[x][y] != 0);
        drawtile(0, plx, ply);
        drawtile(0x14, x, y);
        field[x][y] = 0x14;
        field[plx][ply] = 0;
        plx = x;
        ply = y;
        r = 1;
        playif(5);
        msgline("Done.");
    }
    return r;
}

/* cast spell number w */
castspl(w)
int w;
{
    int t;

    t = 0;
    switch (w) {
    case 0:
        t = splnone();
        break;
    case 8:
        t = splblock();
        break;
    case 9:
        t = splunblk();
        break;
    case 10:
        t = splboost();
        break;
    case 7:
        t = splblink();
        break;
    default:
        t = splbolt();
    }
    if (t == 0) {
        playif(6);
        msgline("Failed!");
    }
}

/* Cast: use up a charge of spell rdyspel and cast it if it is a usable one */
cmdcast()
{
    putstr("Cast ", coltext);
    putstr(spellnam[rdyspel], coltext);
    if (ownspel[rdyspel] == 0) {
        msgline("");
        msgline("You've used up that spell!");
        playif(6);
    } else {
        if (rdyspel)
            ownspel[rdyspel]--;
        if (rdyspel != 3 && rdyspel != 7 && rdyspel != 8 && rdyspel != 9 && rdyspel != 10) {
            msgline("");
            msgline("Failed!");
            playif(6);
        } else
            castspl(rdyspel);
    }
}

/* is the square (player + (dx, dy)) inside the field and of kind 6 (the gem) */
isgem(dx, dy)
int dx, dy;
{
    int x, y;

    x = plx + dx;
    y = ply + dy;
    if (x > 0 && x < 19 && y > 0 && y < 9 && field[x][y] == 6)
        return 1;
    return 0;
}

/* is the gem on any of the eight squares around the player */
gemnear()
{
    return isgem(1, 1) || isgem(1, 0) || isgem(0, 1) || isgem(-1, -1)
        || isgem(-1, 0) || isgem(0, -1) || isgem(1, -1) || isgem(-1, 1);
}

/* Get (Gem): take the gem next to the player and destroy it (hurts the player) */
cmdget()
{
    int t, i;

    putstr("Get (Gem)", coltext);
    if (gemgone != 0 || !gemnear()) {
        msgline("..'tis nothing here!");
        playif(1);
    } else {
        gemgone = redraw = fighton = 1;
        field[14][4] = 0;
        gemfrm = 0;
        t = plstat * 3;
        t /= 4;
        putstr("... ", coltext);
        putnum(t, 1, 0x20, coltext, 1);
        msgline(" damage!");
        for (i = 0; i < 5; i++)
            playif(2);
        msgline("The Gem is DESTROYED!");
        plstat -= t;
        showhp();
        if (mondmode == 30)
            wongame = 1;
    }
}

/* Inform and Search: report on the gem */
cmdinfo()
{
    putstr("Inform and Search", coltext);
    if (gemgone) {
        msgline("");
        msgline("Mondain's magical aura doth");
        msgline("seem substantially diminished");
        msgline("in the absence of the gem.");
    } else {
        msgline("...it looks");
        msgline("as if he is creating the evil");
        msgline("gem!");
    }
}

/* print message s and beep */
refuse(s)
char *s;
{
    msgline(s);
    playif(1);
}

/* "Board?" */
cmdboard() { refuse("Board?"); }
/* "Drop?" */
cmddrop() { refuse("Drop?"); }
/* "Enter?" */
cmdenter() { refuse("Enter?"); }
/* "Fire?" */
cmdfire() { refuse("Fire?"); }
/* "Hyperjump?" */
cmdhyper() { refuse("Hyperjump?"); }
/* "Klimb?" */
cmdklimb() { refuse("Klimb?"); }
/* "Open?" */
cmdopen() { refuse("Open?"); }
/* "Quit...is not allowed!" */
cmdquit() { refuse("Quit...is not allowed!"); }

/* Steal: refused */
cmdsteal()
{
    msgline("Steal...he's watching,");
    refuse("That would be foolish!");
}

/* Transact: refused */
cmdtrans()
{
    msgline("Transact...Mondain will");
    refuse("not negotiate!");
}

/* "Unlock?" */
cmdunlk() { refuse("Unlock?"); }
/* "View?" */
cmdview() { refuse("View?"); }
/* "Xit?" */
cmdxit() { refuse("Xit?"); }

/* prompt for the Ultima I diskette and wait for a space */
diskprmt()
{
    scrlmsg();
    gotoxy(1, 0x17);
    putstr("Insert Ultima I diskette", coltext);
    gotoxy(1, 0x18);
    putstr("Hit space to continue...", coltext);
    while (waitkey(coltext) != 0x20)
        ;
    gotoxy(1, 0x17);
    putstr("                        ", coltext);
    gotoxy(1, 0x18);
    putstr("                        ", coltext);
    gotoxy(1, 0x17);
}

/* "THOU ART DEAD!": defeat message, save inuse.u1, back to OUT.EXE */
defeat()
{
    int r;

    msgline("THOU ART DEAD!");
    clrsmall(colfill);
    gotoxy(5, 6);
    putstr("   Thou hast been defeated by", coltext);
    gotoxy(5, 7);
    putstr("      Mondain the Wizard!", coltext);
    gotoxy(5, 12);
    putstr("    THE UNIVERSE IS DOOMED!", coltext);
    waittick(50);
    writfile("inuse.u1", (int)plname, 0x334);
    for (;;) {
        r = EXECL("out.exe", "out.exe", argv1, argv2, "M", 0);
        diskprmt();
    }
}

/* the player dies: hit points and food to 0, then defeat */
killplr()
{
    plstat = 0;
    plfood = 0;
    showhp();
    showfood();
    defeat();
}

/* draw the whole 19x9 battle map and the player */
drawfld()
{
    int i, j;

    redraw = 0;
    for (i = 0; i < 19; i++)
        for (j = 0; j < 9; j++)
            drawtile(field[i][j], i, j);
    drawtile(mondtile + mondfram, mondx, mondy);
    if (gemfrm) {
        gemfrm = 1;
        gemdir = 1;
    }
}

/* x step for a direction code */
stepx(d)
unsigned char d;
{
    if (d == 3 || d == 4)
        return 0;
    if (d == 2)
        return 1;
    return -1;
}

/* y step for a direction code */
stepy(d)
unsigned char d;
{
    if (d == 2 || d == 1)
        return 0;
    if (d == 3)
        return -1;
    return 1;
}

/* animate: spell effect, the bouncing tile, the player's tile */
animate()
{
    if (fxfrm >= 0) {
        if (fxfrm == 6) {
            fxfrm = -1;
            drawtile(0, magx, magy);
            field[magx][magy] = 0;
        } else {
            drawtile(fxfrm + 14, magx, magy);
            fxfrm++;
        }
    }
    if (gemfrm) {
        gemfrm += gemdir;
        if (gemfrm == 1)
            gemdir = 1;
        else if (gemfrm > 4)
            gemdir = -1;
        drawtile(gemfrm + 6 - 1, 14, 4);
    }
    if (mondtile == 2)
        mondfram = rndrange(1, 200) < 100;
    else if (mondtile == 11) {
        if (mondfram == 2)
            mondfram = 0;
        else
            mondfram++;
    }
    drawtile(mondtile + mondfram, mondx, mondy);
}

/* random sound/colour effect */
fxrand()
{
    int r;

    r = rndrange(1, 255);
    sign(r - 128);
}

/* move the player by (dx, dy) if the target square is free; returns 1 if moved */
movetry(dx, dy)
int dx, dy;
{
    int x, y, moved;

    moved = 0;
    x = mondx + dx;
    y = mondy + dy;
    if (x >= 0 && y >= 0 && x < 19 && y < 9 && field[x][y] == 0) {
        field[mondx][mondy] = 0;
        drawtile(0, mondx, mondy);
        mondx = x;
        mondy = y;
        drawtile(mondtile + mondfram, mondx, mondy);
        moved = 1;
    }
    return moved;
}

/* the player takes n points of damage */
hurtplr(n)
int n;
{
    putstr("Hit! ", coltext);
    putnum(n, 1, 0x20, coltext, 1);
    msgline(" damage!");
    plstat -= n;
    playif(2);
    showhp();
}

/* Mondain attacks in melee */
mondmele()
{
    msgline("Attacked by Mondain!");
    playif(3);
    if (rndrange(1, 255) > (0xb9 - plsta / 2 + (rdyarm << 3)) * 2)
        msgline("Missed!");
    else
        hurtplr(plstat / 32 + rndrange(1, 20));
}

/* Mondain casts a spell: magic missile, mind blaster or psionic shock */
mondcast()
{
    int kind, thr, hit, i;

    kind = rndrange(1, 0xee) % 3;
    putstr("Mondain casts ", coltext);
    switch (kind) {
    case 0:
        msgline("magic missile!");
        playif(5);
        thr = (0xc3 - (plwis + plint / 2)) * 2;
        if (hit = rndrange(1, 255) < thr)
            hurtplr(rndrange(1, 100));
        break;
    case 1:
        msgline("mind blaster!");
        playif(5);
        if (hit = rndrange(1, 200) > 140) {
            msgline("Hit!  Stats are reduced!");
            for (i = 1; i <= 6; i++) {
                if ((&plstat)[i] > 16)
                    (&plstat)[i] -= (&plstat)[i] / 8;
                else if ((&plstat)[i] >= 2)
                    (&plstat)[i] -= 2;
            }
        }
        break;
    case 2:
        msgline("psionic shock!");
        F_4031();
        playif(5);
        F_3FF9();
        if (hit = rndrange(1, 200) > 140) {
            i = plstat / 32;
            if (i > 255)
                i -= 254;
            hurtplr(i);
        }
        break;
    }
    if (!hit)
        msgline("Missed!");
}

/* the monsters act: Mondain chants, melee, casts, or the minions move */
mondturn()
{
    int dx, dy, dist, moved = 0;

    if (!fighton && rndrange(1, 200) < 17)
        msgline("You hear a strange chanting!");
    else if (fighton) {
        if (mondtile == 4)
            mondhp += 25;
        else {
            dx = mondx - plx;
            dy = mondy - ply;
            dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
            if (dist == 1 && mondmode == 20)
                mondmele();
            else if (dist < 7 && rndrange(1, 255) < 128 && mondmode == 20)
                mondcast();
            else {
                if (mondmode == 40) {
                    if (dx == 0)
                        dx = fxrand();
                    if (dy == 0)
                        dy = fxrand();
                }
                if (rndrange(1, 255) & 1) {
                    moved = movetry(0, sign(dy));
                    if (!moved)
                        moved = movetry(sign(dx), 0);
                } else {
                    moved = movetry(sign(dx), 0);
                    if (!moved)
                        moved = movetry(0, sign(dy));
                }
                if (!moved && mondmode == 40)
                    mondhp += 5;
            }
        }
    }
}

/* Mondain's state: spawn a magic square, change form by his hit points, or die */
mondstat()
{
    int n = 0;

    if (fighton && fxfrm == -1 && rndrange(1, 200) < 36) {
        do {
            magx = (rndrange(1, 255) + rndrange(1, 255)) % 19;
            magy = (rndrange(1, 255) + rndrange(1, 255)) % 9;
        } while (field[magx][magy] != 0 && ++n < 100);
        if (n < 100 && magx != mondx && magx != plx &&
            magy != mondy && magy != ply) {
            fxfrm = 0;
            field[magx][magy] = 14;
        }
    }
    if (mondhp > 500) {
        mondtile = 2;
        mondmode = 20;
    } else if (mondhp <= 500 && mondhp > 0 && mondtile != 11) {
        mondtile = 11;
        mondmode = 40;
        mondfram = 0;
    } else if (mondhp <= 0 && mondmode != 30) {
        putstr("Mondain is dead!", coltext);
        if (gemgone == 0)
            msgline("...or is he?");
        else {
            msgline("");
            wongame = 1;
        }
        mondtile = 4;
        mondmode = 30;
        mondfram = 0;
    }
}

/* read a key (waits with the animation, 0 if none) and return it upper-cased */
char pollkey()
{
    unsigned char k;

    k = upcase(getkey(coltext));
    waittick(1);
    if (k == 0) {
        waittick(1);
        if (gemfrm == 0)
            waittick(1);
        else
            vwait(4);
    }
    return k;
}

/* story screen 1: entering the craft, the gems, the colour legend */
story1()
{
    int c[4];
    int i;

    if (vidmode) {
        c[0] = 12;
        c[1] = 10;
        c[2] = 9;
        c[3] = 15;
    } else {
        c[0] = 2;
        c[1] = c[2] = 1;
        c[3] = 3;
    }
    clrbig(colfill);
    printat(5, 3, "Entering the craft, thou dost");
    printat(5, 4, "remark upon four holes marked:");
    printat(12, 6, "R    G    B    W");
    for (i = 0; i < 4; i++) {
        gotoxy(i * 5 + 12, 7);
        putch(4, c[i]);
    }
    printat(4, 9, "The proper gems fit in each hole.");
    printat(5, 11, "Further examination leads thee");
    printat(11, 12, "to two discoveries:");
    printat(4, 14, "There is a button marked LAUNCH,");
    printat(6, 15, "and thou art locked in, with");
    printat(6, 16, "nothing to do but press it!");
}

/* story screen 2: the launch button */
story2()
{
    clrbig(colfill);
    printat(4, 4, "As soon as thou hast pressed the");
    printat(4, 5, "large black button, lights begin");
    printat(4, 6, "  to dance across the control");
    printat(16, 7, "screens.");
    printat(4, 9, " Generators shake the very floor");
    printat(12, 10, "beneath thy feet.");
    printat(4, 12, "The small craft begins to tremble");
    printat(8, 13, "and rattle as thy journey");
    printat(9, 14, "through time begins...");
}

/* story screen 3: pulled from the craft */
story3()
{
    clrbig(colfill);
    printat(4, 8, "After but a few moments thou dost");
    printat(4, 9, "feel a strong magic pulling thee");
    printat(12, 10, "from thy craft...");
}

/* story screen 4: face to face with Mondain */
story4()
{
    clrbig(colfill);
    printat(11, 6, "...A moment later,");
    printat(9, 7, "thou art face to face");
    printat(4, 8, " with the evil Mondain himself!");
    printat(9, 13, "Good luck, this is it!");
}

/* the four story screens, each followed by a pause */
storyrun()
{
    story1();
    presskey();
    story2();
    presskey();
    story3();
    presskey();
    story4();
    presskey();
}

unsigned char D_08BB[8] = { 0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1 };

/* load file `name` (n bytes) and draw it as a 1-bit 320-wide image */
showbmp(n, name)
int n;
char *name;
{
    int x, y, i, j, col;
    unsigned char b;

    j = i = y = x = 0;
    col = 15;
    readfile(name, bmpbuf, n);
    if (vidmode == 0)
        col = 3;
    for (i = 0; i < n; i++) {
        b = bmpbuf[i];
        for (j = 0; j < 8; j++) {
            if (b & D_08BB[j])
                pixel(x, y, col);
            else
                pixel(x, y, 0);
            x++;
            if (x > 319) {
                x = 0;
                y++;
            }
        }
    }
}

/* victory: show the player, wait for a key */
victory()
{
    int i;

    drawtile(4, mondx, mondy);
    pixel(50, 7, colline);
    plnoise = 1;
    clrsmall(colfill);
    printat(11, 8, "THOU ART VICTORIOUS!");
    for (i = 0; i < 3; i++)
        playif(5);
    waittick(10);
    flushkey();
    while (!getkey(0) && i++ < 10000)
        ;
}

/* end of the game: victory screen, nif.bin picture, restart message */
endgame()
{
    victory();
    fillrect(0, 0, 319, 199, 0);
    showbmp(0x1a40, "nif.bin");
    waittick(75);
    gotoxy(4, 24);
    putstr("Press CONTROL-ALT-DEL to Restart", coltext);
    waittick(0);
}

/* main: load data files, story screens, then the key-driven battle loop */
main(argc, argv)
int argc;
unsigned char **argv;
{
    unsigned char key;
    int i, sz;

    sz = 64;
    F_1D0D(1);
    forcecga = upcase(argv[1][0]) == 'C';
    argv1 = argv[1];
    argv2 = argv[2];
    vidinit(0);
    if (vidmode)
        sz = 128;
    for (i = 0; i < 128; i++)
        tilegfx[i] = 0;
    readfile("inuse.u1", (int)plname, 0x334);
    readfile(tilefile[vidmode], tilegfx + sz * 10, tilesize[vidmode]);
    readfile(mondfile[vidmode], tilegfx + sz, 0xa80 - sz);
    gmode = 5;
    showstat();
    seedrnd(plseed + clockrnd());
    storyrun();
    drawfld();
    for (;;) {
        i = 0;
        arrowr(0, 24);
        do {
            key = pollkey();
            i++;
            animate();
        } while (key == 0 && i < 1500);
        if (key == 0)
            key = ' ';
        switch (key) {
        case 3:
        case 4:
        case 2:
        case 1:
            moveplr(key);
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
            gemfrm = 1;
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
            D_204C = 1;
        default:
            cmdhuh();
        }
        if (redraw)
            drawfld();
        mondstat();
        if (wongame) {
            endgame();
            D_204C = 1;
        }
        mondturn();
        if (plstat > 0 && plfood > 0)
            continue;
        killplr();
    }
}
