/* opts: -ms -v -w -cs */
/*
 * OUT.EXE game code 5469-6823: dungeon ladders, chests, thief/steal, shops, tavern, story pages.
 */

/* Origin library */
extern F_352C(), F_357C(), F_3599(), F_35CA(), F_3B46(), F_3C8B(), F_3E85();
extern F_3BF6(), F_3C46(), F_44A2(), F_4437(), F_4558(), F_4761();
extern int F_353A(), F_3C8E(), F_3CBB(), F_4069(), F_40A8(), F_40FE();
extern int F_A363(), F_A3BA();
extern unsigned char F_413C(), F_39C5(), F_3862(), F_33BB();
/* other game code */
extern F_0476(), F_0761(), F_2141(), F_21E7();
extern F_5D34(), F_5BFA(), F_5CBA();
extern int F_2355(), F_30C0(), F_32F8();
/* Lattice long arithmetic */
extern CXC33(), CXD33();

struct cell { int a, b, c, d; };

extern char *D_0180;
extern int D_2046, D_204A, D_206A, D_206C, D_206E, D_207C, D_2080, D_2082;
extern int D_3A94, D_3A96, D_3A98, D_3A9A, D_3AAC, D_3AAE, D_3AB0;
extern int D_3B22, D_8BA4, D_8BA6;
extern long D_3B24, D_8BA8;
extern int D_AD3E, D_AD40, D_AD42, D_AD46, D_AD48, D_AD50, D_AD52, D_AD54;
extern int D_20AC[], D_20B8[];
extern char D_B11E[], D_2058[];
extern unsigned char D_8BAE[][38][18];
extern struct cell D_AD56[][11];
extern char *D_0F5C[];
extern unsigned char *D_0D72[];

/* place-name strings by location code (DATA 2B6C-3125; no entry D_307C in out.syms) */
static char *D_307C[] = {
    "?", "Britian", "Moon",
    "Fawn", "Paws", "Montor",
    "Yew", "Tune", "Grey",
    "Arnold", "Linda", "Helen",
    "Owen", "John", "Gerry",
    "Wolf", "The Snake", "Nassau",
    "Clear Lagoon", "Stout", "Gauntlet",
    "Imagination", "Ponder", "Wealth",
    "Poor", "Gorlab", "Dextron",
    "Magic", "Wheeler", "Bulldozer",
    "The Brother", "Turtle", "Lost Friends",
    "The Castle of Lord British", "The Castle of the Lost King", "The Castle Barataria",
    "The Castle Rondorin", "The Castle of Olympus", "The Black Dragon's Castle",
    "The White Dragon's Castle", "The Castle of Shamino", "The Pillars of Protection",
    "The Tower of Knowledge", "The Pillars of the Argonauts", "The Pillar of Ozymandias",
    "The Sign Post", "The Southern Sign Post", "The Eastern Sign Post",
    "The Grave of the Lost Soul", "The Unholy Hole", "The Dungeon of Perinia",
    "The Dungeon of Montor", "The Mines of Mt. Drash", "Mondain's Gate to Hell",
    "The Lost Caverns", "The Dungeon of Doubt", "The Mines of Mt. Drash II",
    "Death's Awakening", "The Savage Place", "Scorpion Hole",
    "Advari's Hole", "The Dead Warrior's Fight", "The Horror of the Harpies",
    "The Labyrinth", "Where Hercules Died", "The Horror of the Harpies II",
    "The Gorgon Hole", "The Tramp of Doom", "The Viper's Pit",
    "The Long Death", "The End...", "The Viper's Pit II",
    "The Slow Death", "The Guild of Death", "The Metal Twister",
    "The Troll's Hole", "The Skull Smasher", "The Spine Breaker",
    "The Dungeon of Doom", "The Dead Cat's Life", "The Morbid Adventure",
    "Free Death Hole", "Dead Man's Walk", "The Dead Cat's Life II",
    "The Hole to Hades"
};
/* land names (DATA 3126-3173, table at 3174) */
static char *D_3174[] = {
    "of Lord British", "of the Feudal Lords", "of the Dark Unknown", "of Danger and Despair"
};

/* "not the king's permission" message and sound */
F_5469()
{
    F_3B46("");
    F_3B46("Thou hast not the king's");
    F_3B46("permission!");
    F_0476(1);
}

/* shop counter type 0x37: needs a turn counter, then F_5D34 (armour) */
F_5492()
{
    if (D_8BA4 == 0)
        F_5469();
    else {
        D_8BA4--;
        F_5D34(0);
    }
}

/* shop counter type 0x3B: F_5BFA (weapons) */
F_54B5()
{
    if (D_8BA4 == 0)
        F_5469();
    else {
        D_8BA4--;
        F_5BFA(0);
    }
}

/* shop counter type 0x39: F_5CBA (food) */
F_54D8()
{
    if (D_8BA4 == 0)
        F_5469();
    else {
        D_8BA4--;
        F_5CBA(0);
    }
}

/* dispatch on the counter tile the player stands on */
F_54FB()
{
    int c;

    c = D_8BAE[D_AD42][D_AD3E][D_AD40];
    switch (c) {
    case 0x39:
        F_54D8();
        break;
    case 0x37:
        F_5492();
        break;
    case 0x3B:
        F_54B5();
        break;
    default:
        F_3B46(" - nothing here!");
    }
}

/* Get command */
F_5550()
{
    F_3599("Get", D_2046);
    if (D_204A == 2)
        F_54FB();
    else {
        if (D_204A == 1)
            F_3599(" what", D_2046);
        F_3B46("?");
        F_0476(1);
    }
}

/* HyperJump? message */
F_559A()
{
    F_3B46("HyperJump?");
    F_0476(1);
}

/* dungeon: look at the cell in the facing direction (secret door -> ladder) */
F_55B1()
{
    int a, dx, dy;

    dx = F_3C8E();
    dy = F_3CBB();
    a = D_AD56[D_AD3E + dx][D_AD40 + dy].a;
    F_3599("Thou dost find ", D_2046);
    if (a == 2 && D_AD56[D_AD3E][D_AD40].a != 3) {
        F_3B46("a secret door!");
        D_AD56[D_AD3E + dx][D_AD40 + dy].a = 3;
    } else
        F_3B46("nothing");
}

/* Inform command: names the place you are on or looking at */
F_564B()
{
    int m;

    F_3B46("Inform and search");
    if (D_204A == 3)
        F_55B1();
    else {
        m = F_A3BA(D_3AAC, D_3AAE);
        if (m) {
            if (m < 33)
                F_3599("the city of ", D_2046);
            F_3B46(D_307C[m]);
        } else {
            m = F_A363(D_3AAC, D_3AAE);
            if (m == 0)
                F_3B46("You are at sea");
            else if (m == 2)
                F_3B46("You are in the woods");
            else {
                F_3B46("You are in the lands");
                F_3B46(D_3174[F_40FE(D_3AAC, D_3AAE)]);
            }
        }
    }
}

/* Climb up a dungeon ladder: out of the dungeon on level 0 (hit point bonus) */
F_56F7()
{
    D_AD50--;
    F_3599(" up to level ", D_2046);
    F_3C8B(D_AD50, 1, ' ', D_2046, 1);
    F_3B46("");
    if (D_AD50 == 0) {
        if (D_AD54 + D_206E > 9999)
            D_AD54 = 9999 - D_206E;
        if (D_AD54) {
            F_3599("Thou dost gain ", D_2046);
            F_3C8B(D_AD54, 1, ' ', D_2046, 1);
            F_3599(" hit ", D_2046);
            if (D_AD54 > 999)
                F_3B46("");
            F_3B46("points");
            D_206E += D_AD54;
            F_4437();
        }
        D_204A = 0;
        F_3C46();
        F_3BF6();
    } else {
        F_4761();
        D_AD56[D_AD3E][D_AD40].b = -1;
    }
}

/* Climb down a dungeon ladder */
F_57DF()
{
    D_AD50++;
    F_3599(" down to level ", D_2046);
    F_3C8B(D_AD50, 1, ' ', D_2046, 1);
    F_3B46("");
    F_4761();
}

/* K-Limb command (dungeon ladders only) */
F_581A()
{
    unsigned char c;

    F_3599("K-Limb", D_2046);
    if (D_204A != 3) {
        F_3B46("?");
        F_0476(1);
    } else {
        c = D_AD56[D_AD3E][D_AD40].a;
        if (c != 7 && c != 6) {
            F_3B46(" what?");
            F_0476(1);
        } else if (D_AD52 == 2 || D_AD52 == 1) {
            F_3B46("");
            F_3B46("Thou must face the ladder!");
            F_0476(1);
        } else if (c == 7)
            F_56F7();
        else
            F_57DF();
    }
}

/* Noise command: toggle sound */
F_58BD()
{
    F_3599("Noise ", D_2046);
    if (D_3AB0 = !D_3AB0)
        F_3B46("on");
    else
        F_3B46("off");
}

/* put a chest-kept monster/treasure number at (x,y) in the dungeon level */
F_58F8(x, y)
int x, y;
{
    int t, r, n;

    n = 0;
    t = D_AD56[x][y].a;
    if (t == 0 || t == 7 || t == 6) {
        if (D_AD56[x][y].b == -1) {
            do {
                t = F_4069(0, 49) / 10;
            } while (D_B11E[t] && ++n < 50);
            D_B11E[t] = 1;
            r = (D_AD50 - 1) / 2 * 5 + t;
            D_AD56[x][y].b = r;
            D_AD56[x][y].d = F_4069(1, D_AD50 * D_AD50 + 1) + 10 + r;
        }
    }
}

/* Open coffin command (dungeon): monster or pence */
F_59D7()
{
    int x, y;

    x = F_3C8E() + D_AD3E;
    y = F_3CBB() + D_AD40;
    F_3599("Open", D_2046);
    if (D_204A != 3) {
        F_3B46("?");
        F_0476(1);
    } else {
        F_3599(" coffin", D_2046);
        if (D_AD56[D_AD3E][D_AD40].c != 5) {
            F_3B46(" - none here!");
            F_0476(1);
        } else {
            F_3B46("");
            if (F_4069(1, 255) < 104 && D_AD56[x][y].a != 1 && D_AD56[x][y].a != 2 && D_AD56[x][y].b == -1)
                F_58F8(x, y);
            else {
                D_AD56[D_AD3E][D_AD40].c = 0;
                F_3599("Thou dost find: ", D_2046);
                F_0761(F_4069(3, D_AD50 * D_AD50 + 9), 0);
            }
        }
    }
}

/* save the game to the player file */
F_5B0E()
{
    F_3599("saving game...", D_2046);
    D_3B22 = D_8BA6;
    D_3B24 = D_8BA8;
    F_3E85(D_0180, (int)D_2058, 820);
    F_3B46("saved.");
}

/* Quit command (outdoors only) */
F_5B54()
{
    F_3599("Quit - ", D_2046);
    if (D_204A != 0) {
        F_3B46("only allowed outdoors!");
        F_0476(1);
    } else
        F_5B0E();
}

/* did the thief get caught? (returns 1 if not caught) */
F_5B8A()
{
    int r, flag;

    flag = 0;
    r = F_4069(1, 255);
    if (D_AD46 || r < 38)
        flag = 1;
    if (!flag && D_206A == 3)
        return 0;
    if (!flag && r > 77)
        return 0;
    F_3B46("");
    F_3B46("Oh no!  Thou wert caught!");
    return D_AD46 = 1;
}

/* steal a weapon (arg: check for being caught) */
F_5BFA(chk)
int chk;
{
    int i, r;

    r = 0;
    if (chk)
        r = F_5B8A();
    if (r == 0) {
        F_3B46("");
        F_3599("Thou dost find a", D_2046);
        i = F_4069(1, 15);
        if (F_40A8(D_0D72[i][0]))
            F_357C('n', D_2046);
        if (F_353A(D_0D72[i]) > 8)
            F_3B46("");
        else
            F_357C(' ', D_2046);
        F_3B46(D_0D72[i]);
        if (D_20B8[i] < 255)
            D_20B8[i]++;
    }
}

/* steal food */
F_5CBA(chk)
int chk;
{
    int n, r;

    r = 0;
    if (chk)
        r = F_5B8A();
    if (r == 0) {
        F_3B46("");
        F_3599("Thou dost find ", D_2046);
        n = F_4069(2, 31);
        F_3C8B(n, 1, ' ', D_2046, 1);
        F_3B46(" bags of");
        F_3B46("food!");
        D_2080 += n;
        F_44A2();
    }
}

/* steal armour */
F_5D34(chk)
int chk;
{
    int i, r;

    r = 0;
    if (chk)
        r = F_5B8A();
    if (r == 0) {
        F_3B46("");
        F_3599("Thou dost find ", D_2046);
        i = F_4069(1, 5);
        if (i > 4)
            F_3599("a ", D_2046);
        F_3B46(D_0F5C[i]);
        if (D_20AC[i] < 255)
            D_20AC[i]++;
    }
}

/* Steal command */
F_5DB1()
{
    int c;

    F_3599("Steal", D_2046);
    if (D_204A != 1 && D_204A != 2) {
        F_3B46("?");
        F_0476(1);
    } else {
        c = D_8BAE[D_AD42][D_AD3E][D_AD40];
        switch (c) {
        case 0x37:
            F_5D34(1);
            break;
        case 0x3B:
            F_5BFA(1);
            break;
        case 0x39:
            F_5CBA(1);
            break;
        default:
            F_3B46(" - nothing here!");
            F_0476(1);
        }
    }
}

/* clear the shop message box */
F_5E4F()
{
    F_35CA(39, 48, 280, 120, 0);
}

/* "Sold!" message */
F_5E6C()
{
    F_5E4F();
    F_21E7(18, 9, "Sold!");
    F_2141();
}

/* "Done!" message */
F_5E88()
{
    F_5E4F();
    F_21E7(18, 9, "Done!");
    F_2141();
}

/* "Thou canst not afford it!" message */
F_5EA4()
{
    F_5E4F();
    F_21E7(8, 9, "Thou canst not afford it!");
    F_0476(1);
    F_2141();
}

/* story pages 0-8 (tavern seduction, advice from the king / castle people) */
F_5EC9(n)
int n;
{
    F_5E4F();
    if (n)
        F_21E7(11, 7, "Thou had best know");
    switch (n) {
    case 0:
        F_21E7(9, 8, "Thou hast been seduced!");
        F_21E7(11, 10, "After a long night,");
        F_21E7(13, 11, "thou art back.");
        break;
    case 1:
        F_21E7(10, 8, "about space travel!");
        F_21E7(10, 9, "Thou must destroy at");
        F_21E7(9, 10, "least 20 enemy vessels");
        F_21E7(11, 11, "to become an ace!");
        break;
    case 2:
        F_21E7(11, 8, "to watch the ");
        if (D_206C == 0)
            F_3599("wench.", D_2046);
        else
            F_3599("lecher.", D_2046);
        break;
    case 3:
        F_21E7(5, 8, " that the princess will give");
        F_21E7(5, 9, " great reward to the one who");
        F_21E7(5, 10, "rescues her, and an extra gift");
        F_21E7(10, 11, "to an 8th level ace!");
        break;
    case 4:
        F_21E7(7, 8, "thou must go back in time.");
        break;
    case 5:
        F_21E7(10, 8, "thou should destroy");
        F_21E7(13, 9, "the evil gem!");
        break;
    case 6:
        F_21E7(5, 8, "   that many lakes and ponds");
        F_21E7(5, 9, "  have strong magical powers!");
        break;
    case 7:
        F_21E7(10, 8, "this is a great game!");
        break;
    case 8:
        F_21E7(5, 8, "   that over 1000 years ago,");
        F_21E7(5, 9, "Mondain the Wizard created an");
        F_21E7(5, 10, " evil gem.  With this gem, he");
        F_21E7(5, 11, "   is immortal and cannot be");
        F_21E7(15, 12, "defeated.");
        while (!F_413C(0))
            ;
        F_5E4F();
        F_21E7(5, 7, "The quest of --Ultima-- is to");
        F_21E7(5, 8, " traverse the lands in search");
        F_21E7(8, 9, "of a time machine.  Upon");
        F_21E7(5, 10, " finding such a device, thou");
        F_21E7(5, 11, "should go back in time to the");
        F_21E7(5, 12, " days before Mondain created");
        F_21E7(5, 13, "the evil gem and destroy him!");
        while (!F_413C(0))
            ;
    }
    if (n != 8)
        F_2141();
}

/* is the cell at offset (dx,dy) from the player a type-50 entity? */
F_616F(dx, dy)
int dx, dy;
{
    return F_2355(D_AD3E + dx, D_AD40 + dy) == 50;
}

/* is a type-50 entity adjacent (8 directions)? */
F_619C()
{
    return F_616F(1, 1) || F_616F(1, 0) || F_616F(1, -1) || F_616F(0, 1) ||
           F_616F(0, -1) || F_616F(-1, 1) || F_616F(-1, 0) || F_616F(-1, -1);
}

/* tavern: order ale (arg 'B') or refuse */
F_622C(a)
char a;
{
    int r;

    r = F_32F8(D_AD42 - 2);
    if (a == 'B') {
        if (D_207C == 0) {
            F_21E7(12, r + 1, "Thou art broke!");
            F_21E7(8, r + 4, "Come back when thou hast");
            F_21E7(10, r + 5, "some money to spend.");
            F_2141();
            F_352C(15, 24);
            F_3599("nothing", D_2046);
        } else {
            D_207C--;
            F_4558();
            F_21E7(8, r + 1, "The tavern keeper sayeth:");
            F_21E7(9, r + 4, "Here, have a cold one!");
            F_352C(15, 24);
            F_3599("ale", D_2046);
            D_AD48++;
            F_2141();
            if (D_AD48 > D_3A94 / 4 && F_619C()) {
                F_5EC9(0);
                D_207C = D_207C / 2;
                F_4558();
                D_3A98--;
                if (D_3A98 == 4)
                    D_3A98++;
            }
            if (F_4069(1, 255) < 75)
                F_5EC9(F_4069(11, 89) / 10);
        }
    } else {
        F_21E7(8, r + 2, "We have plenty of booze");
        F_21E7(17, r + 3, "already!");
        F_2141();
        F_352C(16, 24);
        F_3599("nothing", D_2046);
    }
    F_3B46("");
}

/* armour/weapon buying price for item n (charisma D_3A9A) */
F_63B3(n)
int n;
{
    return (255 - D_3A9A) * n * n / 256 + 5;
}

/* selling price for item n (D_3A96) */
F_63D4(n)
int n;
{
    return (D_3A96 + 40) * n * n / 256 + 1;
}

/* weapon shop, selling side: list own weapons, sell one */
F_63F2(top)
int top;
{
    int i, n, y;
    unsigned char c, m;

    n = 0;
    y = 0;
    c = 'b';
    i = 1;
    while (i <= 15) {
        if (D_20B8[i])
            n++;
        i++;
    }
    if (n == 0) {
        F_21E7(5, top + 1, "Thou hast no weaponry to sell!");
        F_2141();
        F_352C(16, 24);
        F_3599("nothing", D_2046);
    } else {
        i = 1;
        do {
            if (D_20B8[i]) {
                F_352C(9, top + y);
                F_357C(c, D_2046);
                F_3599(") ", D_2046);
                F_3599(D_0D72[i], D_2046);
                F_352C(26, top + y);
                F_3599("- ", D_2046);
                F_3C8B(F_63D4(i), 3, ' ', D_2046, 1);
                y++;
            }
            c++;
            i++;
        } while (i <= 15 && y < 8);
        m = F_3862(c) - 1;
        F_352C(16, 24);
        c = F_3862(F_39C5(D_2046));
        if (c < 'B' || c > m)
            F_3599("nothing", D_2046);
        else if (D_20B8[c - 'A'] == 0)
            F_3599("nothing", D_2046);
        else {
            D_207C += F_63D4(c - 'A');
            D_20B8[c - 'A']--;
            if (D_2082 == c - 'A')
                D_2082 = 0;
            F_3599(D_0D72[c - 'A'], D_2046);
            F_4558();
            F_5E88();
        }
    }
}

/* print one line of the weapons-for-sale list: letter, name, price */
F_65CE(top, i, cnt)
int top, i, cnt;
{
    unsigned char c;

    c = i + 'a';
    F_352C(9, top + cnt);
    F_357C(c, D_2046);
    F_3599(") ", D_2046);
    F_3599(D_0D72[i], D_2046);
    F_352C(26, top + cnt);
    F_3599("- ", D_2046);
    F_3C8B(F_63B3(i), 3, ' ', D_2046, 1);
}

/* weapon shop: sell (arg 'S') or buy a weapon */
F_665D(a)
char a;
{
    int r, n;
    unsigned char c, lo, hi;
    int cnt;

    cnt = 0;
    r = F_30C0(D_AD42 - 2);
    if (a == 'S')
        F_63F2(r);
    else {
        lo = ((D_AD42 - 1) & 1) + 'b';
        n = D_8BA8 % 0x7FFFL / 1500L;
        if (n > 3 || D_8BA8 > 3000L)
            n = 3;
        n++;
        hi = lo + n * 2;
        n = lo;
        while (hi >= n) {
            F_65CE(r, n - 'a', cnt);
            cnt++;
            n += 2;
        }
        F_352C(15, 24);
        c = F_33BB(F_39C5(D_2046));
        if (c < lo || c > hi)
            F_3599("nothing", D_2046);
        else if (((c - 'a') & 1) != (D_AD42 & 1))
            F_3599("nothing", D_2046);
        else if (F_63B3(c - 'a') > D_207C) {
            F_3599("nothing", D_2046);
            F_5EA4();
        } else {
            D_207C -= F_63B3(c - 'a');
            if (D_20B8[c - 'a'] < 255)
                D_20B8[c - 'a']++;
            F_3599(D_0D72[c - 'a'], D_2046);
            F_4558();
            F_5E6C();
        }
    }
    F_3B46("");
}

/* quarter of (200 - D_3A9A) times n */
F_67F1(n)
int n;
{
    return (200 - D_3A9A) / 4 * n;
}

/* quarter of D_3A96 times n */
F_680C(n)
int n;
{
    return D_3A96 / 4 * n;
}
