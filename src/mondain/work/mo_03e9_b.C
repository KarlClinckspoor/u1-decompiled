/* opts: -ms -v -w -cs */
/*
 * MONDAIN.EXE - the final battle against Mondain, part 2 (F_09C0-F_125C).
 * Work file; names are still the original addresses.
 */
extern int D_2046, D_2082, D_2084, D_207A, D_206E, D_204E;
extern int D_0126, D_0128, D_012A, D_012C, D_012E, D_0190;
extern int D_0244, D_0246, D_0248, D_024A;
extern char D_0216;
extern unsigned char D_0194[][9];
extern char *D_1DC8[], *D_0DF8[], *D_0F04[];
extern int D_0CD2[], D_20D8[];

extern F_3599(), F_3B46(), F_053D(), F_0944(), F_355D(), F_0831(), F_4437();
extern F_3C8B(), F_352C(), F_3EED(), F_0E56();
extern int F_0618(), F_064A(), F_4069(), F_1415(), F_1447(), F_0F7B(), F_0FC7();
extern int F_0AD9(), F_0B07(), F_0BE4(), F_0CAC(), F_0D08(), F_0D88(), F_0A7A();
extern unsigned char F_05DB();
extern char F_39C5();

/* aim: read a direction (or use dir), say it, and attack with weapon w */
F_09C0(dir, w)
int dir, w;
{
    int d;

    if (dir == -1)
        d = F_05DB();
    else
        d = dir;
    if (d > 4)
        F_3B46("nothing");
    else {
        F_3B46(D_1DC8[d]);
        F_0944(d, w);
    }
}

/* Attack: choose weapon, ask for a direction */
F_0A0D(dir)
int dir;
{
    D_0246 = 1;
    F_3599("Attack with ", D_2046);
    F_3599(D_0DF8[D_2082], D_2046);
    if (D_0CD2[D_2082] == 0) {
        F_3B46("?");
        F_053D(1);
    } else {
        F_3599(": ", D_2046);
        F_09C0(dir, 7);
    }
}

/* damage of the spell: random up to D_207A, scaled by the spell */
F_0A7A()
{
    int t;

    t = F_4069(2, D_207A);
    if (D_2082 == 9)
        t <<= 1;
    else if (D_2082 == 8) {
        t *= 3;
        t /= 2;
    } else if (D_2082 == 10 || D_2082 == 11)
        t *= 3;
    return t;
}

/* spell with no effect (D_0618 test) */
F_0AD9()
{
    F_3B46("");
    if (F_0618) {
        F_3B46("Hmmm...no effect!");
        F_053D(6);
        return 1;
    }
    return 0;
}

/* spell: ask a direction, put a block (5) in the empty square there */
F_0B07()
{
    unsigned char c;
    int x, y;

    F_3599(": ", D_2046);
    c = F_05DB();
    if (c > 4) {
        F_3B46("nothing");
        F_053D(6);
        return 1;
    }
    F_3B46(D_1DC8[c]);
    if (!F_0618())
        return 0;
    x = D_0126 + F_1415(c);
    y = D_0128 + F_1447(c);
    if (D_0194[x][y] == 0 && x != D_012A && y != D_012C) {
        D_0194[x][y] = 5;
        F_355D(5, x, y);
        F_053D(5);
        F_3B46("Done.");
        return 1;
    }
    return 0;
}

/* spell: ask a direction, remove a block (5) from the square there */
F_0BE4()
{
    unsigned char c;
    int x, y;

    F_3599(": ", D_2046);
    c = F_05DB();
    if (c > 4) {
        F_3B46("nothing");
        F_053D(6);
        return 1;
    }
    F_3B46(D_1DC8[c]);
    if (!F_0618())
        return 0;
    x = D_0126 + F_1415(c);
    y = D_0128 + F_1447(c);
    if (D_0194[x][y] == 5) {
        D_0194[x][y] = 0;
        F_355D(0, x, y);
        F_053D(5);
        F_3B46("Done.");
        return 1;
    }
    return 0;
}

/* spell: strengthen Mondain (doubles his hit points below 15000) */
F_0CAC()
{
    F_3B46("");
    if (F_0618() && F_064A()) {
        F_3B46("\"INTERFICIO-NUNC!\"");
        F_3B46("The spell doth seem");
        F_3B46("to make him stronger!");
        if (D_012E < 15000)
            D_012E <<= 1;
        F_053D(5);
        return 1;
    }
    return 0;
}

/* spell: ask a direction and fire a bolt of spell damage */
F_0D08()
{
    unsigned char c;
    int t;

    F_3599(": ", D_2046);
    c = F_05DB();
    if (c > 4) {
        F_3B46("nothing");
        F_053D(6);
        return 0;
    }
    F_3B46(D_1DC8[c]);
    if (F_0618() && F_064A()) {
        t = F_0A7A();
        F_0831(c, 5, 5, t, 500);
        return 1;
    }
    return 0;
}

/* spell: teleport the player to a random empty square */
F_0D88()
{
    int x, y, r;

    r = 0;
    F_3B46("");
    if (F_0618()) {
        do {
            x = F_4069(1, 255) % 19;
            y = F_4069(1, 255) % 9;
        } while (D_0194[x][y] != 0);
        F_355D(0, D_0126, D_0128);
        F_355D(0x14, x, y);
        D_0194[x][y] = 0x14;
        D_0194[D_0126][D_0128] = 0;
        D_0126 = x;
        D_0128 = y;
        r = 1;
        F_053D(5);
        F_3B46("Done.");
    }
    return r;
}

/* cast spell number w */
F_0E56(w)
int w;
{
    int t;

    t = 0;
    switch (w) {
    case 0:
        t = F_0AD9();
        break;
    case 8:
        t = F_0B07();
        break;
    case 9:
        t = F_0BE4();
        break;
    case 10:
        t = F_0CAC();
        break;
    case 7:
        t = F_0D88();
        break;
    default:
        t = F_0D08();
    }
    if (t == 0) {
        F_053D(6);
        F_3B46("Failed!");
    }
}

/* Cast: use up a charge of spell D_2084 and cast it if it is a usable one */
F_0ED3()
{
    F_3599("Cast ", D_2046);
    F_3599(D_0F04[D_2084], D_2046);
    if (D_20D8[D_2084] == 0) {
        F_3B46("");
        F_3B46("You've used up that spell!");
        F_053D(6);
    } else {
        if (D_2084)
            D_20D8[D_2084]--;
        if (D_2084 != 3 && D_2084 != 7 && D_2084 != 8 && D_2084 != 9 && D_2084 != 10) {
            F_3B46("");
            F_3B46("Failed!");
            F_053D(6);
        } else
            F_0E56(D_2084);
    }
}

/* is the square (player + (dx, dy)) inside the field and of kind 6 (the gem) */
F_0F7B(dx, dy)
int dx, dy;
{
    int x, y;

    x = D_0126 + dx;
    y = D_0128 + dy;
    if (x > 0 && x < 19 && y > 0 && y < 9 && D_0194[x][y] == 6)
        return 1;
    return 0;
}

/* is the gem on any of the eight squares around the player */
F_0FC7()
{
    return F_0F7B(1, 1) || F_0F7B(1, 0) || F_0F7B(0, 1) || F_0F7B(-1, -1)
        || F_0F7B(-1, 0) || F_0F7B(0, -1) || F_0F7B(1, -1) || F_0F7B(-1, 1);
}

/* Get (Gem): take the gem next to the player and destroy it (hurts the player) */
F_1057()
{
    int t, i;

    F_3599("Get (Gem)", D_2046);
    if (D_0248 != 0 || !F_0FC7()) {
        F_3B46("..'tis nothing here!");
        F_053D(1);
    } else {
        D_0248 = D_204E = D_0246 = 1;
        D_0216 = 0;
        D_0190 = 0;
        t = D_206E * 3;
        t /= 4;
        F_3599("... ", D_2046);
        F_3C8B(t, 1, 0x20, D_2046, 1);
        F_3B46(" damage!");
        for (i = 0; i < 5; i++)
            F_053D(2);
        F_3B46("The Gem is DESTROYED!");
        D_206E -= t;
        F_4437();
        if (D_0244 == 30)
            D_024A = 1;
    }
}

/* Inform and Search: report on the gem */
F_1124()
{
    F_3599("Inform and Search", D_2046);
    if (D_0248) {
        F_3B46("");
        F_3B46("Mondain's magical aura doth");
        F_3B46("seem substantially diminished");
        F_3B46("in the absence of the gem.");
    } else {
        F_3B46("...it looks");
        F_3B46("as if he is creating the evil");
        F_3B46("gem!");
    }
}

/* print message s and beep */
F_117E(s)
char *s;
{
    F_3B46(s);
    F_053D(1);
}

/* "Board?" */
F_1194() { F_117E("Board?"); }
/* "Drop?" */
F_11A2() { F_117E("Drop?"); }
/* "Enter?" */
F_11B0() { F_117E("Enter?"); }
/* "Fire?" */
F_11BE() { F_117E("Fire?"); }
/* "Hyperjump?" */
F_11CC() { F_117E("Hyperjump?"); }
/* "Klimb?" */
F_11DA() { F_117E("Klimb?"); }
/* "Open?" */
F_11E8() { F_117E("Open?"); }
/* "Quit...is not allowed!" */
F_11F6() { F_117E("Quit...is not allowed!"); }

/* Steal: refused */
F_1204()
{
    F_3B46("Steal...he's watching,");
    F_117E("That would be foolish!");
}

/* Transact: refused */
F_121B()
{
    F_3B46("Transact...Mondain will");
    F_117E("not negotiate!");
}

/* "Unlock?" */
F_1232() { F_117E("Unlock?"); }
/* "View?" */
F_1240() { F_117E("View?"); }
/* "Xit?" */
F_124E() { F_117E("Xit?"); }

/* prompt for the Ultima I diskette and wait for a space */
F_125C()
{
    F_3EED();
    F_352C(1, 0x17);
    F_3599("Insert Ultima I diskette", D_2046);
    F_352C(1, 0x18);
    F_3599("Hit space to continue...", D_2046);
    while (F_39C5(D_2046) != 0x20)
        ;
    F_352C(1, 0x17);
    F_3599("                        ", D_2046);
    F_352C(1, 0x18);
    F_3599("                        ", D_2046);
    F_352C(1, 0x17);
}
