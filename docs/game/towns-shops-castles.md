# OUT.EXE: towns, shops, the tavern, stealing, castles, kings and princesses

Sources: `OUTA.C` (F_1AE4, F_1BC0, F_2B6D, F_2E22, shop signs), `OUTB.C` (F_4BD6, F_4DBF, F_4E63, F_5469-F_5EC9, F_622C, F_63B3, F_63D4), `OUTC.C` (F_63F2-F_7C31, F_794E, F_7B17),
the maps in `TCD.BIN` and the tables of `OUT.EXE` (real DGROUP addresses). The maps, special squares and entity lists are in world-and-map.md; entity AI in monsters.md; combat numbers in combat.md.
"Intelligence", "Charisma", "Wisdom" are the record fields at 0x22, 0x1E, 0x20 (`D_3A9A`, `D_3A96`, `D_3A98`). `T` = the 32-bit turn counter `D_8BA8` (8BA8), incremented once per command.

## 1. Being inside a town or castle

State (all in DGROUP): `D_204A` mode (1 town, 2 castle); `D_AD42` map number (castles 0/1, towns 2..9); `D_AD3E`, `D_AD40` player x, y on the 38 x 18 map; `D_AD46` **hostile flag** (set when you
hit anybody or are caught stealing; none of the shops talks to you, guards attack; cleared at the next Enter); `D_ACC6` the 15 entities; `D_AD48` ales drunk in this town; `D_AD4A`
castle number 0..7; `D_AD4C` king number 1..8; `D_AD4E` the number (60/61) of the lock the key fits; `D_8BA0` "you have the key"; `D_8B9E` set when a door is unlocked and cleared when the princess is killed (never read by OUT);
`D_8BA2` "king cannot be hurt" (Lord British's castle); `D_8BA4` number of items you may take from the storerooms; `D_204E` redraw flag.

Moving (F_2E22 via F_066F): one step in the direction (West/East/North/South); the target `t = F_23E7(x, y)` (-1 free or special square, 63 the 0x3F special, an entity type, or 0 = blocked): free -> the old square is redrawn
as floor (tile 1), the player (tile 18) is drawn at the new one, the direction word is printed and the footstep sound plays; blocked -> "Blocked!" and sound 1. A step off the map ends the visit (mode 0): in a castle
with a princess (entity 22) in one of the eight neighbouring cells (F_249D) the princess reward (section 8) happens first.
There is no hunger, no healing and no random event in towns. The map is redrawn from scratch by F_2231 whenever `D_204E` is set (after Transact etc.).

The special squares are listed in world-and-map.md; summary: 54/55 armour, 56/57 food, 58/59 weapons, 60 magic, 61 pub, 62 transport; in castles 55/57/59 are the storerooms, 60/61 the lock squares, 62 the king's throne.

## 2. Transact (T): shops in towns (F_7377)

"Transact"; if `D_AD46` is set: "None will talk to thee!" Otherwise the byte under the player `t` must be 54..62 ("Thou art not by a counter!" + beep). The prompt "-Buy, Sell: " takes B or S; anything else prints " nothing      "
at (9,24). Then (13,24) shows ": " (Buy) or (10,24) "Sell: ", and the shop function runs by `t`: 54, 55 armour (F_6A77); 56, 57 food (F_6DE3); 58, 59 weapons (F_665D); 60 magic (F_6C53); 61 pub (F_622C); 62 transport (F_70CF).
After the shop `D_204E = 1`. Each shop first draws a framed box (F_21C2: clears x 31..287, y 23..127 and draws the border in `D_0DA8`) with its sign (names in world-and-map.md; the shop function receives the town layout
`D_AD42 - 2`), and the sign function returns the first free row of the box (7 for the two-line signs of layout 0 and "Sly Sam's", else 6); the list lines are written below it.
The number typed for an item is a letter; the shop letter of item `i` is `'a' + i`. After a successful trade the messages "Sold!" (F_5E6C, buying) or "Done!" (F_5E88, selling) appear in a cleared box and wait for a key;
"Thou canst not afford it!" (F_5EA4) with a beep when the price exceeds your pence. An unlisted letter prints "nothing". (All text is typed in the message line at row 24; amounts are shown with F_3C8B, width 3.)

### 2.1 Prices (integer arithmetic, all in pence)

| what | price | where |
|---|---|---|
| buy weapon `i` | `(255 - Intelligence) * i * i / 256 + 5` | F_63B3 |
| sell weapon `i` | `(Charisma + 40) * i * i / 256 + 1` | F_63D4 |
| buy armour `i` | `(200 - Intelligence) / 4 * i` | F_67F1 |
| sell armour `i` | `Charisma / 4 * i` | F_680C |
| buy spell `i` | `(200 - Wisdom) / 32 * i` | F_6BAC |
| food, per pack of 10 | `5 - Intelligence / 20` | F_6DE3 |
| transport `i` (1..6) | `(200 - Intelligence) / 5 * i * i` | F_7034 |

Examples (Intelligence 15, Charisma 15, Wisdom 15): weapons bought 5 8 13 20 28 38 50 65 80 98 for `i = 1..10`; armour 46 92 138 184 230; spells 5 per index (Magic Missile 15, Kill 50);
transport 37 148 333 592 925 1332 (horse, cart, raft, frigate, aircar, shuttle); a pack of food 5. (Sums that overflow 16 bits are not guarded, e.g. a weapon index above 11 would wrap negative, but the shops never offer one.)

### 2.2 Armour shop (tiles 54, 55; F_6A77, F_6823)

* **Buy**: items 1..3 (b: Leather armor, c: Chain mail, d: Plate mail) always; after `T > 3000` also 4 and 5 (e: Vacuum suit, f: Reflect suit). The letter is read lower-case; an out-of-range letter, or one you cannot pay, prints
  "nothing" (and the affordability message). Success: pence -= price, `D_20AC[i]++` (max 255). The armour is **not** readied automatically (use Ready).
* **Sell**: lists the pieces you own (index 1..5) with letters `b..f` (letter = 'a' + index), price = Charisma/4 * index; one piece is sold, pence added, and if it was your readied armour you go back to skin.

### 2.3 Food shop (56, 57; F_6DE3)

Buy: " Packs of 10 food cost N pence each. How many dost thou wish to purchase?" (N = `5 - Intelligence/20`, 5 down to 1); a number of up to 4 digits (F_B0B5, cursor at (15,24); Backspace edits; leading zero ignored);
the total `N * n` must not exceed your pence (a 16-bit overflow makes it count as too much); then " packs food", food += 10 * n. Sell: "Used food?  No thanks!".

### 2.4 Weapon shop (58, 59; F_665D, F_63F2, F_65CE)

* **Stock** depends on the town layout and on the turn counter. The weapon indices offered are `lo, lo+2, ...` where `lo = 1` for odd map numbers `D_AD42` (3, 5, 7, 9) and `lo = 2` for even ones (2, 4, 6, 8). The number of items is 2 while `T < 1500`, 3 for
  `1500 <= T < 3000`, 4 for `T == 3000`, 5 once `T > 3000` (computed from `n = T % 32767 / 1500`, capped at 3 when `n > 3 || T > 3000`; the list runs from `lo` to `lo + 2*(n+1)`).
  So odd-numbered maps sell Dagger, Axe, Sword, Bow & Arrows, Wand (in that order of availability) and even-numbered ones Mace, Rope & Spikes, Great Sword, Amulet, Staff. **Nothing above index 10 is ever sold**: Triangle, Pistol, Light Sword, Phazor and Blaster must be stolen
  or won.
* Buying: price F_63B3, `D_20B8[i]++` (max 255), name printed, coins redrawn, "Sold!". A letter whose index has the wrong parity prints "nothing".
* **Selling**: lists up to 8 of your weapons (index 1..15, letter = 'a' + index), price F_63D4; sells one (`D_20B8[i]--`), "Done!"; if it was readied the hands are readied. "Thou hast no weaponry to sell!" if you own none (Hands cannot be sold).

### 2.5 Magic shop (60; F_6C53, F_6BC7)

Selling: "  Sorry, we don't buy spells!". Buying: the spells are chosen by the map parity again: odd map numbers (3, 5, 9 have a magic shop) offer indices 2 (Unlock) and 6 (Ladder Up) and, **for a Wizard only** (`D_206A == 2`),
also 8 (Create) and 10 (Kill); even map numbers (2, 4) offer 1 (Open), 3 (Magic Missile), 5 (Ladder Down) and, for wizards, 7 (Blink) and 9 (Destroy). Prayer (0) is free, Steal (4) is skipped by the shop (`i != 'e'`); the list shows `a) name - price` (price F_6BAC,
`(200 - Wisdom)/32 * index`). A purchase adds one charge (`D_20D8[i]++`, max 255). (The manual says the strong spells cost "gold and experience"; the code charges pence only.)

### 2.6 Pub (61; F_622C)

Buy ("B"): with no pence: "Thou art broke! Come back when thou hast some money to spend." Otherwise one penny is paid, "The tavern keeper sayeth: Here, have a cold one!", the text "ale" is typed at (15,24), `D_AD48++`
(ales drunk in this town; reset at each Enter into a town). If `D_AD48 > Stamina / 4` **and a wench/lecher (type 50) stands in one of the 8 neighbouring cells** (F_619C): "Thou hast been seduced! After a long night, thou art back."
(story page 0): pence are halved, Wisdom `-1` (a value that would fall to exactly 4 becomes 5). Independently, with probability `rand(1,255) < 75` (29 %) a gossip page follows: `F_5EC9(rand(11,89) / 10)` = pages 1..8:

1. "Thou had best know about space travel! Thou must destroy at least 20 enemy vessels to become an ace!"
2. "... to watch the wench." (male player) / "lecher." (female)
3. "... that the princess will give great reward to the one who rescues her, and an extra gift to an 8th level ace!"
4. "... thou must go back in time."
5. "... thou should destroy the evil gem!"
6. "... that many lakes and ponds have strong magical powers!"
7. "... this is a great game!"
8. "... that over 1000 years ago, Mondain the Wizard created an evil gem. With this gem, he is immortal and cannot be defeated." then (after a key) "The quest of --Ultima-- is to traverse the lands in search of a time machine. Upon finding such a device, thou should go back in time to the days before Mondain created the evil gem and destroy him!"

Sell ("S"): "We have plenty of booze already!".

### 2.7 Transport (62; F_70CF, F_6F58)

Sell: "Sorry, we don't deal in used stuff.". Buy: the available vehicles depend on the terrain around the town, counted when you entered (`D_8B94` water cells, `D_8B96` grass, `D_8B98` woods among the 3 x 3 cells around the place),
on the vehicle table and on the time: `D_8B92` = number of vehicle objects (entries of type <= 17, including the player; 15 = table full); `D_3B1A` = number of shuttles you own (= `D_20EE[6]`, record 0xA2); `D_3B1C` = time machines (`D_20EE[7]`, 0xA4).
* "Closed for the day." if no neighbouring cell is water/grass/woods, or the table is full (`D_8B92 == 15`) and you own a shuttle, or full and no grass.
With `flag = (D_3B1A == 0 && D_8B92 == 15)` (the table is full and you own no shuttle):
* a) Horse, b) Cart: if there is woods or grass around and `!flag`;
* c) Raft, d) Frigate: if there is water around and `!flag`;
* e) Aircar: only if `T > 3000`, there is grass around and `!flag`;
* f) Shuttle: if `T > 3000`, there is grass around and you own no shuttle (`D_3B1A == 0`); the code lets you buy it by pressing F even when it is not listed (the selection table entry `x[5]` is set whenever the `T > 3000 && grass` block runs).
So when the table is full and you own no shuttle, only the shuttle can still be bought.
Price F_7034 (list column at x = 23, 4 digits). Buying: `pence -= price`, `D_8B92++`, "Sold!", the transport name is printed; for the shuttle `D_3B28 = D_3B2A = 1000` (its fuel and shield, record 0xB0/0xB2). `F_6F58(k)`: `D_20EE[k]++`; the vehicle appears on the first free neighbouring cell
(rows -1..1, columns -1..1, row-major) of the right terrain: horse/cart on woods if there are woods around else grass, raft/frigate on water, aircar/shuttle on grass; the entry type is `k + 10` (k <= 4) or `k + 11`: 11 horse, 12 cart, 13 raft, 14 frigate, 16 aircar, 17 shuttle; board it with B (commands.md).

## 3. Donations: the Drop command on special squares (F_204D, F_1BC0, F_1AE4)

Drop > Pence takes `n` pence (0 = "none"; more than you have: "Thou hast not that much!"). The pence are gone in every case; then:
* **town** (mode 1): only if you stand on a 0x33 square: hit points `+= 3n / 2` ("Shazam!", sound 5); with probability `rand(1,255) <= 16` (6 %) a free spell instead of the message: `i = rand(1,7)` (3 becomes 4), `spell charge (i+1)++`, "Alakazot!" -- i.e. spells 2 Unlock, 3 Magic Missile, 5 Ladder Down (twice as likely), 6 Ladder Up, 7 Blink, 8 Create. Elsewhere: "Ok!" (the pence are still lost).
* **castle** (mode 2) F_1AE4(n): 0x33: a random attribute `rand(1,6)` (Strength .. Intelligence) `+= n / 10`, max 99; 0x34: if `n >= 10` one random weapon (index `rand(1,15)`), count +1; 0x35: food `+= 3n`. "Shazam!" and sound 5; any other square: "Ok!".

Drop Weapon and Drop Armour (F_1D71, F_1EDF) discard one item (no price). All three can be used only inside towns/castles.

## 4. Stealing (S) and being caught

`F_5DB1` (towns and castles; "Steal?" + beep elsewhere): the square must be 0x37 (55, armour; F_5D34), 0x3B (59, weapons; F_5BFA) or 0x39 (57, food; F_5CBA); else " - nothing here!". Each steal first calls F_5B8A (the caught test):
`r = rand(1,255)`; **caught** if `D_AD46` is already set or `r < 38`, or if the player is not a thief (class != 3) and `r <= 77`; a thief is therefore caught only with `r < 38`: probability about 14.5 % for thieves, 30 % for everybody else.
Caught: "Oh no!  Thou wert caught!", `D_AD46 = 1` (hostile for the rest of the visit); you get nothing. Not caught:
* weapons: "Thou dost find a(n) <weapon>" with `i = rand(1,15)`, count +1 (max 255), weapon index uniform over 1..15 (this is the source of the Pistol, Phazor ...);
* armour: `i = rand(1,5)`, "Thou dost find (a )<armour>", count +1;
* food: `n = rand(2,31)` "bags of food", food `+= n` (F_44A2 redraws).
(Stealing does not remove anything from the shop, and "Steal" has no limit except being caught.)

## 5. Castles: storerooms (Get, G)

`F_5550` (castle only; in a town "Get what?"): the square must be 0x37 / 0x39 / 0x3B; the king must have given permission: `D_8BA4 > 0` else "Thou hast not the king's permission!" with a beep. Each Get decrements `D_8BA4` and gives the same random item as stealing
(F_5D34 / F_5CBA / F_5BFA with no caught test). `D_8BA4` is set to **9** by the white-gem quest of king 8 ("take nine items from my storerooms - but only nine!") and cleared to 0 each time any castle is entered.

## 6. Castles (F_4DBF) and the locked doors

Entering castle `c` (33..40): `D_AD4A = c - 33`, map `(c-1) % 2`, king number `D_AD4C = D_AD4A + 1`, mode 2, entities loaded (F_20FC), player at (0, 9), hostile flag cleared, `D_8BA0 = D_8B9E = D_8BA4 = 0`, the two doors set to tile 11 at x = 35 and 31, y = 14 (map 0) or 4 (map 1),
and `D_AD4E = 60 or 61` at random (`rand(1,255) & 1` odd -> 61). The princess is in a small walled room whose two doors lie directly above the two lock squares (castle 0: lock squares 60 at (31,15) and 61 at (35,15), doors at (31,14) and (35,14)).
**Unlock** (U, F_7C31): only on a 60/61 square (else " what?"); without the key "Thou hast not a key!"; with the key but the wrong square: "Thou hast not the correct key!" (only the square whose number equals `D_AD4E` works); correct: "The door is open!", `D_8B9E = 1`, the cell **north of the lock square**
(y - 1) becomes tile 1 (floor) and is redrawn: the way to the princess is open. The key is obtained by **killing the jester** (type 19, hit points 1; "Thou hast found a key!", `D_8BA0 = 1`, combat.md) -- the jester sings "I've got the key!".

Castles 33, 35, 37, 39 use map 0 (the king in the middle of a hall, 6 guards), castles 34, 36, 38, 40 map 1 (7 guards). The names of the eight castles and their princesses (`D_31CC`, DGROUP 31CC, 8 names right-aligned): 33 The Castle of Lord British - Julia;
34 The Castle of the Lost King - Cassandra; 35 The Castle Barataria - Marsha; 36 The Castle Rondorin - Donna; 37 The Castle of Olympus - Kristen; 38 The Black Dragon's Castle - Beth; 39 The White Dragon's Castle - Lori; 40 The Castle of Shamino - Dianne.

## 7. The kings (T in a castle, F_794E)

"Transact with king". You must stand on a 62 square: "He is not here!" otherwise; if `D_AD46` is set: "He rejects thine offer!". Otherwise F_78AF is tried first (the **reward for a finished quest**, quests-and-progress.md); if no reward was due the menu: "Dost thou offer pence or service: " (P / S, else "neither"):
* **P**: "pence", "How much? " (up to 4 digits); 0 -> "none"; more than you own: "Thou hast not that much!"; else pence `-= amount` and "In return I give unto thee <amount*3/2> hit points" (`HP += amount * 3 / 2`). No limit except the 9999 display cap.
* **S**: if you are already on this king's quest (`D_3AB2[king] == -1`): "Thou art on a quest for me already!"; else the quest (F_7526), see quests-and-progress.md.

## 8. Rescuing the princess (F_2E22 -> F_4BD6)

Leaving a castle by walking off the map edge while a princess (entity type 22) is in one of the neighbouring cells (checked: (-1,1), (-1,0), (-1,-1), (0,1), (0,-1), (1,1), (1,0); the cell (1,-1) is missing from the list) shows the reward screen:
"Thou hast saved the princess <name>. She giveth thee 500 hit points, pence and experience points." -- `pence += 500`, `HP += 500`, `experience += 500` (each shown capped at 9999 afterwards).
If `D_20FE >= 20 && experience >= 7000` (you have destroyed at least 20 enemy vessels in space and are level 8 or more -- see space-flight.md) she adds: "She informs thee that thou art now ready for time travel, and that there is a time machine far to the northwest!" and a **time machine** (type 18) is placed
on a random woods square in the north-west part of the current quarter (position `(x0 + rand(12,21), y0 + rand(11,15))` with `(x0,y0)` = (0,0), (83,0), (0,77) or (83,77) for quarter 0..3; up to 500 tries for a woods square, none placed if there is no woods there);
`D_3B1C` counts the time machines and stops the placement at 9. Then a wait (sound ticks) and any key. The princess can also be killed (then no reward, `D_8B9E = 0`).
