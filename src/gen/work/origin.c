/* opts: -ms -v */
/*
 * Origin support routines, C part (GEN.EXE code 387E-39FB and 3AD7-3C8A).
 * The rest of the support library is assembly: see origin_asm.asm.
 */
extern int D_0DA6;              /* drawing colour (foreground) */
extern int D_0DA8;              /* text/background colour */

extern F_352C();                /* set text cursor (col, row) */
extern F_357C();                /* put character (ch, colour) */
extern F_35CA();                /* fill rectangle (x1, y1, x2, y2, colour) */
extern F_39FC();                /* draw line (x1, y1, x2, y2, colour) */
extern F_3D41();                /* plot pixel and remember it (x, y, colour) */
extern F_3E5E();                /* wait n timer ticks */
extern char F_413C();           /* read key, blinking the cursor in colour c */

/* 387E: draw a right-pointing arrow over text cell (col, row) */
F_387E(col, row)
int col, row;
{
    F_352C(col, row);
    F_357C(0x10, D_0DA8);
    F_39FC(col << 3, row << 3, (col << 3) + 7, (row << 3) + 3, D_0DA6);
    F_39FC((col << 3) + 7, (row << 3) + 3, col << 3, (row << 3) + 7, D_0DA6);
    F_39FC(col << 3, (row << 3) + 1, col << 3, (row << 3) + 6, D_0DA8);
}

/* 3920: draw a left-pointing arrow over text cell (col, row) */
F_3920(col, row)
int col, row;
{
    F_352C(col, row);
    F_357C(0x11, D_0DA8);
    F_39FC((col << 3) + 7, row << 3, col << 3, (row << 3) + 3, D_0DA6);
    F_39FC(col << 3, (row << 3) + 3, (col << 3) + 7, (row << 3) + 7, D_0DA6);
    F_39FC((col << 3) + 7, (row << 3) + 1, (col << 3) + 7, (row << 3) + 6, D_0DA8);
}

/* 39C5: wait for a key, blinking the cursor; returns the key */
char F_39C5(colour)
int colour;
{
    char c, k;

    do {
        c = F_413C(colour);
        if (c == 0)
            F_3E5E(3);
        else
            F_3E5E(1);
    } while (c == 0);
    return c;
}

/* 3AD7: draw the game screen frame */
F_3AD7()
{
    int i;

    F_35CA(0, 0, 0x13c, 6, D_0DA8);
    F_35CA(0, 7, 7, 0x9e, D_0DA8);
    F_35CA(0x138, 2, 0x13f, 0x9e, D_0DA8);
    F_35CA(0, 0x98, 0x13f, 0x9e, D_0DA8);
    F_39FC(8, 7, 0x137, 7, D_0DA6);
    F_39FC(7, 8, 7, 0x97, D_0DA6);
    F_39FC(0x138, 8, 0x138, 0x97, D_0DA6);
    F_39FC(8, 0x98, 0x137, 0x98, D_0DA6);
    F_39FC(0, 0x9f, 0x13f, 0x9f, D_0DA6);
    F_39FC(0xf1, 0x9f, 0xf1, 0xc7, D_0DA6);
    F_39FC(0xf6, 0x9f, 0xf6, 0xc7, D_0DA6);
    for (i = 1; i <= 4; i++) {
        F_39FC(i, 0, 0, i, 0);
        F_39FC(0x13f - i, 0, 0x13f, i, 0);
        F_39FC(i + 0xf1, 0x9f, i + 0xf1, 0xc7, D_0DA8);
    }
    F_3D41(0, 0, 0);
    F_3D41(0, 5, 0);
    F_3D41(5, 0, 0);
    F_3D41(0x13f, 0, 0);
    F_3D41(0x13a, 0, 0);
    F_3D41(0x13f, 5, 0);
}
