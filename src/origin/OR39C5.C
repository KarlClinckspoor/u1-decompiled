/* opts: -ms -v -w -cs */
/*
 * Wait for a key with a blinking cursor (Origin support library, C module).
 */
extern waittick();                /* wait n timer ticks */
extern char getkey();           /* read key, blinking the cursor in colour c */

/* 39C5: wait for a key, blinking the cursor; returns the key */
char waitkey(colour)
int colour;
{
    char c, k;

    do {
        c = getkey(colour);
        if (c == 0)
            waittick(3);
        else
            waittick(1);
    } while (c == 0);
    return c;
}
