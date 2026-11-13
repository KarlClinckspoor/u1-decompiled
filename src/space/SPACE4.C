/* opts: -ms -v -w -cs */
/*
 * SPACE.EXE game code 2B7B-3DED: ship drawing, alien ship, docking, main loop.
 */
struct obj { int x, y, type, f6, f8, dir; };
struct sector { int count, f2, f4, f6, f8; struct obj o[4]; };

extern struct sector galaxy[7][7];
extern int sectx, secty, frontvw, curship;
extern int speed, velx, vely, exhcnt, exhtog;
extern int redrflag, crosx, crosy;
extern int enemyx, enemyy, enemypx, enemypy, enemykd, jumping, speeddly[];
extern int exhdrawn, growcnt, colline, colfill;
extern int hitmaxx[], hitmaxy[];
extern char plname[];
extern int colhigh, coltext, forcecga, shipcol, plnoise, plseed, shutfuel, shutshld;
extern int argv1, argv2, starmode, lastdir, vidmode, rdyarm, plcoin;
extern char *spacefil[], *fightfil[];
extern char inusefil[];
extern char spacebnk[], fightbnk[];


extern plotvp(), exhaust0(), msgline(), drawline(), playif(), waittick(), useshld(), die();
extern arrowr(), movestar(), vwait(), drawfite(), newenemy(), drawcros(), starhit(), planhit();
extern putstr(), gotoxy(), shipfuel(), shipshld(), showfuel(), showshld(), showcoin();
extern scrlmsg(), putnum(), readfile(), vidinit(), seedrnd(), mkgalaxy(), initstar(), drawtop();
extern erron(), cmdarrow(), cmdattk(), cmdboard(), cmdcast(), cmddrop(), cmdenter(), cmdfire();
extern cmdget(), cmdhyper(), cmdinfo(), cmdklimb(), cmdopen(), cmdquit(), cmdready(), cmdsteal();
extern cmdtrans(), cmdunlk(), cmdview(), cmdxit(), ztats(), cmdpass(), cmdhuh(), cmdspeed();
extern exhaust2(), exhaust1(), fightfrm(), alienfir(), crash(), dock(), liftoff(), cmdnoise();
extern PUTS(), _EXIT();
extern int rndrange(), getshld(), sign(), drawship(), portof(), shipin(), advx(), advy();
extern int portx(), porty(), opposite(), clockrnd();
extern char keywait(), waitkey(), upcase();
extern unsigned char getkey();
char *portname[] = { "left", "right", "top", "bottom" };   /* screen edges */

/* add speed velx to x, wrapping at the screen edges 20..275 */
advx(v)
int v;
{
    v += velx;
    if (v > 275)
        return 20;
    if (v < 20)
        return 275;
    return v;
}

/* add speed vely to y, wrapping 10..130 */
advy(v)
int v;
{
    v += vely;
    if (v > 130)
        return 10;
    if (v < 10)
        return 130;
    return v;
}

/* collision: crunch, lose shield, bounce (reverse speeds) */
crash()
{
    starhit();
    planhit();
    msgline((char *)0xC07);
    arrowr(0, 24);
    playif(0);
    useshld(getshld() / 4 + 5);
    if (getshld() == 0) {
        msgline((char *)0xC0F);
        die();
    } else {
        velx = -velx;
        vely = -vely;
    }
}

/* dock at a base: pay 500p, choose a ship (1..4) at this sector */
dock()
{
    struct sector *s;
    struct obj *o;
    int i, n, done;
    unsigned char c;

    done = n = 0;
    s = &galaxy[sectx][secty];
    for (i = 0; i < 4; i++)
        if (s->o[i].type != -1)
            n++;
    o = &s->o[curship];
    msgline("Docked!  Welcome to base!");
    velx = vely = 0;
    if (rdyarm < 4) {
        msgline("The spacedoors open, exposing");
        msgline("thee to a complete vacuum!");
        die();
    }
    if (plcoin < 500) {
        msgline("Sorry, but thou canst not");
        msgline("refuel thy ship without");
        putstr("the 500p docking fee.", coltext);
        c = keywait();
        msgline((char *)0x19A);
    } else {
        plcoin -= 500;
        o->f6 = shipfuel(o->type);
        o->f8 = shipshld(o->type);
        if (n > 1)
            do {
                putstr("Choose thy ship: ", coltext);
                c = keywait();
                msgline((char *)0x19A);
                if (c >= 1 && c <= 4) {
                    c--;
                    i = shipin(c);
                    if (i == -1)
                        msgline("That port is not available.");
                    else {
                        lastdir = galaxy[sectx][secty].o[i].dir;
                        curship = i;
                        done = 1;
                        putstr("Thou art in the ", coltext);
                        putstr(portname[c], coltext);
                        msgline(" ship.");
                    }
                }
            } while (done == 0);
        showfuel();
        showshld();
        showcoin();
    }
    arrowr(0, 24);
    gotoxy(1, 24);
}

/* index (0..3) of the docking slot object k sits in, else -1 */
portof(k)
int k;
{
    int i;
    struct obj *o;

    o = &galaxy[sectx][secty].o[k];
    for (i = 0; i < 4; i++)
        if (o->x == portx(i, sectx, secty, k) && o->y == porty(i, sectx, secty, k) && o->dir == opposite(i))
            break;
    if (i == 4)
        i = -1;
    return i;
}

/* find a free docking slot in the sector for ship k (-1 if none) */
shipin(k)
int k;
{
    int i;
    struct obj *o;

    for (i = 0; i < 4; i++) {
        o = &galaxy[sectx][secty].o[i];
        if (o->x == portx(k, sectx, secty, i) && o->y == porty(k, sectx, secty, i))
            break;
    }
    if (i == 4)
        i = -1;
    return i;
}

/* one game turn: combat or fly the player ship, move it, check collisions, dock */
oneturn()
{
    int d, r;
    struct sector *s;
    struct obj *o;
    int moved;

    s = &galaxy[sectx][secty];
    if (frontvw == 1)
        fightfrm();
    else {
        o = &s->o[curship];
        d = o->dir;
        moved = lastdir != d || velx || vely;
        if (moved || exhcnt) {
            if (moved)
                r = drawship(o->type, lastdir, o->x, o->y);
            if (exhcnt)
                exhaust(o, 0);
            if (moved) {
                o->x = advx(o->x);
                o->y = advy(o->y);
                r = drawship(o->type, d, o->x, o->y);
                lastdir = d;
                if (r)
                    crash();
                else if (portof(curship) != -1) {
                    dock();
                    exhcnt = 0;
                }
            }
            if (exhcnt)
                exhaust(o, 1);
        }
    }
}

/* toggle Noise on/off */
cmdnoise()
{
    putstr("Noise ", coltext);
    plnoise = !plnoise;
    if (plnoise)
        msgline("on");
    else
        msgline("off");
}

/* prompt for the Ultima I diskette and wait for a space */
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

/* lift-off countdown: wait up to 10 beeps/keypress, then "Thou hast lifted off!" */
liftoff()
{
    int i, key;

    i = 10;
    key = 0;
    gotoxy(1, 24);
    do {
        putnum(i, 1, 32, coltext, 1);
        if (i != 1)
            putstr("..", coltext);
        if (key == 0)
            key = getkey(0);
        if (key == 0)
            waittick(18);
        i--;
    } while (i != 0 && key == 0);
    msgline((char *)0x19A);
    msgline("Thou hast lifted off!");
}

/* main(argc, argv): load data, set up the flight screen, key loop */
main(argc, argv)
int argc;
unsigned char **argv;
{
    unsigned char c;
    int n;

    erron(1);
    forcecga = upcase(argv[1][0]) == 67;
    argv1 = argv[1];
    argv2 = argv[2];
    vidinit(0);
    readfile(inusefil, (int)plname, 820);
    readfile(spacefil[vidmode], spacebnk + 1406, 7296);
    readfile(fightfil[vidmode], fightbnk + 499, 3192);
    liftoff();
    if (vidmode == 0)
        shipcol = colfill;
    else
        shipcol = colhigh;
    gotoxy(31, 21);
    putstr("Shld:", coltext);
    gotoxy(31, 22);
    putstr("Fuel:", coltext);
    starmode = 2;
    seedrnd(plseed * 2);
    mkgalaxy();
    seedrnd(plseed + clockrnd());
    initstar();
    drawtop();
    showfuel();
    showshld();
    for (;;) {
        if (getshld() == 0) {
            shutshld = shutfuel = 0;
            showfuel();
            showshld();
            die();
        }
        n = 0;
        arrowr(0, 24);
        do {
            oneturn();
            c = upcase(getkey(coltext));
            n++;
            if (frontvw == 0)
                waittick(2);
        } while (c == 0 && n < 1500);
        if (c == 0)
            c = 32;
        if (c >= 49 && c <= 56 && frontvw == 1)
            cmdspeed(c - 48);
        else
            switch (c) {
            case 3:
            case 4:
            case 2:
            case 1:
                cmdarrow(c);
                break;
            case 65:
                cmdattk();
                break;
            case 66:
                cmdboard();
                break;
            case 67:
                cmdcast();
                break;
            case 68:
                cmddrop();
                break;
            case 69:
                cmdenter();
                break;
            case 70:
                cmdfire();
                break;
            case 71:
                cmdget();
                break;
            case 72:
                cmdhyper();
                break;
            case 73:
                cmdinfo();
                exhtog = redrflag = 1;
                break;
            case 75:
                cmdklimb();
                break;
            case 78:
                cmdnoise();
                break;
            case 79:
                cmdopen();
                break;
            case 81:
                cmdquit();
                break;
            case 82:
                cmdready();
                break;
            case 83:
                cmdsteal();
                break;
            case 84:
                cmdtrans();
                break;
            case 85:
                cmdunlk();
                break;
            case 86:
                cmdview();
                break;
            case 88:
                cmdxit();
                break;
            case 90:
                ztats();
                exhtog = redrflag = 1;
                arrowr(0, 24);
                break;
            case 32:
                cmdpass();
                break;
            case 27:
                (&frontvw)[1] = 1;
            default:
                cmdhuh();
            }
        if (redrflag)
            drawtop();
        if (exhtog) {
            exhtog = 0;
            if (exhcnt && frontvw == 0)
                exhdrawn = !exhdrawn;
        }
    }
}

