/* opts: -ms -v -w -cs */
/*
 * Wait for a key with a blinking cursor (Origin support library, C module).
 */
extern F_3E5E();                /* wait n timer ticks */
extern char F_413C();           /* read key, blinking the cursor in colour c */

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
