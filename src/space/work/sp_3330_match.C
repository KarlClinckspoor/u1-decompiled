/* opts: -ms -v -w -cs */
/*
 * fightfrm (F_3330): one frame of the alien ship in combat. MATCHES the original 376 bytes
 * (tools/lc.py --exe gamedata/SPACE.EXE --syms src/space/space.syms: "FIGHTFRM 3330 MATCH").
 *
 * The key: the old position is first copied into the locals dx/dy themselves, then each is
 * turned into the distance:
 *     dx = enemypx = enemyx;  dy = enemypy = enemyy;
 *     dx = crosx - dx;        dy = crosy - dy;
 * LC keeps locals in registers and drops their dead first stores, so ax/bx hold the copies
 * (`mov ax,[enemyx]; mov [enemypx],ax; mov bx,[enemyy]; mov [enemypy],bx; mov cx,[crosx];
 * sub cx,ax; mov ax,[crosy]; sub ax,bx`). `enemypx = dx = enemyx; enemypy = dy = enemyy;`
 * also matches. Block-local temporaries (`{ int x, y; ... }`) give the same register use but
 * enlarge the frame by 4 and put dy in dx.
 * The rest of the function is unchanged from work/sp_2b7b.C. In the old-name work file:
 *     dx = D_0C96 = D_0C92;
 *     dy = D_0C98 = D_0C94;
 *     dx = D_013A - dx;
 *     dy = D_013C - dy;
 * (with that edit sp_2b7b.C compiles F_3330 and F_386B to matching code.)
 */
struct obj { int x, y, type, f6, f8, dir; };
struct sector { int count, f2, f4, f6, f8; struct obj o[4]; };
extern struct sector galaxy[7][7];
extern int sectx, secty, speed, exhcnt, crosx, crosy;
extern int enemyx, enemyy, enemypx, enemypy, enemykd, jumping, speeddly[];
extern int exhdrawn, growcnt, colline;
extern movestar(), vwait(), drawfite(), newenemy(), drawcros(), alienfir();
extern int rndrange(), sign();

fightfrm()
{
    int r, dx, dy;

    if (exhcnt)
        exhcnt--;
    exhdrawn = 0;
    movestar();
    vwait(speeddly[speed]);
    if (galaxy[sectx][secty].count != 0) {
        dx = enemypx = enemyx;
        dy = enemypy = enemyy;
        dx = crosx - dx;
        dy = crosy - dy;

        enemyx -= sign(dx);
        enemyy -= sign(dy);
        if (enemyx > 280 || enemyx < 30 || enemyy > 125 || enemyy < 24) {
            if (enemypx)
                drawfite(enemypx, enemypy, enemykd, 0);
            newenemy();
        } else {
            if (enemypx)
                drawfite(enemypx, enemypy, enemykd, 0);
            if (jumping && growcnt >= 8 - speed || growcnt > 15) {
                r = rndrange(1, (speed + 3) * 10);
                if (r > 20 || jumping)
                    enemykd++;
                if (enemykd > 6)
                    newenemy();
                growcnt = 0;
            } else
                growcnt++;
        }
        if (enemyx) {
            drawfite(enemyx, enemyy, enemykd, 1);
            alienfir();
            if (((dx) < 0 ? -(dx) : (dx)) < 4 || ((dy) < 0 ? -(dy) : (dy)) < 4)
                drawcros(colline);
        }
    }
}
