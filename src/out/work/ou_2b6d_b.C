/* opts: -ms -v -w -cs */
/*
 * OUT.EXE game code 2E22-33B0 (work file, agent ou_2b6d): player move, shop signs.
 */
extern F_3B46(), F_B34F(), F_0476(), F_4BD6(), F_21C2(), F_21E7();
extern F_249D(), F_23E7();
extern int D_AD3E, D_AD40, D_204A, D_0DA8;
extern char *D_1DC8[];

/* step the player one square in direction dir (1-4): walk, leave the map, or bump */
F_2E22(dir)
unsigned char dir;
{
    int dx, dy, t;

    switch (dir) {
    case 3:
        dy = -1;
        dx = 0;
        break;
    case 4:
        dy = 1;
        dx = 0;
        break;
    case 2:
        dy = 0;
        dx = 1;
        break;
    case 1:
        dy = 0;
        dx = -1;
        break;
    }
    if (D_AD3E + dx < 0 || D_AD40 + dy < 0 || D_AD3E + dx > 37 || D_AD40 + dy > 17) {
        if (D_204A == 2 && F_249D())
            F_4BD6();
        D_204A = 0;
    } else {
        t = F_23E7(D_AD3E + dx, D_AD40 + dy);
        if (t == -1 || t == 63) {
            F_3B46(D_1DC8[dir]);
            F_B34F(1, D_AD3E, D_AD40);
            D_AD3E += dx;
            D_AD40 += dy;
            F_B34F(18, D_AD3E, D_AD40);
            F_0476(4);
        } else {
            F_3B46("Blocked!");
            F_0476(1);
        }
    }
}

/* transport shop sign number n (shuttle/ship dealer); returns 7 if n is 0 or 7, else 6 */
F_2F3B(n)
unsigned n;
{

    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(10, 4, "Scooter's Super Duper");
        F_21E7(13, 5, "Transport, Inc.");
        break;
    case 3:
        F_21E7(9, 4, "Quality Transport Ltd.");
        break;
    case 5:
        F_21E7(7, 4, "O.K. New and Used Transport");
        break;
    case 7:
        F_21E7(8, 4, "Sly Sam's Transportation");
        F_21E7(15, 5, "Specialists");
        break;
    }
    if (n == 0 || n == 7)
        return 7;
    return 6;
}

/* armour shop sign number n; returns 7 if n is 0 else 6 */
F_2FF6(n)
unsigned n;
{

    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(11, 4, "The Armour Shop of");
        F_21E7(8, 5, "Lord Eldric D'Charbonneux");
        break;
    case 1:
        F_21E7(15, 4, "Max's Armoury");
        break;
    case 2:
        F_21E7(13, 4, "The Iron Fist");
        break;
    case 3:
        F_21E7(10, 4, "The Hammer -n- Anvil");
        break;
    case 5:
        F_21E7(5, 4, "Defense Specialties Unlimited");
        break;
    case 7:
        F_21E7(11, 4, "Custom Armor Works");
        break;
    }
    if (n == 0)
        return 7;
    return 6;
}

/* weapon shop sign number n; returns 7 if n is 0 else 6 */
F_30C0(n)
unsigned n;
{

    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(11, 4, "The Weapon Shop of");
        F_21E7(8, 5, "Lord Eldric D'Charbonneux");
        break;
    case 1:
        F_21E7(11, 4, "The Tempered Steel");
        break;
    case 2:
        F_21E7(12, 4, "The Razor's Edge");
        break;
    case 3:
        F_21E7(10, 4, "Cold Steel Creations");
        break;
    case 4:
        F_21E7(12, 4, "The Bloody Blade");
        break;
    case 5:
        F_21E7(13, 4, "The Duelo Shop");
        break;
    case 6:
        F_21E7(12, 4, "Weaponry Supply");
        break;
    case 7:
        F_21E7(10, 4, "Cold Steel Creations");
        break;
    }
    if (n == 0)
        return 7;
    return 6;
}

/* magic shop sign number n; always returns 6 */
F_31B1(n)
unsigned n;
{
    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(9, 4, "Mad Chad's Magic Shoppe");
        break;
    case 1:
        F_21E7(12, 4, "Mystic Melinda's");
        break;
    case 2:
        F_21E7(13, 4, "Words of Power");
        break;
    case 3:
        F_21E7(5, 4, "Psychic Sam's Magical Emporium");
        break;
    case 7:
        F_21E7(11, 4, "The Unleashed Spell");
        break;
    }
    return 6;
}

/* food shop sign number n; always returns 6 */
F_3240(n)
unsigned n;
{
    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(5, 4, "Li'l Karelia's Finnish Grocery");
        break;
    case 1:
        F_21E7(8, 4, "Adventurer's Supply Post");
        break;
    case 3:
        F_21E7(13, 4, "The Brown Bag");
        break;
    case 4:
        F_21E7(9, 4, "Fresh Food Marketplace");
        break;
    case 5:
        F_21E7(11, 4, "Rations Unlimited");
        break;
    case 6:
        F_21E7(6, 4, "Fastest Freshest Food Market");
        break;
    case 7:
        F_21E7(8, 4, "Exploration Provisioners");
        break;
    }
    return 6;
}

/* pub sign number n; always returns 6 */
F_32F8(n)
unsigned n;
{
    F_21C2(D_0DA8);
    switch (n) {
    case 0:
        F_21E7(14, 4, "Pub de Varg");
        break;
    case 1:
        F_21E7(11, 4, "Ye Olde Local Pub");
        break;
    case 2:
        F_21E7(13, 4, "Dr. Cat's Lair");
        break;
    case 3:
        F_21E7(13, 4, "Teaser's House");
        break;
    case 5:
        F_21E7(12, 4, "Mary's Midway");
        break;
    case 6:
        F_21E7(10, 4, "Kurtstable's Korner");
        break;
    case 7:
        F_21E7(11, 4, "Dav's House-o-Suds");
        break;
    }
    return 6;
}
