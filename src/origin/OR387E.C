/* opts: -ms -v -w -cs */
/*
 * Arrows drawn over a text cell (Origin support library, C module).
 */
extern int D_0DA6;              /* drawing colour (foreground) */
extern int D_0DA8;              /* text/background colour */

extern F_352C();                /* set text cursor (col, row) */
extern F_357C();                /* put character (ch, colour) */
extern F_39FC();                /* draw line (x1, y1, x2, y2, colour) */

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
