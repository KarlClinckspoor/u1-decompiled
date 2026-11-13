/* opts: -ms -v -w -cs */
/*
 * Game screen frame (Origin support library, C module).
 */
extern int colline;              /* drawing colour (foreground) */
extern int colfill;              /* text/background colour */

extern fillrect();                /* fill rectangle (x1, y1, x2, y2, colour) */
extern drawline();                /* draw line (x1, y1, x2, y2, colour) */
extern pixel();                /* plot pixel and remember it (x, y, colour) */

/* 3AD7: draw the game screen frame */
gamefram()
{
    int i;

    fillrect(0, 0, 0x13c, 6, colfill);
    fillrect(0, 7, 7, 0x9e, colfill);
    fillrect(0x138, 2, 0x13f, 0x9e, colfill);
    fillrect(0, 0x98, 0x13f, 0x9e, colfill);
    drawline(8, 7, 0x137, 7, colline);
    drawline(7, 8, 7, 0x97, colline);
    drawline(0x138, 8, 0x138, 0x97, colline);
    drawline(8, 0x98, 0x137, 0x98, colline);
    drawline(0, 0x9f, 0x13f, 0x9f, colline);
    drawline(0xf1, 0x9f, 0xf1, 0xc7, colline);
    drawline(0xf6, 0x9f, 0xf6, 0xc7, colline);
    for (i = 1; i <= 4; i++) {
        drawline(i, 0, 0, i, 0);
        drawline(0x13f - i, 0, 0x13f, i, 0);
        drawline(i + 0xf1, 0x9f, i + 0xf1, 0xc7, colfill);
    }
    pixel(0, 0, 0);
    pixel(0, 5, 0);
    pixel(5, 0, 0);
    pixel(0x13f, 0, 0);
    pixel(0x13a, 0, 0);
    pixel(0x13f, 5, 0);
}
