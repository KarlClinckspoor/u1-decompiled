/* opts: -ms -v -w -cs */
/*
 * OUT.EXE - the game, part A of 3 (game code 03E9-32F8: save/load, movement, combat and spells, drop lists, dungeon entities, shops signs, wall drawing start (loadchar-signfood)).
 * The three parts are one program split for the compiler's memory (LC1 stops at about 2200 lines);
 * string literals used in more than one part are named arrays (D_xxxx = their DGROUP address).
 * Builds byte-identical with Lattice C 3.10 (lc -ms -v -w -cs).
 * Names are still the addresses they had in the original (F_xxxx code, D_xxxx DGROUP data).
 */

/* Origin library and Lattice library */
extern F_1D0D(), gotoxy(), strlen2(), drawtile(), putch(), putstr(), fillrect();
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

/* initialised data (DGROUP 0124-0184); the strings of the literals follow in order of use */
int _stack = 3072;
char *tilefile[3] = { "CgaTiles.Bin", "EgaTiles.Bin", "T1KTiles.Bin" };
char *msgleave = "Thy canst not leave it here!";
char *plfile = "playerx.u1";
int escflag = 0;


#define MT ((int (*)[8])objtab)

/* ---- ou_03e9 ---- */
/* read the player file (slot digit c, '1'-'4') or inuse.u1 into the record */
loadchar(c)
unsigned char c;
{
    if (c < '1' || c > '4') {
        textmode();
        EXIT(1);
    }
    plfile[6] = c;
    if (argv3 == 0)
        readfile(plfile, (int)plname, 820);
    else
        readfile("inuse.u1", (int)plname, 820);
}

/* read/write the town or dungeon file for the current level */
loadgfx()
{
    int t;

    if (vidmode == 0)
        t = 0x0D00;
    else
        t = 0x1A00;
    readfile(tilefile[vidmode], tilegfx, t);
}

/* play sound effect n if sound is on */
playif(n)
int n;
{
    if (plnoise)
        playsnd(n);
}

/* print blocked message n, play sound 0, return 0 */
blocked(n)
int n;
{
    msgline(blockmsg[n]);
    playif(0);
    return 0;
}

char sbang[] = "!";
/* try to step in direction c: returns 1 if the square can be entered, else prints why */
canstep(c)
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
    t = objat(viewx(dx + 9), viewy(dy + 4), 1);
    ok = 1;
    lvl = plveh;
    if (t >= 19 && t <= 47) {
        idx = (t - 19) / 2;
        putstr(blockmsg[5], coltext);
        ok = 0;
        if (isvowel(strtab[106 + idx][0]))
            putch('n', coltext);
        putch(' ', coltext);
        if (t == 47) {
            scrlmsg();
            gotoxy(1, 24);
        }
        putstr(strtab[106 + idx], coltext);
        msgline(sbang);
        playif(0);
    } else if (t == 3) {
        ok = blocked(1);
    } else if (t == 0) {
        if (lvl != 4 && lvl != 3 && lvl != 5)
            ok = blocked(0);
    } else if (t == 2 && lvl == 5) {
        ok = blocked(2);
    } else if (lvl == 4 && t != 0) {
        ok = blocked(3);
    } else if (lvl == 3 && t != 0) {
        ok = blocked(4);
    } else if (lvl >= 6) {
        ok = blocked(6);
    }
    return ok;
}

/* movement command in direction c (dungeon/town/space/overworld dispatch) */
domove(c)
unsigned char c;
{
    int t;

    if (gmode == 1 || gmode == 2)
        townstep(c);
    else if (gmode == 3)
        dungcmd(c);
    else if (canstep(c)) {
        t = quadrant(worldx, worldy);
        if (plveh <= 2)
            playif(4);
        msgline(dirtab[c]);
        switch (c) {
        case 3: scrolln(); break;
        case 4: scrolls(); break;
        case 2: scrolle(); break;
        case 1: scrollw(); break;
        }
        objtab[0].x = worldx;
        objtab[0].y = worldy;
        if (quadrant(worldx, worldy) != t)
            rmregion(t);
    }
}

/* name of monster type a (a = 19, 21, ...) */
monname(a)
int a;
{
    return strtab[106 + (a - 19) / 2];
}

char sbangsp[] = "! ";
char scopper[] = " copper!";
char sempty[] = "";
char ssilver[] = " silver!";
char sgold[] = " gold!";
/* give the player n pence (capped at 9999) and print "Thou dost find: x copper! y silver! z gold!" */
givecoin(n, t)
int n, t;
{
    int ones, tens, thou, ok, f;

    f = 0;
    if (n + plcoin > 9999)
        n = 9999 - plcoin;
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
            msgline(sbang);
            ok = 0;
        } else {
            putstr(sbangsp, coltext);
            ok = 1;
        }
        if (ones) {
            putnum(ones, 1, ' ', coltext, 1);
            putstr(scopper, coltext);
            if (ok) {
                msgline(sempty);
                ok = 0;
            } else
                ok = 1;
        }
        if (tens) {
            putnum(tens, 2, ' ', coltext, 1);
            putstr(ssilver, coltext);
            if (ok) {
                msgline(sempty);
                ok = 0;
            } else
                ok = 1;
        }
        if (thou) {
            putnum(thou, 2, ' ', coltext, 1);
            putstr(sgold, coltext);
            if (ok) {
                msgline(sempty);
                ok = 0;
            } else
                ok = 1;
        }
        if (ok)
            msgline(sempty);
        plcoin += n;
        showcoin();
    }
}

/* monster/guard kill reward hook: some objects (9,14,19,24) refresh a counter */
killhook(n)
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
        firstvis(8 - v);
}

char shit[] = "Hit! ";
char sdestroy[] = " destroyed!";
char skilled[] = " killed!";
char sfind[] = "Thou dost find: ";
char sdamagex[] = " damage!";
char smissed[] = "Missed!";
/* attack the square (x,y) with sound snd, hit chance hit%, damage dmg (10000 = destroy) */
dunghit(snd, hit, dmg, x, y)
int snd, hit, dmg, x, y;
{
    int r1, r2, ok;

    ok = 1;
    if (dungmap[townx][towny].a == 3)
        ok = dungmap[x][y].a == 0 || dungmap[x][y].a == 7 || dungmap[x][y].a == 6;
    if (rndrange(1, 100) <= hit
        && dungmap[townx][towny].a != 1
        && dungmap[townx][towny].a != 2
        && dungmap[townx][towny].a != 8
        && ok) {
        playif(snd);
        if (dmg != 10000)
            putstr(shit, coltext);
        if (dungmap[x][y].d <= dmg) {
            featname(dungmap[x][y].b);
            if (dmg == 10000)
                msgline(sdestroy);
            else
                msgline(skilled);
            killhook(dungmap[x][y].b);
            r1 = rndrange(2, dunglvl * 3 + 10 + dungmap[x][y].b);
            putstr(sfind, coltext);
            givecoin(r1, 0);
            dmontype[dungmap[x][y].b % 5] = 0;
            dungmap[x][y].b = -1;
            dungmap[x][y].d = 0;
            r2 = rndrange(2, dunglvl * dunglvl + 10);
            plexp += r2;
            dungbon += r2 * 2;
            showexp();
            dungmon();
        } else {
            putnum(dmg, 1, ' ', coltext, 1);
            msgline(sdamagex);
            dungmap[x][y].d -= dmg;
        }
    } else
        msgline(smissed);
}

/* how far a shot in the current direction travels (0 if blocked at once) */
shotrng(range)
int range;
{
    int i, dx, dy, ox, oy, a, b;

    i = 0;
    dx = facedx();
    dy = facedy();
    do {
        i++;
        ox = dx * i;
        oy = i * dy;
        a = dungmap[townx + ox][towny + oy].a;
        b = dungmap[townx + ox][towny + oy].b;
    } while (i < range && b == -1 && a != 1 && a != 2 && a != 8 && a != 3);
    if (b == -1 || a == 1 || a == 2 || a == 8)
        return 0;
    return i;
}

/* dungeon attack: weapon shot along the facing direction */
dungshot()
{
    int hit, dmg, r;

    msgline(sempty);
    playif(7);
    r = shotrng(wrange[rdyweap]);
    if (!r) {
        msgline("Nothing");
    } else {
        hit = plagi + 50;
        dmg = rndrange(2, plstr + rdyweap * 8);
        dunghit(2, hit, dmg, townx + r * facedx(), towny + r * facedy());
    }
}

char smiss[] = "Miss!";
char skilthe[] = "Killed the ";
char shitthe[] = "Hit the ";
char sdamage[] = " damage";
char smisthe[] = "Missed the ";
/* fire a projectile (tile) from the player in direction c; hits monsters in the path */
projectl(c, snd, range, dmg, hit, tile)
unsigned char c;
int snd, range, dmg, hit, tile;
{
    int t, dx, dy, ox, oy, i, id, x, y, r2, r1, cls;

    i = 1;
    if (gmode == 1 || gmode == 2)
        townatk(c, range, dmg, hit);
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
            drawtile(tile, ox + 9, oy + 4);
            if (i == 1)
                playif(snd);
            else
                waittick(2);
            drawtile(t = objat(worldx + ox, worldy + oy, 1), ox + 9, oy + 4);
            i++;
        } while (i <= range && t < 19 && t != 3);
        if (t < 19)
            msgline(smiss);
        else if (rndrange(1, 100) <= hit) {
            cls = strlen2(monname(t));
            x = viewx(ox + 9);
            y = viewy(oy + 4);
            drawtile(tile, ox + 9, oy + 4);
            id = findmon(x, y, 1);
            objtab[id].f8 -= dmg;
            if (objtab[id].f8 <= 0)
                putstr(skilthe, coltext);
            else
                putstr(shitthe, coltext);
            putstr(monname(t), coltext);
            if (objtab[id].f8 > 0) {
                if (cls > 6)
                    msgline(sbang);
                else
                    putstr(sbangsp, coltext);
                putnum(dmg, 2, ' ', coltext, 1);
                msgline(sdamage);
                playif(2);
                drawtile(t, ox + 9, oy + 4);
            } else {
                delmon(x, y);
                nmons--;
                r1 = rndrange(1, dmgfact[(t - 19) / 2] * 4) + 10;
                r2 = rndrange(2, dmgfact[(t - 19) / 2] * 5);
                plexp += r2;
                playif(2);
                drawtile(terrain(x, y), ox + 9, oy + 4);
                givecoin(r1, cls);
                showexp();
            }
        } else {
            putstr(smisthe, coltext);
            putstr(monname(t), coltext);
            msgline(sbang);
        }
    }
}

/* shoot in direction c with weapon (k = 7) or spell: picks range/hit/damage then projectl */
shootdir(c, k)
unsigned char c;
int k;
{
    int dmg, hit, range;

    if (k == 7) {
        range = wrange[rdyweap];
        hit = plagi + 50;
        dmg = rndrange(2, plstr + rdyweap * 8);
    } else {
        range = 3;
        hit = 80;
        dmg = rndrange(1, plveh * 10) + 30;
    }
    projectl(c, k, range, dmg, hit, 50);
}

char snothing[] = "nothing";
/* aim: direction a (-1 = ask the player), print it, shoot */
aimshot(a, b)
int a, b;
{
    unsigned char c;

    if (a == -1)
        c = keywait();
    else
        c = a;
    if (c > 4)
        msgline(snothing);
    else {
        msgline(dirtab[c]);
        shootdir(c, b);
    }
}

char sattkwth[] = "Attack with ";
char squest[] = "?";
char scolon[] = ": ";
/* Attack command: weapon name, "?" if it has no range, else aim and shoot */
cmdattk(a)
int a;
{
    putstr(sattkwth, coltext);
    putstr(wnames[rdyweap], coltext);
    if (wrange[rdyweap] == 0) {
        msgline(squest);
        playif(1);
    } else if (gmode == 3)
        dungshot();
    else {
        putstr(scolon, coltext);
        aimshot(a, 7);
    }
}

/* save state and exec the vehicle program (space.exe / mondain.exe) */
execveh(prog, tile)
char *prog;
int tile;
{
    int r;

    drawtile(tile, 9, 4);
    savlast = lastmon;
    savturns = turns;
    writfile("inuse.u1", (int)plname, 820);
    for (;;) {
        r = EXECL(prog, prog, argv1, argv2, 0);
        diskprmt();
    }
}

/* Board command: mount a horse/vehicle, or start the space ship / Mondain's fight */
cmdboard()
{
    int t;

    if (gmode != 0) {
        msgline("Board?");
        playif(1);
    } else if (plveh != 0) {
        msgline("X-it thy craft first!");
        playif(1);
    } else {
        t = objat(worldx, worldy, 1);
        if (t < 11 || t > 18) {
            msgline("Nothing to Board!");
            playif(1);
        } else {
            objtab[0].type = t;
            delmon(worldx, worldy);
            plveh = t - 11 + 1;
            if (plveh > 4)
                plveh--;
            if (t == 11 || t == 12)
                putstr("Mount ", coltext);
            else
                putstr("Board ", coltext);
            msgline(vehnam[plveh]);
            if (t == 17)
                execveh("space.exe", t);
            else if (t == 18) {
                if (gemred && gemwhite && gemblue && gemgreen)
                    execveh("mondain.exe", t);
                else {
                    boxclr(colfill);
                    boxtext(6, 7, "Thou canst not determine how");
                    boxtext(6, 8, "to operate the craft at this");
                    boxtext(6, 9, "           time.");
                    waitpoll();
                    gotoxy(1, 24);
                }
            }
        }
    }
}

/* spell damage roll, scaled by the selected spell rdyweap */
spldamg()
{
    int r;

    r = rndrange(1, plint);
    if (rdyweap == 9)
        r *= 2;
    else if (rdyweap == 8) {
        r *= 3;
        r /= 2;
    } else if (rdyweap == 10 || rdyweap == 11)
        r *= 3;
    return r;
}

/* open a coffin/chest: clears the container and pays out pence */
openchst()
{
    msgline(" opened!");
    dungmap[townx][towny].c = 0;
    putstr(sfind, coltext);
    givecoin(rndrange(3, dunglvl * dunglvl + 9), 0);
}

char scoffin[] = "Coffin";
char schest[] = "Chest";
char sladdcr[] = "Ladder created!";
char stelep[] = "Teleported!";
char sfieldcr[] = "Field created!";
char sfielddl[] = "Field destroyed!";
char sfailed[] = "Failed!";
/* cast spell a (dungeon spells 1-10); b = print its name first */
dungspel(a, b)
int a, b;
{
    int f, i, j;

    f = 0;
    if (b)
        msgline(strtab[185 + a]);
    switch (a) {
    case 1:
        if (dungmap[townx][towny].c == 5) {
            putstr(scoffin, coltext);
            openchst();
        } else
            f = 1;
        break;
    case 2:
        if (dungmap[townx][towny].c == 4) {
            putstr(schest, coltext);
            openchst();
        } else
            f = 1;
        break;
    case 3:
        i = shotrng(5);
        if (i) {
            dunghit(5, 101, spldamg(), townx + i * facedx(), towny + i * facedy());
        } else
            f = 1;
        break;
    case 5:
        if (dunglvl == 10 || dungmap[townx][towny].a == 8)
            f = 1;
        else if ((townx & 1) || (towny & 1)) {
            dungmap[townx][towny].a = 6;
            msgline(sladdcr);
        } else
            f = 1;
        break;
    case 6:
        if (((townx & 1) || (towny & 1)) && dungmap[townx][towny].a != 8) {
            dungmap[townx][towny].a = 7;
            msgline(sladdcr);
        } else
            f = 1;
        break;
    case 7:
        do {
            i = rndrange(1, 9);
            j = rndrange(1, 9);
        } while ((i == townx && j == towny)
                 || (dungmap[i][j].a == 8 || dungmap[i][j].a == 1 || dungmap[i][j].a == 2)
                 || dungmap[i][j].b != -1);
        townx = i;
        towny = j;
        msgline(stelep);
        break;
    case 8:
        i = townx + facedx();
        j = towny + facedy();
        if (dungmap[i][j].a == 0 && dungmap[i][j].b == -1 && dungmap[i][j].c == 0) {
            dungmap[i][j].a = 8;
            msgline(sfieldcr);
        } else
            f = 1;
        break;
    case 9:
        i = townx + facedx();
        j = towny + facedy();
        if (dungmap[i][j].a == 8) {
            dungmap[i][j].a = 0;
            msgline(sfielddl);
        } else
            f = 1;
        break;
    case 10:
        i = townx + facedx();
        j = towny + facedy();
        if (dungmap[i][j].b != -1)
            dunghit(5, 101, 10000, i, j);
        else
            f = 1;
        break;
    }
    if (f) {
        playif(6);
        msgline(sfailed);
    } else if (a != 10 && a != 3)
        playif(5);
}

/* ---- ou_1769_a ---- */
/* first spell (Prayer): heals or feeds when low, else may remove a nearby monster */
splprayr()
{
    int i, done;
    struct mon *m;

    i = 1;
    done = 0;
    msgline(sempty);
    msgline(strtab[185]);
    if (gmode == 3) {
        if ((i = rndrange(1, 10)) == 4)
            i++;
        dungspel(i, 0);
    } else {
        if (plstat[0] < 15) {
            plstat[0] = 15;
            msgline(" Shazam!");
            showhp();
            playif(5);
            done = 1;
        } else if (plfood < 15) {
            plfood = 15;
            msgline(" Shazam!");
            showfood();
            playif(5);
            done = 1;
        } else if (rndrange(1, 100) < 25) {
            for (; i <= lastmon && !done; i++) {
                m = &objtab[i];
                if (m->type >= 19) {
                    if (done = monreach(m->type, worldx - m->x, worldy - m->y)) {
                        drawtile(0x31, m->x - worldx + 9, m->y - worldy + 4);
                        msgline("Monster removed!");
                        playif(5);
                        drawtile(terrain(m->x, m->y), m->x - worldx + 9, m->y - worldy + 4);
                        delmon(m->x, m->y);
                    }
                }
            }
        }
        if (!done) {
            msgline("Hmmmm... no effect!");
            playif(6);
        }
    }
}

/* cast a combat spell (Magic Missile / Kill): direction, then hit roll */
splcomb()
{
    unsigned char c;
    int miss, dmg, hit;

    miss = 0;
    putstr(scolon, coltext);
    if ((c = keywait()) > 4) {
        miss++;
        msgline("Nothing");
    } else
        msgline(dirtab[c]);
    if (miss)
        ownspel[rdyspel]++;
    else {
    if (rdyspel == 3) {
        putstr("\"DELCIO-ERE-UI\" ", coltext);
        dmg = spldamg();
    } else {
        putstr("\"INTERFICIO-NUNC!\" ", coltext);
        dmg = 9999;
    }
    if (plclass == 1)
        hit = 1;
    else
        hit = rndrange(1, 100) < plwis + 50;
    if (!hit) {
        msgline(sfailed);
        playif(6);
    } else {
        playif(5);
        msgline(sempty);
        projectl(c, 7, 3, dmg, 0x65, 0x31);
    }
    }
}

/* Cast command: the readied spell rdyspel */
cmdcast()
{
    if (gmode == 1 || gmode == 2 || gmode == 4) {
        putstr("Cast -- ", coltext);
        msgline("Hmmmm... no effect!");
        playif(6);
    } else {
        putstr("Cast ", coltext);
        putstr(spellnam[rdyspel], coltext);
        if (rdyspel == 0)
            splprayr();
        else if (ownspel[rdyspel] == 0) {
            msgline(sempty);
            msgline("You've used up that spell!");
            playif(6);
        } else {
            ownspel[rdyspel]--;
            if (gmode == 3)
                dungspel(rdyspel, 1);
            else if (rdyspel != 3 && rdyspel != 10) {
                msgline(sempty);
                msgline("Failed, dungeon spell only!");
                playif(6);
            } else
                splcomb();
        }
    }
}

/* pence offered at a dungeon altar (tiles 33-35): stats, items or food */
altar(n)
int n;
{
    int t, r;

    t = tcdbin[townmap][townx][towny];
    if (t != 0x33 && t != 0x34 && t != 0x35) {
        putstr("Ok!", coltext);
    } else {
    putstr("Shazam!", coltext);
    playif(5);
    switch (t) {
    case 0x33:
        r = rndrange(1, 6);
        plstat[r] += n / 10;
        if (plstat[r] > 99)
            plstat[r] = 99;
        break;
    case 0x34:
        if (n >= 10) {
            r = rndrange(1, 15);
            if (ownweap[r] < 255)
                ownweap[r]++;
        }
        break;
    case 0x35:
        plfood += n * 3;
        showfood();
        break;
    }
    }
}

char spence[] = "pence: ";
char snone[] = "none";
char snotmuch[] = "Thou hast not that much!";
char salakazt[] = "Alakazot!";
/* drop pence: in a dungeon an offering, at a shrine tile hit points and a gem */
droppay()
{
    int n, t;

    putstr(spence, coltext);
    if ((n = readnum(12, 24)) == 0) {
        gotoxy(13, 24);
        putstr(snone, coltext);
    } else {
        msgline(sempty);
        if (n > plcoin) {
            putstr(snotmuch, coltext);
            playif(1);
        } else {
            plcoin -= n;
            showcoin();
            if (gmode == 2)
                altar(n);
            else if (tcdbin[townmap][townx][towny] != 0x33)
                putstr("Ok!", coltext);
            else {
                t = n * 3;
                plstat[0] += (t = t / 2);
                showhp();
                if (rndrange(1, 255) > 16)
                    putstr("Shazam!", coltext);
                else {
                    if ((n = rndrange(1, 7)) == 3)
                        n++;
                    ownspl1[n]++;
                    putstr(salakazt, coltext);
                }
                playif(5);
            }
        }
    }
}

/* clear the map area for the drop list */
clrdrop()
{
    fillrect(8, 8, 0x137, 0x97, 0);
    arrowr(0x10, 0);
    putstr(" Drop ", coltext);
    arrowl(0x17, 0);
}

/* draw a frame bar at the top of the screen */
dropbar()
{
    fillrect(0x7f, 0, 0xc7, 6, colfill);
    drawline(0x7f, 7, 0xc7, 7, colline);
    redraw = 1;
}

/* ---- ou_1769_b ---- */
char sweapon[] = "weapon: ";
char sparen[] = ") ";
/* Weapon drop list: choose a weapon letter and drop one */
dropweap()
{
    int i, y, n;
    unsigned char c;

    n = 0;
    c = 'b';
    putstr(sweapon, coltext);
    for (i = 1; i <= 15; i++)
        if (ownweap[i])
            n++;
    if (n == 0) {
        putstr(snone, coltext);
    } else {
    clrdrop();
    y = 10 - n / 2;
    n = 0;
    for (i = 1; i <= 15; c++, i++) {
        if (ownweap[i]) {
            gotoxy(15, y + n);
            putch(c, coltext);
            putstr(sparen, coltext);
            putstr(wnamelng[i], coltext);
            n++;
        }
    }
    gotoxy(14, 24);
    c = lowcase(waitkey(coltext)) - 'a';
    if (c < 1 || c > 15)
        putstr(snone, coltext);
    else if (ownweap[c] == 0)
        putstr(snone, coltext);
    else {
        putstr(wnamelng[c], coltext);
        ownweap[c]--;
        if (c == rdyweap && ownweap[c] == 0)
            rdyweap = 0;
    }
    dropbar();
    }
}

/* Armour drop list: choose an armour letter and drop one */
droparm()
{
    int i, y, n;
    unsigned char c;

    n = 0;
    c = 'b';
    putstr("armour: ", coltext);
    for (i = 1; i <= 5; i++)
        if (ownarm[i])
            n++;
    if (n == 0) {
        putstr(snone, coltext);
    } else {
    clrdrop();
    y = 10 - n / 2;
    n = 0;
    for (i = 1; i <= 5; c++, i++) {
        if (ownarm[i]) {
            gotoxy(13, y + n);
            putch(c, coltext);
            putstr(sparen, coltext);
            putstr(armornam[i], coltext);
            n++;
        }
    }
    gotoxy(14, 24);
    c = lowcase(waitkey(coltext)) - 'a';
    if (c < 1 || c > 5)
        putstr(snone, coltext);
    else if (ownarm[c] == 0)
        putstr(snone, coltext);
    else {
        putstr(armornam[c], coltext);
        ownarm[c]--;
        if (c == rdyarm && ownarm[c] == 0)
            rdyarm = 0;
    }
    dropbar();
    }
}

/* Drop command: only in dungeons/towns; asks pence/weapon/armour */
cmddrop()
{
    unsigned char c;

    if (gmode != 1 && gmode != 2) {
        putstr("Drop?", coltext);
        playif(1);
    } else {
        putstr("Drop Pence,Weapon,Armour:", coltext);
        c = upcase(waitkey(coltext));
        fillrect(40, 192, 230, 199, 0);
        gotoxy(6, 24);
        switch (c) {
        case 'P':
            droppay();
            break;
        case 'W':
            dropweap();
            break;
        case 'A':
            droparm();
            break;
        default:
            putstr(snothing, coltext);
        }
    }
    msgline(sempty);
}

/* copy the 15 entries of the current level's table into ents */
loadents()
{
    int i;

    for (i = 0; i < 15; i++)
        ents[i] = enttmpl[townmap][i];
}

/* wait for a key (up to 10000 polls) */
waitpoll()
{
    int i;

    i = 0;
    while (!getkey(0) && i++ < 10000)
        ;
}

/* draw a frame (outline) in colour c */
drawfrm(c)
int c;
{
    drawline(34, 26, 284, 26, c);
    lineto(284, 124, c);
    lineto(34, 124, c);
    lineto(34, 26, c);
    gotoxy(5, 4);
}

/* clear the frame area and draw the frame */
boxclr(c)
int c;
{
    fillrect(31, 23, 287, 127, 0);
    drawfrm(c);
}

/* print string s at column x, row y */
boxtext(x, y, s)
int x, y;
char *s;
{
    gotoxy(x, y);
    putstr(s, coltext);
}

/* random -1, 0 or 1 */
rnd3()
{
    int r;

    r = rndrange(1, 215) % 3;
    if (r == 2)
        r = -1;
    return r;
}

/* redraw the whole dungeon level map */
drawlvl()
{
    int y, x;

    for (y = 0; y < 38; y++)
        for (x = 0; x < 18; x++) {
            if (tcdbin[townmap][y][x] < 0x33)
                towntile(tcdbin[townmap][y][x], y, x);
            else
                towntile(1, y, x);
        }
    towntile(18, townx, towny);
}

/* redraw all active entities */
drawents()
{
    int i;

    for (i = 0; i < 15; i++)
        if (ents[i].f0 != -1)
            towntile(ents[i].f0, ents[i].f2, ents[i].f4);
}

/* find the entity at (x, y) in the ents table; -1 if none or dead */
entat(x, y)
int x, y;
{
    int i, r;

    i = 0;
    r = -1;
    do {
        if (ents[i].f2 == x && ents[i].f4 == y)
            r = i;
        i++;
    } while (r == -1 && i < 15);
    if (r != -1 && ents[r].f0 == -1)
        r = -1;
    return r;
}

/* what is at map position (x, y): -1 outside, 18 player, else entity type or -1 */
whatat(x, y)
int x, y;
{
    int i, r;

    r = i = 0;
    if (x < 0 || x > 37 || y < 0 || y > 17)
        return -1;
    if (townx == x && towny == y)
        r = 18;
    else
        do {
            if (ents[i].f2 == x && ents[i].f4 == y)
                r = ents[i].f0;
            i++;
        } while (r == 0 && i < 15);
    if (r == 0)
        r = -1;
    return r;
}

/* classify map position (x, y): 63 outside, entity code, -1 blocked, 0 free */
posclass(x, y)
int x, y;
{
    int t, r;

    if (x > 37 || x < 0 || y > 17 || y < 0)
        return 63;
    t = tcdbin[townmap][x][y];
    if ((r = whatat(x, y)) != -1)
        return r;
    if (t == 63)
        return 63;
    if (t == 1 || t >= 0x33)
        return -1;
    return 0;
}

/* is entity at offset (a, b) from the player a type 22 (guard)? */
isguard(a, b)
int a, b;
{
    return whatat(townx + a, towny + b) == 22;
}

/* is there a type 22 entity next to the player in any of 8 directions */
guardnr()
{
    return isguard(-1, 1) || isguard(-1, 0) || isguard(-1, -1) || isguard(0, 1)
        || isguard(0, -1) || isguard(-1, 0) || isguard(1, 1) || isguard(1, 0);
}

/* thief/jester entity m wanders; a thief at the player steals an item, jester sings */
wander(m)
int m;
{
    int dx, dy, r, i = 15, done = 0;

    if (!hostile) {
        dx = rnd3();
        dy = rnd3();
        if ((r = posclass(ents[m].f2 + dx, ents[m].f4 + dy)) == -1) {
            towntile(1, ents[m].f2, ents[m].f4);
            ents[m].f2 += dx;
            ents[m].f4 += dy;
            playif(4);
        } else if (r == 18) {
            do {
                if (ownweap[i] && i != rdyweap) {
                    ownweap[i]--;
                    done = 1;
                }
                i--;
            } while (i > 0 && !done);
            if (rndrange(1, 255) < plagi + 128 && done) {
                if (gmode == 1)
                    msgline("Iolo stole something!");
                else
                    msgline(" The jester stole something!");
            }
        }
        if (done == 0) {
            if (rndrange(1, 255) < 15) {
                if (gmode == 1) {
                    msgline("Iolo the Bard sings:");
                    msgline("Ho eyoh he hum!");
                } else {
                    msgline("Gwino the jester sings:");
                    msgline("I've got the key!");
                }
            }
        }
    }
}

/* wandering entity m moves randomly */
randmove(m)
int m;
{
    int dx, dy;

    if (hostile == 0) {
        dx = rnd3();
        dy = rnd3();
        if (posclass(ents[m].f2 + dx, ents[m].f4 + dy) == -1) {
            towntile(1, ents[m].f2, ents[m].f4);
            ents[m].f2 += dx;
            ents[m].f4 += dy;
            playif(4);
        }
    }
}

/* try to move entity m by (dx, dy); 1 if moved */
moveent(m, dx, dy)
int m, dx, dy;
{
    int r, x, y;

    x = ents[m].f2 + dx;
    y = ents[m].f4 + dy;
    if (x < 0 || x > 37 || y < 0 || y > 17)
        return 0;
    if ((r = posclass(x, y)) == -1 || r == 63) {
        towntile(1, ents[m].f2, ents[m].f4);
        ents[m].f2 += dx;
        ents[m].f4 += dy;
        return 1;
    }
    return 0;
}

/* entity m (jester/guard) moves randomly or away from the player */
jestmove(m)
int m;
{
    int dx, dy, moved = 0;

    if (!hostile)
        randmove(m);
    else {
        dx = ents[m].f2 - townx;
        dy = ents[m].f4 - towny;
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
        if (rndrange(1, 100) < 50) {
            if (dy)
                moved = moveent(m, 0, dy);
            if (moved == 0 && dx)
                moved = moveent(m, dx, 0);
        } else {
            if (dx)
                moved = moveent(m, dx, 0);
            if (moved == 0 && dy)
                moved = moveent(m, 0, dy);
        }
        if (moved)
            playif(4);
    }
}

/* guard m approaches the player; returns 1 if it moved */
guardapp(m)
int m;
{
    int dx, dy, adx, ady, moved;

    moved = 0;
    dx = ents[m].f2 - townx;
    dy = ents[m].f4 - towny;
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
            moved = moveent(m, dx, 0);
        if (moved == 0 && dy)
            moved = moveent(m, 0, dy);
    }
    if (moved) {
        playif(4);
        return 1;
    }
    return 0;
}

/* guard m next to the player attacks him */
guardatk(m)
int m;
{
    int adx, ady, mx, dmg;

    adx = ABS(ents[m].f2 - townx);
    ady = ABS(ents[m].f4 - towny);
    if (adx < 2 && ady < 2) {
        msgline("Attacked by guard!");
        playif(7);
        if (rndrange(1, 255) < plsta / 2 + rdyarm * 8 + 56)
            msgline(smissed);
        else {
            putstr("Hit!...", coltext);
            dmg = rndrange(2, mx = plstat[0] / 128 + 15);
            putnum(dmg, 1, 32, coltext, 1);
            msgline(sdamagex);
            playif(2);
            plstat[0] -= dmg;
            showhp();
        }
    }
}

/* per-turn update of all entities in the current dungeon level */
entturn()
{
    int i, list[10], n;

    n = 0;
    if (redraw) {
        drawlvl();
        redraw = 0;
    }
    if (gmode != prevmode)
        drawents();
    else {
        for (i = 0; i < 15; i++) {
            if (ents[i].f0 == 19)
                wander(i);
            else if (ents[i].f0 == 50)
                randmove(i);
            else if (ents[i].f0 == 22)
                jestmove(i);
            else if (ents[i].f0 == 17 && hostile && !guardapp(i))
                list[n++] = i;
        }
        drawents();
        if (hostile && n)
            for (i = 0; n > i; i++) {
                guardatk(list[i]);
                if (plstat[0] <= 0)
                    break;
            }
    }
}

/* ---- ou_2b6d ---- */
/* attack in direction dir (1-4) up to range squares: hit chance, damage to the monster met */
townatk(dir, range, damage, chance)
unsigned char dir;
int range, damage, chance;
{
    int m, i, dx, dy, tile, ox, oy, t2;

    i = 1;
    playif(7);
    switch (dir) {
    case 3:
        dx = 0;
        dy = -1;
        break;
    case 4:
        dx = 0;
        dy = 1;
        break;
    case 2:
        dx = 1;
        dy = 0;
        break;
    case 1:
        dx = -1;
        dy = 0;
        break;
    }
    do {
        ox = dx * i;
        oy = i * dy;
        tile = whatat(townx + ox, towny + oy);
        i++;
        t2 = posclass(townx + ox, towny + oy);
        if (t2 == 0x3f)
            t2 = -1;
    } while (i <= range && tile != 0x11 && tile != 0x16 && tile != 0x13 && tile != 0x32
             && tile != 0x14 && tile != 0x15 && t2 == -1);
    m = entat(townx + ox, towny + oy);
    if (m == -1 || rndrange(1, 100) > chance) {
        msgline(smissed);
    } else {
        putstr("Hit ", coltext);
        switch (tile) {
        case 0x14:
            putstr("King! ", coltext);
            break;
        case 0x11:
            putstr("Guard! ", coltext);
            break;
        case 0x15:
            putstr("Merchant! ", coltext);
            break;
        case 0x16:
            putstr("Princess! ", coltext);
            break;
        case 0x13:
            if (gmode == 1)
                putstr("Bard! ", coltext);
            else
                putstr("Jester! ", coltext);
            break;
        case 0x32:
            if (plsex == 0)
                putstr("Wench! ", coltext);
            else
                putstr("Lecher! ", coltext);
            break;
        }
        hostile = 1;
        playif(2);
        if (damage >= ents[m].f6) {
            msgline("Killed!");
            if (tile != 0x16) {
                plexp++;
                showexp();
            } else
                doorunl = 0;
            ents[m].f0 = -1;
            ents[m].f2 = 0x7f;
            towntile(1, townx + ox, towny + oy);
            if (tile == 0x11)
                plexp += 14;
            if (tile == 0x13 && gmode == 2) {
                msgline("Thou hast found a key!");
                hasjkey = 1;
            }
        } else {
            if (tile != 0x14 || isbrit == 0)
                ents[m].f6 -= damage;
            putnum(damage, 2, 0x20, coltext, 1);
            msgline(sdamagex);
        }

    }
}

/* ---- ou_2b6d_b ---- */
/* step the player one square in direction dir (1-4): walk, leave the map, or bump */
townstep(dir)
unsigned char dir;
{
    int dx, dy, t;

    switch (dir) {
    case 3:
        dy = -1;
        dx = 0;
        break;
    case 4:
        dy = 1;
        dx = 0;
        break;
    case 2:
        dy = 0;
        dx = 1;
        break;
    case 1:
        dy = 0;
        dx = -1;
        break;
    }
    if (townx + dx < 0 || towny + dy < 0 || townx + dx > 37 || towny + dy > 17) {
        if (gmode == 2 && guardnr())
            princess();
        gmode = 0;
    } else {
        t = posclass(townx + dx, towny + dy);
        if (t == -1 || t == 63) {
            msgline(dirtab[dir]);
            towntile(1, townx, towny);
            townx += dx;
            towny += dy;
            towntile(18, townx, towny);
            playif(4);
        } else {
            msgline("Blocked!");
            playif(1);
        }
    }
}

/* transport shop sign number n (shuttle/ship dealer); returns 7 if n is 0 or 7, else 6 */
signship(n)
unsigned n;
{

    boxclr(colfill);
    switch (n) {
    case 0:
        boxtext(10, 4, "Scooter's Super Duper");
        boxtext(13, 5, "Transport, Inc.");
        break;
    case 3:
        boxtext(9, 4, "Quality Transport Ltd.");
        break;
    case 5:
        boxtext(7, 4, "O.K. New and Used Transport");
        break;
    case 7:
        boxtext(8, 4, "Sly Sam's Transportation");
        boxtext(15, 5, "Specialists");
        break;
    }
    if (n == 0 || n == 7)
        return 7;
    return 6;
}

/* armour shop sign number n; returns 7 if n is 0 else 6 */
signarm(n)
unsigned n;
{

    boxclr(colfill);
    switch (n) {
    case 0:
        boxtext(11, 4, "The Armour Shop of");
        boxtext(8, 5, "Lord Eldric D'Charbonneux");
        break;
    case 1:
        boxtext(15, 4, "Max's Armoury");
        break;
    case 2:
        boxtext(13, 4, "The Iron Fist");
        break;
    case 3:
        boxtext(10, 4, "The Hammer -n- Anvil");
        break;
    case 5:
        boxtext(5, 4, "Defense Specialties Unlimited");
        break;
    case 7:
        boxtext(11, 4, "Custom Armor Works");
        break;
    }
    if (n == 0)
        return 7;
    return 6;
}

/* weapon shop sign number n; returns 7 if n is 0 else 6 */
signweap(n)
unsigned n;
{

    boxclr(colfill);
    switch (n) {
    case 0:
        boxtext(11, 4, "The Weapon Shop of");
        boxtext(8, 5, "Lord Eldric D'Charbonneux");
        break;
    case 1:
        boxtext(11, 4, "The Tempered Steel");
        break;
    case 2:
        boxtext(12, 4, "The Razor's Edge");
        break;
    case 3:
        boxtext(10, 4, "Cold Steel Creations");
        break;
    case 4:
        boxtext(12, 4, "The Bloody Blade");
        break;
    case 5:
        boxtext(13, 4, "The Duelo Shop");
        break;
    case 6:
        boxtext(12, 4, "Weaponry Supply");
        break;
    case 7:
        boxtext(10, 4, "Cold Steel Creations");
        break;
    }
    if (n == 0)
        return 7;
    return 6;
}

/* magic shop sign number n; always returns 6 */
signmagi(n)
unsigned n;
{
    boxclr(colfill);
    switch (n) {
    case 0:
        boxtext(9, 4, "Mad Chad's Magic Shoppe");
        break;
    case 1:
        boxtext(12, 4, "Mystic Melinda's");
        break;
    case 2:
        boxtext(13, 4, "Words of Power");
        break;
    case 3:
        boxtext(5, 4, "Psychic Sam's Magical Emporium");
        break;
    case 7:
        boxtext(11, 4, "The Unleashed Spell");
        break;
    }
    return 6;
}

/* food shop sign number n; always returns 6 */
signfood(n)
unsigned n;
{
    boxclr(colfill);
    switch (n) {
    case 0:
        boxtext(5, 4, "Li'l Karelia's Finnish Grocery");
        break;
    case 1:
        boxtext(8, 4, "Adventurer's Supply Post");
        break;
    case 3:
        boxtext(13, 4, "The Brown Bag");
        break;
    case 4:
        boxtext(9, 4, "Fresh Food Marketplace");
        break;
    case 5:
        boxtext(11, 4, "Rations Unlimited");
        break;
    case 6:
        boxtext(6, 4, "Fastest Freshest Food Market");
        break;
    case 7:
        boxtext(8, 4, "Exploration Provisioners");
        break;
    }
    return 6;
}
