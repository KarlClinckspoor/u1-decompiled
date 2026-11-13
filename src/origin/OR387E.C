/* opts: -ms -v -w -cs */
/*
 * Arrows drawn over a text cell (Origin support library, C module).
 */
extern int colline;              /* drawing colour (foreground) */
extern int colfill;              /* text/background colour */

extern gotoxy();                /* set text cursor (col, row) */
extern putch();                /* put character (ch, colour) */
extern drawline();                /* draw line (x1, y1, x2, y2, colour) */

/* 387E: draw a right-pointing arrow over text cell (col, row) */
arrowr(col, row)
int col, row;
{
    gotoxy(col, row);
    putch(0x10, colfill);
    drawline(col << 3, row << 3, (col << 3) + 7, (row << 3) + 3, colline);
    drawline((col << 3) + 7, (row << 3) + 3, col << 3, (row << 3) + 7, colline);
    drawline(col << 3, (row << 3) + 1, col << 3, (row << 3) + 6, colfill);
}

/* 3920: draw a left-pointing arrow over text cell (col, row) */
arrowl(col, row)
int col, row;
{
    gotoxy(col, row);
    putch(0x11, colfill);
    drawline((col << 3) + 7, row << 3, col << 3, (row << 3) + 3, colline);
    drawline(col << 3, (row << 3) + 3, (col << 3) + 7, (row << 3) + 7, colline);
    drawline((col << 3) + 7, (row << 3) + 1, (col << 3) + 7, (row << 3) + 6, colfill);
}
