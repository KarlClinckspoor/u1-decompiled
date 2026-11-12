/* opts: -ms -v -w -cs */
/*
 * OUT.EXE game code 2B6D-4013 (work file, agent ou_2b6d).
 */

/* Origin library */
extern F_3B46(), F_3599(), F_3C8B(), F_39FC(), F_4069();
/* other game code */
extern F_0476(), F_2355(), F_23E7(), F_22F3(), F_450D(), F_B34F();

extern int D_2046;                      /* text colour */
extern int D_AD3E, D_AD40;              /* player x, y */
extern int D_AD46;
extern int D_ACC6[], D_ACC8[], D_ACCC[];        /* monster table, 8 bytes each */
extern int D_207E;
extern int D_8B9E, D_8BA0, D_8BA2;
extern int D_204A;
extern int D_206C;

/* attack in direction dir (1-4) up to range squares: hit chance, damage to the monster met */
F_2B6D(dir, range, damage, chance)
unsigned char dir;
int range, damage, chance;
{
    int m, i, dx, dy, tile, ox, oy, t2;

    i = 1;
    F_0476(7);
    switch (dir) {
    case 3:
        dx = 0;
        dy = -1;
        break;
    case 4:
        dx = 0;
        dy = 1;
        break;
    case 2:
        dx = 1;
        dy = 0;
        break;
    case 1:
        dx = -1;
        dy = 0;
        break;
    }
    do {
        ox = dx * i;
        oy = i * dy;
        tile = F_2355(D_AD3E + ox, D_AD40 + oy);
        i++;
        t2 = F_23E7(D_AD3E + ox, D_AD40 + oy);
        if (t2 == 0x3f)
            t2 = -1;
    } while (i <= range && tile != 0x11 && tile != 0x16 && tile != 0x13 && tile != 0x32
             && tile != 0x14 && tile != 0x15 && t2 == -1);
    m = F_22F3(D_AD3E + ox, D_AD40 + oy);
    if (m == -1 || F_4069(1, 100) > chance) {
        F_3B46("Missed!");
        return;
    }
    F_3599("Hit ", D_2046);
    switch (tile) {
    case 0x14:
        F_3599("King! ", D_2046);
        break;
    case 0x11:
        F_3599("Guard! ", D_2046);
        break;
    case 0x15:
        F_3599("Merchant! ", D_2046);
        break;
    case 0x16:
        F_3599("Princess! ", D_2046);
        break;
    case 0x13:
        if (D_204A == 1)
            F_3599("Bard! ", D_2046);
        else
            F_3599("Jester! ", D_2046);
        break;
    case 0x32:
        if (D_206C == 0)
            F_3599("Wench! ", D_2046);
        else
            F_3599("Lecher! ", D_2046);
        break;
    }
    D_AD46 = 1;
    F_0476(2);
    if (damage >= D_ACCC[m * 4]) {
        F_3B46("Killed!");
        if (tile != 0x16) {
            D_207E++;
            F_450D();
        } else
            D_8B9E = 0;
        D_ACC6[m * 4] = -1;
        D_ACC8[m * 4] = 0x7f;
        F_B34F(1, D_AD3E + ox, D_AD40 + oy);
        if (tile == 0x11)
            D_207E += 14;
        if (tile == 0x13 && D_204A == 2) {
            F_3B46("Thou hast found a key!");
            D_8BA0 = 1;
        }
    } else {
        if (tile != 0x14 || D_8BA2 == 0)
            D_ACCC[m * 4] -= damage;
        F_3C8B(damage, 2, 0x20, D_2046, 1);
        F_3B46(" damage!");
    }
}
