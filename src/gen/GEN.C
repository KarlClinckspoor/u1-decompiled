/* opts: -ms -v -w -cs */
/*
 * GEN.EXE - Ultima I character generation (Origin Systems, PC conversion by John Fachini).
 * Decompiled; builds byte-identical with Lattice C 3.10 (lc -ms -v -w -cs).
 *
 * Names are still the addresses they had in the original (F_xxxx code, D_xxxx DGROUP data).
 */
#include <stdio.h>

/* Origin support routines (ORIGIN.ASM) and their data */
extern int D_082C, D_082E, D_0830;
extern int F_34BF();
extern F_3547(), F_4115(), F_40EE(), F_4071(), F_3EC4(), F_3EED(), F_3AD7();
extern F_35CA(), F_39FC(), F_3D41(), F_352C(), F_3599(), F_3C8B(), F_357C();
extern F_387E(), F_3920(), F_3F62();
extern unsigned char F_39C5(), F_3862();
/* Lattice library */
extern F_1D0D();                        /* ONERROR */

/*
 * Initialised data (DGROUP 0124-01B2)
 */
char *D_013C[] = { "Human", "Elf", "Dwarf", "Bobbit" };        /* race names */
char *D_0160[] = { "Fighter", "Cleric", "Wizard", "Thief" };   /* class names */
char *D_0194[] = { "player1.u1", "player2.u1", "player3.u1", "player4.u1" };
char *D_01A0 = "out";                   /* the game, run with execl */
char *D_01A4 = "C";                     /* its arguments: video mode (C, E, T) */
unsigned char *D_01A8 = "0";            /*   and character number */
int D_01AA[4] = { 0, 0, 0, 0 };         /* which player files exist */

/*
 * The character record, written to playerN.u1 in two blocks (DGROUP 0A30-0DB0)
 */
char D_0A30[16];                        /* name */
int D_0A40;                             /* race */
int D_0A42;                             /* class */
int D_0A44;                             /* sex */
int D_0A46[7];                          /* hit points, then Str Agi Sta Cha Wis Int */
int D_0A54, D_0A56, D_0A58, D_0A5A, D_0A5C, D_0A5E, D_0A60, D_0A62;
int D_0A64, D_0A66, D_0A68;
int D_0A6A[9];
int D_0A7C[4];
int D_0A84[6];
int D_0A90[16];
int D_0AB0[11];
int D_0AC6[8];
int D_0AD6, D_0AD8, D_0ADA;
long D_0ADC;
int D_0AE0[2];
int D_0AE4, D_0AE6, D_0AE8, D_0AEA;     /* second block: 0x280 bytes from D_0AE4 */
int D_0AEC[316];
char D_0D64[4][15];                     /* names of the saved characters */
int D_0DA0;                             /* points left to distribute */
int D_0DA2;
int D_0DA4;                             /* number of saved characters */
int D_0DA6, D_0DA8, D_0DAA, D_0DAC;     /* colours */
int D_0DAE;                             /* forced CGA (command line "C") */

/* pick colours for the machine type (F_34BF: BIOS model byte) and, if setmode,
   switch to the graphics mode in D_082C (0 CGA, 1 EGA, 2 Tandy/PCjr) */
F_03E9(setmode)
int setmode;
{
    switch (F_34BF()) {
    case 0:
        D_0DA6 = 3;
        D_0DA8 = 2;
        D_0DAA = 1;
        D_0DAC = 3;
        D_082E = 6;
        break;
    case 1:
    case 2:
        D_0DA6 = 15;
        D_0DA8 = 1;
        D_0DAA = 12;
        D_0DAC = 11;
        D_082E = 7;
        break;
    }
    if (D_0DAE) {
        D_082C = 0;
        D_0DA6 = 3;
        D_0DA8 = 2;
        D_0DAA = 1;
    }
    F_4071();
    if (setmode) {
        if (D_082C == 0)
            F_3547();
        else if (D_082C == 1)
            F_4115();
        else if (D_082C == 2)
            F_40EE();
    }
}

/* rounded corners of the border */
F_048A()
{
    int i;

    for (i = 1; i <= 4; i++) {
        F_39FC(i, 0, 0, i, 0);
        F_39FC(0x13f - i, 0, 0x13f, i, 0);
        F_39FC(0, 0xc7 - i, i, 0xc7, 0);
        F_39FC(0x13f, 0xc7 - i, 0x13f - i, 0xc7, 0);
    }
    F_3D41(0, 0, 0);
    F_3D41(0, 5, 0);
    F_3D41(5, 0, 0);
    F_3D41(0x13f, 0, 0);
    F_3D41(0x13a, 0, 0);
    F_3D41(0x13f, 5, 0);
    F_3D41(0, 0xc7, 0);
    F_3D41(0, 0xc2, 0);
    F_3D41(5, 0xc7, 0);
    F_3D41(0x13f, 0xc7, 0);
    F_3D41(0x13f, 0xc2, 0);
    F_3D41(0x13a, 0xc7, 0);
}

/* draw the screen border */
F_05B2()
{
    F_35CA(0, 0, 0x13c, 6, D_0DA8);
    F_35CA(0, 6, 6, 0xc7, D_0DA8);
    F_35CA(0x139, 2, 0x13f, 0xc7, D_0DA8);
    F_35CA(0, 0xc1, 0x13f, 0xc7, D_0DA8);
    F_39FC(7, 7, 0x138, 7, D_0DA6);
    F_39FC(7, 7, 7, 0xc0, D_0DA6);
    F_39FC(0x138, 7, 0x138, 0xc0, D_0DA6);
    F_39FC(7, 0xc0, 0x138, 0xc0, D_0DA6);
    F_048A();
}

/* clear the inside of the frame and draw the title bar */
F_0670()
{
    F_35CA(8, 8, 0x137, 0xbf, 0);
    F_35CA(0x1e, 0, 0x12c, 6, D_0DA8);
    F_39FC(0x1e, 7, 0x12c, 7, D_0DA6);
}

/* main menu: generate new character / continue previous game / quit */
unsigned char F_06BB()
{
    unsigned char c;

    F_352C(12, 6);
    F_3599("*** Ultima I ***", D_0DA6);
    F_352C(14, 9);
    F_3599("from darkest", D_0DA6);
    F_352C(14, 10);
    F_3599("dungeons, to", D_0DA6);
    F_352C(13, 11);
    F_3599("deepest space!", D_0DA6);
    F_352C(8, 14);
    F_3599("a) Generate new character", D_0DAC);
    F_352C(8, 15);
    F_3599("b) Continue previous game", D_0DAC);
    F_352C(13, 18);
    F_3599("Thy choice: ", D_0DAC);
    do
        c = F_3862(F_39C5(D_0DAC));
    while (c != 'A' && c != 'B' && c != 'Q');
    return c;
}

/* print the value of attribute n */
F_07A8(n)
int n;
{
    F_352C(0x1c, n + 5);
    F_3C8B(D_0A46[n], 2, ' ', D_0DAC, 0);
}

/* choose the race; adjusts attributes, returns 0..3 */
F_07E2()
{
    int done, race;

    F_35CA(0x30, 0x20, 0x12c, 0x28, 0);
    F_35CA(0xe, 0x80, 0x12e, 0xb0, 0);
    F_352C(3, 0x11);
    F_3599("Select thy race: ", D_0DAC);
    F_352C(12, 0x13);
    F_3599("a) Human", D_0DAC);
    F_352C(12, 0x14);
    F_3599("b) Elf", D_0DAC);
    F_352C(12, 0x15);
    F_3599("c) Dwarf", D_0DAC);
    F_352C(12, 0x16);
    F_3599("d) Bobbit", D_0DAC);
    F_352C(0x14, 0x11);
    do {
        switch (F_3862(F_39C5(D_0DAC))) {
        case 'A':
            race = 0;
            D_0A46[6] += 5;
            F_07A8(6);
            done = 1;
            break;
        case 'B':
            race = 1;
            D_0A46[2] += 5;
            F_07A8(2);
            done = 1;
            break;
        case 'C':
            race = 2;
            D_0A46[1] += 5;
            F_07A8(1);
            done = 1;
            break;
        case 'D':
            race = 3;
            D_0A46[5] += 10;
            D_0A46[1] -= 5;
            F_07A8(5);
            F_07A8(1);
            done = 1;
            break;
        default:
            done = 0;
        }
    } while (!done);
    F_352C(0xe, 0xd);
    F_3599("Race: ", D_0DAC);
    F_3599(D_013C[race], D_0DAC);
    return race;
}

/* choose the sex, returns 0 male, 1 female */
F_0995()
{
    int done, sex;

    F_35CA(0x18, 0x91, 0x12c, 0xb8, 0);
    F_352C(3, 0x11);
    F_3599("Select thy sex: ", D_0DAC);
    F_352C(12, 0x13);
    F_3599("a) Male", D_0DAC);
    F_352C(12, 0x14);
    F_3599("b) Female", D_0DAC);
    F_352C(0x13, 0x11);
    do {
        switch (F_3862(F_39C5(D_0DAC))) {
        case 'A':
            sex = 0;
            done = 1;
            break;
        case 'B':
            sex = 1;
            done = 1;
            break;
        default:
            done = 0;
        }
    } while (!done);
    F_352C(0xf, 0xe);
    F_3599("Sex: ", D_0DAC);
    if (sex == 0)
        F_3599("Male", D_0DAC);
    else
        F_3599("Female", D_0DAC);
    return sex;
}

/* choose the class; adjusts attributes, returns 0..3 */
F_0A91()
{
    int done, cls;

    F_35CA(0x18, 0x91, 0x12c, 0xb8, 0);
    F_352C(3, 0x11);
    F_3599("Select thy class: ", D_0DAC);
    F_352C(12, 0x13);
    F_3599("a) Fighter", D_0DAC);
    F_352C(12, 0x14);
    F_3599("b) Cleric", D_0DAC);
    F_352C(12, 0x15);
    F_3599("c) Wizard", D_0DAC);
    F_352C(12, 0x16);
    F_3599("d) Thief", D_0DAC);
    F_352C(0x15, 0x11);
    do {
        switch (F_3862(F_39C5(D_0DAC))) {
        case 'A':
            cls = 0;
            D_0A46[1] += 10;
            D_0A46[2] += 10;
            F_07A8(1);
            F_07A8(2);
            done = 1;
            break;
        case 'B':
            cls = 1;
            D_0A46[5] += 10;
            F_07A8(5);
            done = 1;
            break;
        case 'C':
            cls = 2;
            D_0A46[6] += 10;
            F_07A8(6);
            done = 1;
            break;
        case 'D':
            cls = 3;
            D_0A46[2] += 10;
            F_07A8(2);
            done = 1;
            break;
        default:
            done = 0;
        }
    } while (!done);
    F_352C(0xd, 0xf);
    F_3599("Class: ", D_0DAC);
    F_3599(D_0160[cls], D_0DAC);
    return cls;
}

/* read the character's name into D_0A30 */
F_0C2C()
{
    int done, len;
    unsigned char c;

    len = done = 0;
    F_35CA(0x18, 0x80, 0x12c, 0xb8, 0);
    F_352C(3, 0x11);
    F_3599("Enter thy name: ", D_0DAC);
    do {
        c = F_39C5(D_0DAC);
        if (c > ' ' && len < 14 || c == ' ' && len != 0 && len < 14) {
            D_0A30[len] = c;
            len++;
            F_357C(c, D_0DAC);
        } else if ((c == 1 || c == 8) && len != 0) {
            len--;
            F_352C(len + 0x13, 0x11);
        } else if (c == '\r' && len != 0) {
            D_0A30[len] = 0;
            done = 1;
        }
    } while (!done);
    F_352C(12, 4);
    F_3599(D_0A30, D_0DAC);
}

/* title bar text */
F_0D18()
{
    F_387E(8, 0);
    F_3599(" Character Generation ", D_0DA6);
    F_3920(0x1f, 0);
}

/* graphics driver call 1 (dispatch table in F_3F62) */
F_0D42()
{
    F_3F62(1);
}

/* ask whether to save the character: 1 yes, 0 no */
F_0D50()
{
    unsigned char c;

    F_35CA(0x18, 0x80, 0x12c, 0xb8, 0);
    F_352C(3, 0x16);
    F_3599("Save this character? (Y-N) ", D_0DAC);
    for (;;) {
        c = F_3862(F_39C5(D_0DAC));
        if (c == 'Y') {
            F_3599("Yes", D_0DAC);
            return 1;
        }
        if (c == 'N')
            return 0;
    }
}

/* erase the cursor marks around attribute line n */
F_0DC2(n)
int n;
{
    F_352C(0xb, n + 5);
    F_3599(" ", D_0DAC);
    F_352C(0x1e, n + 5);
    F_3599(" ", D_0DAC);
}

/* draw the cursor marks around attribute line n */
F_0E07(n)
int n;
{
    F_387E(0xb, n + 5);
    F_3920(0x1e, n + 5);
}

/* decrease attribute n (not below 10); 1 if done */
F_0E32(n)
int n;
{
    if (D_0A46[n] > 10) {
        D_0A46[n]--;
        F_07A8(n);
        return 1;
    }
    return 0;
}

/* increase attribute n (not above 25, needs points left); 1 if done */
F_0E61(n)
int n;
{
    if (D_0A46[n] < 25 && D_0DA0) {
        D_0A46[n]++;
        F_07A8(n);
        return 1;
    }
    return 0;
}

/* move the cursor up one attribute (wraps) */
F_0E9E(n)
int n;
{
    F_0DC2(n);
    n--;
    if (n < 1)
        n = 6;
    F_0E07(n);
    return n;
}

/* move the cursor down one attribute (wraps) */
F_0ECA(n)
int n;
{
    F_0DC2(n);
    n++;
    if (n > 6)
        n = 1;
    F_0E07(n);
    return n;
}

/* attribute screen: labels, help text, all attributes to 10 */
F_0EF6()
{
    int i;

    F_35CA(10, 10, 0x136, 0xb9, 0);
    F_352C(6, 4);
    F_3599("Points left to distribute:", D_0DAC);
    F_352C(12, 6);
    F_3599("Strength........10", D_0DAC);
    F_352C(12, 7);
    F_3599("Agility.........10", D_0DAC);
    F_352C(12, 8);
    F_3599("Stamina.........10", D_0DAC);
    F_352C(12, 9);
    F_3599("Charisma........10", D_0DAC);
    F_352C(12, 10);
    F_3599("Wisdom..........10", D_0DAC);
    F_352C(12, 11);
    F_3599("Intelligence....10", D_0DAC);
    F_352C(2, 0x10);
    F_3599("Move cursor with up and down arrows;", D_0DAC);
    F_352C(2, 0x11);
    F_3599("increase and decrease attributes", D_0DAC);
    F_352C(2, 0x12);
    F_3599("with left and right arrows.  Press", D_0DAC);
    F_352C(2, 0x13);
    F_3599("space bar when finished, or escape", D_0DAC);
    F_352C(2, 0x14);
    F_3599("to return to the main menu.", D_0DAC);
    for (i = 1; i <= 6; i++)
        D_0A46[i] = 10;
}

/* Character generation screen: build a character and save it to slot */
F_106A(slot)
int slot;
{
    int sel, done, esc, j, n;
    unsigned char c;
    FILE *fp;

    do {
        F_0D18();
        F_0EF6();
        F_0E07(sel = 1);
        D_0DA0 = 30;
        done = esc = 0;
        do {
            F_352C(32, 4);
            F_3C8B(D_0DA0, 3, 32, D_0DAC, 0);
            switch (F_39C5(0)) {
            case 1:
                if (F_0E32(sel))
                    D_0DA0++;
                break;
            case 2:
                if (F_0E61(sel))
                    D_0DA0--;
                break;
            case 3:
                sel = F_0E9E(sel);
                break;
            case 4:
                sel = F_0ECA(sel);
                break;
            case 0x1b:
                esc = 1;
                break;
            case ' ':
                if (!(done = (D_0DA0 == 0)))
                    F_0D42();
                break;
            }
        } while (!done && !esc);
        if (esc)
            break;
        F_0DC2(sel);
        D_0A40 = F_07E2();
        D_0A44 = F_0995();
        D_0A42 = F_0A91();
        F_0C2C();
        if (done = F_0D50()) {
            D_0A62 = F_3EC4() + D_0A46[5] + D_0A42;
            D_0A46[0] = 150;
            D_0A54 = 100;
            D_0A56 = 0;
            D_0A58 = 200;
            D_0A5A = 1;
            D_0A5C = 0;
            D_0A5E = 1;
            D_0A60 = 0;
            D_0A64 = 49;
            D_0A66 = 40;
            D_0A68 = 1;
            for (sel = 1; sel <= 8; sel++)
                D_0A6A[sel] = 0;
            for (sel = 0; sel <= 3; sel++)
                D_0A7C[sel] = 0;
            D_0AB0[0] = -1;
            for (sel = 1; sel <= 10; sel++)
                D_0AB0[sel] = 0;
            D_0A84[0] = -1;
            D_0A84[1] = 1;
            for (sel = 2; sel <= 5; sel++)
                D_0A84[sel] = 0;
            D_0A90[0] = -1;
            D_0A90[1] = 2;
            for (sel = 2; sel <= 15; sel++)
                D_0A90[sel] = 0;
            D_0AC6[0] = -1;
            for (sel = 1; sel <= 7; sel++)
                D_0AC6[sel] = 0;
            D_0AD6 = 0;
            D_0ADA = 0;
            D_0ADC = 0;
            D_0AD8 = -1;
            D_0AE4 = 10;
            D_0AE6 = 1;
            D_0AE8 = D_0A64;
            D_0AEA = D_0A66;
            if ((fp = fopen(D_0194[slot], "wb")) != NULL) {
                n = fwrite((char *)&D_0A30, 0xb4, 1, fp);
                n = fwrite((char *)&D_0AE4, 0x280, 1, fp);
                fclose(fp);
                D_01AA[slot] = 1;
                D_0DA4++;
                for (j = 0; j <= 14; j++)
                    D_0D64[slot][j] = D_0A30[j];
            } else {
                F_352C(7, 18);
                F_3599("Error writing out character", D_0DAA);
                F_352C(14, 19);
                F_3599("Press RETURN", D_0DAA);
                do {
                    c = F_39C5(D_0DAA);
                } while (c != '\r');
            }
        }
    } while (!done);
}

/* Load the names of the saved characters (player1..4.u1) */
F_13B2()
{
    int i, n;
    FILE *fp;

    D_0DA4 = 0;
    for (i = 0; i <= 3; i++) {
        if ((fp = fopen(D_0194[i], "rb")) != NULL) {
            n = fread(D_0D64[i], 15, 1, fp);
            fclose(fp);
            D_01AA[i] = 1;
            D_0DA4++;
        }
    }
}

/* Return the first free roster slot, or -1 */
F_1423()
{
    int i;

    i = 0;
    do {
        if (D_01AA[i] == 0)
            return i;
        i++;
    } while (i < 4);
    return -1;
}

/* Print the roster: "n) name" for the four slots */
F_1455()
{
    int i;

    for (i = 0; i <= 3; i++) {
        F_352C(15, i + 6);
        F_3C8B(i + 1, 1, 32, D_0DA6, 1);
        F_3599(") ", D_0DA6);
        F_3599(D_0D64[i], D_0DA6);
    }
}

/* Roster full: ask which character to replace; returns slot or -1 */
F_14BB()
{
    unsigned char c;

    F_35CA(10, 10, 300, 180, 0);
    F_387E(8, 0);
    F_3599(" Character Replacement ", D_0DA6);
    F_3920(32, 0);
    F_1455();
    F_352C(4, 14);
    F_3599("The roster is full.  Type the", D_0DAC);
    F_352C(4, 15);
    F_3599("number of the character to wish", D_0DAC);
    F_352C(4, 16);
    F_3599("to replace with a new one.", D_0DAC);
    F_352C(4, 17);
    F_3599("Press ESCAPE to return to the", D_0DAC);
    F_352C(4, 18);
    F_3599("main menu...", D_0DAC);
    do {
        c = F_39C5(D_0DAC);
        if (c >= '1' && c < '5')
            return c - '1';
    } while (c != 0x1b);
    return -1;
}

/* Ask for the Ultima I diskette */
F_15B3()
{
    F_3EED();
    F_352C(1, 23);
    F_3599("Insert Ultima I diskette", D_0DAC);
    F_352C(1, 24);
    F_3599("Hit space to continue...", D_0DAC);
    while (F_39C5(D_0DAC) != ' ')
        ;
    F_352C(1, 23);
    F_3599("                        ", D_0DAC);
    F_352C(1, 24);
    F_3599("                        ", D_0DAC);
    F_352C(1, 23);
}

/* Run the game (OUT.EXE) with video mode and character number */
F_1643(n)
int n;
{
    if (D_082C == 1)
        *D_01A4 = 'E';
    else if (D_082C == 2)
        *D_01A4 = 'T';
    {
        register unsigned char *num;    /* the character number argument */

        num = D_01A8;
        *num += n;
    }
    F_35CA(0, 0, 319, 199, 0);
    F_3AD7();
    F_352C(11, 8);
    F_3599("Please wait whilst", D_0DA6);
    F_352C(13, 10);
    F_3599("thy game loads", D_0DA6);
    for (;;) {
        int rc;

        rc = execl(D_01A0, D_01A0, D_01A4, D_01A8, 0);
        F_15B3();
    }
}

/* Continue previous game: pick a saved character and start the game */
F_16DC()
{
    unsigned char c;

    F_35CA(10, 10, 309, 189, 0);
    F_387E(9, 0);
    F_3599(" Character Selection ", D_0DA6);
    F_3920(31, 0);
    if (D_0DA4 == 0) {
        F_352C(4, 10);
        F_3599("There are no Ultima I characters", D_0DA6);
        F_352C(4, 11);
        F_3599("saved on this disk.  Press RETURN", D_0DA6);
        F_352C(4, 12);
        F_3599("to generate a character or ESCAPE", D_0DA6);
        F_352C(4, 13);
        F_3599("to return to the main menu...", D_0DA6);
        do {
            c = F_39C5(D_0DA6);
        } while (c != '\r' && c != 0x1b);
        if (c == '\r')
            F_106A(0);
    } else {
        F_1455();
        F_352C(4, 14);
        F_3599("Type number of character you wish", D_0DAC);
        F_352C(4, 15);
        F_3599("to use in Ultima I.  Press ESCAPE", D_0DAC);
        F_352C(4, 16);
        F_3599("to go to the main menu...", D_0DAC);
        do {
            c = F_39C5(D_0DAC);
            if (c >= '1' && c <= '4') {
                if (D_01AA[c - '1'])
                    F_1643(c - '0');
            }
        } while (c != 0x1b);
    }
}

/* main: main menu loop (a = generate new character, b = continue game) */
F_1853(argc, argv)
int argc;
unsigned char **argv;
{
    int slot;
    unsigned char c;

    F_1D0D(1);
    D_0DAE = F_3862(argv[1][0]) == 'C';
    F_13B2();
    F_03E9(0);
    F_35CA(0, 0, 319, 199, 0);
    D_0830 = 0;
    F_05B2();
    for (;;) {
        c = F_3862(F_06BB());
        switch (c) {
        case 'A':
            if ((slot = F_1423()) == -1) {
                slot = F_14BB();
                F_35CA(256, 0, 264, 6, D_0DA8);
                F_39FC(256, 7, 264, 7, D_0DA6);
            }
            if (slot != -1)
                F_106A(slot);
            F_0670();
            break;
        case 'B':
            F_16DC();
            F_0670();
            break;
        }
        if (c == 'A' || c == 'B')
            F_35CA(10, 10, 300, 190, 0);
    }
}
