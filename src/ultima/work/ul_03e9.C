/* opts: -ms -v -w -cs */
/*
 * ULTIMA.EXE - title screen: "Ultima I" banner, the bouncing-star picture, the story,
 * loading castle.4/castle.16, then execl("gen.exe") (code 03E9-0E4D).
 * Work file; names are still the original addresses.
 */
extern int D_082C;                      /* video mode: 0 CGA, 1 EGA, 2 Tandy */
extern unsigned D_4996;
extern int D_0DA6, D_2044, D_0DA8, D_2046, D_2048;
extern unsigned char *D_4A2A;           /* argv[1] */
extern char D_4A2C[];                   /* castle data buffer */
extern char *D_3262[], *D_4320[];       /* the picture, rows of '*' */
extern int D_43A0[];                    /* LFSR taps */

extern F_352C(), F_3599(), F_35CA(), F_39FC(), F_3BD5(), F_3C6C(), F_3C87(), F_3CA2();
extern F_3CCD(), F_3CE9(), F_3D41(), F_3D6A(), F_3E5E(), F_3F58(), F_4069(), F_1D0D();
extern F_126F();
extern int EXECL(), FOPEN(), FREAD(), FCLOSE(), EXIT(), F_34BF();
extern unsigned char F_413C(), F_3862();

/* glyphs for the three-way animation (8x8 each) */
char D_0124[8][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 },
    { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };
char D_0164[8][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 1, 1, 0, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1, 1, 0, 0 }, { 1, 1, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };
char D_01A4[8][8] = {
    { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 }, { 1, 1, 1, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } };

/* draw a 7x7 bitmap p at x (row 0x37) in two colours */
F_03E9(p, x) char (*p)[8]; int x;
{
    int c1, c2, j, i;

    c1 = 1;
    c2 = 2;
    if (D_082C) {
        c1 = 0xb;
        c2 = 0xa;
    }
    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            if (p[i][j])
                F_3D41(x + j, i + 0x37, c2);
            else
                F_3D41(x + j, i + 0x37, c1);
}

/* draw one of three bitmaps at random at x; returns a pressed key */
F_0473(x) int x;
{
    int n;

    n = F_4069(1, 0xef) / 80;
    if (n == 0)
        F_03E9(D_0124, x);
    else if (n == 1)
        F_03E9(D_0164, x);
    else
        F_03E9(D_01A4, x);
    return F_413C(0);
}

/* check the video adapter (needs CGA); sets the video mode */
F_04D0()
{
    int v;

    v = F_34BF();
    if (v == -1) {
        F_126F("\nUltima I requires a Color Graphics Adapter.\n");
        EXIT(1);
    }
    if (v == 2 && !D_2048)
        F_3BD5(1);
    else
        F_3BD5(0);
}

/* exit with a message when there is not enough memory */
F_0516()
{
    if (D_4996 < 0x1900) {
        F_126F("\nNot enough memory.\n");
        EXIT(1);
    }
}

/* "Please insert the ULTIMA I disk...", wait for a key, blank the line */
F_053B()
{
    F_352C(3, 0x10);
    F_3599("Please insert the ULTIMA I disk...", D_2044);
    while (!F_413C(0))
        ;
    F_352C(3, 0x10);
    F_3599("                                   ", D_0DA6);
}

/* draw the title banner: lines, "Ultima I", subtitle, copyright */
F_0586()
{
    int i;

    F_3F58(1);
    for (i = 0; i < 3; i++) {
        F_39FC(0x70, i + 0x3a, 0xc8, i + 0x3a, D_0DA8);
        F_39FC(0x70, i + 0x4a, 0xc8, i + 0x4a, D_0DA8);
    }
    F_352C(0x10, 8);
    F_3599("Ultima I", D_0DA6);
    F_352C(8, 0xb);
    F_3599("The First Age of Darkness", D_0DA6);
    F_352C(0, 0x15);
    F_3599("Copyright (C) 1987 Origin Systems, Inc.", D_0DA6);
}

/* LFSR-ordered reveal of the picture (two rows at a time, 4 columns at a time); returns key */
F_0628(t1, t2, h, w, x, y, col)
char **t1, **t2; int h, w, x, y, col;
{
    int n, m, bits, q, r;
    long tap;
    unsigned long s;
    unsigned char key;

    n = h * w;
    m = n - 1;
    tap = 0;
    bits = F_0722(m);
    tap = D_43A0[bits - 2];
    x--;
    s = 1;
    do {
        q = s / w;
        r = s % w;
        if (h > q)
            F_0744(t1, t2, q, r, x, y, col, w, h);
        if (s & 1)
            s = (s >> 1) ^ tap;
        else
            s >>= 1;
        key = F_413C(0);
    } while (s != 1 && key == 0);
    return key;
}

/* number of significant bits in x */
F_0722(x) unsigned x;
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
F_0744(t1, t2, q, r, x, y, col, d, h)
char **t1, **t2; int q, r, x, y, col, d, h;
{
    int k;

    k = 0;
    if (t1[q][r] == '*')
        F_3D41(x + r, y + q, col);
    if (t1[q + h][r] == '*')
        F_3D41(x + r, y + q + h, col);
    while (k < 3) {
        if (r + d > 0xc6) {
            if (t2[q][r + d - 0xc7] == '*')
                F_3D41(x + r + d, y + q, col);
            if (t2[q + h][r + d - 0xc7] == '*')
                F_3D41(x + r + d, y + q + h, col);
        } else {
            if (t1[q][r + d] == '*')
                F_3D41(x + r + d, y + q, col);
            if (t1[q + h][r + d] == '*')
                F_3D41(x + r + d, y + q + h, col);
        }
        r += d;
        k++;
    }
}

/* set up the geometry for F_0628 from the rectangle (x0, y0)-(x1, y1) and run it */
F_08BF(t1, t2, x0, y0, x1, y1, col)
char **t1, **t2; int x0, y0, x1, y1, col;
{
    int b, a;

    a = (x1 - x0) / 4 + (x1 - x0) % 2;
    b = (y1 - y0 - 1) / 2 + (y1 - y0 - 1) % 2;
    return F_0628(t1, t2, b, a, x0, y0, col);
}

/* draw the whole picture at once, mirrored (rows i and 63-i) */
F_0926()
{
    int j, i, x, y1, y2, col;
    char ch;

    y1 = 0x14;
    y2 = 0x53;
    col = 9;
    F_35CA(0, y1, 0x13f, y2, 0);
    if (!D_082C)
        col = D_2044;
    for (i = 0; i < 0x20; y1++, y2--, i++) {
        x = 0x14;
        j = 0;
        do {
            if (j < 0xc6)
                ch = D_3262[i][j];
            else
                ch = D_4320[i][j - 0xc6];
            if (ch == '*')
                F_3D41(x, y1, col);
            if (j < 0xc6)
                ch = D_3262[0x3f - i][j];
            else
                ch = D_4320[0x3f - i][j - 0xc6];
            if (ch == '*')
                F_3D41(x, y2, col);
            x++;
        } while (++j < 0x113);
    }
}

/* the credits screen; then execl("gen.exe") until it works */
F_0A27()
{
    int r;

    F_0926();
    F_352C(1, 0x11);
    F_3599("ULTIMA and LORD BRITISH are registered", D_0DA6);
    F_352C(2, 0x12);
    F_3599("trademarks of Richard Garriott and", D_0DA6);
    F_352C(0xb, 0x13);
    F_3599("Origin Systems, Inc.", D_0DA6);
    F_352C(6, 0x17);
    F_3599("Conversion by: John Fachini", D_0DA6);
    F_3E5E(0x3c);
    for (;;) {
        r = EXECL("gen.exe", "gen.exe", D_4A2A, 0);
        F_352C(1, 0x15);
        F_3599("Insert ULTIMA I disk and press RETURN", D_2044);
        while (F_413C(0) != 0xd)
            ;
        F_352C(1, 0x15);
        F_3599("                                      ", D_2044);
    }
}

/* read castle.4 (CGA) or castle.16 into D_4A2C */
F_0AF7()
{
    int fp, n;

    if (!D_082C) {
        fp = FOPEN("castle.4", "rb");
        n = FREAD(D_4A2C, 0x4000, 1, fp);
        FCLOSE(fp);
    } else {
        fp = FOPEN("castle.16", "rb");
        n = FREAD(D_4A2C, 0x7d00, 1, fp);
        FCLOSE(fp);
    }
}

/* clear the screen, show the castle picture and wait for a key; returns it (0 = none) */
F_0B65()
{
    int i, k;

    F_35CA(0, 0, 0x13f, 0xc7, 0);
    if (D_082C)
        F_3C6C();
    switch (D_082C) {
    case 0:
        F_3CCD();
        break;
    case 1:
        F_3D6A();
        break;
    case 2:
        F_3CA2();
        break;
    }
    for (i = 0; i < 100; i++) {
        F_3E5E(3);
        if (!(k = F_0473(0x7b)))
            k = F_0473(0xc4);
        if (k)
            break;
    }
    F_35CA(0, 0, 0x13f, 0xc7, 0);
    if (D_082C)
        F_3C87();
    return k;
}

/* the "new release" story text, waiting for a key between pages; then the credits */
F_0C08()
{
    int k, i;

    F_352C(0xe, 0xd);
    F_3599("           ", D_2046);
    F_352C(5, 0xc);
    F_3599("...a new release of the best", D_2046);
    F_352C(5, 0xd);
    F_3599("   selling personal computer", D_2046);
    F_352C(5, 0xe);
    F_3599("   role-playing adventure...", D_2046);
    for (i = 0; i < 0x4b; i++) {
        F_3E5E(1);
        k = F_413C(0);
        if (k)
            break;
    }
    if (k == 0) {
        F_35CA(0, 0x5a, 0x13f, 0xb3, 0);
        F_352C(6, 0xc);
        F_3599("...Lord British's original", D_2046);
        F_352C(6, 0xd);
        F_3599("   fantasy masterpiece...", D_2046);
        for (i = 0; i < 0x4b; i++) {
            F_3E5E(1);
            k = F_413C(0);
            if (k)
                break;
        }
        if (k == 0)
            k = F_0B65();
    }
    if (k) {
        F_35CA(0, 0, 0x13f, 0xc7, 0);
        F_0A27();
    }
}

/* draw the picture (star by star unless a key is pressed) and its dot; returns the key */
F_0D4C()
{
    int col, r;

    col = 9;
    F_35CA(0, 0, 0x13f, 0xc7, 0);
    if (!D_082C)
        col = D_2044;
    r = F_08BF(D_3262, D_4320, 0x15, 0x14, 0x129, 0x53, col);
    F_3D41(0x59, 0x33, col);
    return r;
}

/* the "Presents..." screen after the picture, or the credits if a key was pressed */
F_0DB3()
{
    int k;

    k = F_0D4C();
    if (k == 0) {
        F_352C(0xe, 0xd);
        F_3E5E(5);
        F_3599("Presents...", D_2046);
        F_3E5E(0x2d);
    } else
        F_0A27();
}

/* main: ULTIMA.EXE entry (argv[1] = video mode letter) */
F_1853(argc, argv) int argc; unsigned char **argv;
{
    F_1D0D(1);
    D_4A2A = argv[1];
    D_2048 = F_3862(argv[1][0]) == 'C';
    F_04D0();
    F_0516();
    F_0586();
    F_0AF7();
    if (D_082C == 1)
        F_3CE9();
    for (;;) {
        F_0DB3();
        F_0C08();
    }
}
