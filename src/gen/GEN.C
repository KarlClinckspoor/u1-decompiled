/* opts: -ms -v -w -cs */
/*
 * GEN.EXE - Ultima I character generation (Origin Systems, PC conversion by John Fachini).
 * Decompiled; builds byte-identical with Lattice C 3.10 (lc -ms -v -w -cs).
 *
 * Names are still the addresses they had in the original (F_xxxx code, D_xxxx DGROUP data).
 */
#include <stdio.h>

/* Origin support routines (ORIGIN.ASM) and their data */
extern int vidmode, D_082E, vidmarg;
extern int detvid();
extern modecga(), modeega(), modetand(), mkrowtab(), clockrnd(), scrlmsg(), gamefram();
extern fillrect(), drawline(), pixel(), gotoxy(), putstr(), putnum(), putch();
extern arrowr(), arrowl(), playsnd();
extern unsigned char waitkey(), upcase();
/* Lattice library */
extern erron();                        /* ONERROR */

/*
 * Initialised data (DGROUP 0124-01B2)
 */
char *racenam[] = { "Human", "Elf", "Dwarf", "Bobbit" };        /* race names */
char *classnam[] = { "Fighter", "Cleric", "Wizard", "Thief" };   /* class names */
char *plfiles[] = { "player1.u1", "player2.u1", "player3.u1", "player4.u1" };
char *outname = "out";                   /* the game, run with execl */
char *vidarg = "C";                     /* its arguments: video mode (C, E, T) */
unsigned char *slotarg = "0";            /*   and character number */
int slotused[4] = { 0, 0, 0, 0 };         /* which player files exist */

/*
 * The character record, written to playerN.u1 in two blocks (DGROUP 0A30-0DB0)
 */
char plname[16];                        /* name */
int plrace;                             /* race */
int plclass;                             /* class */
int plsex;                             /* sex */
int plstat[7];                          /* hit points, then Str Agi Sta Cha Wis Int */
int plcoin, plexp, plfood, rdyweap, rdyspel, rdyarm, plveh, plseed;
int worldx, worldy, plnoise;
int plquest[9];
int gems[4];
int ownarm[6];
int ownweap[16];
int ownspel[11];
int ownveh[8];
int plkills, lastmnmt, savlast;
long savturns;
int shuttle[2];
int objtype, objterr, objx, objy;     /* second block: 0x280 bytes from objtype */
int objrest[316];
char rosternm[4][15];                     /* names of the saved characters */
int ptsleft;                             /* points left to distribute */
int D_0DA2;
int nsaved;                             /* number of saved characters */
int colline, colfill, colhigh, coltext;     /* colours */
int forcecga;                             /* forced CGA (command line "C") */

/* pick colours for the machine type (detvid: BIOS model byte) and, if setmode,
   switch to the graphics mode in vidmode (0 CGA, 1 EGA, 2 Tandy/PCjr) */
setvid(setmode)
int setmode;
{
    switch (detvid()) {
    case 0:
        colline = 3;
        colfill = 2;
        colhigh = 1;
        coltext = 3;
        D_082E = 6;
        break;
    case 1:
    case 2:
        colline = 15;
        colfill = 1;
        colhigh = 12;
        coltext = 11;
        D_082E = 7;
        break;
    }
    if (forcecga) {
        vidmode = 0;
        colline = 3;
        colfill = 2;
        colhigh = 1;
    }
    mkrowtab();
    if (setmode) {
        if (vidmode == 0)
            modecga();
        else if (vidmode == 1)
            modeega();
        else if (vidmode == 2)
            modetand();
    }
}

/* rounded corners of the border */
bordcorn()
{
    int i;

    for (i = 1; i <= 4; i++) {
        drawline(i, 0, 0, i, 0);
        drawline(0x13f - i, 0, 0x13f, i, 0);
        drawline(0, 0xc7 - i, i, 0xc7, 0);
        drawline(0x13f, 0xc7 - i, 0x13f - i, 0xc7, 0);
    }
    pixel(0, 0, 0);
    pixel(0, 5, 0);
    pixel(5, 0, 0);
    pixel(0x13f, 0, 0);
    pixel(0x13a, 0, 0);
    pixel(0x13f, 5, 0);
    pixel(0, 0xc7, 0);
    pixel(0, 0xc2, 0);
    pixel(5, 0xc7, 0);
    pixel(0x13f, 0xc7, 0);
    pixel(0x13f, 0xc2, 0);
    pixel(0x13a, 0xc7, 0);
}

/* draw the screen border */
drawbord()
{
    fillrect(0, 0, 0x13c, 6, colfill);
    fillrect(0, 6, 6, 0xc7, colfill);
    fillrect(0x139, 2, 0x13f, 0xc7, colfill);
    fillrect(0, 0xc1, 0x13f, 0xc7, colfill);
    drawline(7, 7, 0x138, 7, colline);
    drawline(7, 7, 7, 0xc0, colline);
    drawline(0x138, 7, 0x138, 0xc0, colline);
    drawline(7, 0xc0, 0x138, 0xc0, colline);
    bordcorn();
}

/* clear the inside of the frame and draw the title bar */
clrframe()
{
    fillrect(8, 8, 0x137, 0xbf, 0);
    fillrect(0x1e, 0, 0x12c, 6, colfill);
    drawline(0x1e, 7, 0x12c, 7, colline);
}

/* main menu: generate new character / continue previous game / quit */
unsigned char mainmenu()
{
    unsigned char c;

    gotoxy(12, 6);
    putstr("*** Ultima I ***", colline);
    gotoxy(14, 9);
    putstr("from darkest", colline);
    gotoxy(14, 10);
    putstr("dungeons, to", colline);
    gotoxy(13, 11);
    putstr("deepest space!", colline);
    gotoxy(8, 14);
    putstr("a) Generate new character", coltext);
    gotoxy(8, 15);
    putstr("b) Continue previous game", coltext);
    gotoxy(13, 18);
    putstr("Thy choice: ", coltext);
    do
        c = upcase(waitkey(coltext));
    while (c != 'A' && c != 'B' && c != 'Q');
    return c;
}

/* print the value of attribute n */
showattr(n)
int n;
{
    gotoxy(0x1c, n + 5);
    putnum(plstat[n], 2, ' ', coltext, 0);
}

/* choose the race; adjusts attributes, returns 0..3 */
pickrace()
{
    int done, race;

    fillrect(0x30, 0x20, 0x12c, 0x28, 0);
    fillrect(0xe, 0x80, 0x12e, 0xb0, 0);
    gotoxy(3, 0x11);
    putstr("Select thy race: ", coltext);
    gotoxy(12, 0x13);
    putstr("a) Human", coltext);
    gotoxy(12, 0x14);
    putstr("b) Elf", coltext);
    gotoxy(12, 0x15);
    putstr("c) Dwarf", coltext);
    gotoxy(12, 0x16);
    putstr("d) Bobbit", coltext);
    gotoxy(0x14, 0x11);
    do {
        switch (upcase(waitkey(coltext))) {
        case 'A':
            race = 0;
            plstat[6] += 5;
            showattr(6);
            done = 1;
            break;
        case 'B':
            race = 1;
            plstat[2] += 5;
            showattr(2);
            done = 1;
            break;
        case 'C':
            race = 2;
            plstat[1] += 5;
            showattr(1);
            done = 1;
            break;
        case 'D':
            race = 3;
            plstat[5] += 10;
            plstat[1] -= 5;
            showattr(5);
            showattr(1);
            done = 1;
            break;
        default:
            done = 0;
        }
    } while (!done);
    gotoxy(0xe, 0xd);
    putstr("Race: ", coltext);
    putstr(racenam[race], coltext);
    return race;
}

/* choose the sex, returns 0 male, 1 female */
picksex()
{
    int done, sex;

    fillrect(0x18, 0x91, 0x12c, 0xb8, 0);
    gotoxy(3, 0x11);
    putstr("Select thy sex: ", coltext);
    gotoxy(12, 0x13);
    putstr("a) Male", coltext);
    gotoxy(12, 0x14);
    putstr("b) Female", coltext);
    gotoxy(0x13, 0x11);
    do {
        switch (upcase(waitkey(coltext))) {
        case 'A':
            sex = 0;
            done = 1;
            break;
        case 'B':
            sex = 1;
            done = 1;
            break;
        default:
            done = 0;
        }
    } while (!done);
    gotoxy(0xf, 0xe);
    putstr("Sex: ", coltext);
    if (sex == 0)
        putstr("Male", coltext);
    else
        putstr("Female", coltext);
    return sex;
}

/* choose the class; adjusts attributes, returns 0..3 */
pickclas()
{
    int done, cls;

    fillrect(0x18, 0x91, 0x12c, 0xb8, 0);
    gotoxy(3, 0x11);
    putstr("Select thy class: ", coltext);
    gotoxy(12, 0x13);
    putstr("a) Fighter", coltext);
    gotoxy(12, 0x14);
    putstr("b) Cleric", coltext);
    gotoxy(12, 0x15);
    putstr("c) Wizard", coltext);
    gotoxy(12, 0x16);
    putstr("d) Thief", coltext);
    gotoxy(0x15, 0x11);
    do {
        switch (upcase(waitkey(coltext))) {
        case 'A':
            cls = 0;
            plstat[1] += 10;
            plstat[2] += 10;
            showattr(1);
            showattr(2);
            done = 1;
            break;
        case 'B':
            cls = 1;
            plstat[5] += 10;
            showattr(5);
            done = 1;
            break;
        case 'C':
            cls = 2;
            plstat[6] += 10;
            showattr(6);
            done = 1;
            break;
        case 'D':
            cls = 3;
            plstat[2] += 10;
            showattr(2);
            done = 1;
            break;
        default:
            done = 0;
        }
    } while (!done);
    gotoxy(0xd, 0xf);
    putstr("Class: ", coltext);
    putstr(classnam[cls], coltext);
    return cls;
}

/* read the character's name into plname */
readname()
{
    int done, len;
    unsigned char c;

    len = done = 0;
    fillrect(0x18, 0x80, 0x12c, 0xb8, 0);
    gotoxy(3, 0x11);
    putstr("Enter thy name: ", coltext);
    do {
        c = waitkey(coltext);
        if (c > ' ' && len < 14 || c == ' ' && len != 0 && len < 14) {
            plname[len] = c;
            len++;
            putch(c, coltext);
        } else if ((c == 1 || c == 8) && len != 0) {
            len--;
            gotoxy(len + 0x13, 0x11);
        } else if (c == '\r' && len != 0) {
            plname[len] = 0;
            done = 1;
        }
    } while (!done);
    gotoxy(12, 4);
    putstr(plname, coltext);
}

/* title bar text */
titlebar()
{
    arrowr(8, 0);
    putstr(" Character Generation ", colline);
    arrowl(0x1f, 0);
}

/* graphics driver call 1 (dispatch table in playsnd) */
beep()
{
    playsnd(1);
}

/* ask whether to save the character: 1 yes, 0 no */
asksave()
{
    unsigned char c;

    fillrect(0x18, 0x80, 0x12c, 0xb8, 0);
    gotoxy(3, 0x16);
    putstr("Save this character? (Y-N) ", coltext);
    for (;;) {
        c = upcase(waitkey(coltext));
        if (c == 'Y') {
            putstr("Yes", coltext);
            return 1;
        }
        if (c == 'N')
            return 0;
    }
}

/* erase the cursor marks around attribute line n */
curoff(n)
int n;
{
    gotoxy(0xb, n + 5);
    putstr(" ", coltext);
    gotoxy(0x1e, n + 5);
    putstr(" ", coltext);
}

/* draw the cursor marks around attribute line n */
curon(n)
int n;
{
    arrowr(0xb, n + 5);
    arrowl(0x1e, n + 5);
}

/* decrease attribute n (not below 10); 1 if done */
attrdec(n)
int n;
{
    if (plstat[n] > 10) {
        plstat[n]--;
        showattr(n);
        return 1;
    }
    return 0;
}

/* increase attribute n (not above 25, needs points left); 1 if done */
attrinc(n)
int n;
{
    if (plstat[n] < 25 && ptsleft) {
        plstat[n]++;
        showattr(n);
        return 1;
    }
    return 0;
}

/* move the cursor up one attribute (wraps) */
curup(n)
int n;
{
    curoff(n);
    n--;
    if (n < 1)
        n = 6;
    curon(n);
    return n;
}

/* move the cursor down one attribute (wraps) */
curdown(n)
int n;
{
    curoff(n);
    n++;
    if (n > 6)
        n = 1;
    curon(n);
    return n;
}

/* attribute screen: labels, help text, all attributes to 10 */
attrscr()
{
    int i;

    fillrect(10, 10, 0x136, 0xb9, 0);
    gotoxy(6, 4);
    putstr("Points left to distribute:", coltext);
    gotoxy(12, 6);
    putstr("Strength........10", coltext);
    gotoxy(12, 7);
    putstr("Agility.........10", coltext);
    gotoxy(12, 8);
    putstr("Stamina.........10", coltext);
    gotoxy(12, 9);
    putstr("Charisma........10", coltext);
    gotoxy(12, 10);
    putstr("Wisdom..........10", coltext);
    gotoxy(12, 11);
    putstr("Intelligence....10", coltext);
    gotoxy(2, 0x10);
    putstr("Move cursor with up and down arrows;", coltext);
    gotoxy(2, 0x11);
    putstr("increase and decrease attributes", coltext);
    gotoxy(2, 0x12);
    putstr("with left and right arrows.  Press", coltext);
    gotoxy(2, 0x13);
    putstr("space bar when finished, or escape", coltext);
    gotoxy(2, 0x14);
    putstr("to return to the main menu.", coltext);
    for (i = 1; i <= 6; i++)
        plstat[i] = 10;
}

/* Character generation screen: build a character and save it to slot */
chargen(slot)
int slot;
{
    int sel, done, esc, j, n;
    unsigned char c;
    FILE *fp;

    do {
        titlebar();
        attrscr();
        curon(sel = 1);
        ptsleft = 30;
        done = esc = 0;
        do {
            gotoxy(32, 4);
            putnum(ptsleft, 3, 32, coltext, 0);
            switch (waitkey(0)) {
            case 1:
                if (attrdec(sel))
                    ptsleft++;
                break;
            case 2:
                if (attrinc(sel))
                    ptsleft--;
                break;
            case 3:
                sel = curup(sel);
                break;
            case 4:
                sel = curdown(sel);
                break;
            case 0x1b:
                esc = 1;
                break;
            case ' ':
                if (!(done = (ptsleft == 0)))
                    beep();
                break;
            }
        } while (!done && !esc);
        if (esc)
            break;
        curoff(sel);
        plrace = pickrace();
        plsex = picksex();
        plclass = pickclas();
        readname();
        if (done = asksave()) {
            plseed = clockrnd() + plstat[5] + plclass;
            plstat[0] = 150;
            plcoin = 100;
            plexp = 0;
            plfood = 200;
            rdyweap = 1;
            rdyspel = 0;
            rdyarm = 1;
            plveh = 0;
            worldx = 49;
            worldy = 40;
            plnoise = 1;
            for (sel = 1; sel <= 8; sel++)
                plquest[sel] = 0;
            for (sel = 0; sel <= 3; sel++)
                gems[sel] = 0;
            ownspel[0] = -1;
            for (sel = 1; sel <= 10; sel++)
                ownspel[sel] = 0;
            ownarm[0] = -1;
            ownarm[1] = 1;
            for (sel = 2; sel <= 5; sel++)
                ownarm[sel] = 0;
            ownweap[0] = -1;
            ownweap[1] = 2;
            for (sel = 2; sel <= 15; sel++)
                ownweap[sel] = 0;
            ownveh[0] = -1;
            for (sel = 1; sel <= 7; sel++)
                ownveh[sel] = 0;
            plkills = 0;
            savlast = 0;
            savturns = 0;
            lastmnmt = -1;
            objtype = 10;
            objterr = 1;
            objx = worldx;
            objy = worldy;
            if ((fp = fopen(plfiles[slot], "wb")) != NULL) {
                n = fwrite((char *)&plname, 0xb4, 1, fp);
                n = fwrite((char *)&objtype, 0x280, 1, fp);
                fclose(fp);
                slotused[slot] = 1;
                nsaved++;
                for (j = 0; j <= 14; j++)
                    rosternm[slot][j] = plname[j];
            } else {
                gotoxy(7, 18);
                putstr("Error writing out character", colhigh);
                gotoxy(14, 19);
                putstr("Press RETURN", colhigh);
                do {
                    c = waitkey(colhigh);
                } while (c != '\r');
            }
        }
    } while (!done);
}

/* Load the names of the saved characters (player1..4.u1) */
ldnames()
{
    int i, n;
    FILE *fp;

    nsaved = 0;
    for (i = 0; i <= 3; i++) {
        if ((fp = fopen(plfiles[i], "rb")) != NULL) {
            n = fread(rosternm[i], 15, 1, fp);
            fclose(fp);
            slotused[i] = 1;
            nsaved++;
        }
    }
}

/* Return the first free roster slot, or -1 */
freeslot()
{
    int i;

    i = 0;
    do {
        if (slotused[i] == 0)
            return i;
        i++;
    } while (i < 4);
    return -1;
}

/* Print the roster: "n) name" for the four slots */
showrost()
{
    int i;

    for (i = 0; i <= 3; i++) {
        gotoxy(15, i + 6);
        putnum(i + 1, 1, 32, colline, 1);
        putstr(") ", colline);
        putstr(rosternm[i], colline);
    }
}

/* Roster full: ask which character to replace; returns slot or -1 */
askrepl()
{
    unsigned char c;

    fillrect(10, 10, 300, 180, 0);
    arrowr(8, 0);
    putstr(" Character Replacement ", colline);
    arrowl(32, 0);
    showrost();
    gotoxy(4, 14);
    putstr("The roster is full.  Type the", coltext);
    gotoxy(4, 15);
    putstr("number of the character to wish", coltext);
    gotoxy(4, 16);
    putstr("to replace with a new one.", coltext);
    gotoxy(4, 17);
    putstr("Press ESCAPE to return to the", coltext);
    gotoxy(4, 18);
    putstr("main menu...", coltext);
    do {
        c = waitkey(coltext);
        if (c >= '1' && c < '5')
            return c - '1';
    } while (c != 0x1b);
    return -1;
}

/* Ask for the Ultima I diskette */
askdisk()
{
    scrlmsg();
    gotoxy(1, 23);
    putstr("Insert Ultima I diskette", coltext);
    gotoxy(1, 24);
    putstr("Hit space to continue...", coltext);
    while (waitkey(coltext) != ' ')
        ;
    gotoxy(1, 23);
    putstr("                        ", coltext);
    gotoxy(1, 24);
    putstr("                        ", coltext);
    gotoxy(1, 23);
}

/* Run the game (OUT.EXE) with video mode and character number */
runout(n)
int n;
{
    if (vidmode == 1)
        *vidarg = 'E';
    else if (vidmode == 2)
        *vidarg = 'T';
    {
        register unsigned char *num;    /* the character number argument */

        num = slotarg;
        *num += n;
    }
    fillrect(0, 0, 319, 199, 0);
    gamefram();
    gotoxy(11, 8);
    putstr("Please wait whilst", colline);
    gotoxy(13, 10);
    putstr("thy game loads", colline);
    for (;;) {
        int rc;

        rc = execl(outname, outname, vidarg, slotarg, 0);
        askdisk();
    }
}

/* Continue previous game: pick a saved character and start the game */
contgame()
{
    unsigned char c;

    fillrect(10, 10, 309, 189, 0);
    arrowr(9, 0);
    putstr(" Character Selection ", colline);
    arrowl(31, 0);
    if (nsaved == 0) {
        gotoxy(4, 10);
        putstr("There are no Ultima I characters", colline);
        gotoxy(4, 11);
        putstr("saved on this disk.  Press RETURN", colline);
        gotoxy(4, 12);
        putstr("to generate a character or ESCAPE", colline);
        gotoxy(4, 13);
        putstr("to return to the main menu...", colline);
        do {
            c = waitkey(colline);
        } while (c != '\r' && c != 0x1b);
        if (c == '\r')
            chargen(0);
    } else {
        showrost();
        gotoxy(4, 14);
        putstr("Type number of character you wish", coltext);
        gotoxy(4, 15);
        putstr("to use in Ultima I.  Press ESCAPE", coltext);
        gotoxy(4, 16);
        putstr("to go to the main menu...", coltext);
        do {
            c = waitkey(coltext);
            if (c >= '1' && c <= '4') {
                if (slotused[c - '1'])
                    runout(c - '0');
            }
        } while (c != 0x1b);
    }
}

/* main: main menu loop (a = generate new character, b = continue game) */
main(argc, argv)
int argc;
unsigned char **argv;
{
    int slot;
    unsigned char c;

    erron(1);
    forcecga = upcase(argv[1][0]) == 'C';
    ldnames();
    setvid(0);
    fillrect(0, 0, 319, 199, 0);
    vidmarg = 0;
    drawbord();
    for (;;) {
        c = upcase(mainmenu());
        switch (c) {
        case 'A':
            if ((slot = freeslot()) == -1) {
                slot = askrepl();
                fillrect(256, 0, 264, 6, colfill);
                drawline(256, 7, 264, 7, colline);
            }
            if (slot != -1)
                chargen(slot);
            clrframe();
            break;
        case 'B':
            contgame();
            clrframe();
            break;
        }
        if (c == 'A' || c == 'B')
            fillrect(10, 10, 300, 190, 0);
    }
}
