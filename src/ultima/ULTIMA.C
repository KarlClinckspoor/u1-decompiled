/* opts: -ms -v -w -cs */
/*
 * ULTIMA.EXE - title screen: "Ultima I" banner, the bouncing-star picture, the story,
 * loading castle.4/castle.16, then execl("gen.exe") (code 03E9-0E4D).
 * Names are still the original addresses. The picture tables (pictrow1, pictrow2) sit
 * between drawbann and credits, as in the original DATA.
 */
extern int vidmode;                      /* video mode: 0 CGA, 1 EGA, 2 Tandy */
extern unsigned freeparg;
extern char *pictrow1[], *pictrow2[];       /* the picture, rows of '*' (defined below) */
extern int lfsrtap[];                    /* LFSR taps */

extern gotoxy(), putstr(), fillrect(), drawline(), memcheck(), palcastl(), palnorm(), blittand();
extern blitcga(), cvtplane(), pixel(), blitega(), waittick(), vidinit(), rndrange(), erron();
extern F_126F();
extern int EXECL(), FOPEN(), FREAD(), FCLOSE(), EXIT(), detvid();
extern unsigned char getkey(), upcase();

/* BSS (uninitialised, in definition order) */
int colline, colhigh, colfill, coltext, forcecga;     /* text colours; forcecga: argv[1] is 'C' */
unsigned char *argv1;                  /* argv[1] */
char castlbuf[0x7d00];                    /* castle data buffer */

/* glyphs for the three-way animation (8x8 each) */
char flick1[8][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 },
    { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };
char flick2[8][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 0, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1, 1, 0, 0 }, { 1, 1, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };
char flick3[8][8] = {
    { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 }, { 1, 1, 1, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };

/* draw a 7x7 bitmap p at x (row 0x37) in two colours */
drawbmp(p, x) char (*p)[8]; int x;
{
    int c1, c2, j, i;

    c1 = 1;
    c2 = 2;
    if (vidmode) {
        c1 = 0xb;
        c2 = 0xa;
    }
    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            if (p[i][j])
                pixel(x + j, i + 0x37, c2);
            else
                pixel(x + j, i + 0x37, c1);
}

/* draw one of three bitmaps at random at x; returns a pressed key */
randpict(x) int x;
{
    int n;

    n = rndrange(1, 0xef) / 80;
    if (n == 0)
        drawbmp(flick1, x);
    else if (n == 1)
        drawbmp(flick2, x);
    else
        drawbmp(flick3, x);
    return getkey(0);
}

/* check the video adapter (needs CGA); sets the video mode */
chkvideo()
{
    int v;

    v = detvid();
    if (v == -1) {
        F_126F("\nUltima I requires a Color Graphics Adapter.\n");
        EXIT(1);
    }
    if (v == 2 && !forcecga)
        memcheck(1);
    else
        memcheck(0);
}

/* exit with a message when there is not enough memory */
chkmem()
{
    if (freeparg < 0x1900) {
        F_126F("\nNot enough memory.\n");
        EXIT(1);
    }
}

/* "Please insert the ULTIMA I disk...", wait for a key, blank the line */
insdisk()
{
    gotoxy(3, 0x10);
    putstr("Please insert the ULTIMA I disk...", colhigh);
    while (!getkey(0))
        ;
    gotoxy(3, 0x10);
    putstr("                                   ", colline);
}

/* draw the title banner: lines, "Ultima I", subtitle, copyright */
drawbann()
{
    int i;

    vidinit(1);
    for (i = 0; i < 3; i++) {
        drawline(0x70, i + 0x3a, 0xc8, i + 0x3a, colfill);
        drawline(0x70, i + 0x4a, 0xc8, i + 0x4a, colfill);
    }
    gotoxy(0x10, 8);
    putstr("Ultima I", colline);
    gotoxy(8, 0xb);
    putstr("The First Age of Darkness", colline);
    gotoxy(0, 0x15);
    putstr("Copyright (C) 1987 Origin Systems, Inc.", colline);
}

/* LFSR-ordered reveal of the picture (two rows at a time, 4 columns at a time); returns key */
revealpc(t1, t2, h, w, x, y, col)
char **t1, **t2; int h, w, x, y, col;
{
    int n, m, bits, q, r;
    long tap;
    unsigned long s;
    unsigned char key;

    n = h * w;
    m = n - 1;
    tap = 0;
    bits = nbits(m);
    tap = lfsrtap[bits - 2];
    x--;
    s = 1;
    do {
        q = s / w;
        r = s % w;
        if (h > q)
            plotcell(t1, t2, q, r, x, y, col, w, h);
        if (s & 1)
            s = (s >> 1) ^ tap;
        else
            s >>= 1;
        key = getkey(0);
    } while (s != 1 && key == 0);
    return key;
}

/* number of significant bits in x */
nbits(x) unsigned x;
{
    int u, n;

    n = 0;
    while (x) {
        x >>= 1;
        n++;
    }
    return n;
}

/* plot the '*' points of cell (q, r) of the picture, and its three neighbours to the right */
plotcell(t1, t2, q, r, x, y, col, d, h)
char **t1, **t2; int q, r, x, y, col, d, h;
{
    int k;

    k = 0;
    if (t1[q][r] == '*')
        pixel(x + r, y + q, col);
    if (t1[q + h][r] == '*')
        pixel(x + r, y + q + h, col);
    while (k < 3) {
        if (r + d > 0xc6) {
            if (t2[q][r + d - 0xc7] == '*')
                pixel(x + r + d, y + q, col);
            if (t2[q + h][r + d - 0xc7] == '*')
                pixel(x + r + d, y + q + h, col);
        } else {
            if (t1[q][r + d] == '*')
                pixel(x + r + d, y + q, col);
            if (t1[q + h][r + d] == '*')
                pixel(x + r + d, y + q + h, col);
        }
        r += d;
        k++;
    }
}

/* set up the geometry for revealpc from the rectangle (x0, y0)-(x1, y1) and run it */
revsetup(t1, t2, x0, y0, x1, y1, col)
char **t1, **t2; int x0, y0, x1, y1, col;
{
    int b, a;

    a = (x1 - x0) / 4 + (x1 - x0) % 2;
    b = (y1 - y0 - 1) / 2 + (y1 - y0 - 1) % 2;
    return revealpc(t1, t2, b, a, x0, y0, col);
}

/* draw the whole picture at once, mirrored (rows i and 63-i) */
pictmirr()
{
    int j, i, x, y1, y2, col;
    char ch;

    y1 = 0x14;
    y2 = 0x53;
    col = 9;
    fillrect(0, y1, 0x13f, y2, 0);
    if (!vidmode)
        col = colhigh;
    for (i = 0; i < 0x20; y1++, y2--, i++) {
        x = 0x14;
        j = 0;
        do {
            if (j < 0xc6)
                ch = pictrow1[i][j];
            else
                ch = pictrow2[i][j - 0xc6];
            if (ch == '*')
                pixel(x, y1, col);
            if (j < 0xc6)
                ch = pictrow1[0x3f - i][j];
            else
                ch = pictrow2[0x3f - i][j - 0xc6];
            if (ch == '*')
                pixel(x, y2, col);
            x++;
        } while (++j < 0x113);
    }
}

/* The picture: 62 rows of 199 columns (left part) and 76 columns (right part). */
char *pictrow1[] = {
    "                                                                                                                                                                                                       ",
    "                              *************************************************************************************************************************************************************************",
    "                              **          **          **          **          **          **          **                                                                                               ",
    "                             **          **          **          ***         **          **          **                                                                                                ",
    "                             **          **          **         *****        **          **          **                                                                                                ",
    "                            **          **          **        *******       **          **          **          ****************                  ****************                                *****",
    "                            **          **          **       *********      **          **          **      **********************            **********************            ****          *********",
    "                           **          **          **      ***********     **          **          **      *************************        **************************         ****         ***********",
    "                           **          **          **     *************    **          **          **     *****                *****        *****                *****         ****        *****       ",
    "                          **          **          **    ***************   **          **          **     ****                  ****        ****                  ****         ****         ****        ",
    "                          **          **          **   *****************  **          **          **     ****                  ****        ****                  ****         ****        ****         ",
    "                         **************************************************************************     ****                  ****        ****                  ****         ****         ****         ",
    "                         **          **          **        ****          **          **          **     ****                  ****        ****                  ****         ****        ****          ",
    "                        **          **          **        ****          **          **          **     ****                  ****        ****                  ****         ****         ****          ",
    "                        **          **          **        ****          **          **          **     ****                  ****        ****                 *****         ****        ****           ",
    "                       **          **          **        ****          **          **          **     ****                  ****        ****              ********         ****         ****           ",
    "                       **          **          **        ****          **          **          **     ****                  ****        ****        ************           ****        ****            ",
    "                      **          **          **        ****          **          **          **     ****                  ****        ****       **********              ****         ****            ",
    "                      **          **          **        ****          **          **          **     ****                  ****        ****      ******                   ****        ****          ***",
    "                     **          **          **        ****          **          **          **     ****                  ****        ****       ****                    ****         ****          ***",
    "                     **          **          **        ****          **          **          **     ****                  ****        ****        ****                   ****        ****           ***",
    "                    **************************************************************************     ****                  ****        ****          ****                 ****         ****              ",
    "                    **          **          **        ****          **          **          **     ****                  ****        ****           ****                ****        ****               ",
    "                   **          **          **        ****          **          **          **     ****                  ****        ****             ****              ****         ****               ",
    "                   **         ***          **        ****          **          ***         **     ****                  ****        ****              ****             ****        ****                ",
    "                  **        ****          **        ****          **          *****       **     ****                  ****        ****                ****           ****         ****                ",
    "                  **      ******          **        ****          **          ******      **     ****                  ****        ****                 ****          ****        ****                 ",
    "                 **    ********          **        ****          **          ********    **     ******               *****        ****                  ****         ****         ******               ",
    "                 **  **********          **        ****          **          *********   **     **************************        ****                  ****         ****         *********************",
    "                *  ***********          **        ****          **          ***********  *        **********************         ****                   ****        ****            *******************",
    "                 ***********************************************************************            ****************             ****                  ****         ****              **************** ",
    "               **************************************************************************                                                                                                              ",
    "                ***********************************************************************                                                                                                                ",
    "              *   **********          **        ****          **          ***********  *                                                                                                               ",
    "              **    ********          **        ****          **          *********   **                                                                                                               ",
    "             **      ******          **        ****          **          *******     **                                                                                                                ",
    "             **       *****          **        ****          **          *****       **                                                                                                                ",
    "            **         ***          **        ****          **          ***         **                                                                                                                 ",
    "            **          **          **        ****          **          **          **                                                                                                                 ",
    "           **          **          **        ****          **          **          **        ********    ****    ****      ********     *************    **********    ****        ****     ********   ",
    "           **          **          **        ****          **          **          **     ***********    ****    ****   ***********     *************    **********    ****        ****  ***********   ",
    "          **************************************************************************     ****           ****    ****   ****                 ****        ****          ******    ******  ****           ",
    "          **          **          **        ****          **          **          **    ****            ****    ****  ****                  ****        ****          ******  ******** ****            ",
    "         **          **          **        ****          **          **          **    ****            ****    ****  ****                  ****        ****          **************** ****             ",
    "         **          **          **        ****          **          **          **     ****           ****    ****   ****                 ****        ****          ****  ********** ****             ",
    "        **          **          **        ****          **          **          **       ********      ***********     ********           ****        *********     ****   ***  ****   *********       ",
    "        **          **          **        ****          **          **          **          *******    *********          *******         ****        *********     ****        ****      ********     ",
    "       **          **          **        ****          **          **          **               ****     ****                 ****       ****        ****          ****        ****            ****    ",
    "       **          **          **        ****          **          **          **               ****     ****                 ****       ****        ****          ****        ****            ****    ",
    "      **          **          **        ****          **          **          **               ****     ****                 ****       ****        ****          ****        ****            ****     ",
    "      **          **          **        ****          **          **          **              ****      ****                ****        ****        ****          ****        ****           ****      ",
    "     **************************************************************************        **********      ****          **********        ****        **********    ****        ****     **********       ",
    "     **          **          **  *****************   **          **          **        *******         ****          ********          ****        **********    ****        ****     ********         ",
    "    **          **          **    ***************   **          **          **                                                                                                                         ",
    "    **          **          **    *************     **          **          **                                                                                                                         ",
    "   **          **          **      ***********     **          **          **                                                                                                                          ",
    "   **          **          **      *********       **          **          **                                                                                                                          ",
    "  **          **          **       ********       **          **          **                                                                                                                           ",
    "  **          **          **        *****         **          **          **                                                                                                                           ",
    " **          **          **         ***          **          **          **                                                                                                                            ",
    " **          **          **         **           **          **          **                                                                                                                            ",
    "*******************************************************************************************************************************************************************************************************",
    "                                                                                                                                                                                                       ",
    "                                                                                                                                                                                                       "
};

char *pictrow2[] = {
    "                                                                            ",
    "*************************************************************************** ",
    "                                                                        **  ",
    "                                                                        **  ",
    "                                                                       **   ",
    "***********                                                            **   ",
    "*************          ****          ****               ****          **    ",
    "***************        ****          ****               ****          **    ",
    "          *****       ****          ******             ****          **     ",
    "          ****        ****          ******             ****          **     ",
    "          ****       ****          *******            ****          **      ",
    "                     ****          ********           ****          **      ",
    "                    ****          *********          ****          **       ",
    "                    ****          **** ****          ****          **       ",
    "                   ****          ****   ****        ****          **        ",
    "                   ****          ****   ****        ****          **        ",
    "                  ****          ****    ****       ****          **         ",
    "                  ****          ****     ****      ****          **         ",
    "********         ****          ****      ****     ****          **          ",
    "*********        ****          ****      ****     ****          **          ",
    "*********       ****          ****        ****   ****          **           ",
    "   *****        ****          ****        ****   ****          **           ",
    "    ****       ****          ****         ****  ****          **            ",
    "   ****        ****          ****          **** ****          **            ",
    "   ****       ****          ****           ********          **             ",
    "  ****        ****          ****           ********          **             ",
    "  ****       ****          ****             ******          **              ",
    "*****        ****          ****             ******          **              ",
    "****        ****          ****              *****          **               ",
    "***         ****          ****              *****          **               ",
    "           ****          ****               ****          **                ",
    "                                                          **                ",
    "                                                         **                 ",
    "                                                         **                 ",
    "                                                        **                  ",
    "                                                        **                  ",
    "                                                       **                   ",
    "                                                       **                   ",
    "                                                      **                    ",
    "       ****    ****   ****      ******                **                    ",
    "       ****    ****   ****    ********               **                     ",
    "      ****    *****  ****    ****                    **                     ",
    "      ****    *****  ****   ****                    **                      ",
    "     ****    ****** ****   ****                     **                      ",
    "     ****    ****** ****   ****                    **                       ",
    "    ****    ***********   ****                     **                       ",
    "    ****    ***********   ****                    **                        ",
    "   ****    ***********   ****                     **                        ",
    "   ****    **** ******   ****                    **                         ",
    "  ****    ****  *****   ****                     **                         ",
    "  ****    ****  *****   ****                    **                          ",
    " ****    ****   ****     *******    **          **                          ",
    " ****    ****   ****      ******    **         **                           ",
    "                                               **                           ",
    "                                              **                            ",
    "                                              **                            ",
    "                                             **                             ",
    "                                             **                             ",
    "                                            **                              ",
    "                                            **                              ",
    "                                           **                               ",
    "*********************************************                               ",
    "                                                                            ",
    "                                                                            "
};

/* LFSR feedback taps, by register width */
int lfsrtap[] = {
    0x3, 0x6, 0xc, 0x14, 0x30, 0x60, 0xb8, 0x110, 0x240, 0x500, 0xca0, 0x1b00, 0x3500, 0x6000, 0xb400, 0x2000 };

/* the credits screen; then execl("gen.exe") until it works */
credits()
{
    int r;

    pictmirr();
    gotoxy(1, 0x11);
    putstr("ULTIMA and LORD BRITISH are registered", colline);
    gotoxy(2, 0x12);
    putstr("trademarks of Richard Garriott and", colline);
    gotoxy(0xb, 0x13);
    putstr("Origin Systems, Inc.", colline);
    gotoxy(6, 0x17);
    putstr("Conversion by: John Fachini", colline);
    waittick(0x3c);
    for (;;) {
        r = EXECL("gen.exe", "gen.exe", argv1, 0);
        gotoxy(1, 0x15);
        putstr("Insert ULTIMA I disk and press RETURN", colhigh);
        while (getkey(0) != 0xd)
            ;
        gotoxy(1, 0x15);
        putstr("                                      ", colhigh);
    }
}

/* read castle.4 (CGA) or castle.16 into castlbuf */
ldcastle()
{
    int fp, n;

    if (!vidmode) {
        fp = FOPEN("castle.4", "rb");
        n = FREAD(castlbuf, 0x4000, 1, fp);
        FCLOSE(fp);
    } else {
        fp = FOPEN("castle.16", "rb");
        n = FREAD(castlbuf, 0x7d00, 1, fp);
        FCLOSE(fp);
    }
}

/* clear the screen, show the castle picture and wait for a key; returns it (0 = none) */
castlpic()
{
    int i, k;

    fillrect(0, 0, 0x13f, 0xc7, 0);
    if (vidmode)
        palcastl();
    switch (vidmode) {
    case 0:
        blitcga();
        break;
    case 1:
        blitega();
        break;
    case 2:
        blittand();
        break;
    }
    for (i = 0; i < 100; i++) {
        waittick(3);
        if (!(k = randpict(0x7b)))
            k = randpict(0xc4);
        if (k)
            break;
    }
    fillrect(0, 0, 0x13f, 0xc7, 0);
    if (vidmode)
        palnorm();
    return k;
}

/* the "new release" story text, waiting for a key between pages; then the credits */
story()
{
    int k, i;

    gotoxy(0xe, 0xd);
    putstr("           ", coltext);
    gotoxy(5, 0xc);
    putstr("...a new release of the best", coltext);
    gotoxy(5, 0xd);
    putstr("   selling personal computer", coltext);
    gotoxy(5, 0xe);
    putstr("   role-playing adventure...", coltext);
    for (i = 0; i < 0x4b; i++) {
        waittick(1);
        k = getkey(0);
        if (k)
            break;
    }
    if (k == 0) {
        fillrect(0, 0x5a, 0x13f, 0xb3, 0);
        gotoxy(6, 0xc);
        putstr("...Lord British's original", coltext);
        gotoxy(6, 0xd);
        putstr("   fantasy masterpiece...", coltext);
        for (i = 0; i < 0x4b; i++) {
            waittick(1);
            k = getkey(0);
            if (k)
                break;
        }
        if (k == 0)
            k = castlpic();
    }
    if (k) {
        fillrect(0, 0, 0x13f, 0xc7, 0);
        credits();
    }
}

/* draw the picture (star by star unless a key is pressed) and its dot; returns the key */
pictstar()
{
    int col, r;

    col = 9;
    fillrect(0, 0, 0x13f, 0xc7, 0);
    if (!vidmode)
        col = colhigh;
    r = revsetup(pictrow1, pictrow2, 0x15, 0x14, 0x129, 0x53, col);
    pixel(0x59, 0x33, col);
    return r;
}

/* the "Presents..." screen after the picture, or the credits if a key was pressed */
presents()
{
    int k;

    k = pictstar();
    if (k == 0) {
        gotoxy(0xe, 0xd);
        waittick(5);
        putstr("Presents...", coltext);
        waittick(0x2d);
    } else
        credits();
}

/* main: ULTIMA.EXE entry (argv[1] = video mode letter) */
main(argc, argv) int argc; unsigned char **argv;
{
    erron(1);
    argv1 = argv[1];
    forcecga = upcase(argv[1][0]) == 'C';
    chkvideo();
    chkmem();
    drawbann();
    ldcastle();
    if (vidmode == 1)
        cvtplane();
    for (;;) {
        presents();
        story();
    }
}
