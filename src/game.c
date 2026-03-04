#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "game.h"
#include "intro.h"
#include "sons.h"
#include "map.h"
#include "ia.h"
#include "fin_jeu.h"
#include "options.h"
#include <stdio.h>
#include <math.h>
#include "save.h"

// #define VOLUME_MUSIQUE 32
// #define VOLUME_BRUITAGES 32

// -- Pour les sons --

// Sound Effects
// static Mix_Chunk *sonTransition = NULL;
static Mix_Chunk *sonPickUp = NULL;
static Mix_Chunk *sonOpenDoor = NULL;
static Mix_Chunk *sonCloseDoor = NULL;
static Mix_Chunk *sonScreamer = NULL;
static Mix_Chunk *sonPendule = NULL;
static Mix_Chunk *sonProjectileHit = NULL;

// Variables pour l'effet de flash lors d'une collision
static int hitFlashTimer = 0;
static int hitFlashDuration = 30;  // Durée du flash en frames (plus long pour bien voir)
static int knockbackTimer = 0;
static float knockbackVX = 0.0f;
static float knockbackVY = 0.0f;
static int hitCount = 0;  // Compteur de coups (3 coups = retour chambre)
static int nightmareFrame = 0;  // Compteur pour effet cauchemar pulsant

// Musiques D'ambiance
static Mix_Music *MusicInterior = NULL;
static Mix_Music *MusicExterior = NULL;
static int currentZoneState = -1;

// Pour les touches
static int toucheE_Relache = 1;
static int toucheEnter_Relache = 1;

// --- VARIABLES GLOBALES ---

SDL_Texture *tilesetTexture = NULL;
SDL_Texture *textureScreamer = NULL; 
// static SDL_Texture *playerTexture = NULL; 
// static SDL_Texture *playerTexture = NULL;
SDL_Texture *texturePlayerVieux = NULL;
// livre
SDL_Texture *textureLivre = NULL;
int livreOuvert = 0;

int rayon = 0;

int currentLevel = 0; // 0 = Chambre, 1 = Couloir

/*
0,1 = sol en bois
2 = mur en brique
3 = cube 1
4 = cube 2
5 = perso
6 = doudou
7 = perso actuel
8 = haut gauche armoire fermée
9 = haut droit armoire fermée
10 = bas gauche armoire fermée
11 = bas droit armoire fermée
12 = haut gauche armoire ouverte
13 = haut droit armoire ouverte
14 = bas gauche armoire ouverte
15 = bas droit armoire ouverte
16 = haut gauche armoire ouverte doudou
17 = haut droit armoire ouverte doudou
18 = bas gauche armoire ouverte doudou
19 = bas droit armoire ouverte doudou
20 = livre
21 = lampe
22 = armoire vu de haut partie gauche
23 = armoire vu de haut partie droite
24,25,26 = bas tapis
27,28,29 = haut tapis
30,31 = tapis rond
32,33 = haut grand lit bb
34,35 = bas grand lit bb
36,37 = déco haut du lit
38,39 = petit train
40 = avion
41 = dino
42,43 = commode mur gauche
44 = cubes
45 = cubes
46,47,48,49 = bureau enfant
50 = cube noir
51 = haut chaise
52,53,54 = bas tente
55,56,57 = haut tente
58 = coussin bas
59,60 = coussins
61 = digicode 0
78 = hanse basket
79 = ballon de basket
80,81 = commode mur droit
367, 368, 369 = bas tapis voitures
370,371, 372 = haut tapis voitures
373 = legos sombre
....................
-tente-
87,88,89,90,91 = bas tapis
92,93,94,95,96 = mid tapis v1
97,98,99,100,101 = mid tapis v2
102,103,104,105,106 = fin tapis
147 = doudou duolingo
148 = lampe
149 = cookie
150 = mur tente
151,152 = leds rouge
174 = dessin theo
364 = leds mur droit 
365 = leds mur derriere
366 = leds mur gauche
........................  
62 = monstre vu de gauche
63 = monstre de face
64 = monstre vu de droite
82 = sol labyrinthe
83 = mur labyrinthe
.........................
-couloir-
65, 66 = haut tapis
67, 68 = tapis
69, 70 = bas mur
71 72 = table avec broc d'eau
71, 172 = table avec broc sans eau
71, 173 = table sans broc
73 = vase
74, 75 = lampe murale gauche
76, 77 = lampe murale droite
376 = bas de la seconde table 
377 = haut seconde table
378 = bougie deco
...........................
-haul d'entrée-
84(bas), 85(tete rouge) = statue mur gauche
86 = tete statue yeux verts
107, 108 = bas tableau
109, 110 = haut tableau
111,112 = bas tableau vide
113,114 = haut tableau vide
115(bas gauche),116(bas droit),117,118 = morceaux tableau
119-224 = bas tapis
125-130 = haut tapis
131 = tableau pomme
13(haut),133 = commode mur gauche
134,135 = table mur haut
136,137 = pieds de la table + chat
138 = carreau foncé
139,140 = bas mirroir
141,142 = haut mirroir
143,144 = pot de fleurs
145,146 = fleur du pot de fleurs
153(bas),154 = fenetre
378(gauche), 379(droite) 380(haut g), 381 = fauteuil de coté
382(bas), 383,384(haut g),385  = fauteuil de face
386,387 = pieds de la table sans chat
...........................
-cuisine-
155 = sol
156(g),157(d) = bas bouche
158(g),159(d) = haut bouche
160-166 = grande table
167,168 = lavabo
169,170 = plaque cuisson
171 = bloc seul
175 = pain
176 = os
177 = crane + chenille
178 = poils + seringue
179, 180 = grimoire
181 = tirroir mur gauche
182,183 = bouteille renversée
184 = toile d'araignée
240 = fiole poussiere
241 = dent
242 = araignée
243 = coeur rouge
244 = coeur noir
245,246,247 = bas armoire
248,249,250 = potions armoire
251,252,253 = fin armoire
254,255,256 = bas tapis
257,258,259 = haut tapis
260,261,262 = bas table
263(haut),264(bas) = chaise (coté gauche de la table)
265 = oeil
266,267 = bas du four
268,269 = haut du four
270 = toile d'araignée (avec début araignée)
271(bas d), 272(bas g), 273(haut d) = table bordel
274 = mini plante
275 = grosse plante
293 = sur la table crane SANS chenille
294,295 (bas) 296,297 = chaudron vide
294,295,298,299(ou 300,301 effet) = chaudron avec bulles
326 = la soupe aux cauchemars
327 = le pain du chagrin
375 = livres sur étagère
388(haut meuble),374(mid),389 = meuble mur de droite
390,391,392 = tapis sous chaudron
393 = décors meuble mur d 1
394 = décors meuble mu d 2
395 = sac de sang
396 = sac sombre
397 = poele au mur
.........................
-chambre parents-
185 = commode avec télécommande (bas)
186 = suite commode (haut)
187,188,189, 190 = lit haut avec monstre yeux ouverts
191,192,193,194 = lit bas
195,196,197,198 = lit haut haut
199,200,201,202 = lit bas bas
207,208,209 = bas commode PAS FINIT
210,211,212 = mid commode PAS FINIT
213,214,215 = haut commode PAS FINIT
216 = lego bleu
217 = legos rouge vert
218 = commode sans télécommande
219,220,221,222 = monstre endormis
223 = verre cassé
224 = morceaux de verres
225 = pantalon
226 = bonnet
227 = t shirt
228 = sac
229 = pokeball
230 = chausette 1
231 = chausette 2
232 = talon
233 = lunette
234 = collier
235 = écharpe
236 = bas de pyj
.................
-salle à manger-
237 = couteau vers la droite
238 = vers la gauche
239 = assiette
276, 277(mid utilisé tt la table), 278 = haut de la table
279(g), 280(d) = cotés de la grande table
281, 277, 282 = fin de la table
283(haut g), 284(bas g) = chaises gauche
285(haut d), 286 = chaises droite
287 = poulet
288 = viande
289 = bol yeux
290 = carafe sang
291 = asiettes dents
292 = asiette os
.................
-salon-
302(g), 303(d) = bas télé sur commode
304,305 = haut télé
306,307 = continuité de la table
308,309,310,311 = haut canapé
312,313,314,315 = bas canapé
316,317 = bas télé (effet bug v1)
318,319 = haut télé (effet bug v1)
320,321 = bas télé (effet bug v2)
322,323 = haut télé (effet bug v2)
324,325 = bas du meuble de la télé
328,329 = bas porte fin ouverte
330,331 = haut porte fin ouverte
332,333 = bas cheminée
334,335 = haut cheminée 
336,337 = bas tableau
338,339 = haut tableau 
340 = carré bleu (sol)
341 = carré rouge 
342 = carré vert 
343 = lampe 
398 = petite commode -> haut de la lampe 
399 = petite commode (bas)
400, 401 = autre table avec radio 
402, 403 = antenne radio
404,405,406 = haut mini billard
407,408,409 = bas mini billard
410,411,412 = bas billard
413(bas),414(mid),415(haut) = horloge
416, 417,418 = bas piano
419,420,421 = mid piano
422 = haut piano
435,436(bas),437,438(haut) = porte fin fermée
439 = maman par terre sang
440, 441 = maman debout
442, 443 = perso debout adulte
...................
423,424 (bas), 425,426 (mid), 427,428 = porte
429,430 (bas), 431,432(mid), 433,434 = porte mur gauche
................
personnage 
7 = perso actuel de face
344 = perso profil droit jambe levé (avance vers la droite)
345 = perso profil gauche jambe levé (avance vers la gauche)
346 = perso profil droit
347 = perso profil gauche
348 = perso de dos
349 = perso profil droit jambe derriere levé (avance vers la droite)
350 = perso profil gauche jambe derriere levé (avance vers la gauche)
351 = perso de dos jambe droite levé (avance dos à nous)
352 = perso de dos jambe gauche levé (avance dos à nous)
353 = perso perso profil droit (très rapproché)
354 = perso profil gauche
355 = perso de face cligne les yeux (respiration)
356 = perso de face jambe en avant gauche (avance vers nous)
357 = perso en avant jambe droite en avant (avance vers nous)
358 = perso de face avec 1 pixel en +
359 = perso de dos respiration
360 = perso profil droit respiration 
361 = perso profil gauche respiration
362 = perso profil droit espiration fin (pixel en +)
363 = perso profil gauche espiration fin (pixel en +)
*/

// --- LA CARTE DU NIVEAU ---
const int maps_origine[NB_LEVELS][MAP_HEIGHT][MAP_WIDTH] = {
    {
        // carte 1 (chambre) index 0 
        {2,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0, 2,  2,  5,  2,  8,  9,  2, 2}, // Trou en haut
        {2,  2,  2, 36, 37,  2,  2,  2,  0,  0,  0,  0, 2,  2, 41,  2, 10, 11,  2, 2},
        {2,  1,  0, 32, 33, 21,  0,  1,  0,  1,  0,  0, 0,230,  0,  1,  0,  1,  0, 2},
        {2,  1,  0, 34, 35,  1,  0,  1,  0,  1, 44,  1, 0,  1,  0,  1,  0,373,  0, 2},
        {2,  1, 30, 31,  0,  1,  0,  1,  0,  1,  0,  1, 0,  1,  0,  1,  0,  1,  0, 2},
        {2, 20,  0, 1,  0,  1,  0,  1,370,371,372,  1, 0,  1,  0,  1,  0,  1,  0, 2},
        {2, 42,  0,  1,  0,  1,  0,  1,367,368,369, 45, 0,  1,  0,  1, 55, 56, 57, 2},
        {2, 43,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1, 0,  1,  0,  1, 52, 53, 54, 2},
        {2,  1,  0,  1,  0,  3, 44,  1,  0,  1,  0,  1, 0,  1,  0, 58,  0, 59, 60, 2}, 
        {2,  1,  0, 40,  0,  1,  4,  1,  0,373,  0,  1, 0,  1,  0,  1,  0,  1,  0, 2},
        {2,  1,  0,  1, 38, 39,  0,  1,  0,  1,  0,  1, 0,  1,  0,  1, 40,  1,  0, 2},
        {2, 78,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1, 0, 27, 28, 29,  0,  1, 81, 2},
        {2,  1, 79,  1,  0,  1, 51,  1,  0,  1,  0,  1, 0, 24, 25, 26,  0,  1, 80, 2}, // Bas fermé
        {2,  1,  0,  1,  0,  1, 46, 47, 48, 49,  0,  1, 0, 22, 23,  0,  0,230,  0, 2},
        {2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2,  2,  2,  2,  2, 2} // Bas fermé
    },
    // CARTE 2 : COULOIR (Niveau 1) index 1
    {
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 69, 70,  0,  2,  2, 2, 2, 2, 2, 2, 2}, // Haut (Suite)
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68,  0,  2,  2, 2, 2, 2, 2, 2, 2},   // Toits cotés
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68,  0,  2,  2, 2, 2, 2, 2, 2, 2},   // Murs cotés
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68, 76, 77,  2, 2, 2, 2, 2, 2, 2}, // Vide (Sol)
        {2, 2, 2, 2, 2, 2, 2, 74,  75, 67, 68,377,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68,376,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2, 172, 67, 68,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,  71, 67, 68,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68, 76, 77,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 74,  75, 67, 68,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68, 73,  2,  2, 2, 2, 2, 2, 2, 2}, // Bas (Entrée)
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 67, 68,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0, 65, 66,  0,  2,  2, 2, 2, 2, 2, 2, 2}},
    // CARTE 3 : HALL (Niveau 2)
    {         
        {2,  2,  2,    2,  2,    2,    2,  2,    2,  2,  2,   2,    2,    2,    2,   2,     2,  2,  2, 2}, // Trou en haut (tout fermé en 2 ici ?)
        {2,  2,  2,    2,  2,  113,  114,  2,    0,  0,  0,   0,    2,    2,  131,   2,   141,142,  2, 2},
        {2,  2,  2,    2,  2,  111,  112,134,  275,  0,  0,   0,    2,  377,  135,   2,   139,140,  2, 2},
        {2,  2,  2,  145,146,    0,    0,386,  387,  0,  0,   0,    0,  136,  137,   0,     0,  0,  2, 2},
        {2,  2,  2,  143,144,    0,    0,  0,    0,  0,  0,   0,    0,    0,    0,   0,     0,  0,  2, 2},
        {2,  2,  2,    0,  0,    0,    0,  0,    0,  0,  0,   0,    0,    0,    0,   0,     0, 85,  2, 2},
        {2,  2,  2,    0,  0,    0,    0,  0,    0,  0,  0,   0,    0,    0,    0,   0,     0, 84,  2, 2},
        {2,  2,  2,  125,126,  126,  126,126,  126,126,126, 126,  126,  126,  126,  126,  126,130,  0, 0},
        {2,  2,  2,  119,120,  120,  120,120,  120,120,120, 120,  120,  120,  120,  120,  120,124,  0, 0},
        {2,  2,  2,    0,  0,    0,    0,  0,    0,  0,  0,   0,    0,    0,    0,   0,     0, 85,  2, 2},
        {2,  2,  2,    0,  0,  384,  385,  0,    0,  0,  0,   0,    0,    0,    0,   0,     0, 84,  2, 2},
        {2,  2,  2,  380,381,  382,  383,  0,    0,  0,  0,   0,    0,    0,    0,   0,     0,  0,  2, 2},
        {2,  2,  2,  378,379,    0,    0,  0,    0,  0,  0,   0,    0,    0,    0, 145,   146,  0,  2, 2}, // Bas fermé
        {2,  2,  2,    0,  0,    0,    0,  0,    0,  0,  0,   0,    0,    0,    0, 143,   144,  0,  2, 2},
        {2,  2,  2,    2,  2,    2,    2,  2,    0,  0,  0,   0,    2,    2,    2,   2,     2,  2,  2, 2}
    },  
    // CARTE 4 : CUISINE (Niveau 3) index 3
    {
        {2,   2,   2,   2,   2,   2,   2,   2,   0,   0,   0, 0, 2, 251, 252, 253,   2,   2, 2, 2}, // Trou en haut
        {2,   2, 270, 268, 269,   2, 273,   2,   0,   0,   0, 0, 2, 248, 250, 249,   2, 184, 2, 2},
        {2,   2, 242, 266, 267, 271, 272,   0,   0,   0,   0, 0, 0, 245, 246, 247, 244,   0, 2, 2},
        {2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0, 395, 2, 2},
        {2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 158, 159,   0,   0, 396, 2, 2},
        {2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 156, 157,   0,   0,   0, 2, 2},  
        {2, 184,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0,397, 2},
        {2,   2,   0,   0, 263, 176,   0, 175,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0, 0, 0},
        {2,   2, 275,   0, 264,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0, 0, 0},
        {2,   2, 265,   0, 263, 179, 180,   0, 177,   0,   0, 0, 0,   0,   0,   0,   0,   0, 2, 2},
        {2,   2, 171,   0, 264, 260, 261,   0, 262,   0,   0, 0, 0, 296, 297,   0,   0,   0, 2, 2},
        {2,   2, 181,   0,   0, 257, 258, 258, 259,   0,   0, 0, 0, 294, 295,   0,   0, 394, 2, 2},
        {2,   2,   0,   0,   0, 254, 255, 255, 256,   0,   0, 0, 0,   0,   0,   0,   0, 393, 2, 2}, // Bas fermé
        {2,   2,   0, 178, 169, 170, 167, 168, 243, 274,   0, 0, 0,   0,   0,   0,   0,   0, 2, 2},
        {2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2, 2, 2,   2,   2,   2,   2,   2, 2, 2}
    },
    // CARTE 5 : SALLE A MANGER (Niveau 4 - dernière du bloc précédent) index 4
    {
        {2, 2, 2, 2, 2, 2, 2, 2,  2,  0,  0,  2,  2, 2, 2, 2, 2, 2, 2, 2}, // Trou en haut
        {2, 2, 2, 2, 2, 2, 2, 2,  2,  0,  0,  2,  2, 2, 2, 2, 2, 8, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,283,281,277,292,  0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,284,  0,291,239,285, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,283,  0,290,  0,286, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,284,  0,  0,  0,285, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 0, 0, 0, 0, 0, 0,283,239,  0,288,286, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 0, 0, 0, 0, 0, 0,284,  0,  0,239,285, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 0, 0, 0, 0, 0, 0,283,  0,287,  0,286, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,284,  0,  0,  0,285, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,283,239,  0,289,286, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,284,276,277,278,  0, 0, 0, 0, 0, 0, 2, 2}, // Bas fermé
        {2, 2, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2,  0,  0,  0,  0,  2, 2, 2, 2, 2, 2, 2, 2}
    }, 
    // --- LABYRINTHE 1 (Index 5) --- 
    {
        { 2, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83}, 
        { 2, 82, 82, 82, 83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 83, 82, 82, 83, 117, 83}, 
        { 2, 83, 83, 82, 83, 82, 83, 83, 83, 82, 83, 82, 83, 82, 83, 82, 83, 83, 82, 83},
        { 2, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 83},
        { 2, 82, 83, 83, 83, 82, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 82, 83}, 
        { 2, 82, 83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 83, 82, 83},
        { 2, 82, 83, 82, 83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 83, 83, 82, 83, 82, 83}, 
        { 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83}, 
        { 82, 82, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83}, 
        { 2, 82, 82, 82, 82, 82, 83, 82, 82, 82, 83, 82, 82, 82, 83, 82, 82, 82, 82, 83},
        { 2, 83, 83, 83, 83, 82, 83, 82, 83, 82, 83, 82, 83, 82, 83, 83, 83, 83, 82, 83}, 
        { 2, 82, 82, 82, 83, 82, 82, 82, 83, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 83},
        { 2, 82, 83, 82, 83, 83, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 82, 82}, 
        { 2, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83}, 
        { 2, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83}
    },

    // --- LABYRINTHE 2 (Index 6) ---
    {
        {83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83}, // Mur Haut
        {83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 83},
        {83, 82, 83, 83, 83, 82, 83, 82, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 82, 83},
        {83, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83, 118, 82, 82, 82, 83, 82, 83},
        {83, 82, 83, 82, 83, 83, 83, 83, 83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 82, 83}, // Gros bloc central
        {83, 82, 82, 82, 83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 83},
        {83, 83, 83, 82, 83, 82, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 83, 83, 82, 83},
        {83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 83, 82, 82, 82, 83},
        {83, 82, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 82, 83, 82, 83, 83, 83},
        {83, 82, 83, 82, 82, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 83, 82, 82, 82, 83},
        {83, 82, 83, 82, 83, 83, 83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 82, 83, 82, 83},
        {83, 82, 82, 82, 83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 83, 82, 83},
        {82, 82, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 82, 83}, // ENTRÉE GAUCHE (Ligne 12, connectée à l'autre)
        {83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83},
        {83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 82, 82, 83, 83, 83, 83, 83, 83, 83, 83}  // SORTIE BAS (Ligne 14, Colonnes 10-11)
    },
    // --- LABYRINTHE 3 (Index 7) ---
    {
        {83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 82, 82, 83, 83, 83, 83, 83, 83, 83, 83}, // ENTRÉE HAUT (Alignée avec sortie précédente)
        {83, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83}, // Zone ouverte pour tromper
        {83, 82, 83, 83, 83, 82, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 82, 83},
        {83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 83, 82, 83}, // Début des fausses pistes
        {83, 83, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 82, 83, 83, 82, 83, 82, 83},
        {83, 82, 82, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 83, 82, 82, 83, 82, 83},
        {83, 82, 83, 83, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 82, 83, 83, 82, 83},
        {83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83}, // Grand couloir transversal
        {83, 82, 83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83}, // Cul de sac massif à droite
        {83, 82, 82, 82, 83, 82, 83, 82, 82, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 83},
        {83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 82, 83},
        {83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 83},
        {82, 82, 82, 82, 83, 83, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 83, 83}, // SORTIE GAUCHE (Ligne 12)
        {83, 83, 83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 115, 82, 83},
        {83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83}  // Mur Bas fermé
    },

    // --- LABYRINTHE 4 (Index 8) ---
    {
        {83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83},
        {83, 82, 82, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83},
        {83, 82, 83, 83, 83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 83, 83, 83, 83, 82, 83},
        {83, 116, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83, 82, 83},
        {83, 82, 83, 82, 83, 83, 83, 83, 83, 83, 83, 82, 83, 83, 83, 82, 82, 83, 82, 83},
        {83, 82, 83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 83, 82, 83},
        {83, 82, 83, 83, 83, 82, 83, 83, 83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 82, 83},
        {83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83},
        {83, 82, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 82, 83},
        {83, 82, 83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 83, 82, 82, 82, 82, 83, 82, 83},
        {83, 82, 83, 82, 83, 83, 83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 82, 83, 82, 83},
        {83, 82, 82, 82, 83, 82, 82, 82, 82, 82, 83, 82, 82, 82, 82, 82, 82, 83, 82, 83},
        {83, 83, 83, 82, 83, 82, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 82, 82, 82, 82},
        {83, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83},
        {83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83}
    },
    // TENTE (index 9)
    {
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50}, // Marge Haut
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50, 151, 152, 151, 152, 151, 152, 151, 152, 151, 50, 50, 50, 50, 50, 50}, // Début Rectangle
        {50, 50, 50, 50, 50, 366,   1,   0,   1,   0,  58,  41,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50, 366,  44,   0,   1,   0,   1,   0, 147, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,   0,   1,   0,   1,   0,   1,   0,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,   0,   1,   0, 148,  59,  60,   0,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,   0,   1,   0,   1,   0, 149,  45, 174, 364, 50, 50, 50, 50, 50, 50}, 
        {50, 50, 50, 50, 50, 366,  58,   0,   1,   0,   1,  40,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50, 366,   1,   0,   1,   0,   1,   0,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50, 365, 365, 365, 365, 365, 365, 365, 365, 364, 50, 50, 50, 50, 50, 50}, // Fin Rectangle
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50},          // Marge Bas
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50}},
    // Chambre des parents (index 10)
    {    
        {2, 2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2,  2,  2,  2, 2, 2, 2, 2}, // Trou en haut
        {2, 2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2,213,214,215, 2, 2, 2, 2},
        {2, 2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2,210,211,212, 2, 2, 2, 2},
        {2, 2,  0,234,  0,235,217,  0,228, 0,229,  0, 0,207,208,209, 0, 0, 2, 2}, // Zone haute chargée
        {2, 2,185,231,  0,  0,  0,224,  0, 0,216, 59,60,  0,230,  0, 0, 0, 2, 2}, // Protection télécommande
        {2, 2,186,  0,231,  0,233,  0,  0, 0,232,  0, 0,225,  0,235,226, 0, 2, 2},
        {2, 2,195,196,197,198,  0,216,230, 0,  0,227, 0,234,  0,  0, 0, 0, 2, 2},
        {2, 2,219,220,221,222,223,  0,  0, 0,217,  0, 0,  0,236,  0,216, 0, 2, 2}, // Le monstre dort ici
        {2, 2,191,192,193,194,  0,233,  0,58,  0,217, 0,216,  0,224, 0, 0, 2, 2},
        {2, 2,199,200,201,202,232,  0,225, 0,  0,225, 0,  0,  0,  0, 0, 0, 2, 2},
        {2, 2,203,204,205,206,  0,  0,216, 0,228,  0, 0,236,229,  0, 0, 0, 2, 2},
        {2, 2,  0, 30, 31,  0,234,  0,  0, 0,  0,230, 0,  0,  0,223, 0, 0, 2, 2}, // Tapis safe
        {2, 2,235,  0,  0,  0,227,  0,231, 0,226,  0, 0,228,  0,  0, 0, 0, 2, 2}, // Bas fermé mais passage
        {2, 2, 0,  0,217,  0,  0,  0,  0, 0,  0,  0, 0,  0,216,  0, 0, 0, 2, 2}, // Entrée piègeuse
        {2, 2,  2,  2,  2,  2,  2,  2,  0, 0,  0,  0, 2,  2,  2,  2, 2, 2, 2, 2}  // Porte en bas
    },
    // Salon (index 11)
    {    
        {2, 2, 2,338,339, 2,  2,  2,334, 335,  2,  2, 2, 2,   2,   2,415,  2, 2}, // Trou en haut
        {2, 2, 2,336,337, 2,  2,  2,332, 333,  2,  2, 2, 2,   2,   2,414,  0,  2, 2},
        {2, 2, 0,  0,  0, 0,  0,   0,  0,  0,  0,  0, 0, 0,   0,   0,413,  0,  2, 2},
        {2, 2, 0,  0,  0, 0,398, 308,309,310,311,398, 0, 0, 304, 305,  0,  0,  2, 2},
        {2, 2, 0,  0,  0, 0,399, 312,313,314,315,399, 0, 0, 302, 303,  0,  0,  2, 2},
        {2, 2, 0,  0,  0, 0,  0,   0,  0,  0,  0,  0, 0, 0, 324, 325,  0,  0,  2, 2},
        {0, 0, 0,  0,  0, 0,  0,   0,  0,  0,  0,  0, 0, 0,   0,   0,  0,  0,  2, 2},
        {0, 0, 0,  0,  0, 0,  0,   0,  0,  0,  0,  0, 0, 0,   0,   0,  0,437,438, 2},
        {0, 0, 0,  0,  0, 0,  0,   0,  0,  0,  0,  0, 0, 0,   0,   0,  0,435,436, 2},
        {2, 2,402,403, 0, 0,  0,   0,  0,  0,  0,  0, 0, 0,   0,   0,  0,  0,  2, 2},
        {2, 2,400,401, 0, 0,  0,   0,  0,  0,  0,  0, 0, 0,   0,   0,384,385,  2, 2},
        {2, 2,324,325, 0,404,405,406,  0,  0,  0,  0, 0, 0, 380, 381,382,383,  2, 2},
        {2, 2, 0,  0,  0,407,408,409,  0,  0,  0,  0, 0, 0, 378, 379,  0,  0,  2, 2}, // Bas fermé
        {2, 2, 0,  0,  0,410,411,412,  0,  0,  0,  0, 0, 0,   0,   0,  0,  0,  2, 2},
        {2, 2, 2,  2,  2, 2,  2,   2,  2,  2,  2,  2, 2, 2,   2,   2,  2,  2,  2, 2}
    }
};

int maps[NB_LEVELS][MAP_HEIGHT][MAP_WIDTH];

int dialogueStep = 0;
int dialogueStep_sortie1 = 0;
int dialogue_hasDoudou = 0;

int dialogue_statue_haut = 0;
int dialogue_statue_bas = 0;
int dialogue_entree_labyrinthe = 0;
int dialogue_max_objet = 0;
int max_objets = 0;
int dialogue_salle_manger = 0;
int dialogue_chambre_parents = 0;
int toucheRelache = 0;
int hasDoudou = 0;
int showInteractPrompt = 0;
int showInteractPrompt2 = 0;
int showInteractPrompt3 = 0;
int showInteractPromptTente = 0;
int show_interact_prompt_statue_haut = 0;
int show_interact_prompt_statue_bas = 0;
int show_interact_prompt_eau = 0;
int show_interact_prompt_dessin = 0;
int show_interact_prompt_livre = 0;
int interact_statue_haut = 0;
int interact_statue_bas = 0;
int interact_truc_vert = 0;
int interact_spider = 0;
int interact_pain = 0;
int interact_heart = 0;
int interact_eye = 0;
int interact_os = 0;
int interact_coeur_rouge = 0;
int has_water = 0;
int has_drawing = 0;
int statue_has_water = 0;
int statue_has_drawing = 0;

int interact_porte_fin = 0;
int fin_du_jeu = 0;
int interact_mur_fin = 0;
int interraction_maman_fin =0;
int dialogue_maman = 0;
int dialogue_maman_2 = 0;
int dialogue_Step_fin = 0;
int estAdulte = 0; // 0 jeune 1 vieux
int menu_fin = 0;
int ellipse = 0;

SDL_Rect doudouRect = { 200, 150, 12, 12 };

static int playerDir = 0;       // 0=Bas, 1=Gauche, 2=Droite, 3=Haut
static int isPlayerMoving = 0;  // 0=Immobile, 1=Marche

int screamer = 0;
    
int has_truc_vert = 0;
int has_spider = 0;
int has_pain = 0;
int has_heart = 0;
int has_eye = 0;
int chaudron_has_truc_vert = 0;
int chaudron_has_spider = 0;
int chaudron_has_pain = 0;
int chaudron_has_heart = 0;
int chaudron_has_eye = 0;
int interact_chaudron_cuisiner = 0;
int has_interact_livre = 0;
int chaudron_anim = 0;
Uint32 debut_anim_chaudron = 0;
int cuisiner = 0;
int dialogue_chaudron_true = 0;
int dialogue_chaudron_false = 0;
int plat = 0;
int plat_pret_a_servir = 0;    // 0 = Rien, 1 = Soupe prête, 2 = Pain prêt (sur le chaudron)
int type_resultat_cuisson = 0; // 0 = Rien, 1 = Réussite, 2 = Raté
int has_soupe = 0;
int has_pain_chagrin = 0;
int has_os = 0;
int chaudron_has_os = 0;
int has_coeur_rouge = 0;
int chaudron_has_coeur_rouge = 0;
int bouche_has_soupe = 0;
int bouche_has_pain = 0;
int interact_bouche = 0;
int dialogue_entree_SAM = 0;

int carafeX = -1;
int carafeY = -1;
int dessinX = -1;
int dessinY = -1;
int truc_vertX = -1;
int truc_vertY = -1;
int painX = -1;
int painY = -1;
int spiderX = -1;
int spiderY = -1;
int heartX = -1;
int heartY = -1;
int eyeY = -1;
int eyeX = -1;
int chaudronGX = -1;
int chaudronGY = -1;
int chaudronDX = -1;
int chaudronDY = -1;
int osX = -1;
int osY = -1;
int coeur_rougeX = -1;
int coeur_rougeY = -1;

int showInteractPromptObjetTableau = 0;
int showInteractTableau = 0;
int showInteractImpossibleObjet = 0;
int whichTableauPiece = 0;

int cpt_piece_tableau = 4;

// --- VARIABLES PROJECTILES ---
Projectile projectiles[MAX_PROJECTILES];
int projectileSpawnTimer = 0;
#define PROJECTILE_SPAWN_RATE 60
#define PROJECTILE_SPEED 2.5f

// --- VARIABLES SALLE À MANGER (3 voies style Subway Surfers) ---
int onTable = 0;           // 0 = pas sur la table, 1 = sur la table
int currentLane = 1;       // 0 = gauche (col 8), 1 = milieu (col 9-10), 2 = droite (col 11)
int laneKeyReleased = 1;   // Anti-rebond pour changement de voie
int spaceKeyReleased = 1;  // Anti-rebond pour saut (ESPACE)
float tableLaneX[3];       // Positions X des 3 voies (calculées à l'init)
#define TABLE_TOP_ROW 2    // Rangée du début de la table
#define TABLE_BOT_ROW 12   // Rangée de la fin de la table
#define TABLE_SPEED 1.2f   // Vitesse de montée auto sur la table

// --- DÉCLARATIONS FONCTIONS ---
int isInOrganicPath(int gridX, int gridY);
int estNourriture(int tuile);
int estChaise(int tuile);
void getHitboxObjet(int tuile, int colonne, int ligne, float *hx, float *hy, float *hw, float *hh);
int collisionObstacles(float x, float y, int largeur, int hauteur);

int TuilesNotSpecial[] = {0, 1, 2, 446};
int tailleTuilesNotSpecial = (int)sizeof(TuilesNotSpecial) / (int)sizeof(TuilesNotSpecial[0]);

int papaReveil = 0;
int forceSleep = 0;
int affichePapaReveil = 0;
int showInteractTelecommande = 0;
int hasTelecommande = 0;

int dialoguePasTelecommande = 0;
int interactTelecommandeTurnOn = 0;
int teleOn = 0;
Uint32 debutTeleOn = 0;
int salonPattern[MAP_HEIGHT][MAP_WIDTH] = {0};
Uint32 tempsTeleOn = 10000;
int aFiniSalon = 0;

int showInteractPendule = 0;
Uint32 debutPendule = 0;
int penduleEnCours = 0;

// --- INITIALISATION ---
int etapeChargement = 0;

int InitGameStepByStep(SDL_Renderer *renderer, int *pourcentage) {
    if (etapeChargement == 0) {
        player.x = 80;
        player.y = 50; 
        player.w = 12; 
        player.h = 14;
        dialogueStep = 1; 
        toucheRelache = 0; 
        estAdulte = 0;
        hasDoudou = 0; 
        screamer = 0;
        *pourcentage = 10;
    }
    
    else if (etapeChargement == 1) {
        fantome.x = 8 * TILE_SIZE; fantome.y = 11 * TILE_SIZE;
        fantome.w = 15; fantome.h = 15; fantome.direction = 0; fantome.timer = 0;
         InitProjectiles();
        onTable = 0;
        currentLane = 1;
        laneKeyReleased = 1;
        spaceKeyReleased = 1;
        // Positions X des 3 voies : gauche (col 8), milieu (entre col 9-10), droite (col 11)
        tableLaneX[0] = 8 * TILE_SIZE + 2;            // Voie gauche
        tableLaneX[1] = 9 * TILE_SIZE + (TILE_SIZE/2); // Voie milieu
        tableLaneX[2] = 11 * TILE_SIZE + 2;            // Voie droite
        *pourcentage = 20;
    }
    else if (etapeChargement == 2) {
        SDL_Surface *surface = SDL_LoadBMP("assets/tuille_into.bmp");
        if (surface) {
            SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
            tilesetTexture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
        *pourcentage = 35;
    }
    else if (etapeChargement == 3) {
        SDL_Surface *surfScreamer = SDL_LoadBMP("assets/screamer.bmp");
        if (surfScreamer) {
            Uint32 colorkey = SDL_MapRGB(surfScreamer->format, 255, 0, 255);
            SDL_SetColorKey(surfScreamer, SDL_TRUE, colorkey);
            textureScreamer = SDL_CreateTextureFromSurface(renderer, surfScreamer);
            SDL_FreeSurface(surfScreamer);
        }
        *pourcentage = 60;
    }
    else if (etapeChargement == 4) {
        SDL_Surface *surface = SDL_LoadBMP("assets/livre.bmp");
        if (surface) {
            SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
            textureLivre = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
        *pourcentage = 80;
    }
    else if (etapeChargement == 5) {
        sonPickUp = chargement_son_item_pick_up();
        sonOpenDoor = chargement_son_door_open();
        sonCloseDoor = chargement_son_door_close();
        sonScreamer = chargement_son_screamer();
        sonPendule = chargement_son_pendule();
        *pourcentage = 90;
    }
    else if (etapeChargement == 6) {
        MusicInterior = chargement_son_ambiance();
        MusicExterior = chargement_son_exterieur();
        *pourcentage = 100;
    }
    else if (etapeChargement == 7) {
        etapeChargement = 0; 
        return 1; 
    }
    
    etapeChargement++;
    return 0;
}

// Fonction utilitaire collision
int isWall(float x, float y)
{
    int caseX = (int)x / TILE_SIZE;
    int caseY = (int)y / TILE_SIZE;

    int localX = (int)x % TILE_SIZE;
    int localY = (int)y % TILE_SIZE;

    // Sécurité bornes map
    if (caseX < 0 || caseX >= MAP_WIDTH || caseY < 0 || caseY >= MAP_HEIGHT)
    {
        return 1;
    }

    int type = maps[currentLevel][caseY][caseX];
    // int typeBelow = maps[currentLevel][caseY+1][caseX];
    int type_pattern = maps_patern[currentLevel][caseY][caseX];

    // --- LOGIQUE SPÉCIALE LABYRINTHE (83) ---
    if (type == 83)
    {

        // 1. TOIT (Perspective : Le haut est toujours solide)
        if (localY < 6)
        {
            return 1;
        }

        // 2. ANALYSE ENVIRONNEMENT (haut dessus 82 et en dessous 82)
        int upIs82 = 0;
        int downIs82 = 0;

        if (caseY - 1 >= 0)
        {
            if (maps[currentLevel][caseY - 1][caseX] == 82)
            {
                upIs82 = 1;
            }
        }
        if (caseY + 1 < MAP_HEIGHT)
        {
            if (maps[currentLevel][caseY + 1][caseX] == 82)
            {
                downIs82 = 1;
            }
        }
        if (upIs82 && downIs82)
        {

            // On regarde si les pieds sont physiquement plus bas que le mur
            float wallPixelBottom = (caseY + 1) * TILE_SIZE;

            if ((player.y + player.h) >= wallPixelBottom)
            {
                return 0; // Je suis devant le mur, je passe
            }

            int localX = (int)x % TILE_SIZE;
            float wallPixelLeft = caseX * TILE_SIZE;
            float wallPixelRight = (caseX + 1) * TILE_SIZE;

            // Test Bord GAUCHE
            if (localX < 4)
            {
                int leftIs82 = 0;
                if (caseX > 0)
                {
                    if (maps[currentLevel][caseY][caseX - 1] == 82)
                    {
                        leftIs82 = 1;
                    }
                }

                if (leftIs82)
                {
                    // Si épaule DROITE est encore DANS le mur (+ marge 2px)
                    if ((player.x + player.w) > (wallPixelLeft + 2))
                    {
                        return 0;
                    }
                    return 1; // Sinon je tape
                }
            }

            // Test Bord DROIT
            if (localX > 12)
            {
                int rightIs82 = 0;
                if (caseX < MAP_WIDTH - 1)
                {
                    if (maps[currentLevel][caseY][caseX + 1] == 82)
                    {
                        rightIs82 = 1;
                    }
                }

                if (rightIs82)
                {
                    // Si épaule GAUCHE est encore DANS le mur (- marge 2px)
                    if (player.x < (wallPixelRight - 2))
                    {
                        return 0;
                    }
                    return 1; // Sinon je tape
                }
            }

            // Si on n'est ni devant (pieds), ni en train de glisser (côtés),
            // alors on est DANS le mur -> COLLISION
            return 1;
        }

        // --- CAS STANDARD (Gros murs sans passage haut/bas) ---
        int isSolidBelow = 0;
        if (caseY + 1 < MAP_HEIGHT)
        {
            int typeBelow = maps[currentLevel][caseY + 1][caseX];
            if (typeBelow == 83 || maps_patern[currentLevel][caseY + 1][caseX] == 2)
            {
                isSolidBelow = 1;
            }
        }
        else
        {
            isSolidBelow = 1;
        }

        if (isSolidBelow)
        {
            return 1;
        }

        return 0;
    }

    // --- RESTE DU CODE (Chambre...) ---
    if (type_pattern == 2 || type_pattern == 150)
    {
        int caseY_Below = caseY + 1;
        if (caseY_Below < MAP_HEIGHT)
        {
            int typeBelow = maps[currentLevel][caseY_Below][caseX];
            int typeBelow_pattern = maps_patern[currentLevel][caseY_Below][caseX];
            if (typeBelow_pattern == 2 || typeBelow == 83 || typeBelow_pattern == 150)
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }

        if (localY < 4)
        {
            return 1;
        }
        return 0;
    }

    if (type == 10 || type == 11 || type == 14 || type == 15 || type == 18 || type == 19)
    {
        if (localY < 4)
        {
            return 1;
        }
        return 0;
    }

    if (type == 22 || type == 23)
    {
        if (localY > 2)
        {
            return 1;
        }
        return 0;
    }

    int isTopCorner = (y < player.y + 8) ;
    if (type == 20) {
        int localX = (int)x % TILE_SIZE;
        
        if (localX >= 11) return 0; // On laisse passer complètement à droite
        if (isTopCorner) return 0;  // La tête passe (perspective)
        if(localY<3)return 0;
        
        return 1; 
    }

    // --- COLLISION LAMPE (21) ---
    if (type == 21) {
        int localX = (int)x % TILE_SIZE;
        
        // Ajuste ce "11" (entre 8 et 15) pour définir jusqu'où on peut rentrer par la droite
        if (localX >= 12) return 0; 
        
        if (isTopCorner && (localY >= 5) ) return 0;  // La tête passe au-dessus du pied de la lampe
        
        return 1; // Les pieds bloquent
    }

    // --- COLLISION COMMODE (42, 43) ---
    if (type == 42 || type == 43) {
        int localX = (int)x % TILE_SIZE;
        
        if (localX >= 11) return 0; // La moitié droite est libre
        
        // Le meuble entier bloque les pieds, mais laisse passer la tête
        if (isTopCorner) return 0; 
        
        return 1;
    }

    // --- COLLISION LIT (32, 33, 34, 35) ---
    if (type == 32 || type == 33 || type == 34 || type == 35) {
        // Le lit entier est géré d'un coup !
        if (isTopCorner) return 0; // La tête passe par-dessus les draps
        if( (type == 32 || type == 34 ) && localX<4)return 0;
        if( (type == 33 || type == 35) && localX >11 )return 0;

        return 1; // Les pieds tapent le matelas
    }
    // --- COLLISION TENTE (52 à 57) ---
    if (type >= 52 && type <= 57) {
        
        // 1. Haut Gauche (55) : on laisse passer un peu par la gauche
        if (type == 55) {
            int localX = (int)x % TILE_SIZE;
            // Ajuste ce '8' (ex: 6 ou 10) pour définir jusqu'où tu peux rentrer
            if (localX < 8) return 0; 
            return 1;
        }

        // 2. Haut Milieu et Haut Droite (56, 57) : 100% solide
        if (type == 56 || type == 57) {
            return 1;
        }

        // 3. Bas de la tente (52, 53, 54) : collision basique avec perspective
        if (type == 52 || type == 53 || type == 54) {
            if (isTopCorner) return 0; // La tête passe par-dessus
            return 1; // Les pieds bloquent
        }
    }

    if (type == 80 || type == 81) {
        // La commode est à droite, donc on laisse la moitié GAUCHE libre
        if (localX < 6) return 0; 
        
        // La tête passe par-dessus pour la perspective
        if (isTopCorner) return 0; 
        
        return 1; // Les pieds bloquent
    }

    if ((type >= 46 && type <= 49) || type == 51) {
        
        // 1. CHAISE
        if (type == 51) {
            if (localX < 8 || localX > 11) return 0; 
            if (isTopCorner) return 0;
            return 1; // Verrouille la chaise
        }
        
        // 2. BUREAU
        if (type == 46 && localX < 3) return 0;
        if (type == 49 && localX > 8) return 0;

        static int X_chaise = -1, Y_chaise = -1;
        if (X_chaise == -1) {
            TrouveCoordonnees(&X_chaise, &Y_chaise, 51, 0); 
        }

        if (localY < 2) {
            if (caseX == X_chaise) {
                if (localX < 8 || localX > 11) return 0;
                return 1; 
            }
            return 0; 
        }
        
        if (isTopCorner) return 0; 
        return 1;
    }

    if ((type_pattern == 160 || type_pattern == 163) && currentLevel == 1 ) {
        if(type_pattern == 163 && (localY>11 ))return 0;
        if(localX<2)return 0;
        return 1; // Le haut de la table est 100% solide
    }

    // 2. EXTRÉMITÉS GAUCHE (Couche objets : maps)
    if (type == 71 || type == 72 || type == 172 || type == 173) {
        // On laisse le joueur glisser sur la droite de l'extrémité
        if (localX >= 12) return 0;
        
        if (isTopCorner) return 0; // Perspective
        return 1;
    }

    // 3. EXTRÉMITÉS DROITE (Couche objets : maps)
    if (type == 377) {
        // On laisse le joueur glisser sur la gauche de l'extrémité
        if (localX < 6) return 0; 
        
        if (isTopCorner) return 0; // Perspective
        return 1;
    }

    // --- PETIT VASE / POT DU COULOIR (73) ---
    if (type == 73) {
        if (localX < 4) return 0; // Glisse sur les côtés
        if(localY<2)return 0;
        if (isTopCorner) return 0; 
        return 1;
    }

    if (type >= 378 && type <= 385) {

        if (type >= 382 && type <= 385) {
            
            if ((type == 382 ) && localX < 6) return 0; 
            

            if ((type == 383 ) && localX > 6) return 0;
            

            // if (type == 384 || type == 385) {
            //     if (localY < 9) return 0; 
            //     return 1; 
            // }
            
           
            if (type == 382 || type == 383) {
                if (isTopCorner) return 0;
                return 1; 
            }
        }

        if (type >= 378 && type <= 381) {
            
            
            // if ((type == 380 || type == 381) && localY < 9) return 0;

            
            if ((type == 378 ) && localX < 2) return 0;

            if ((type == 379 ) && localX > 2) return 0; 

            // if (type == 380 || type == 381) {
            //     return 1;
            // }
            
            if (type == 378 || type == 379) {
                if (isTopCorner) return 0; 
                return 1; 
            }
        }
    }
    if(type == 143 ){
        if(localX<9 || isTopCorner)return 0;
        return 1;
    }
    if(type == 144){
        if(localX>6 || isTopCorner)return 0;
        return 1;
    }
    if(type == 386 || type == 387){
        if(y < player.y + 10) return 0; 
        return 1;
    }
    if(type == 134)return 1;
    if(type == 275){
        if(localX>14)return 0;
        return 1;
    }

    if(type == 84){
        if(isTopCorner)return 0;
        return 1;
    }
    if (type == 134 || type == 135) {
        return 1; // Le fond est 100% solide
    }

    if (type == 136 || type == 137 || type == 386 || type == 387) {
        
        if (y < player.y + 8) return 0; 
        
        return 1; 
    }

    if(type == 294 || type == 295){
        if(isTopCorner)return 0;
        if(type == 294 && localX<4)return 0;
        if(type == 295 && localX>12)return 0;
        return 1;
    }
    if(type == 296 || type == 297){
        if(localY<14)return 0;
        if(type == 296 && localX<4)return 0;
        if(type == 297 && localX >12)return 0;
        return 1;
    }
    if(type_pattern == 388 || type_pattern == 389 || type_pattern == 374){
        if(localX<3)return 0;
        return 1;
    }
    if(type == 395 || type == 396){
        if(isTopCorner)return 0;
        if(localX<2)return 0;
        return 1;
    }
    if(type == 245 || type == 246 || type == 247){
        if(isTopCorner)return 0;
        if(type == 245 && localX<3)return 0;
        if(type == 247 && localX>12)return 0;
        return 1;
    }
    if(type == 275 || type == 265 || type == 171 || type == 181){
        if(localX>14)return 0;
        if(type == 181 && localY>4)return 0;
        return 1;
    }
    if(type == 178 || type == 169 || type == 170 || type == 167 || type == 168 || type == 243 || type == 274){
        if(isTopCorner)return 0;
        if(type == 274){
            if(localY<4)return 0;
            if(localX>12)return 0;
            return 1;
        }
        return 1;
    }
    if(type_pattern == 160 || type_pattern == 161 || type_pattern == 162 || type_pattern == 163 || type_pattern == 164 || type_pattern == 165 || type_pattern == 166){
        if( (type_pattern == 161 || type_pattern == 163 || type_pattern == 166)){
            if(localY>13)return 0;
            if(type_pattern == 163 && localX<2 )return 0;
            if(type_pattern == 161 && localX>13)return 0;
        }
        if((type_pattern == 160 || type_pattern == 162) && localX<2 )return 0;
        if((type_pattern == 164 || type_pattern == 165 ) && localX>13)return 0;
        return 1;
    }
    if(type == 264){
        if(localX<3)return 0;
        if(isTopCorner)return 0;
        return 1;
    }
    if(type == 207 || type == 208 || type == 209){
        if(type == 207 && localX<13)return 0;
        if(type == 209 && localX>12)return 0;
        if(isTopCorner)return 0;
        return 1;
    }
    if ((type >= 187 && type <= 206) || (type >= 219 && type <= 222)) {
        
        // Ta logique de collision ici (par exemple :)
        if (isTopCorner) return 0;
        return 1;
    }
    if(type == 185 || type == 186 || type == 218){
        if(localX>12)return 0;
        if(isTopCorner)return 0;
        return 1;
    }
    if(type == 399){
        if(isTopCorner)return 0;
        if(localX<4)return 0;
        return 1;
    }
    if(type >= 312 && type <= 315){
        if(isTopCorner)return 0;
        return 1;
    }
    if ((type >= 413 && type <= 415) || (type >= 652 && type <= 654) ) {
        if (type == 415) {
            return 1; 
        }
        // On calcule sur quelle case se trouve exactement le milieu du joueur
        float playerCenterX = player.x + (player.w / 2.0f);
        int playerCenterTileX = (int)playerCenterX / TILE_SIZE;

        if (playerCenterTileX == caseX) {
            // Le joueur est de face : on bloque ses pieds, mais la tête passe 
            if (isTopCorner) return 0;
            return 1;
        } 
        else {
            // Le joueur frôle l'horloge en marchant sur la case de gauche ou de droite :
            // On le laisse glisser sur les bords
            if (localX < 4 || localX > 11) return 0;
            
            if (isTopCorner) return 0;
            return 1;
        }
    }
    if((type >= 302 && type <= 303) || (type == 324 || type == 325) || type == 400 || type == 401){
        if((type == 302 || type == 324) && localX<1)return 0;
        if( (type == 303 || type == 325 || type == 401) && localX>14)return 0;
        if(isTopCorner)return 0;
        return 1;
    }
    if(type >= 404 && type <= 412){
        if( (type >= 410 && type <= 412) && localY>6)return 0;
        if(isTopCorner)return 0;
        return 1;
    }
    if(type >= 435 && type <= 438){
        if((type == 435 || type == 437) && localX<4 )return 0;
        if(isTopCorner)return 0;
        return 1;
    }
    // 156 157 158 159
    if(type >= 156 && type <= 159){
        if((type == 156 || type == 158) && localX<7)return 0;
        if( (type == 157 || type == 159) && localX>10)return 0;
        if(isTopCorner)return 0;
        return 1;
    }

    return 0;
}

// -- Fonctions pour vérification de directions et d'emplacements

// Entrer le xDébut et xFin du trou, CurrLvl correspond au lvl courant (<==> currentLevel),
// yDiff correpond à la marge de détection que vous accordez au joueur pour passer à l'autre salle
// à combien de blocks de la porte vous accordez le joueur pour pouvoir passer d'une salle à l'autre

int IsLocationUp(int xDebut, int xFin, int CurrLvl, int yDiff)
{
    return (currentLevel == CurrLvl && player.y < yDiff && player.x >= (xDebut * TILE_SIZE) && player.x <= (xFin * TILE_SIZE));
}

int IsLocationDown(int xDebut, int xFin, int CurrLvl, int yDiff)
{
    return (currentLevel == CurrLvl && player.y > (MAP_HEIGHT * TILE_SIZE) - yDiff && player.x >= (xDebut * TILE_SIZE) && player.x <= (xFin * TILE_SIZE));
}

int IsLocationRight(int yDebut, int yFin, int CurrLvl, int xDiff)
{
    return (currentLevel == CurrLvl && player.x > (MAP_WIDTH * TILE_SIZE) - xDiff && player.y >= (yDebut * TILE_SIZE) && player.y <= (yFin * TILE_SIZE));
}

int IsLocationLeft(int yDebut, int yFin, int CurrLvl, int xDiff)
{
    return (currentLevel == CurrLvl && player.x < xDiff && player.y >= (yDebut * TILE_SIZE) && player.y <= (yFin * TILE_SIZE));
}

void ManageMusic()
{
    int newZoneState = 0;
    Mix_Volume(-1, globalVolumeBruitages);
    // if(screamer){
    //     Mix_Volume(2, 64); // On commence volume à 0
    // }
    // else{
    //     Mix_Volume(2, 0);
    // }

    // Si on est dans les niveaux 5, 6, 7 ou 8, on est à l'EXTERIEUR
    if (currentLevel >= 5 && currentLevel <= 8)
    {
        newZoneState = 1;
    }
    else
    {
        newZoneState = 0;
    }

    // Si la zone a changé (ex: on passe de 4 à 5, ou au démarrage du jeu)
    if (newZoneState != currentZoneState)
    {
        Mix_VolumeMusic(globalVolumeMusique);
        if (newZoneState == 1)
        {
            if (MusicExterior)
                Mix_FadeInMusic(MusicExterior, -1, 1000);
        }
        else
        {
            Mix_VolumeMusic(globalVolumeMusique);
            if (MusicInterior)
                Mix_FadeInMusic(MusicInterior, -1, 1000);
        }
        currentZoneState = newZoneState;
    }
}

// --- UPDATE ---
void UpdateGame(void)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
     // bouche_has_pain = 1;
     // bouche_has_soupe = 1;
    // hasTelecommande = 1;
    // cpt_piece_tableau = 4;
     // hasDoudou = 1;
    // statue_has_water=1;
    // statue_has_drawing=1;
    // currentLevel = 11;

    if (chaudron_anim == 1)
    {
        Uint32 tempsActuel = SDL_GetTicks();

        if (tempsActuel - debut_anim_chaudron > 5000)
        {

            chaudron_anim = 0;

            if (currentLevel == 3)
            {
                maps[3][chaudronGY][chaudronGX] = 296;
                maps[3][chaudronDY][chaudronDX] = 297;
            }

            if (type_resultat_cuisson == 1 || type_resultat_cuisson == 3)
            {
                plat++;
                if (type_resultat_cuisson == 1) {
                    plat_pret_a_servir = 1;
                }
                else if (type_resultat_cuisson == 3) {
                    plat_pret_a_servir = 2; 
                }
                dialogue_chaudron_true = 1;
            }

            else if (type_resultat_cuisson == 2)
            {
                dialogue_chaudron_false = 1;

                if (chaudron_has_pain == 1)
                {
                    maps[3][painY][painX] = 175;
                    chaudron_has_pain = 0;
                }
                if (chaudron_has_spider == 1)
                {
                    maps[3][spiderY][spiderX] = 242;
                    chaudron_has_spider = 0;
                }
                if (chaudron_has_eye == 1)
                {
                    maps[3][eyeY][eyeX] = 265;
                    chaudron_has_eye = 0;
                }
                if (chaudron_has_heart == 1)
                {
                    maps[3][heartY][heartX] = 244;
                    chaudron_has_heart = 0;
                }
                if (chaudron_has_truc_vert == 1)
                {
                    maps[3][truc_vertY][truc_vertX] = 177;
                    chaudron_has_truc_vert = 0;
                }
                if (chaudron_has_os == 1)
                {
                    maps[3][osY][osX] = 176;
                    chaudron_has_os = 0;
                }
                if (chaudron_has_coeur_rouge == 1)
                {
                    maps[3][coeur_rougeY][coeur_rougeX] = 243;
                    chaudron_has_coeur_rouge = 0;
                }

                if (has_eye == 0 && has_pain == 0 && has_spider == 0 && has_truc_vert == 0 && 
                    has_heart == 0 && has_os == 0 && has_coeur_rouge == 0) 
                {
                    max_objets = 0;
                }
            }

            type_resultat_cuisson = 0;
            toucheRelache = 0;
        }
    }

    if(ellipse == 1){
            if (state[SDL_SCANCODE_RETURN] && toucheRelache) 
                {
                    player.x = 80;
                    player.y = 50;
                    currentLevel = 0;

                    // 2. On change l'état (ex: pour charger le nouveau sprite)
                    //estAdulte = 1; 
                    ellipse = 0; 
                    toucheRelache = 0;
                    interact_mur_fin = 1;
                    estAdulte = 1;
                    fin_du_jeu = 1;
                }
            else if (!state[SDL_SCANCODE_RETURN]) // Si la touche n'est pas appuyée
            {
                toucheRelache = 1; // Alors on autorise le prochain appui
            }
            return; //on bloque le jeu et on reste dans l'ellipse
    }  
    dialogue_Step_fin = 0;
    if (dialogue_Step_fin > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_Step_fin = 0;
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }   
    if (dialogueStep > 0)
    {

        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogueStep++;
                if (dialogueStep > 2)
                {
                    dialogueStep = 0;
                }
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    dialogueStep_sortie1 = 0;
    if (dialogueStep_sortie1 > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogueStep_sortie1 = 0; // On ferme le dialogue
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (dialogue_maman > 0){
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                toucheRelache = 0;
                dialogue_maman = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (dialogue_maman_2 > 0)
    {
        // on bloque le joueur
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_maman_2++; // on passe au texte suivant
                toucheRelache = 0;   // eviter que le texte defile
                
                // Si + 3eme texte on ferme tout
                if (dialogue_maman_2 > 3) 
                {
                    dialogue_maman_2 = 0; 
                    ellipse = 1;
                    toucheRelache = 0;
                }
            }
        }
        else if (!state[SDL_SCANCODE_RETURN]) { toucheRelache = 1; }
        return;

    }
    if (dialogue_statue_haut > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_statue_haut++; // On ferme le dialogue
                interact_statue_haut = 1;
                if (dialogue_statue_haut > 2)
                {
                    dialogue_statue_haut = 0;
                }

                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (dialogue_statue_bas > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_statue_bas++; // On ferme le dialogue
                interact_statue_bas = 1;
                if (dialogue_statue_bas > 2)
                {
                    dialogue_statue_bas = 0;
                }

                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (dialogue_hasDoudou > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_hasDoudou++;
                if (dialogue_hasDoudou > 3)
                {
                    dialogue_hasDoudou = 0;
                }

                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    dialogue_entree_labyrinthe = 0;
    if (dialogue_entree_labyrinthe > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_entree_labyrinthe = 0; // On ferme le dialogue
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    dialogue_entree_SAM = 0;
    if (dialogue_entree_SAM > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_entree_SAM = 0; // On ferme le dialogue
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (showInteractImpossibleObjet > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                showInteractImpossibleObjet = 0;
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (dialogue_max_objet > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_max_objet = 0;
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (livreOuvert == 1)
    {
        if (state[SDL_SCANCODE_E])
        {
            if (toucheRelache)
            {
                livreOuvert = 0;
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (dialogue_chaudron_false > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_chaudron_false++;
                if (dialogue_chaudron_false > 2)
                {
                    dialogue_chaudron_false = 0;
                }
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    if (dialogue_chaudron_true > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheRelache)
            {
                dialogue_chaudron_true = 0;
                toucheRelache = 0;
            }
        }
        else
        {
            toucheRelache = 1;
        }
        return;
    }
    
    dialoguePasTelecommande = 0; // Pour le dialogue quand le joueur veut aller dans le salon sans la télécommande
    interactTelecommandeTurnOn = 0;

    

    float dirX = 0;
    float dirY = 0;

    if (state[SDL_SCANCODE_UP])
        dirY -= 1;
    if (state[SDL_SCANCODE_DOWN])
        dirY += 1;
    if (state[SDL_SCANCODE_LEFT])
        dirX -= 1;
    if (state[SDL_SCANCODE_RIGHT])
        dirX += 1;

    isPlayerMoving = 0;
    if (dirX != 0 || dirY != 0) {
        isPlayerMoving = 1;
        
        if (dirY > 0)      playerDir = 0; // Vers le Bas
        else if (dirY < 0) playerDir = 3; // Vers le Haut
        else if (dirX < 0) playerDir = 1; // Vers la Gauche
        else if (dirX > 0) playerDir = 2; // Vers la Droite
    }
    static int premiereFoisAllumeeTele = 0;

    Uint32 tempsMtn = SDL_GetTicks();

    if(currentLevel == 11 && player.x >= 28 && player.x <= 32 && !hasTelecommande){
        if(dirX > 0){
            dirX = 0;
        }
        dialoguePasTelecommande = 1;
    }

    else if(currentLevel == 11 && hasTelecommande && player.x >= 28 && player.x <= 32 && premiereFoisAllumeeTele == 0) {
        if(!teleOn)interactTelecommandeTurnOn = 1;
        if( (premiereFoisAllumeeTele == 0 || (tempsMtn - debutTeleOn) <= tempsTeleOn ) && dirX > 0) {
            dirX = 0;
        }
    }

    // Si on bouge sur les deux axes en même temps (Diagonale)
    if (dirX != 0 && dirY != 0)
    {
        // On multiplie par 0.707 (environ 1/racine(2)) pour ralentir
        dirX *= 0.7071f;
        dirY *= 0.7071f;
    }

    // Petit fallback : permettre de sauter sur la table si on est en bas (sécurité)
    if (currentLevel == 4 && dialogue_chambre_parents < 1 && !onTable) {
        int pGridY_fallback = (int)(player.y + player.h / 2) / TILE_SIZE;
        if (pGridY_fallback >= TABLE_TOP_ROW && state[SDL_SCANCODE_SPACE] && spaceKeyReleased) {
            onTable = 1;
            player.x = 10 * TILE_SIZE + 2;  // Centre de la colonne 10
            player.y = TABLE_BOT_ROW * TILE_SIZE;  // Premiere ligne (bas de la table)
            spaceKeyReleased = 0;
            goto skip_normal_movement;
        }
    }

    // if (dirX != 0 || dirY != 0) {
    //     Mix_Volume(2, 32);
    // } else {
    //     Mix_Volume(2, 0);
    // }

    // 3. On applique la VITESSE
    float currentSpeed = PLAYER_SPEED;
    
    // === SALLE À MANGER : MODE 3 VOIES ===
    if (currentLevel == 4 && dialogue_chambre_parents < 1) {
        // Anti-rebond ESPACE
        if (!state[SDL_SCANCODE_SPACE]) {
            spaceKeyReleased = 1;
        }
        
        if (!onTable) {
            // Vérifier si le joueur est en haut de la salle (a traversé la table)
            int pGridY = (int)(player.y + player.h / 2) / TILE_SIZE;
            
            if (pGridY < TABLE_TOP_ROW) {
                // Zone au-dessus de la table : mouvement libre vers la chambre des parents
                float nextX = player.x + (dirX * currentSpeed);
                float nextY = player.y + (dirY * currentSpeed);
                
                float margin = 1.0f;
                int touchWallX = 0;
                if (isWall(nextX + margin, player.y + margin)) touchWallX = 1;
                if (isWall(nextX + player.w - margin, player.y + margin)) touchWallX = 1;
                if (isWall(nextX + margin, player.y + player.h - margin)) touchWallX = 1;
                if (isWall(nextX + player.w - margin, player.y + player.h - margin)) touchWallX = 1;
                if (!touchWallX) player.x = nextX;
                
                int touchWallY = 0;
                if (isWall(player.x + margin, nextY + margin)) touchWallY = 1;
                if (isWall(player.x + player.w - margin, nextY + margin)) touchWallY = 1;
                if (isWall(player.x + margin, nextY + player.h - margin)) touchWallY = 1;
                if (isWall(player.x + player.w - margin, nextY + player.h - margin)) touchWallY = 1;
                
                // Empêcher de redescendre sur la table
                if (nextY > (TABLE_TOP_ROW - 1) * TILE_SIZE) {
                    touchWallY = 1;
                }
                if (!touchWallY) player.y = nextY;
            } else {
                // Zone en bas : bloqué, doit sauter sur la table (ESPACE)
                // Monter depuis n'importe quelle position en bas (comportement d'origine)
                if (state[SDL_SCANCODE_SPACE] && spaceKeyReleased) {
                    onTable = 1;
                    player.x = 10 * TILE_SIZE + 2;  // Centre de la colonne 10
                    player.y = TABLE_BOT_ROW * TILE_SIZE;  // Premiere ligne (bas de la table)
                    spaceKeyReleased = 0;
                }
            }
        } else {
            // Sur la table : déplacement libre (flèches directionnelles)
            
            float tableSpeed = PLAYER_SPEED * 0.8f;
            
            // Limites de la table - étendues pour que les chaises gèrent la collision
            // Colonne 8 = chaises gauche, Colonne 12 = chaises droite
            float tableLeft = 8 * TILE_SIZE + TILE_SIZE;  // Après les chaises gauche (col 9)
            float tableRight = 12 * TILE_SIZE - player.w; // Avant les chaises droite (col 12)
            
            // Déplacement horizontal (gauche/droite) avec collision obstacles
            float nouvelleX = player.x;
            if (dirX < 0) {
                nouvelleX = player.x - tableSpeed;
            } else if (dirX > 0) {
                nouvelleX = player.x + tableSpeed;
            }
            
            // Vérifier collision avant de bouger en X
            if (!collisionObstacles(nouvelleX, player.y, player.w, player.h)) {
                player.x = nouvelleX;
            }
            
            // Clamper X aux limites de la table (sécurité)
            if (player.x < tableLeft) player.x = tableLeft;
            if (player.x > tableRight) player.x = tableRight;
            
            // Déplacement vertical (haut/bas) avec collision obstacles
            float nouvelleY = player.y;
            if (dirY < 0) {
                nouvelleY = player.y - tableSpeed;  // Monter
            } else if (dirY > 0) {
                nouvelleY = player.y + tableSpeed;  // Descendre
            }
            
            // Vérifier collision avant de bouger en Y
            if (!collisionObstacles(player.x, nouvelleY, player.w, player.h)) {
                player.y = nouvelleY;
            }
            
            // Clamper le joueur entre le haut et le bas de la table
            if (player.y < TABLE_TOP_ROW * TILE_SIZE) {
                player.y = TABLE_TOP_ROW * TILE_SIZE;
            }
            if (player.y > TABLE_BOT_ROW * TILE_SIZE) {
                player.y = TABLE_BOT_ROW * TILE_SIZE;
            }
            
            // Appuyer sur ESPACE en haut de la table pour descendre
            if (state[SDL_SCANCODE_SPACE] && spaceKeyReleased) {
                if (player.y <= (TABLE_TOP_ROW + 1) * TILE_SIZE) {
                    // Sauter de la table : atterrir au-dessus (rangée 1)
                    onTable = 0;
                    player.y = 1 * TILE_SIZE;  // Au-dessus de la table
                    player.x = 9 * TILE_SIZE;  // Centre de la porte
                    spaceKeyReleased = 0;
                    InitProjectiles();  // Arrêter les projectiles
                }
            }
        }
        // Skip le reste du mouvement normal
        goto skip_normal_movement;
    }

    // === SALLE À MANGER : PHASE 2 (après chambre des parents, pas de projectiles) ===
    if (currentLevel == 4 && dialogue_chambre_parents >= 1) {
        // Anti-rebond ESPACE
        if (!state[SDL_SCANCODE_SPACE]) {
            spaceKeyReleased = 1;
        }
        
        if (!onTable) {
            // Mouvement normal hors de la table
            float nextX = player.x + (dirX * currentSpeed);
            float nextY = player.y + (dirY * currentSpeed);
            
            float margin = 1.0f;
            int touchWallX = 0;
            if (isWall(nextX + margin, player.y + margin)) touchWallX = 1;
            if (isWall(nextX + player.w - margin, player.y + margin)) touchWallX = 1;
            if (isWall(nextX + margin, player.y + player.h - margin)) touchWallX = 1;
            if (isWall(nextX + player.w - margin, player.y + player.h - margin)) touchWallX = 1;
            
            // Collision avec la table ET ses chaises (ne pas marcher dessus)
            // Colonnes 8 à 12 inclus = de x=128 à x=208
            float tblLeft  = 8 * TILE_SIZE;          // Col 8 (chaises gauche)
            float tblRight = 13 * TILE_SIZE;          // Col 12 inclus (chaises droite)
            float tblTop   = TABLE_TOP_ROW * TILE_SIZE;
            float tblBot   = (TABLE_BOT_ROW + 1) * TILE_SIZE;
            float pL = nextX + margin;
            float pR = nextX + player.w - margin;
            float pT = player.y + margin;
            float pB = player.y + player.h - margin;
            if (pR > tblLeft && pL < tblRight && pB > tblTop && pT < tblBot) {
                touchWallX = 1;
            }
            
            if (!touchWallX) player.x = nextX;
            
            int touchWallY = 0;
            if (isWall(player.x + margin, nextY + margin)) touchWallY = 1;
            if (isWall(player.x + player.w - margin, nextY + margin)) touchWallY = 1;
            if (isWall(player.x + margin, nextY + player.h - margin)) touchWallY = 1;
            if (isWall(player.x + player.w - margin, nextY + player.h - margin)) touchWallY = 1;
            
            // Collision Y avec la table ET ses chaises
            pL = player.x + margin;
            pR = player.x + player.w - margin;
            pT = nextY + margin;
            pB = nextY + player.h - margin;
            if (pR > tblLeft && pL < tblRight && pB > tblTop && pT < tblBot) {
                touchWallY = 1;
            }
            
            if (!touchWallY) player.y = nextY;
            
            // Monter sur la table avec ESPACE (depuis le BAS ou le HAUT)
            int pGridY = (int)(player.y + player.h / 2) / TILE_SIZE;
            int pGridX = (int)(player.x + player.w / 2) / TILE_SIZE;
            
            if (state[SDL_SCANCODE_SPACE] && spaceKeyReleased) {
                // Monter depuis le BAS de la table
                if (pGridY == TABLE_BOT_ROW + 1 && pGridX >= 8 && pGridX <= 11) {
                    onTable = 1;
                    player.x = 10 * TILE_SIZE + 2;  // Centre de la colonne 10
                    player.y = TABLE_BOT_ROW * TILE_SIZE;  // Premiere ligne (bas)
                    spaceKeyReleased = 0;
                }
                // Monter depuis le HAUT de la table
                else if (pGridY == TABLE_TOP_ROW - 1 && pGridX >= 8 && pGridX <= 11) {
                    onTable = 1;
                    player.x = 10 * TILE_SIZE + 2;  // Centre de la colonne 10
                    player.y = TABLE_TOP_ROW * TILE_SIZE;  // Premiere ligne (haut)
                    spaceKeyReleased = 0;
                }
            }
        } else {
            // Sur la table en phase 2 : déplacement avec obstacles
            float tableSpeed = PLAYER_SPEED * 0.8f;
            
            // Limites de la table - étendues pour que les chaises gèrent la collision
            float tableLeft = 8 * TILE_SIZE + TILE_SIZE;  // Après les chaises gauche
            float tableRight = 12 * TILE_SIZE - player.w; // Avant les chaises droite
            
            // Déplacement horizontal avec collision obstacles
            float nouvelleX = player.x;
            if (dirX < 0) {
                nouvelleX = player.x - tableSpeed;
            } else if (dirX > 0) {
                nouvelleX = player.x + tableSpeed;
            }
            
            if (!collisionObstacles(nouvelleX, player.y, player.w, player.h)) {
                player.x = nouvelleX;
            }
            if (player.x < tableLeft) player.x = tableLeft;
            if (player.x > tableRight) player.x = tableRight;
            
            // Déplacement vertical avec collision obstacles
            float nouvelleY = player.y;
            if (dirY < 0) {
                nouvelleY = player.y - tableSpeed;
            } else if (dirY > 0) {
                nouvelleY = player.y + tableSpeed;
            }
            
            if (!collisionObstacles(player.x, nouvelleY, player.w, player.h)) {
                player.y = nouvelleY;
            }
            
            // Limites haut/bas de la table
            if (player.y < TABLE_TOP_ROW * TILE_SIZE) {
                player.y = TABLE_TOP_ROW * TILE_SIZE;
            }
            if (player.y > TABLE_BOT_ROW * TILE_SIZE) {
                player.y = TABLE_BOT_ROW * TILE_SIZE;
            }
            
            // Descendre de la table avec ESPACE (aux deux extrémités)
            if (state[SDL_SCANCODE_SPACE] && spaceKeyReleased) {
                // Descendre par le HAUT
                if (player.y <= (TABLE_TOP_ROW + 1) * TILE_SIZE) {
                    onTable = 0;
                    player.y = 1 * TILE_SIZE;
                    player.x = 9 * TILE_SIZE;
                    spaceKeyReleased = 0;
                }
                // Descendre par le BAS
                else if (player.y >= (TABLE_BOT_ROW - 1) * TILE_SIZE) {
                    onTable = 0;
                    player.y = (TABLE_BOT_ROW + 1) * TILE_SIZE;
                    player.x = 9 * TILE_SIZE;
                    spaceKeyReleased = 0;
                }
            }
        }
        // Skip le reste du mouvement normal
        goto skip_normal_movement;
    }

    float nextX = player.x + (dirX * currentSpeed);
    float nextY = player.y + (dirY * currentSpeed);
    
    // Marge de sécurité pour éviter les bugs de collision (1 pixel)
    float margin = 1.0f;
    
    // Collision X avec marge de sécurité
    int touchWallX = 0;
    if (isWall(nextX, player.y))
        touchWallX = 1;
    if (isWall(nextX + player.w, player.y))
        touchWallX = 1;
    if (isWall(nextX, player.y + player.h))
        touchWallX = 1;
    if (isWall(nextX + player.w, player.y + player.h))
        touchWallX = 1;
    
    if (!touchWallX)
        player.x = nextX;
      if (currentLevel == 4) {
        float tblLeft  = 8 * TILE_SIZE;
        float tblRight = 12 * TILE_SIZE;
        float tblTop   = TABLE_TOP_ROW * TILE_SIZE;
        float tblBot   = (TABLE_BOT_ROW + 1) * TILE_SIZE;
        float pL = nextX + margin;
        float pR = nextX + player.w - margin;
        float pT = player.y + margin;
        float pB = player.y + player.h - margin;
        if (pR > tblLeft && pL < tblRight && pB > tblTop && pT < tblBot) {
            touchWallX = 1;
        }
    }
    
    // Collision Y
    int touchWallY = 0;
    if (isWall(player.x, nextY))
        touchWallY = 1;
    if (isWall(player.x + player.w, nextY))
        touchWallY = 1;
    if (isWall(player.x, nextY + player.h))
        touchWallY = 1;
    if (isWall(player.x + player.w, nextY + player.h))
        touchWallY = 1;

    if (!touchWallY)
        player.y = nextY;
    if (currentLevel == 4) {
            float tblLeft  = 8 * TILE_SIZE;
            float tblRight = 12 * TILE_SIZE;
            float tblTop   = TABLE_TOP_ROW * TILE_SIZE;
            float tblBot   = (TABLE_BOT_ROW + 1) * TILE_SIZE;
            float pL = player.x + margin;
            float pR = player.x + player.w - margin;
            float pT = nextY + margin;
            float pB = nextY + player.h - margin;
            if (pR > tblLeft && pL < tblRight && pB > tblTop && pT < tblBot) {
                touchWallY = 1;
            }
        }
        

    skip_normal_movement: ;  // Label pour le saut de mouvement en mode salle à manger

    // if (dirX != 0 || dirY != 0) {
    //     Mix_Volume(2, 64); // On met le son si on bouge
    // } else {
    //     Mix_Volume(2, 0);  // On coupe le son si on arrête
    // }
    
    // --- GESTION SONORE SCREAMER ---
    static int screamerActive = 0;    
    static Uint32 debutScreamer = 0;  

    if( (dirX != 0 || dirY != 0) && papaReveil == 1 && currentLevel == 10){
        player.y = (MAP_HEIGHT * TILE_SIZE) - 30; 
        player.x = 10 * TILE_SIZE;

        // Si le screamer n'est pas déjà affiché on le lance
        if (screamer == 0) {
            screamer = 1; 
            hasTelecommande = 0;                  
            debutScreamer = SDL_GetTicks(); // démarre le chrono
            int telecommandeX = 2;
            int telecommandeY = 4;
            maps[10][telecommandeY][telecommandeX] = 185;
            
            // Lance le son
            if (screamerActive == 0) {
                Mix_Volume(2, 128); 
                Mix_PlayChannel(2, sonScreamer, 0); 
                screamerActive = 1;
            }
        }
    }
    
    if (screamer == 1) {
        if (SDL_GetTicks() - debutScreamer > 2000) {
            screamer = 0; // On coupe l'image
            
            // On coupe le son
            if (screamerActive == 1) {
                Mix_HaltChannel(2); 
                screamerActive = 0;
            }
            forceSleep = 1;
        }
    }

    float distance;

    showInteractPrompt = 0;
    showInteractPrompt2 = 0;
    showInteractPrompt3 = 0;

    if (IsLocationObjet(24, 0, 8, &distance, -1, -1))
    {
        showInteractPrompt = 1;
    }

    else if (IsLocationObjet(24, 0, 16, &distance, -1, -1))
    {
        showInteractPrompt2 = 1;
    }

    else if (IsLocationObjet(24, 0, 12, &distance, -1, -1))
    {
        showInteractPrompt3 = 1;
    }

    float distance_tente;
    showInteractPromptTente = 0;
    if (IsLocationObjet(24, 0, 55, &distance_tente, -1, -1))
    {
        showInteractPromptTente = 1;
    }
    show_interact_prompt_statue_haut = 0;
    show_interact_prompt_statue_bas = 0;
    // --- GESTION PROMPT STATUES (NIVEAU 2) ---
    float distStatueHaut = 9999.0f;
    float distStatueBas = 9999.0f;

    if (IsLocationObjet(24, 2, 84, &distStatueHaut, 17, 6))
    {
        show_interact_prompt_statue_haut = 1;
    }
    if (IsLocationObjet(24, 2, 84, &distStatueBas, 17, 10))
    {
        show_interact_prompt_statue_bas = 1;
    }
    show_interact_prompt_eau = 0;
    show_interact_prompt_dessin = 0;
    float distance_carafe = 9999.0f;
    float distance_dessin = 9999.0f;

    if (carafeX == -1 && carafeY == -1)
        TrouveCoordonnees(&carafeX, &carafeY, 172, 1);
    else
    {
        TrouveCoordonnees(&carafeX, &carafeY, 72, 1);
    }

    if (IsLocationObjet(14, 1, 72, &distance_carafe, -1, -1) && interact_statue_haut == 1)
    {
        show_interact_prompt_eau = 1;
    }

    TrouveCoordonnees(&dessinX, &dessinY, 174, 9);
    if (IsLocationObjet(14, 9, 174, &distance_dessin, -1, -1) && interact_statue_bas == 1)
    {
        show_interact_prompt_dessin = 1;
    }

    show_interact_prompt_livre = 0;
    float distance_livreD = 9999.0f;
    float distance_livreG = 9999.0f; // j'ai créé 2 variable car sinon la detection se faisais sur une seul tuile (pas fou)
    if (IsLocationObjet(20, 3, 179, &distance_livreG, -1, -1))
    {
        show_interact_prompt_livre = 1;
    }
    if (IsLocationObjet(20, 3, 180, &distance_livreD, -1, -1))
    {
        show_interact_prompt_livre = 1;
    }

    interact_truc_vert = 0;
    float distance_truc_vert = 9999.0f;
    TrouveCoordonnees(&truc_vertX, &truc_vertY, 177, 3);
    if (IsLocationObjet(20, 3, 177, &distance_truc_vert, -1, -1) && has_interact_livre == 1 && chaudron_anim == 0)
    {
        interact_truc_vert = 1;
    }

    interact_pain = 0;
    float distance_pain = 9999.0f;
    TrouveCoordonnees(&painX, &painY, 175, 3);
    if (IsLocationObjet(20, 3, 175, &distance_pain, -1, -1) && has_interact_livre == 1 && chaudron_anim == 0)
    {
        interact_pain = 1;
    }

    interact_spider = 0;
    float distance_spider = 9999.0f;
    TrouveCoordonnees(&spiderX, &spiderY, 242, 3);
    if (IsLocationObjet(20, 3, 242, &distance_spider, -1, -1) && has_interact_livre == 1 && chaudron_anim == 0)
    {
        interact_spider = 1;
    }
    interact_eye = 0;
    float distance_eye = 9999.0f;
    TrouveCoordonnees(&eyeX, &eyeY, 265, 3);
    if (IsLocationObjet(20, 3, 265, &distance_eye, -1, -1) && has_interact_livre == 1 && chaudron_anim == 0)
    {
        interact_eye = 1;
    }
    interact_heart = 0;
    float distance_heart = 9999.0f;
    TrouveCoordonnees(&heartX, &heartY, 244, 3);
    if (IsLocationObjet(20, 3, 244, &distance_heart, -1, -1) && has_interact_livre == 1 && chaudron_anim == 0)
    {
        interact_heart = 1;
    }
    interact_chaudron_cuisiner = 0;
    float distance_chaudronHG = 9999.0f;
    float distance_chaudronBG = 9999.0f;
    float distance_chaudronHD = 9999.0f;
    float distance_chaudronBD = 9999.0f;
    TrouveCoordonnees(&chaudronDX, &chaudronDY, 297, 3);
    TrouveCoordonnees(&chaudronGX, &chaudronGY, 296, 3);
    if (IsLocationObjet(20, 3, 296, &distance_chaudronHG, -1, -1))
    {
        interact_chaudron_cuisiner = 1;
    }
    if (IsLocationObjet(20, 3, 297, &distance_chaudronHD, -1, -1))
    {
        interact_chaudron_cuisiner = 1;
    }
    if (IsLocationObjet(20, 3, 294, &distance_chaudronBG, -1, -1))
    {
        interact_chaudron_cuisiner = 1;
    }
    if (IsLocationObjet(20, 3, 295, &distance_chaudronBD, -1, -1))
    {
        interact_chaudron_cuisiner = 1;
    }
    interact_bouche = 0;
    float distance_boucheHG = 9999.0f;
    float distance_boucheHD = 9999.0f;
    float distance_boucheBG = 9999.0f;
    float distance_boucheBD = 9999.0f;
    if (IsLocationObjet(20, 3, 158, &distance_boucheHG, -1, -1))
    {
        interact_bouche = 1;
    }
    if (IsLocationObjet(20, 3, 159, &distance_boucheHD, -1, -1))
    {
        interact_bouche = 1;
    }
    if (IsLocationObjet(20, 3, 156, &distance_boucheBG, -1, -1))
    {
        interact_bouche = 1;
    }
    if (IsLocationObjet(20, 3, 157, &distance_boucheBD, -1, -1))
    {
        interact_bouche = 1;
    }

    interact_os = 0;
    float dist_os = 9999.0f;
    TrouveCoordonnees(&osX, &osY, 176, 3);
    if (IsLocationObjet(20, 3, 176, &dist_os, -1, -1) && has_interact_livre == 1 && chaudron_anim == 0)
    {
        interact_os = 1;
    }

    interact_coeur_rouge = 0;
    float dist_cr = 999;
    TrouveCoordonnees(&coeur_rougeX, &coeur_rougeY, 243, 3);
    if (IsLocationObjet(20, 3, 243, &dist_cr, -1, -1) && has_interact_livre == 1 && chaudron_anim == 0)
    {
        interact_coeur_rouge = 1;
    }
    // --- Calcul pour les pièces du tableau dans le labyrinthe ---
    showInteractPromptObjetTableau = 0;

    // Calcul distance entre joueur et première pièce du tableau (117)
    int morceauTableau1x;
    int morceauTableau1y;
    TrouveCoordonnees(&morceauTableau1x, &morceauTableau1y, 117, 5);

    float distance_morceauTableau1;
    if (IsLocationObjet(16, 5, 117, &distance_morceauTableau1, -1, -1))
        showInteractPromptObjetTableau = 1;

    // Calcul distance entre joueur et deuxième pièce du tableau (118)
    int morceauTableau2x;
    int morceauTableau2y;
    TrouveCoordonnees(&morceauTableau2x, &morceauTableau2y, 118, 6);

    float distance_morceauTableau2;
    if (IsLocationObjet(16, 6, 118, &distance_morceauTableau2, -1, -1))
        showInteractPromptObjetTableau = 1;

    // Calcul distance entre joueur et troisième pièce du tableau (115)
    int morceauTableau3x;
    int morceauTableau3y;
    TrouveCoordonnees(&morceauTableau3x, &morceauTableau3y, 115, 7);

    float distance_morceauTableau3;
    if (IsLocationObjet(16, 7, 115, &distance_morceauTableau3, -1, -1))
        showInteractPromptObjetTableau = 1;

    // Calcul distance entre joueur et quatrième pièce du tableau (116)
    int morceauTableau4x;
    int morceauTableau4y;
    TrouveCoordonnees(&morceauTableau4x, &morceauTableau4y, 116, 8);

    float distance_morceauTableau4;
    if (IsLocationObjet(16, 8, 116, &distance_morceauTableau4, -1, -1))
        showInteractPromptObjetTableau = 1;

    // Ouverture de la salle du niveau 3
    if (cpt_piece_tableau == 4)
    {
        maps_patern[2][7][0] = 138;
        maps_patern[2][8][0] = 138;
    }
    // Calcul distance entre joueur et le tableau

    showInteractTableau = 0;
    float distance_Tableau;
    if ((IsLocationObjet(24, 2, 113, &distance_Tableau, -1, -1) || IsLocationObjet(24, 2, 109, &distance_Tableau, -1, -1)) && cpt_piece_tableau != 4)
    {
        showInteractTableau = 1;
    }

    // Transition tente à la chambre
    if (IsLocationLeft(6, 10, 9, 6 * TILE_SIZE - 8))
    {
        player.x = 15 * TILE_SIZE;
        currentLevel = 0;
    }
    if (interact_statue_haut == 1 && maps[1][carafeY][carafeX] == 172)
    {
        maps[1][carafeY][carafeX] = 72;
    }

    // --- Chambre des parents ---
    showInteractTelecommande = 0;
    int telecommandeX, telecommandeY;
    float distance_Telecommande;
    TrouveCoordonnees(&telecommandeX, &telecommandeY, 185, 10);
    if( IsLocationObjet(16, 10, 185, &distance_Telecommande, -1, -1)){
        showInteractTelecommande = 1;
    }

    // Easter egg pendule
    showInteractPendule = 0;
    static int penduleX = -1; 
    static int penduleY = -1;
    
    if (penduleX == -1 || penduleY == -1 || currentLevel != 11) {
        TrouveCoordonnees(&penduleX, &penduleY, 414, 11);
        if (penduleX == -1) { 
            TrouveCoordonnees(&penduleX, &penduleY, 653, 11);
        }
    }

    float distance_pendule = 9999.0f;
    if (penduleX != -1) {
        if (IsLocationObjet(20, 11, 414, &distance_pendule, penduleX, penduleY)) {
            showInteractPendule = 1;
        }
    }


    if (state[SDL_SCANCODE_E]) {
        if (toucheE_Relache) {
            // Si le joueur est à moins de 16 pixel (une tuile)
            if (distance < 16 && currentLevel == 0)
            {
                // On vérifie si l'armoire est FERMÉE (8) (tuile en haut à gauche de l'armoire)
                // On l'ouvre avec le doudou dedans
                if (maps[0][0][16] == 8 && hasDoudou == 0)
                {
                    if (sonOpenDoor)
                        Mix_PlayChannel(-1, sonOpenDoor, 0);
                    SDL_Delay(250);
                    maps[0][0][16] = 16;
                    maps[0][0][17] = 17;
                    maps[0][1][16] = 18;
                    maps[0][1][17] = 19;
                }
                // Si elle est OUVERTE (16 = avec le doudou dedans), on peut la refermer
                else if (maps[0][0][16] == 16 || maps[0][0][16] == 12)
                {
                    if (sonCloseDoor)
                        Mix_PlayChannel(-1, sonCloseDoor, 0);
                    SDL_Delay(250);
                    maps[0][0][16] = 8;
                    maps[0][0][17] = 9;
                    maps[0][1][16] = 10;
                    maps[0][1][17] = 11;
                }
                else
                {
                    if (sonOpenDoor)
                        Mix_PlayChannel(-1, sonOpenDoor, 0);
                    SDL_Delay(250);
                    maps[0][0][16] = 12;
                    maps[0][0][17] = 13;
                    maps[0][1][16] = 14;
                    maps[0][1][17] = 15;
                }
            }

            if (currentLevel == 1 && distance_carafe <= 14 && has_drawing == 0)
            {
                maps[1][carafeY][carafeX] = 173;
                has_water = 1;
            }

            if (currentLevel == 2 && distStatueHaut <= 24 && has_water == 0 && statue_has_water == 0)
            {
                dialogue_statue_haut = 1;
            }
            else if (currentLevel == 2 && distStatueHaut <= 24 && has_water == 1)
            {
                maps[2][5][17] = 86;
                statue_has_water = 1;
                has_water = 0;
            }

            if (currentLevel == 3 && distance_truc_vert <= 20 && max_objets == 0 && has_interact_livre == 1 && chaudron_anim == 0)
            {
                has_truc_vert = 1;
                maps[3][truc_vertY][truc_vertX] = 293;
                max_objets = 1;
                toucheE_Relache = 0;
            }
            if (currentLevel == 3 && distance_truc_vert <= 20 && max_objets == 1 && has_truc_vert == 0)
            {
                dialogue_max_objet = 1;
            }
            if (currentLevel == 3 && distance_pain <= 20 && max_objets == 0 && has_interact_livre == 1 && chaudron_anim == 0)
            {
                has_pain = 1;
                maps[3][painY][painX] = 166;
                max_objets = 1;
                toucheE_Relache = 0;
            }
            if (currentLevel == 3 && distance_pain <= 20 && max_objets == 1 && has_pain == 0)
            {
                dialogue_max_objet = 1;
            }
            if (currentLevel == 3 && distance_spider <= 20 && max_objets == 0 && has_interact_livre == 1 && chaudron_anim == 0)
            {
                has_spider = 1;
                maps[3][spiderY][spiderX] = 2;
                max_objets = 1;
                toucheE_Relache = 0;
            }
            if (currentLevel == 3 && distance_spider <= 20 && max_objets == 1 && has_spider == 0)
            {
                dialogue_max_objet = 1;
            }
            if (currentLevel == 3 && distance_eye <= 20 && max_objets == 0 && has_interact_livre == 1 && chaudron_anim == 0)
            {
                has_eye = 1;
                maps[3][eyeY][eyeX] = 181;
                max_objets = 1;
                toucheE_Relache = 0;
            }
            if (currentLevel == 3 && distance_eye <= 20 && max_objets == 1 && has_eye == 0)
            {
                dialogue_max_objet = 1;
            }
            if (currentLevel == 3 && distance_heart <= 20 && max_objets == 0 && has_interact_livre == 1 && chaudron_anim == 0)
            {
                has_heart = 1;
                maps[3][heartY][heartX] = 0;
                max_objets = 1;
                toucheE_Relache = 0;
            }
            if (currentLevel == 3 && distance_heart <= 20 && max_objets == 1 && has_heart == 0)
            {
                dialogue_max_objet = 1;
            }
            if (currentLevel == 3 && dist_cr <= 20 && max_objets == 0 && has_interact_livre == 1 && chaudron_anim == 0)
            {
                has_coeur_rouge = 1;
                maps[3][coeur_rougeY][coeur_rougeX] = 0;
                max_objets = 1;
                toucheE_Relache = 0;
            }
            if (currentLevel == 3 && dist_cr <= 20 && max_objets == 1 && has_coeur_rouge == 0)
            {
                dialogue_max_objet = 1;
            }
            if (currentLevel == 3 && dist_os <= 20 && max_objets == 0 && has_interact_livre == 1 && chaudron_anim == 0)
            {
                has_os = 1;
                maps[3][osY][osX] = 160;
                max_objets = 1;
                toucheE_Relache = 0;
            }
            if (currentLevel == 3 && dist_os <= 20 && max_objets == 1 && has_os == 0)
            {
                dialogue_max_objet = 1;
            }
            if (currentLevel == 2 && distStatueBas <= 24 && has_drawing == 0 && statue_has_drawing == 0)
            {
                dialogue_statue_bas = 1;
                toucheE_Relache = 0;
            }
            if (currentLevel == 9 && distance_dessin <= 16 && has_drawing == 0 && has_water == 0 && interact_statue_bas == 1 && chaudron_anim == 0)
            {
                maps[9][dessinY][dessinX] = 1;
                has_drawing = 1;
            }
            if (currentLevel == 9 && distance_dessin <= 16 && has_drawing == 0 && has_water == 1)
            {
                dialogue_max_objet = 1;
            }
            if (currentLevel == 1 && distance_carafe <= 14 && has_drawing == 1 && has_water == 0)
            {
                dialogue_max_objet = 1;
            }
            else if (currentLevel == 2 && distStatueBas <= 24 && has_drawing == 1)
            {
                maps[2][9][17] = 86;
                statue_has_drawing = 1;
                has_drawing = 0;
            }
            if (currentLevel == 3 && distance_livreD <= 20)
            {
                livreOuvert = 1;
                has_interact_livre = 1;
            }
            if (currentLevel == 3 && distance_livreG <= 20)
            {
                livreOuvert = 1;
                has_interact_livre = 1;
            }
            if (currentLevel == 3 && (distance_boucheBD <= 20 || distance_boucheBG <= 20 || distance_boucheHD <= 20 || distance_boucheHG <= 20) && has_pain_chagrin == 1)
            {
                bouche_has_pain = 1;
                max_objets = 0;
                has_pain_chagrin = 0;
            }
            if (currentLevel == 3 && (distance_boucheBD <= 20 || distance_boucheBG <= 20 || distance_boucheHD <= 20 || distance_boucheHG <= 20) && has_soupe == 1)
            {
                bouche_has_soupe = 1;
                max_objets = 0;
                has_soupe = 0;
            }
            if (currentLevel == 3 && (distance_chaudronBD <= 20 || distance_chaudronBG <= 20 || distance_chaudronHD <= 20 || distance_chaudronHG <= 20))
            {
                if (plat_pret_a_servir > 0)
                {
                    
                    if (has_eye)         { maps[3][9][2] = 265;   has_eye = 0; }
                    if (has_pain)        { maps[3][7][7] = 175;   has_pain = 0; }
                    if (has_spider)      { maps[3][2][2] = 242;   has_spider = 0; }
                    if (has_truc_vert)   { maps[3][9][8] = 177;   has_truc_vert = 0; }
                    if (has_heart)       { maps[3][2][16] = 244;  has_heart = 0; } // Coeur noir
                    if (has_os)          { maps[3][7][5] = 176;   has_os = 0; }
                    if (has_coeur_rouge) { maps[3][13][8] = 243;  has_coeur_rouge = 0; }


                    if (has_soupe) has_soupe = 0;
                    if (has_pain_chagrin) has_pain_chagrin = 0;

                    if (plat_pret_a_servir == 1) has_soupe = 1;
                    if (plat_pret_a_servir == 2) has_pain_chagrin = 1;
                    
                    plat_pret_a_servir = 0;
                    max_objets = 1; 
                    toucheE_Relache = 0;
                }
                else
                {
                    if (has_eye) { has_eye = 0; max_objets = 0; chaudron_has_eye = 1; toucheE_Relache = 0; }
                    else if (has_pain) { has_pain = 0; max_objets = 0; chaudron_has_pain = 1; toucheE_Relache = 0; }
                    else if (has_spider) { has_spider = 0; max_objets = 0; chaudron_has_spider = 1; toucheE_Relache = 0; }
                    else if (has_truc_vert) { has_truc_vert = 0; max_objets = 0; chaudron_has_truc_vert = 1; toucheE_Relache = 0; }
                    else if (has_coeur_rouge) { has_coeur_rouge = 0; max_objets = 0; chaudron_has_coeur_rouge = 1; toucheE_Relache = 0; }
                    else if (has_heart) { has_heart = 0; max_objets = 0; chaudron_has_heart = 1; toucheE_Relache = 0; }
                    else if (has_os) { has_os = 0; max_objets = 0; maps[3][osY][osX] = 166; chaudron_has_os = 1; toucheE_Relache = 0; }
                }
            }

            if (currentLevel == 3 && dist_cr <= 20 && max_objets == 1 && has_coeur_rouge == 0) dialogue_max_objet = 1;
            if (currentLevel == 3 && dist_os <= 20 && max_objets == 1 && has_os == 0) dialogue_max_objet = 1;
            else if (distance_tente <= 24 && currentLevel == 0 && maps[0][6][16] == 55)
            {
                currentLevel = 9;
                player.x = 7 * TILE_SIZE;
                player.y = 7 * TILE_SIZE;
            }

            else if (distance_morceauTableau1 <= 16 && currentLevel == 5 && maps[5][morceauTableau1y][morceauTableau1x] == 117)
            {
                if (whichTableauPiece != 0)
                {
                    showInteractImpossibleObjet = 1;
                }
                else
                {
                    whichTableauPiece = 1;
                    maps[5][1][18] = 82;
                }
            }
            else if (distance_morceauTableau2 <= 16 && currentLevel == 6 && maps[6][morceauTableau2y][morceauTableau2x] == 118)
            {
                if (whichTableauPiece != 0)
                {
                    showInteractImpossibleObjet = 1;
                }
                else
                {
                    whichTableauPiece = 2;
                    maps[6][3][13] = 82;
                }
            }
            else if (distance_morceauTableau3 <= 16 && currentLevel == 7 && maps[7][morceauTableau3y][morceauTableau3x] == 115)
            {
                if (whichTableauPiece != 0)
                {
                    showInteractImpossibleObjet = 1;
                }
                else
                {
                    whichTableauPiece = 3;
                    maps[7][morceauTableau3y][morceauTableau3x] = 82;
                }
            }
            else if (distance_morceauTableau4 <= 16 && currentLevel == 8 && maps[8][morceauTableau4y][morceauTableau4x] == 116)
            {
                if (whichTableauPiece != 0)
                {
                    showInteractImpossibleObjet = 1;
                }
                else
                {
                    whichTableauPiece = 4;
                    maps[8][morceauTableau4y][morceauTableau4x] = 82;
                }
            }
            else if (distance_Tableau <= 24 && currentLevel == 2 && whichTableauPiece != 0)
            {
                if (whichTableauPiece == 1)
                {
                    maps[2][1][5] = 109;
                }
                else if (whichTableauPiece == 2)
                {
                    maps[2][1][6] = 110;
                }
                else if (whichTableauPiece == 3)
                {
                    maps[2][2][5] = 107;
                }
                else if (whichTableauPiece == 4)
                {
                    maps[2][2][6] = 108;
                }
                whichTableauPiece = 0;
                cpt_piece_tableau++;
            }

            if(distance_Telecommande <= 16 && currentLevel == 10 && maps[currentLevel][telecommandeY][telecommandeX] == 185){
                 maps[currentLevel][telecommandeY][telecommandeX] = 218;
                 hasTelecommande = 1;
            }

            if(player.x >= 28 && player.x <= 32 && hasTelecommande && !premiereFoisAllumeeTele && interactTelecommandeTurnOn && !aFiniSalon){
                teleOn = 1;
                debutTeleOn = SDL_GetTicks();
                interactTelecommandeTurnOn = 0;
                GestionMemoSalon();
            }
            if(distance_pendule <= 20 && currentLevel == 11 && maps[11][penduleY][penduleX] == 414){
                if (showInteractPendule) {
                    debutPendule = SDL_GetTicks();
                    
                    // Si le son n'est pas déjà en train de jouer, on le lance
                    if (!penduleEnCours) {
                        if (sonPendule) {
                            Mix_Volume(3, 128); // On utilise le canal 3 pour ne pas couper le screamer
                            Mix_PlayChannel(3, sonPendule, 0); 
                        }
                    }
                    penduleEnCours = 1;
                }
            }
            toucheE_Relache = 0; // On verrouille tant qu'on n'a pas lâché E
        }
    }
    else
    {
        toucheE_Relache = 1; // On a lâché la touche E on peut re appuyer
    }

    if (state[SDL_SCANCODE_RETURN] || state[SDL_SCANCODE_KP_ENTER])
    {
        if (toucheEnter_Relache)
        {
            if (distance < 16 && currentLevel == 0)
            {
                if (maps[0][0][16] == 16)
                {
                    if (sonPickUp)
                        Mix_PlayChannel(-1, sonPickUp, 0);
                    SDL_Delay(250);
                    maps[0][0][16] = 12;
                    maps[0][0][17] = 13;
                    maps[0][1][16] = 14;
                    maps[0][1][17] = 15;
                    hasDoudou = 1;
                    dialogue_hasDoudou = 1;
                }
            }
        }
        if (toucheEnter_Relache)
        {

            if (currentLevel == 3 && (distance_chaudronBD <= 20 || distance_chaudronBG <= 20 || distance_chaudronHD <= 20 || distance_chaudronHG <= 20) && interact_chaudron_cuisiner == 1 && has_interact_livre == 1)
            {

                int chaudron_est_vide = (chaudron_has_truc_vert == 0 && chaudron_has_eye == 0 && chaudron_has_spider == 0 && chaudron_has_heart == 0 && chaudron_has_pain == 0 && chaudron_has_coeur_rouge == 0 && chaudron_has_os == 0);

                if (chaudron_est_vide == 0)
                {
                    int recette1_ok = (chaudron_has_truc_vert == 1 && chaudron_has_eye == 1 && chaudron_has_spider == 1 && chaudron_has_heart == 0 && chaudron_has_pain == 0 && chaudron_has_os == 0 && chaudron_has_coeur_rouge == 0);

                    int recette2_ok = (chaudron_has_pain == 1 && chaudron_has_heart == 1 && chaudron_has_truc_vert == 0 && chaudron_has_eye == 0 && chaudron_has_spider == 0 && chaudron_has_os == 0 && chaudron_has_coeur_rouge == 0);

                    if (recette1_ok || recette2_ok)
                    {

                        debut_anim_chaudron = SDL_GetTicks();
                        chaudron_anim = 1;
                        if (recette1_ok == 1) {
                            type_resultat_cuisson = 1; // Code 1 = Soupe
                        } else {
                            type_resultat_cuisson = 3; // Code 3 = Pain
                        }

                        chaudron_has_eye = 0;
                        chaudron_has_heart = 0;
                        chaudron_has_pain = 0;
                        chaudron_has_spider = 0;
                        chaudron_has_truc_vert = 0;

                        max_objets = 0;
                    }
                    else
                    {
                        debut_anim_chaudron = SDL_GetTicks();
                        chaudron_anim = 1;
                        type_resultat_cuisson = 2;
                    }
                }
            }
            toucheEnter_Relache = 0;
        }
    }
    else
    {
        toucheEnter_Relache = 1;
    }

    Uint32 tempsActuel = SDL_GetTicks();
    if (chaudron_anim == 1 && currentLevel == 3)
    {
        interact_chaudron_cuisiner = 0;
        if (tempsActuel - debut_anim_chaudron >= 5000)
        {
            maps[3][chaudronGY][chaudronGX] = 296;
            maps[3][chaudronDY][chaudronDX] = 297;
        }

        else
        {
            int frame = (tempsActuel / 300) % 2;

            if (frame == 0)
            {
                maps[3][chaudronGY][chaudronGX] = 298;
                maps[3][chaudronDY][chaudronDX] = 299;
            }
            else
            {
                maps[3][chaudronGY][chaudronGX] = 300;
                maps[3][chaudronDY][chaudronDX] = 301;
            }
        }
    }
    if (penduleEnCours == 1 && currentLevel == 11 && penduleX != -1 && penduleY != -1) {
        if (tempsActuel - debutPendule >= 19000) {
            // Fin du temps : Retour à la normale
            maps[11][penduleY-1][penduleX] = 415;
            maps[11][penduleY][penduleX]   = 414; 
            maps[11][penduleY+1][penduleX] = 413;
            penduleEnCours = 0;
        }
        else {
            // Animation : Alternance toutes les 300ms
            int frame = (tempsActuel / 1100) % 2;
            if(frame == 0){
                maps[11][penduleY-1][penduleX] = 415;
                maps[11][penduleY][penduleX]   = 414; 
                maps[11][penduleY+1][penduleX] = 413;
            }
            else{
                maps[11][penduleY-1][penduleX] = 654;
                maps[11][penduleY][penduleX]   = 653; 
                maps[11][penduleY+1][penduleX] = 652;
            }
        }
    }

    // 1. Quitter la CHAMBRE (Niveau 0) par le HAUT
    // On vérifie si on est au niveau 0 ET si on dépasse le haut de l'écran (y < 5)
    if (IsLocationUp(8, 13, 0, 5))
    {
        if (hasDoudou == 1 && fin_du_jeu == 0)
        {
            currentLevel = 1;
            player.y = (MAP_HEIGHT * TILE_SIZE) - 20;
        }
        else
        {
            player.y = 5;
        }
    }
    if (IsLocationUp(8, 13, 0, 10) && (hasDoudou == 0 || fin_du_jeu == 1))
    {
        if(hasDoudou == 0) dialogueStep_sortie1 = 1;
        if(fin_du_jeu == 1) dialogue_Step_fin = 1;
    }

    // 2. Quitter le COULOIR (Niveau 1) par le BAS
    // On vérifie si on est au niveau 1 ET si on dépasse le haut de l'écran
    else if (IsLocationDown(8, 13, 1, 20))
    {
        currentLevel = 0; // On retourne à la CHAMBRE
        player.y = 10;    // On apparaît tout en HAUT de la chambre
    }

    if (IsLocationUp(8, 13, 1, 5))
    {
        currentLevel = 2;
        player.y = (MAP_HEIGHT * TILE_SIZE) - 20;
    }

    else if (IsLocationDown(8, 13, 2, 20))
    {
        currentLevel = 1;
        player.y = 10;
    }

    if (IsLocationLeft(5, 9, 2, 5))
    {
        currentLevel = 3;
        player.x = (MAP_WIDTH * TILE_SIZE) - 20;
    }

    else if (IsLocationRight(5, 9, 3, 20))
    {
        currentLevel = 2;
        player.x = 5;
    }
    // --- TRANSITION CUISINE -> SALLE A MANGER ---
    // 1. Affiche le dialogue si on n'a pas tout donné
    if (IsLocationUp(8, 13, 3, 20) && (bouche_has_soupe == 0 || bouche_has_pain == 0)) {
        dialogue_entree_SAM = 1;
    }
    // 2. Bloque le joueur physiquement s'il essaie de passer en force
    else if (IsLocationUp(8, 13, 3, 5) && (bouche_has_soupe == 0 || bouche_has_pain == 0)) {
        player.y = 5;
    }
    // 3. Autorise le passage (et initie les projectiles) SEULEMENT si c'est nourri
    else if (IsLocationUp(8, 13, 3, 5) && bouche_has_soupe == 1 && bouche_has_pain == 1) {
        currentLevel = 4;  // Salle à manger cauchemar
        InitProjectiles();
        onTable = 0;  // Pas encore sur la table
        currentLane = 1;
        hitCount = 0;  // Reset des tentatives
        player.x = 9 * TILE_SIZE + (TILE_SIZE / 2);  // Centré sur la table
        player.y = 13 * TILE_SIZE; // Rangée 13 = juste sous la table
    }
    else if (IsLocationDown(8, 13, 4, 20))
    {
        
        currentLevel = 3; // On retourne à la CHAMBRE
        player.y = 10;    // On apparaît tout en HAUT de la chambre
        InitProjectiles(); 
    }

    // --- TRANSITIONS DU LABYRINTHE ---

    // 1. Entrée dans le labyrinthe (Niveau 2 -> 5)
    if (IsLocationRight(6, 10, 2, 20) && (statue_has_drawing == 1 && statue_has_water == 1))
    {
        currentLevel = 5;
        player.x = 5;
        SpawnFantomeRandom(); 
    }
    if (IsLocationRight(6, 10, 2, 20) && (statue_has_drawing == 0 || statue_has_water == 0))
    {

        dialogue_entree_labyrinthe = 1;
    }
    if (IsLocationUp(8, 13, 3, 20) && (bouche_has_soupe == 0 || bouche_has_pain == 0))
    {
        dialogue_entree_SAM = 1;
    }
    if (IsLocationUp(8, 13, 3, 5) && (bouche_has_soupe == 0 || bouche_has_pain == 0))
    {
        player.y = 5;
    }
    if (IsLocationUp(8, 13, 3, 5) && (bouche_has_soupe == 1 && bouche_has_pain == 1))
    {
        currentLevel = 4;
        player.y = (MAP_HEIGHT * TILE_SIZE) - 20;
    }

    // 2. Retour couloir (5 -> 2)
    else if (IsLocationLeft(6, 10, 5, 5))
    {
        currentLevel = 2;
        player.x = (MAP_WIDTH * TILE_SIZE) - 20;
        // Pas de fantôme dans le couloir, on le laisse où il est (invisible)
    }

    // 3. Passage 5 -> 6
    if (IsLocationRight(11, 14, 5, 20))
    {
        currentLevel = 6;
        player.x = 5;
        SpawnFantomeRandom(); 
    }
    // Retour 6 -> 5
    else if (IsLocationLeft(11, 14, 6, 5))
    {
        currentLevel = 5;
        player.x = (MAP_WIDTH * TILE_SIZE) - 20;
        SpawnFantomeRandom(); 
    }

    // 4. Passage 6 -> 7
    if (IsLocationDown(10, 13, 6, 20))
    {
        currentLevel = 7;
        player.y = 10;
        SpawnFantomeRandom(); 
    }
    // Retour 7 -> 6
    else if (IsLocationUp(10, 13, 7, 5))
    {
        currentLevel = 6;
        player.y = (MAP_HEIGHT * TILE_SIZE) - 20;
        SpawnFantomeRandom(); 
    }

    // 5. Passage 7 -> 8 (Dernier niveau)
    if (IsLocationLeft(11, 14, 7, 5))
    {
        currentLevel = 8;
        player.x = (MAP_WIDTH * TILE_SIZE) - 20;
        SpawnFantomeRandom(); 
    }
    // Retour 8 -> 7
    else if (IsLocationRight(11, 14, 8, 20))
    {
        currentLevel = 7;
        player.x = 5;
        SpawnFantomeRandom(); 
    }

    // Changement de son d'ambiance
    ManageMusic();
    // === MISE À JOUR DES PROJECTILES (SALLE À MANGER) ===
    UpdateProjectiles();
    
    // Mise à jour du timer de flash
    if (hitFlashTimer > 0) {
        hitFlashTimer--;
    }
    
    // Mise à jour du recul (knockback)
    if (knockbackTimer > 0) {
        knockbackTimer--;
        // Applique le recul
        float nextKnockX = player.x + knockbackVX;
        float nextKnockY = player.y + knockbackVY;
        
        if (currentLevel == 4 && onTable && dialogue_chambre_parents < 1) {
            if (!isWall(player.x, nextKnockY) && !isWall(player.x + player.w, nextKnockY) &&
                !isWall(player.x, nextKnockY + player.h) && !isWall(player.x + player.w, nextKnockY + player.h)) {
                float clampedY = nextKnockY;
                if (clampedY < TABLE_TOP_ROW * TILE_SIZE) clampedY = TABLE_TOP_ROW * TILE_SIZE;
                if (clampedY > TABLE_BOT_ROW * TILE_SIZE) clampedY = TABLE_BOT_ROW * TILE_SIZE;
                player.y = clampedY;
            }
            // Knockback Y seulement, X libre sur la table
            float tblLeft = 8 * TILE_SIZE + 1;
            float tblRight = 11 * TILE_SIZE + TILE_SIZE - player.w - 1;
            if (player.x < tblLeft) player.x = tblLeft;
            if (player.x > tblRight) player.x = tblRight;
        } else {
            if (!isWall(nextKnockX, player.y) && !isWall(nextKnockX + player.w, player.y) &&
                !isWall(nextKnockX, player.y + player.h) && !isWall(nextKnockX + player.w, player.y + player.h)) {
                player.x = nextKnockX;
            }
            
            if (!isWall(player.x, nextKnockY) && !isWall(player.x + player.w, nextKnockY) &&
                !isWall(player.x, nextKnockY + player.h) && !isWall(player.x + player.w, nextKnockY + player.h)) {
                player.y = nextKnockY;
            }
        }
        
        // Réduction progressive du recul
        knockbackVX *= 0.85f;
        knockbackVY *= 0.85f;
    }
    
    // Vérification de collision avec les projectiles (seulement si pas déjà en recul)
    if (knockbackTimer <= 0 && CheckProjectileCollision(player.x, player.y, player.w, player.h)) {
        hitCount++;
        printf("TOUCHE PAR UN PROJECTILE ! (%d/3)\n", hitCount);
        
        if (sonProjectileHit) {
            Mix_PlayChannel(-1, sonProjectileHit, 0);
        }
        
        hitFlashTimer = hitFlashDuration;
        
        if (hitCount >= 3) {
            // 3 tentatives épuisées : retour à la chambre de l'enfant (niveau 0)
            // Le doudou est conservé (hasDoudou reste à 1)
            currentLevel = 0;
            player.x = 80;
            player.y = 50;
            InitProjectiles();
            hitCount = 0;
            onTable = 0;
            knockbackTimer = 0;
        } else {
            // Tentative perdue : recommencer depuis le début de la salle à manger
            InitProjectiles();
            onTable = 0;
            player.x = 9 * TILE_SIZE + (TILE_SIZE / 2); // Centre de la table
            player.y = 13 * TILE_SIZE;  // Devant la table, prêt à sauter
            knockbackTimer = 0;
        }
    }
    
    // Reset hitCount seulement si on n'est PAS dans la salle à manger cauchemar
    if (currentLevel != 4 || dialogue_chambre_parents >= 1) {
        hitCount = 0;
    }

    if (currentLevel >= 5) {
        ActionFantome(200); 
    }
    // currentLevel = 4;  // Test salle à manger désactivé
    // --- GESTION COLLISION JOUEUR / FANTOME (GAME OVER / RESET) ---
    if (currentLevel >= 5 && currentLevel <= 8)
    {

        // 1. On définit la hitbox d'attaque du fantôme
        // On veut qu'il attrape plus haut (pour la perspective) et un peu plus large
        float killZoneHaut = fantome.y - 12;
        float killZoneBas = fantome.y + fantome.h;
        float killZoneGauche = fantome.x;
        float killZoneDroite = fantome.x + fantome.w;

        // 2. On vérifie si le joueur est dans cette zone
        if (player.x < killZoneDroite &&
            player.x + player.w > killZoneGauche &&
            player.y < killZoneBas &&
            player.y + player.h > killZoneHaut) // C'est cette ligne qui change tout
        {
            printf("GAME OVER - ATTRAPE !\n");

            // RESET
            currentLevel = 0;
            player.x = 80;
            player.y = 50;

            fantome.x = 8 * TILE_SIZE;
            fantome.y = 11 * TILE_SIZE;
            fantome.timer = 0;

            if (whichTableauPiece != 0)
            {
                if (whichTableauPiece == 1)
                    maps[5][1][18] = 117;

                if (whichTableauPiece == 2)
                    maps[6][3][13] = 118;

                if (whichTableauPiece == 3)
                    maps[7][13][17] = 115;

                if (whichTableauPiece == 4)
                    maps[8][3][1] = 116;

                whichTableauPiece = 0;
            }
        }
    }

    else if (IsLocationUp(8, 11, 4, 5)) {
        currentLevel = 10;  // Chambre des parents
        player.x = 155;
        player.y = (MAP_HEIGHT * TILE_SIZE) - 25;
        dialogue_chambre_parents = 1;  // Flag : salle à manger réussie
    }
    else if (IsLocationDown(8, 13, 10, 20)) {
        currentLevel = 4;
        player.x = 9 * TILE_SIZE;  // Centre de la porte du haut
        player.y = 1 * TILE_SIZE + 4;  // Rangée 1, en sécurité sous la porte
        // dialogue_chambre_parents reste à 1 : pièce normale
    }
    if(currentLevel == 10)GestionPapa();

    if(IsLocationRight(5, 10, 4, 20)){
        currentLevel = 11;
        player.x = 5;
    }
    else if(IsLocationLeft(5, 10, 11, 5)){
        currentLevel = 4;
        player.x = (MAP_WIDTH * TILE_SIZE) - 20;
    }

    // --- ANIMATION TV (GRESILLEMENT) ---
    tempsActuel = SDL_GetTicks();
    if (currentLevel == 11 && teleOn) {
        
        if (tempsActuel - debutTeleOn >= tempsTeleOn) {
            teleOn = 0; 
            premiereFoisAllumeeTele = 1;
            maps[11][4][14] = 302; maps[11][4][15] = 303;
            maps[11][3][14] = 304; maps[11][3][15] = 305;

            for (int y = 6; y <= 8; y++) {
                for (int x = 2; x <= 15; x++) { // x jusqu'à 15 pour être sûr
                    if (maps[11][y][x] == 340 || maps[11][y][x] == 341 || maps[11][y][x] == 342) maps[11][y][x] = 0; 
                }
            }
        } 
        else {
            int frame = (tempsActuel / 200) % 2;
            if (frame == 0) {
                maps[11][4][14] = 316; maps[11][4][15] = 317;
                maps[11][3][14] = 318; maps[11][3][15] = 319;
            } else {
                maps[11][4][14] = 320; maps[11][4][15] = 321;
                maps[11][3][14] = 322; maps[11][3][15] = 323;
            }
        }
    }
    else if(!teleOn && currentLevel == 11){
        maps[11][4][14] = 302; maps[11][4][15] = 303;
        maps[11][3][14] = 304; maps[11][3][15] = 305;
    }

    if(currentLevel == 11 && player.x >= 15*TILE_SIZE)aFiniSalon = 1;

    // // Gestion de réussite ou non du joueur pour le chemin à mémoriser
    if(currentLevel == 11 && !teleOn && premiereFoisAllumeeTele != 0 && !aFiniSalon){
        int caseX = (player.x + player.w / 2) / TILE_SIZE;
        int caseY = (player.y + player.h) / TILE_SIZE;
        int indexTuile = salonPattern[caseY][caseX];

        if(indexTuile != 340 && indexTuile != 341 && indexTuile != 342 && player.x >= 2*TILE_SIZE && player.x <= 14*TILE_SIZE){
            printf("mauvais chemin\n");
            player.y = 7*TILE_SIZE;
            player.x = 1 * TILE_SIZE;
            premiereFoisAllumeeTele = 0;
        }
    }

    // 302(g), 303(d) = bas télé sur commode 
    // 304,305 = haut télé
    // 316,317 = bas télé (effet bug v1)
    // 318,319 = haut télé (effet bug v1)
    // 320,321 = bas télé (effet bug v2)
    // 322,323 = haut télé (effet bug v2)
    

        //------ Fin du jeu------
    interact_porte_fin = 0;
    interraction_maman_fin =0;

    float distance_porte = 9999.0f;
    float distance_maman = 9999.0f;
    // coordonnées bas gauche de la porte
    int porteFinX = -1, porteFinY = -1;
    //coordonnées maman
    int mamanX = 12, mamanY = 13;

    TrouveCoordonnees(&porteFinX, &porteFinY, 437, 11);

    if (IsLocationObjet(20, 11, 437, &distance_porte, -1, -1)) interact_porte_fin = 1;

    static int toucheE_Relache = 1; //empeche ouverture en boucle
    //static int open_door = 0;

    if (state[SDL_SCANCODE_E] && toucheE_Relache)
    {
        toucheE_Relache = 0;

        if (interact_porte_fin && currentLevel == 11)
        {
            if (porteFinX != -1 && porteFinY != -1)
            {
                if (sonOpenDoor)
                    Mix_PlayChannel(-1, sonOpenDoor, 0);

                maps[11][porteFinY + 1 ][porteFinX]         = 328;
                maps[11][porteFinY + 1 ][porteFinX + 1]     = 329;
                maps[11][porteFinY][porteFinX]     = 330;
                maps[11][porteFinY][porteFinX + 1] = 331;
                //open_door = 1;
                dialogue_maman = 1;
                maps[11][mamanY][mamanX] = 439;
                menu_fin = 1;
                interact_porte_fin=0;
            }
        }
    }
    if (!state[SDL_SCANCODE_E])
        toucheE_Relache = 1;

//........... FIN RESTER .................
    TrouveCoordonnees(&mamanX, &mamanY, 439, 11);
    if (IsLocationObjet(16, 11, 439, &distance_maman, -1, -1)) interraction_maman_fin = 1;
    //static int toucheE_Relache = 1;
    static int toucheE_Relache_Maman = 1;

    if (state[SDL_SCANCODE_E] && toucheE_Relache_Maman)
    {
        toucheE_Relache_Maman = 0;
        // static toucheE_Relache = 0;

        if (interraction_maman_fin && currentLevel == 11)
        {
            if (mamanX != -1 && mamanY != -1)
            {
                // if (sonOpenDoor) //son mathys en attente
                //     Mix_PlayChannel(-1, sonOpenDoor, 0);

                maps[11][mamanY ][mamanX]    = 440;
                maps[11][mamanY -1][mamanX]  = 441;
                dialogue_maman_2 = 1;
                interraction_maman_fin =0;
            }
        }
    }

    if (currentLevel == 0 && interact_mur_fin == 1) {
            if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) { 
                if (IsLocationUp(8, 13, 0, 20)) {
                    dialogue_Step_fin = 1; // On active le message
                } 
            }
        }

    if (!state[SDL_SCANCODE_E])
        toucheE_Relache_Maman = 1;
}

void copieTableau (int src[MAP_HEIGHT][MAP_WIDTH], int dest[MAP_HEIGHT][MAP_WIDTH]){
    for (int i = 0; i < MAP_HEIGHT; ++i)
    {
        for (int j = 0; j < MAP_WIDTH; ++j)
        {
            dest[i][j] = src[i][j];
        }
    }
}

void GestionMemoSalon() {
    // On réinitialise le terrain
    int finX = 14;
    for (int y = 6; y <= 8; y++) {
        for (int x = 2; x <= finX+1; x++) {
            // On efface toutes les tuiles pour repartir sur un chemin vierge
            if ((maps[11][y][x] >= 340 && maps[11][y][x] <= 342)) {
                maps[11][y][x] = 0; 
            }
        }
    }

    if(teleOn){
        int curX = 2;
        int curY = 7;
        
        // Point de départ avec calcul de la tuile
        maps[11][curY][curX] = 340 + (curY - 6); 

        while (curX < finX) {
            int futursY[4]; 
            int nbChoix = 0;

            futursY[nbChoix++] = curY; 
            futursY[nbChoix++] = curY; 
            if (curY > 6) futursY[nbChoix++] = curY - 1;
            if (curY < 8) futursY[nbChoix++] = curY + 1;

            int targetY = futursY[rand() % nbChoix];
            
            if (targetY == curY) {
                curX++;
                maps[11][curY][curX] = 340 + (curY - 6);
            } 
            else {
                curX++;
                if (curX > finX) break; 
                maps[11][curY][curX] = 340 + (curY - 6);

                maps[11][targetY][curX] = 340 + (targetY - 6);
                curY = targetY; 

                if (curX < finX) {
                    curX++;
                    maps[11][curY][curX] = 340 + (curY - 6);
                }
            }
        }
        copieTableau(maps[11], salonPattern);
    }
}


int IsDansTab(int tabIndexTuile[], int taille, int index){
    for (int i = 0; i < taille; ++i)
    {
        if(tabIndexTuile[i] == index)return 1;
    }
    return 0;
}

void GestionPapa() {
    static Uint32 debutSequence = 0; 
    static int sequenceActive = 0;   
    static int waitRelease = 0;
    static int premiereCaseMonstreX = -1;
    static int premiereCaseMonstreY = -1;

    // Si dans updateGame ça nous dit qu'on doit forcer le monstre à dormir
    if (forceSleep == 1) {
        if (premiereCaseMonstreX != -1 && premiereCaseMonstreY != -1) {
            // On remet visuellement le monstre au lit
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX] = 219;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+1] = 220;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+2] = 221;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+3] = 222;
        }
        sequenceActive = 0;    
        papaReveil = 0;        
        affichePapaReveil = 0; 
        waitRelease = 1;       
        forceSleep = 0;        
        return;                
    }

    int tabIndexObjetBruit[] = {59, 60, 217, 216, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236};

    if(premiereCaseMonstreX == -1 || premiereCaseMonstreY == -1){
        TrouveCoordonnees(&premiereCaseMonstreX, &premiereCaseMonstreY, 219, 10);
        if (premiereCaseMonstreX == -1 || premiereCaseMonstreY == -1 )return; // Problème pour assigner la ou les valeurs des coordonnées
    }

    Uint32 tempsActuel = SDL_GetTicks();

    int caseX = (player.x + player.w / 2) / TILE_SIZE;
    int caseY = (player.y + player.h) / TILE_SIZE;
    int indexTuile = maps[currentLevel][caseY][caseX];
    int tailleTabObjetBruit = sizeof(tabIndexObjetBruit) / sizeof(tabIndexObjetBruit[0]);

    int surObjetBruit = ( IsDansTab(tabIndexObjetBruit, tailleTabObjetBruit, indexTuile));

    if (caseX < 0 || caseX >= MAP_WIDTH || caseY < 0 || caseY >= MAP_HEIGHT) return; // Sécurité pour pas que ça sorte de la map

    // Si on n'est plus sur le doudou on enlève le verrou, on peut donc re réveiller le papa
    if (!surObjetBruit) {
        waitRelease = 0;
    }

    // On est sur le doudou rien en cours et pas verrouillé
    if (surObjetBruit && sequenceActive == 0 && waitRelease == 0) {
        sequenceActive = 1;        
        debutSequence = tempsActuel; 
        affichePapaReveil = 1;     
        papaReveil = 0;            
    }

    // gestion du temps
    if (sequenceActive == 1) {
        Uint32 tempsEcoule = tempsActuel - debutSequence;

        // On laisse 400 ms de temps de réaction au joueur mais on affiche que le papa est réveillé
        if (tempsEcoule < 400) {
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX] = 187;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+1] = 188;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+2] = 189;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+3] = 190;

            papaReveil = 0;        
            affichePapaReveil = 1; 
        }
        // Si le joueur bouge il se refait tp en début de map
        else if (tempsEcoule < 5000) {
            papaReveil = 1;        
            affichePapaReveil = 1;
        }
        // Le joueur peut re bouger
        else {
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX] = 219;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+1] = 220;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+2] = 221;
            maps[10][premiereCaseMonstreY][premiereCaseMonstreX+3] = 222;
            sequenceActive = 0;    
            papaReveil = 0;
            affichePapaReveil = 0;
            waitRelease = 1; // waitRelease sert à laisser la possibilité de bouger au bout de 5s même si il est encore sur le doudou
                             // sinon il est bloqué
        }
    }
}

int IsLocationObjet(int rayon, int CurrLvl, int indexTuile, float *distance, int x, int y)
{
    if (x == -1 || y == -1)
    {
        TrouveCoordonnees(&x, &y, indexTuile, CurrLvl);
    }

    if (x == -1 || y == -1)
    {
        *distance = 9999.0f;
        return 0;
    }

    float targetX = x * TILE_SIZE + 8;
    float targetY = y * TILE_SIZE + 8; //convertir en pixels

    switch (indexTuile)
    {
    case 8:
    case 12:
    case 16:
        targetX += 8;
        targetY += 16;
        break;
    case 55:
        targetY += 8;
        break;
    case 113:
    case 109:
    case 437:
        targetY += 8;
        targetX += 8;
        break;
    }

    // 3. Calcul de la distance avec la position CORRIGÉE
    float dx = (player.x + player.w / 2) - targetX;
    float dy = (player.y + player.h / 2) - targetY;
    *distance = sqrt(dx * dx + dy * dy);

    return (*distance <= rayon && currentLevel == CurrLvl);
}

void TrouveCoordonnees(int *nvx, int *nvy, int indexObjet, int CurrLvl)
{
    for (int y = 0; y < MAP_HEIGHT; ++y)
    {
        for (int x = 0; x < MAP_WIDTH; ++x)
        {
            if (maps[CurrLvl][y][x] == indexObjet)
            {
                *nvx = x;
                *nvy = y;
                return;
            }
        }
    }
}

static inline int IsLampe(int index) {
    switch (index) {
        case 21: case 75: case 76: case 85: case 86: 
        case 148: case 186: case 317: case 319: case 321: 
        case 323: case 340: case 341: case 342: case 343:
        case 398: case 439 :case 440: case 441: case 332 : case 333:
            return 1;
        default: 
            return 0;
    }
}

// Fonction très rapide pour savoir si la tuile doit s'afficher par-dessus le joueur
static inline int IsTuileDessus(int index) {
    return (index == 78 || index == 75 || index == 76 || index == 145 || index == 146 || index == 85 
        || index == 86 || index == 296 || index == 297 || index == 298 || index == 299 || index == 300
         || index == 301 || index == 380 || index == 381 || index == 384 || index == 385 || index == 263 
         || index == 398 || index == 308 || index == 309 || index == 310 || index == 311 || index == 304
         || index == 305 || index == 402 || index == 403 || index == 639 || index == 640 || index == 653 
         || index == 654);
}

float getLuminosite(int gridX, int gridY, int rayonPx)
{
    float maxIntensite = 0.0f;

    // --- 1. Lumière du JOUEUR ---
    int tileCenterX = (gridX * TILE_SIZE) + (TILE_SIZE / 2);
    int tileCenterY = (gridY * TILE_SIZE) + (TILE_SIZE / 2);
    int playerCenterX = (int)player.x + (player.w / 2);
    int playerCenterY = (int)player.y + (player.h / 2);

    float dx = (float)(tileCenterX - playerCenterX);
    float dy = (float)(tileCenterY - playerCenterY);
    float distPx = sqrtf(dx * dx + dy * dy);

    if (distPx < rayonPx) {
        float i = 1.0f - (distPx / (float)rayonPx);
        if (i > maxIntensite) maxIntensite = i;
    }

    // --- 2. Lumière des LAMPES ---
    if (currentLevel == 4 && dialogue_chambre_parents < 1) {
        float dx = (float)(tileCenterX - playerCenterX);
        float dy = (float)(tileCenterY - playerCenterY);
        float distPx = sqrtf(dx*dx + dy*dy);
        if (distPx < rayonPx) {
            float i = 1.0f - (distPx / (float)rayonPx);
            if (i > maxIntensite) maxIntensite = i;
        }
        
        // 2) Faible lueur ambiante sur la table (colonnes 8-11, rangées 2-12)
        if (gridX >= 8 && gridX <= 11 && gridY >= TABLE_TOP_ROW && gridY <= TABLE_BOT_ROW) {
            float tableLum = 0.10f;  // Lueur faible sur la table
            if (tableLum > maxIntensite) maxIntensite = tableLum;
        }
    } else {
        float dx = (float)(tileCenterX - playerCenterX);
        float dy = (float)(tileCenterY - playerCenterY);
        float distPx = sqrtf(dx*dx + dy*dy);
        if (distPx < rayonPx) {
            float i = 1.0f - (distPx / (float)rayonPx);
            if (i > maxIntensite) maxIntensite = i;
        }
    }
    // --- 2. Lumière des LAMPES (Calcul en cases) ---
    for (int ly = 0; ly < MAP_HEIGHT; ly++) {
        for (int lx = 0; lx < MAP_WIDTH; lx++) {
            int indexTuile = maps[currentLevel][ly][lx];
            // UTILISATION DE LA FONCTION RAPIDE ICI
            if (IsLampe(indexTuile) || (indexTuile >= 107 && indexTuile <= 114 && statue_has_water && statue_has_drawing)) { 
                float distGrid = sqrtf(powf(gridX - lx, 2) + powf(gridY - ly, 2));
                float rayonLampe = (indexTuile == 186) ? 3.5f : 2.5f; 
                
                if (distGrid < rayonLampe) {
                    float i = 1.0f - (distGrid / rayonLampe);
                    if (i > maxIntensite) maxIntensite = i;
                }
            }
        }
    }
    return maxIntensite;
}

// Retourne 1 si la case est dans la lumière sinon 0.
int estEclaire(int gridX, int gridY, int rayon)
{

    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT)
    {
        return 0;
    }

    // 2. Calcul de distance
    int tileCenterX = (gridX * TILE_SIZE) + (TILE_SIZE / 2);
    int tileCenterY = (gridY * TILE_SIZE) + (TILE_SIZE / 2);
    int playerCenterX = (int)player.x + (player.w / 2);
    int playerCenterY = (int)player.y + (player.h / 2);

    int dx = tileCenterX - playerCenterX;
    int dy = tileCenterY - playerCenterY;
    double distance = sqrt(dx * dx + dy * dy);

    if (distance <= rayon)
    {
        return 1;
    }
    return 0; // C'est éteint
}

int estVisible(int x, int y, int rayonJoueur)
{
    if (getLuminosite(x, y, rayonJoueur) > 0.0f)
    {
        return 1;
    }
    return 0;
}

void DrawTuiles(float x, float y, int indexTuile, SDL_Renderer *renderer, int luminosite)
{
    extern int isColorMode;
    
    if (isColorMode == 1) {
        luminosite = 255; 
        indexTuile = indexTuile ; 
    }

    SDL_SetTextureColorMod(tilesetTexture, luminosite, luminosite, luminosite);
    SDL_Rect srcTuile = {TILE_SIZE * indexTuile, 0, TILE_SIZE, TILE_SIZE};
    SDL_Rect destTuile = {(int)x * TILE_SIZE, (int)y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
    SDL_RenderCopy(renderer, tilesetTexture, &srcTuile, &destTuile);
    SDL_SetTextureColorMod(tilesetTexture, 255, 255, 255);
}

int IsTuileSpecial(int index)
{
    for (int i = 0; i < tailleTuilesNotSpecial; ++i)
    {
        if (TuilesNotSpecial[i] == index)
        {
            return 0;
        }
    }
    return 1;
}



// --- DESSIN ---
void DrawGame(SDL_Renderer *renderer, TTF_Font *font, TTF_Font *fontMini)
{
    // 1. Fond Noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    rayon = (hasDoudou == 1) ? 60 : 30;

    // --- PRÉCALCUL DE LA LUMIÈRE (Le grand secret de l'optimisation) ---
    // On calcule la lumière 1 seule fois par frame et on la sauvegarde.
    int tileLuminosite[MAP_HEIGHT][MAP_WIDTH] = {0};
    extern int isColorMode;
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            float intensite = getLuminosite(x, y, rayon);
            if (intensite > 0.0f) {
                tileLuminosite[y][x] = (int)(intensite * 255);
            }
        }
    }

    // --- COUCHE 1 : SOL ET MURS DU DESSOUS ---
    if (tilesetTexture) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                int lum = tileLuminosite[y][x];
                
                if (isColorMode == 1) {
                    lum = 255;
                } else {
                    if (lum == 0) continue; // On passe si c'est dans le noir total
                }

                if (lum <= 0)
                {
                    continue;
                }

                // --- A. DESSINER LA TUILE ---
                int type = maps_patern[currentLevel][y][x];
                DrawTuiles(x, y, type, renderer, lum);

                // B. Dessiner les objets normaux (Sauf ceux qui vont par-dessus)
                int type_maps = maps[currentLevel][y][x];
                if (!IsTuileDessus(type_maps) && IsTuileSpecial(type_maps)) {
                    DrawTuiles(x, y, type_maps, renderer, lum);
                }
            }
        }
    }

    // --- EFFET SOMMEIL / REVEIL PARENTS ---
    if (currentLevel == 10) { 
        static int litX = -1, litY = -1;
        TrouveCoordonnees(&litX, &litY, 219, 10);
        int pixelLitX = litX * TILE_SIZE;
        int pixelLitY = litY * TILE_SIZE;

        if (affichePapaReveil) {
            SDL_Color cRouge = {255, 0, 0, 255};
            SDL_Surface *sBang = TTF_RenderText_Solid(font, "!", cRouge); 
            if (sBang) {
                SDL_Texture *tBang = SDL_CreateTextureFromSurface(renderer, sBang);
                SDL_Rect rBang = { pixelLitX, pixelLitY - 20, sBang->w, sBang->h }; 
                SDL_RenderCopy(renderer, tBang, NULL, &rBang);
                SDL_FreeSurface(sBang);
                SDL_DestroyTexture(tBang);
            }
        } 
        else {
            int offsetAnim = (SDL_GetTicks() / 200) % 10; 
            SDL_Color cBlanc = {200, 200, 255, 255}; 
            SDL_Surface *sZzz = TTF_RenderText_Solid(fontMini, "Zzz...", cBlanc);
            if (sZzz) {
                SDL_Texture *tZzz = SDL_CreateTextureFromSurface(renderer, sZzz);
                SDL_Rect rZzz = { pixelLitX, pixelLitY - 10 - offsetAnim, sZzz->w, sZzz->h }; 
                SDL_RenderCopy(renderer, tZzz, NULL, &rZzz);
                SDL_FreeSurface(sZzz);
                SDL_DestroyTexture(tZzz);
            }
        }
    }
        

    // ANIMATION
    int indexJoueur = 7; 

    if (isPlayerMoving) {
        int etape = (SDL_GetTicks() / 120) % 4; 
        if (playerDir == 0) {
            if (etape == 0) {
                indexJoueur = 356; 
            } 
            else if (etape == 1 || etape == 3) { 
                if ((SDL_GetTicks() % 3000) < 150) {
                    indexJoueur = 355; 
                } else {
                    indexJoueur = 7; 
                }
            } 
            else if (etape == 2) {
                indexJoueur = 357; 
            }
        }
        // 2. Vers la GAUCHE
        else if (playerDir == 1) {
            if (etape == 0)      indexJoueur = 345; 
            else if (etape == 1) indexJoueur = 347; 
            else if (etape == 2) indexJoueur = 350; 
            else if (etape == 3) indexJoueur = 347; 
        }
        // 3. Vers la DROITE
        else if (playerDir == 2) {
            if (etape == 0)      indexJoueur = 344; 
            else if (etape == 1) indexJoueur = 346; 
            else if (etape == 2) indexJoueur = 349; 
            else if (etape == 3) indexJoueur = 346; 
        }
        // 4. Vers le HAUT (Dos)
        else if (playerDir == 3) {
            if (etape == 0)      indexJoueur = 352; 
            else if (etape == 1) indexJoueur = 348; 
            else if (etape == 2) indexJoueur = 351; 
            else if (etape == 3) indexJoueur = 348; 
        }
    } 
    else {
        //idle
        int idleTime = SDL_GetTicks() % 2000;
        
        // 1. Vers le BAS (Face)
        if (playerDir == 0) {
            if (idleTime < 1200) {
                if ((SDL_GetTicks() % 3000) < 150) {
                    indexJoueur = 355; 
                } else {
                    indexJoueur = 7;   
                }
            } 
            else if (idleTime < 1600) indexJoueur = 358; 
            else                      indexJoueur = 7;  
        }
        // 2. Vers la GAUCHE
        else if (playerDir == 1) {
            if (idleTime < 1200)      indexJoueur = 347; 
            else if (idleTime < 1600) indexJoueur = 361; 
            else                      indexJoueur = 363; 
        }
        // 3. Vers la DROITE
        else if (playerDir == 2) {
            if (idleTime < 1200)      indexJoueur = 346; 
            else if (idleTime < 1600) indexJoueur = 360; 
            else                      indexJoueur = 362; 
        }
        // 4. Vers le HAUT (Dos)
        else if (playerDir == 3) {
            if (idleTime < 1200)      indexJoueur = 348; 
            else if (idleTime < 1600) indexJoueur = 359; 
            else                      indexJoueur = 348; 
        }
    }

    int dx = (int)roundf(player.x) - 2;
    int dy = (int)roundf(player.y) - 2;

    if (estAdulte) {
    // bas = corps (441), haut = tête (442)
    SDL_Rect srcBas = { 442 * 16, 0, 16, 16 };
    SDL_Rect srcHaut = { 443 * 16, 0, 16, 16 };

    SDL_Rect dstBas  = { dx, dy, 16, 16 };
    SDL_Rect dstHaut = { dx, dy - 16, 16, 16 }; // 1 tuile au-dessus

    SDL_RenderCopy(renderer, tilesetTexture, &srcHaut, &dstHaut);
    SDL_RenderCopy(renderer, tilesetTexture, &srcBas,  &dstBas);
    }
    else {
        SDL_Rect srcPlayer = { indexJoueur * 16, 0, 16, 16 };
        SDL_Rect destPlayer = { (int)roundf(player.x) - 2, (int)roundf(player.y) - 2, 16, 16 };
        SDL_RenderCopy(renderer, tilesetTexture, &srcPlayer, &destPlayer);
    }

    // --- COUCHE 2 : TUILES PAR-DESSUS LE JOUEUR (Panier, lampes murales...) ---
    if (tilesetTexture) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                int lum = tileLuminosite[y][x];
                if (isColorMode == 1) {
                    lum = 255;
                } else {
                    if (lum == 0) continue;
                } 

                int type_maps = maps[currentLevel][y][x];
                if (IsTuileDessus(type_maps)) {
                    DrawTuiles(x, y, type_maps, renderer, lum); 
                }
            }
        }
    }

    // --- DESSIN FANTOME ---
    if (currentLevel >= 5 && currentLevel <= 8) {
        int caseFantomeX = (int)(fantome.x / TILE_SIZE);
        int caseFantomeY = (int)(fantome.y / TILE_SIZE);

        // Sécurité pour ne pas déborder du tableau
        if (caseFantomeX >= 0 && caseFantomeX < MAP_WIDTH && caseFantomeY >= 0 && caseFantomeY < MAP_HEIGHT) {
            int lumFantome = tileLuminosite[caseFantomeY][caseFantomeX];
            if (isColorMode == 1) {
                lumFantome = 255;
            }
            
            if (lumFantome > 0) {
                SDL_SetTextureColorMod(tilesetTexture, lumFantome, lumFantome, lumFantome);
                SDL_Rect src = {63 * TILE_SIZE, 0, 16, 16};
                SDL_Rect dest = {(int)fantome.x, (int)fantome.y, 16, 16};
                SDL_RenderCopy(renderer, tilesetTexture, &src, &dest);
                SDL_SetTextureColorMod(tilesetTexture, 255, 255, 255);
            }
        }
    }

    if (currentLevel == 3 && plat_pret_a_servir > 0)
    {
        int idPlat = 0;

        if (plat_pret_a_servir == 1)
            idPlat = 326; // Soupe
        else if (plat_pret_a_servir == 2)
            idPlat = 327; // Pain
        
        if (idPlat != 0)
        {
            // sinusoide pour les mouvement (merci rayen <3)
            float offsetFlottement = sin(SDL_GetTicks() / 300.0f) * 4;

            int xPlat = (chaudronGX * TILE_SIZE) + 8;

            int yPlat = (chaudronGY * TILE_SIZE) - 12 + (int)offsetFlottement;

            float intensite = getLuminosite(chaudronGX, chaudronGY, rayon);
            int lum = (int)(intensite * 255);

            // 2. On applique cette luminosité à la texture (Assombrissement)
            SDL_SetTextureColorMod(tilesetTexture, lum, lum, lum);

            SDL_Rect src = {idPlat * TILE_SIZE, 0, 16, 16};
            SDL_Rect dest = {xPlat, yPlat, 16, 16};

            SDL_RenderCopy(renderer, tilesetTexture, &src, &dest);
        }
    }
    
    //dialogues
    if (dialogue_maman > 0) {
        char *texteAffiche = "Au secours mon fils viens m'aider !";
        DrawTexte(texteAffiche, renderer, font, 20, 180 ,280, 50);
    }

    if (dialogue_maman_2 > 0) {
        char *texteAffiche = "";
        if (dialogue_maman_2 == 1) texteAffiche = "Pourquoi tu es encore la...?";
        if (dialogue_maman_2 == 2) texteAffiche = "Tu aurais du partir";
        if (dialogue_maman_2 == 3) texteAffiche = "Il arrive...";

        DrawTexte(texteAffiche, renderer, font, 20, 180 ,280, 50);
    }

    if (dialogue_Step_fin > 0) {
        char *texteAffiche = "";
        if (dialogue_Step_fin == 1) texteAffiche = "je suis coince ici";
        DrawTexte(texteAffiche, renderer, font, 20, 180 ,280, 50);
    }

    if (dialogueStep > 0)
    {
        char *texteAffiche = "";
        if (dialogueStep == 1)
            texteAffiche = "Maman ? Papa ? Il fait tout noir...";
        if (dialogueStep == 2)
            texteAffiche = "J'ai peur... Ou est mon Doudou ?";

        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogueStep_sortie1 > 0)
    {
        char *texteAffiche = "";
        if (dialogueStep_sortie1 == 1)
            texteAffiche = "je peux pas sortir sans mon doudou...";
        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_hasDoudou > 0)
    {
        char *texteAffiche = "";
        if (dialogue_hasDoudou == 1)
            texteAffiche = "je te tiens";
        if (dialogue_hasDoudou == 2)
            texteAffiche = "OH...";
        if (dialogue_hasDoudou == 3)
            texteAffiche = "De la lumiere !";
        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_statue_haut > 0)
    {
        show_interact_prompt_statue_haut = 0;
        char *texteAffiche = "";
        if (dialogue_statue_haut == 1)
            texteAffiche = "Cette statue tient une coupe vide,";
        if (dialogue_statue_haut == 2)
            texteAffiche = "elle doit avoir soif...";

        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_statue_bas > 0)
    {
        show_interact_prompt_statue_bas = 0;
        char *texteAffiche = "";
        if (dialogue_statue_bas == 1)
            texteAffiche = "Son visage est tordu par la haine.";
        if (dialogue_statue_bas == 2)
            texteAffiche = "Un sourir ne ferait pas de mal...";

        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_entree_labyrinthe > 0)
    {
        char *texteAffiche = "";
        if (dialogue_entree_labyrinthe == 1)
            texteAffiche = "Les statues bloquent le passage...";

        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_entree_SAM > 0)
    {
        char *texteAffiche = "";
        if (dialogue_entree_SAM == 1)
            texteAffiche = "Je dois d'abord cuisiner...";

        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_max_objet > 0)
    {
        show_interact_prompt_dessin = 0;
        show_interact_prompt_eau = 0;
        char *texteAffiche = "";
        if (dialogue_max_objet == 1)
            texteAffiche = "Je n'ai que deux mains...";

        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_chaudron_true > 0 && plat_pret_a_servir == 1)
    {
        interact_chaudron_cuisiner = 0;
        char *texteAffiche = "";
        if (dialogue_chaudron_true == 1)
            texteAffiche = "Voila la soupe aux cauchemars ! ";

        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_chaudron_true > 0 && plat_pret_a_servir == 2)
    {
        interact_chaudron_cuisiner = 0;
        char *texteAffiche = "";
        if (dialogue_chaudron_true == 1)
            texteAffiche = "Voila le pain du chagrin !";

        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    if (dialogue_chaudron_false > 0)
    {
        interact_chaudron_cuisiner = 0;
        char *texteAffiche = "";
        if (dialogue_chaudron_false == 1)
            texteAffiche = "Oups !";
        if (dialogue_chaudron_false == 2)
            texteAffiche = "j'ai du me tromper dans la recette...";
        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }
    
    // === PROMPT SAUT SUR LA TABLE ===
    // Phase 1 : Cauchemar avec projectiles (prompt en bas seulement)
    if (currentLevel == 4 && dialogue_chambre_parents < 1) {
        if (!onTable) {
            int pGridY = (int)(player.y + player.h / 2) / TILE_SIZE;
            if (pGridY >= TABLE_TOP_ROW) {
                SDL_Color cBlanc = {255, 255, 255, 255};
                SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[ESPACE] Sauter sur la table", cBlanc);
                if (sText) DrawInteractions(renderer, sText);
            }
        } else {
            // Sur la table - message pour descendre en haut
            if (player.y <= (TABLE_TOP_ROW + 1) * TILE_SIZE) {
                SDL_Color cBlanc = {255, 255, 255, 255};
                SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[ESPACE] Descendre", cBlanc);
                if (sText) DrawInteractions(renderer, sText);
            }
        }
    }
    
    // Phase 2 : Après chambre des parents (monter/descendre des deux côtés)
    if (currentLevel == 4 && dialogue_chambre_parents >= 1) {
        int pGridY = (int)(player.y + player.h / 2) / TILE_SIZE;
        int pGridX = (int)(player.x + player.w / 2) / TILE_SIZE;
        
        if (!onTable) {
            // Près du bas de la table
            if (pGridY == TABLE_BOT_ROW + 1 && pGridX >= 8 && pGridX <= 11) {
                SDL_Color cBlanc = {255, 255, 255, 255};
                SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[ESPACE] Monter sur la table", cBlanc);
                if (sText) DrawInteractions(renderer, sText);
            }
            // Près du haut de la table
            else if (pGridY == TABLE_TOP_ROW - 1 && pGridX >= 8 && pGridX <= 11) {
                SDL_Color cBlanc = {255, 255, 255, 255};
                SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[ESPACE] Monter sur la table", cBlanc);
                if (sText) DrawInteractions(renderer, sText);
            }
        } else {
            // Sur la table - messages pour descendre aux extrémités
            if (player.y <= (TABLE_TOP_ROW + 1) * TILE_SIZE) {
                SDL_Color cBlanc = {255, 255, 255, 255};
                SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[ESPACE] Descendre", cBlanc);
                if (sText) DrawInteractions(renderer, sText);
            } else if (player.y >= (TABLE_BOT_ROW - 1) * TILE_SIZE) {
                SDL_Color cBlanc = {255, 255, 255, 255};
                SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[ESPACE] Descendre", cBlanc);
                if (sText) DrawInteractions(renderer, sText);
            }
        }
    }
    
    // === DESSINER LES PROJECTILES (SALLE À MANGER) ===
    DrawProjectiles(renderer);


    if (showInteractPrompt == 1) {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Ouvrir", cBlanc);
        if (sText)
            DrawInteractions(renderer, sText);
    }

    if (showInteractPrompt2 == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[ENTER] Interagir", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (showInteractPrompt3 == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Fermer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (showInteractPromptTente == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Entrer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (showInteractImpossibleObjet == 1)
    {
        char *texteAffiche = "j'ai deja une piece";
        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }

    if (showInteractPromptObjetTableau == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_porte_fin == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Ouvrir", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interraction_maman_fin == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Parler", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }

    if (showInteractTableau == 1)
    {
        char Prompt[100] = "";
        if (cpt_piece_tableau == 0)
        {
            sprintf(Prompt, "[E] Ajouter la %d ere piece", cpt_piece_tableau + 1);
        }
        else
        {
            sprintf(Prompt, "[E] Ajouter la %d eme piece", cpt_piece_tableau + 1);
        }
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, Prompt, cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (show_interact_prompt_statue_haut == 1 && has_water == 0 && statue_has_water == 0)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Interagir", cBlanc);
        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (show_interact_prompt_statue_haut == 1 && has_water == 1 && statue_has_water == 0)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Donner", cBlanc);
        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (show_interact_prompt_statue_bas == 1 && has_drawing == 0 && statue_has_drawing == 0)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Interagir", cBlanc);
        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (show_interact_prompt_statue_bas == 1 && has_drawing == 1 && statue_has_drawing == 0)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Donner", cBlanc);
        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (show_interact_prompt_eau == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (show_interact_prompt_dessin == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if(showInteractPendule){
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] ???", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (show_interact_prompt_livre == 1 && livreOuvert == 0)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Lire", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_truc_vert == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_pain == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_spider == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_eye == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_heart == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_coeur_rouge == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_os == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_bouche == 1 && (has_pain_chagrin == 1 || has_soupe == 1))
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Nourrir", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if (interact_chaudron_cuisiner == 1 && has_interact_livre == 1)
    {

        SDL_Color cBlanc = {255, 255, 255, 255};
        if (plat_pret_a_servir > 0)
        {
            char *nomPlat = (plat_pret_a_servir == 1) ? "la Soupe" : "le Pain";
            char bufferRecup[64];
            sprintf(bufferRecup, "[E] Recuperer %s", nomPlat);

            SDL_Surface *sTextRecup = TTF_RenderText_Solid(fontMini, bufferRecup, cBlanc);
            if (sTextRecup)
                DrawInteractions(renderer, sTextRecup);
        }
        else
        {
            char *nomIngredient = NULL;
            int decalageY = 0;

            if (has_pain)
                nomIngredient = "le pain";
            else if (has_spider)
                nomIngredient = "l'araignee";
            else if (has_eye)
                nomIngredient = "l'oeil";
            else if (has_heart)
                nomIngredient = "le coeur";
            else if (has_truc_vert)
                nomIngredient = "la chenille";
            else if (has_coeur_rouge)
                nomIngredient = "le coeur rouge";
            else if (has_os)
                nomIngredient = "l'os";

            if (nomIngredient != NULL)
            {
                char bufferPrompt[64];
                sprintf(bufferPrompt, "[E] Ajouter %s", nomIngredient);

                SDL_Surface *sTextAjout = TTF_RenderText_Solid(fontMini, bufferPrompt, cBlanc);
                if (sTextAjout)
                {
                    DrawInteractions(renderer, sTextAjout);
                    decalageY = 15;
                }
            }
            int chaudron_est_vide = (chaudron_has_truc_vert == 0 && chaudron_has_spider == 0 && chaudron_has_pain == 0 && chaudron_has_heart == 0 && chaudron_has_eye == 0 && chaudron_has_os == 0 && chaudron_has_coeur_rouge == 0);

            if (chaudron_est_vide == 0)
            {
                SDL_Surface *sTextCuisiner = TTF_RenderText_Solid(fontMini, "[ENTER] Cuisiner", cBlanc);

                if (sTextCuisiner)
                {
                    SDL_Texture *tText = SDL_CreateTextureFromSurface(renderer, sTextCuisiner);

                    int posX = (int)player.x - (sTextCuisiner->w / 2) + 8;

                    // On applique le décalage si on a affiché "Ajouter" juste avant
                    int posY = (int)player.y + 20 + decalageY;

                    if (posX < 2)
                        posX = 2;
                    else if (posX + sTextCuisiner->w > LOGICAL_WIDTH - 2)
                        posX = LOGICAL_WIDTH - sTextCuisiner->w - 2;

                    SDL_Rect rText = {posX, posY, sTextCuisiner->w, sTextCuisiner->h};

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
                    SDL_Rect rFond = {rText.x - 2, rText.y - 1, rText.w + 4, rText.h + 2};
                    SDL_RenderFillRect(renderer, &rFond);

                    SDL_RenderCopy(renderer, tText, NULL, &rText);

                    SDL_FreeSurface(sTextCuisiner);
                    SDL_DestroyTexture(tText);
                }
            }
        }
    }
    //...... DESSINER L'ELLIPSE..........
    if (ellipse == 1){
            DrawEllipse(renderer,font);
    }
    if (livreOuvert == 1 && textureLivre != NULL)
    {

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, NULL);

        int texW, texH;
        SDL_QueryTexture(textureLivre, NULL, NULL, &texW, &texH);

        int displayW = LOGICAL_WIDTH * 0.85;
        float ratio = (float)texW / (float)texH;
        int displayH = (int)(displayW / ratio);

        if (displayH > LOGICAL_HEIGHT - 40)
        {
            displayH = LOGICAL_HEIGHT - 40;
            displayW = (int)(displayH * ratio);
        }

        SDL_Rect posLivre = {
            (LOGICAL_WIDTH - displayW) / 2,  // Centré X
            (LOGICAL_HEIGHT - displayH) / 2, // Centré Y
            displayW,
            displayH};

        SDL_RenderCopy(renderer, textureLivre, NULL, &posLivre);

        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Fermer", cBlanc);

        if (sText)
        {
            SDL_Texture *tText = SDL_CreateTextureFromSurface(renderer, sText);

            SDL_Rect rText = {
                (LOGICAL_WIDTH - sText->w) / 2,
                posLivre.y + posLivre.h + 5,
                sText->w,
                sText->h};

            SDL_RenderCopy(renderer, tText, NULL, &rText);

            SDL_FreeSurface(sText);
            SDL_DestroyTexture(tText);
        }
    }
    if(showInteractTelecommande){
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Recuperer", cBlanc);
        
        if (sText) DrawInteractions(renderer, sText);
    }
    if(dialoguePasTelecommande){
        char *texteAffiche = "Je dois recuperer la telecommande";
        DrawTexte(texteAffiche, renderer, font, 20, 180 ,280, 50);
    }
    if(interactTelecommandeTurnOn){
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Allumer la TV", cBlanc);
        
        if (sText) DrawInteractions(renderer, sText);
    }
   
    
    // === AFFICHAGE DES VIES RESTANTES (salle à manger) ===
    if (currentLevel == 4 && dialogue_chambre_parents < 1) {
        int livesLeft = 3 - hitCount;
        // Motif cœur 9x9 pixels (1 = pixel plein)
        static const int heartPattern[9][9] = {
            {0,1,1,0,0,0,1,1,0},
            {1,1,1,1,0,1,1,1,1},
            {1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1},
            {0,1,1,1,1,1,1,1,0},
            {0,0,1,1,1,1,1,0,0},
            {0,0,0,1,1,1,0,0,0},
            {0,0,0,0,1,0,0,0,0},
            {0,0,0,0,0,0,0,0,0},
        };
        for (int i = 0; i < 3; i++) {
            int hx = 4 + i * 14;
            int hy = 3;
            int r, g, b;
            if (i < livesLeft) {
                r = 220; g = 30; b = 30;  // Cœur plein (rouge)
            } else {
                r = 50; g = 15; b = 15;   // Cœur vide (sombre)
            }
            for (int py = 0; py < 9; py++) {
                for (int px = 0; px < 9; px++) {
                    if (heartPattern[py][px]) {
                        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                        SDL_RenderDrawPoint(renderer, hx + px, hy + py);
                    }
                }
            }
        }
    }
    
    // === YEUX JAUNES SUR LA TABLE ===
    if (currentLevel == 4 && onTable && dialogue_chambre_parents < 1) {
        nightmareFrame++;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        float playerProgress = 1.0f - (player.y / (float)(MAP_HEIGHT * TILE_SIZE));
        float horror = playerProgress * playerProgress;
        
        // Yeux jaunes qui apparaissent dans l'obscurité sur la table
        int numEyes = 2 + (int)(horror * 6);
        for (int e = 0; e < numEyes; e++) {
            int seed = e * 7723 + (nightmareFrame / 120) * 1000;
            // Positionner les yeux seulement dans la zone sombre (hors table cols 8-11)
            int eyeX, eyeY;
            if (e % 2 == 0) {
                // Côté gauche de la table
                eyeX = 2 + (seed * 13) % (6 * TILE_SIZE);
            } else {
                // Côté droit de la table
                eyeX = 12 * TILE_SIZE + (seed * 13) % (5 * TILE_SIZE);
            }
            eyeY = 10 + (seed * 29) % (LOGICAL_HEIGHT - 20);
            
            // Les yeux apparaissent/disparaissent lentement
            float eyePhase = fmodf((nightmareFrame + seed) * 0.015f, 6.28f);
            float eyeAlphaF = (sinf(eyePhase) + 1.0f) / 2.0f;
            if (eyeAlphaF < 0.3f) continue;
            
            int alpha = (int)(eyeAlphaF * 200);
            
            // Oeil gauche (2 pixels)
            SDL_SetRenderDrawColor(renderer, 220, 200, 40, alpha);
            SDL_RenderDrawPoint(renderer, eyeX, eyeY);
            SDL_RenderDrawPoint(renderer, eyeX + 1, eyeY);
            // Pupille noire
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_RenderDrawPoint(renderer, eyeX, eyeY);
            
            // Oeil droit (2 pixels)
            SDL_SetRenderDrawColor(renderer, 220, 200, 40, alpha);
            SDL_RenderDrawPoint(renderer, eyeX + 5, eyeY);
            SDL_RenderDrawPoint(renderer, eyeX + 6, eyeY);
            // Pupille noire
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_RenderDrawPoint(renderer, eyeX + 5, eyeY);
        }
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
    
    // === EFFET FLASH ROUGE (impact projectile) ===
    if (hitFlashTimer > 0) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        // Intensité décroissante du flash
        int alpha = (int)(180.0f * ((float)hitFlashTimer / (float)hitFlashDuration));
        if (alpha > 255) alpha = 255;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha);
        SDL_Rect fullScreen = { 0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT };
        SDL_RenderFillRect(renderer, &fullScreen);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
    if (screamer ==  1 && textureScreamer != NULL) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, NULL);
        int texW, texH;
        SDL_QueryTexture(textureScreamer, NULL, NULL, &texW, &texH);

        int displayW = LOGICAL_WIDTH;
        float ratio = (float)texW / (float)texH;
        int displayH = (int)(displayW / ratio);

        if (displayH > LOGICAL_HEIGHT) {
            displayH = LOGICAL_HEIGHT;
            displayW = (int)(displayH * ratio);
        }

        SDL_Rect posScreamer = {
            (LOGICAL_WIDTH - displayW) / 2,  // Centré X
            (LOGICAL_HEIGHT - displayH) / 2, // Centré Y
            displayW,
            displayH
        };

        SDL_RenderCopy(renderer, textureScreamer, NULL, &posScreamer);
        }
    }



void DrawInteractions(SDL_Renderer *renderer, SDL_Surface *sText)
{
    SDL_Texture *tText = SDL_CreateTextureFromSurface(renderer, sText);

    // 1. Calcul de la position théorique
    int posX = (int)player.x - (sText->w / 2) + 8;
    int posY = (int)player.y + 20;

    // 2. CORRECTION (Clamping) pour ne pas sortir de l'écran
    if (posX < 2)
    {
        posX = 2; // Bloque à gauche
    }
    else if (posX + sText->w > LOGICAL_WIDTH - 2)
    {
        posX = LOGICAL_WIDTH - sText->w - 2; // Bloque à droite
    }

    // 3. On applique la position corrigée
    SDL_Rect rText = {posX, posY, sText->w, sText->h};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect rFond = {rText.x - 2, rText.y - 1, rText.w + 4, rText.h + 2};
    SDL_RenderFillRect(renderer, &rFond);

    SDL_RenderCopy(renderer, tText, NULL, &rText);

    SDL_FreeSurface(sText);
    SDL_DestroyTexture(tText);
}

void DrawTexte(char *texteAffiche, SDL_Renderer *renderer, TTF_Font *font, int x, int y, int w, int h)
{
    SDL_Rect rectBulle = {x, y, w, h};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
    SDL_RenderFillRect(renderer, &rectBulle);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rectBulle);

    SDL_Color couleurNoire = {0, 0, 0, 255};
    SDL_Surface *surfaceTexte = TTF_RenderText_Solid(font, texteAffiche, couleurNoire);
    if (surfaceTexte)
    {
        SDL_Texture *textureTexte = SDL_CreateTextureFromSurface(renderer, surfaceTexte);
        SDL_Rect rectTexte = {
            rectBulle.x + 10,
            rectBulle.y + 15,
            surfaceTexte->w,
            surfaceTexte->h};
        SDL_RenderCopy(renderer, textureTexte, NULL, &rectTexte);

        SDL_FreeSurface(surfaceTexte);
        SDL_DestroyTexture(textureTexte);
    }
}

// --- NETTOYAGE DE LA MÉMOIRE ---
void CleanGame() {
    // 1. Libérer les textures (Images)
    if (tilesetTexture != NULL) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = NULL;
    }
    if (textureScreamer != NULL) {
        SDL_DestroyTexture(textureScreamer);
        textureScreamer = NULL;
    }
    if (textureLivre != NULL) {
        SDL_DestroyTexture(textureLivre);
        textureLivre = NULL;
    }

    // 2. Libérer les effets sonores (Mix_Chunk)
    if (sonPickUp != NULL) {
        Mix_FreeChunk(sonPickUp);
        sonPickUp = NULL;
    }
    if (sonOpenDoor != NULL) {
        Mix_FreeChunk(sonOpenDoor);
        sonOpenDoor = NULL;
    }
    if (sonCloseDoor != NULL) {
        Mix_FreeChunk(sonCloseDoor);
        sonCloseDoor = NULL;
    }
    if (sonScreamer != NULL) {
        Mix_FreeChunk(sonScreamer);
        sonScreamer = NULL;
    }

    // 3. Libérer les musiques (Mix_Music)
    if (MusicInterior != NULL) {
        Mix_FreeMusic(MusicInterior);
        MusicInterior = NULL;
    }
    if (MusicExterior != NULL) {
        Mix_FreeMusic(MusicExterior);
        MusicExterior = NULL;
    }
}

void ResetGame(void) {
    Mix_HaltChannel(-1); // Coupe tous les bruitages (screamer, pendule, portes...) sur tous les canaux
    Mix_HaltMusic();  

    currentZoneState = -1;
    
    // 1. Restaure TOUTE la map 
    memcpy(maps, maps_origine, sizeof(maps_origine));

    // 2. Restaure les positions et l'état du joueur
    currentLevel = 0;
    player.x = 80;
    player.y = 50;
    player.w = 12;
    player.h = 12;
    estAdulte = 0;
    hasDoudou = 0;
    
    // 3. Restaure la progression du jeu
    fin_du_jeu = 0;
    menu_fin = 0;
    ellipse = 0;
    dialogueStep = 0;
    dialogueStep_sortie1 = 0;
    dialogue_maman = 0;
    dialogue_maman_2 = 0;
    
    // 4. Restaure les objets et les puzzles
    hasTelecommande = 0;
    teleOn = 0;
    cpt_piece_tableau = 0;
    whichTableauPiece = 0;
    has_water = 0;
    has_drawing = 0;
    statue_has_water = 0;
    statue_has_drawing = 0;
    
    // 5. Restaure la cuisine
    plat_pret_a_servir = 0;
    has_truc_vert = 0; has_spider = 0; has_pain = 0; 
    has_heart = 0; has_eye = 0; has_os = 0; has_coeur_rouge = 0;
    chaudron_anim = 0;
    max_objets = 0;
    bouche_has_soupe = 0;
    bouche_has_pain = 0;

    screamer = 0;
    forceSleep = 0;
    papaReveil = 0;
    affichePapaReveil = 0;
    penduleEnCours = 0; 
}
// Fonction helper pour vérifier si une position est sur la table (le chemin)
int isInOrganicPath(int gridX, int gridY) {
    // La table occupe les colonnes 8 à 11 (tuiles 160-166)
    (void)gridY;
    return (gridX >= 8 && gridX <= 11);
}


// Fnct de collision simple pour la tble 


// Vérifie si une tuile est de la nourriture (obstacle)
// Retourne 1 si oui, 0 si non
int estNourriture(int tuile) {
    // 239 = assiette, 287 = poulet, 288 = viande
    // 289 = bol yeux, 290 = carafe sang
    // 291 = assiettes dents, 292 = os
    if (tuile == 239) return 1;
    if (tuile == 287) return 1;
    if (tuile == 288) return 1;
    if (tuile == 289) return 1;
    if (tuile == 290) return 1;
    if (tuile == 291) return 1;
    if (tuile == 292) return 1;
    return 0;
}

// Vérifie si une tuile est une chaise (obstacle)
// Seulement les chaises latérales bloquent (283,284,285,286)
// Les chaises haut/bas de table (281,276) ne bloquent pas
int estChaise(int tuile) {
    // 283, 284 = chaises gauche (bloquent)
    // 285, 286 = chaises droite (bloquent) 
    if (tuile == 283) return 1;
    if (tuile == 284) return 1;
    if (tuile == 285) return 1;
    if (tuile == 286) return 1;
    
    // 281 = chaise haut, 276 = chaise bas (ne bloquent pas)
    // Le joueur peut passer dessus
    return 0;
}

// Retourne la hitbox réduite d'un objet selon son type
// Les hitboxes sont centrées dans la tuile (10x10 par défaut)
void getHitboxObjet(int tuile, int colonne, int ligne, float *hx, float *hy, float *hw, float *hh) {
    float baseX = colonne * TILE_SIZE;
    float baseY = ligne * TILE_SIZE;
    
    // Hitbox par défaut : 10x10 centrée dans la tuile 16x16
    *hx = baseX + 3;
    *hy = baseY + 3;
    *hw = 10;
    *hh = 10;
    
    // Hitbox spécifique selon le type d'objet
    if (tuile == 239) {
        // Assiette : 10x10 centrée
        *hx = baseX + 3;
        *hy = baseY + 3;
        *hw = 10;
        *hh = 10;
    }
    else if (tuile == 287 || tuile == 288) {
        // Poulet/Viande : 10x10 centrée
        *hx = baseX + 3;
        *hy = baseY + 3;
        *hw = 10;
        *hh = 10;
    }
    else if (tuile == 289) {
        // Bol yeux : 8x8 centrée (petit objet)
        *hx = baseX + 4;
        *hy = baseY + 4;
        *hw = 8;
        *hh = 8;
    }
    else if (tuile == 290) {
        // Carafe sang : 10x12 (un peu plus haute)
        *hx = baseX + 3;
        *hy = baseY + 2;
        *hw = 10;
        *hh = 12;
    }
    else if (tuile == 291 || tuile == 292) {
        // Assiettes dents/os : 10x10 centrée
        *hx = baseX + 3;
        *hy = baseY + 3;
        *hw = 10;
        *hh = 10;
    }
}

// Vérifie si la position (x, y) entre en collision avec un obstacle
// Système 3D : hitbox du joueur = ses pieds, hitbox objets = réduite
// Retourne 1 si collision, 0 si pas de collision
int collisionObstacles(float x, float y, int largeur, int hauteur) {
    // Ne fonctionne que dans la salle à manger
    if (currentLevel != 4) return 0;
    
    // === HITBOX DU JOUEUR POUR NOURRITURE : petit rectangle 4x4 ===
    float jGauche = x + (largeur / 2) - 2;
    float jDroite = x + (largeur / 2) + 2;
    float jHaut   = y + hauteur - 6;
    float jBas    = y + hauteur - 2;
    
    // === HITBOX DU JOUEUR POUR CHAISES : taille réelle du sprite ===
    float pGauche = x;
    float pDroite = x + largeur;
    float pHaut   = y;
    float pBas    = y + hauteur;
    
    // === Zone de vérification élargie pour inclure les chaises (colonnes 8 et 12) ===
    int colMin = 8;   // Chaises gauche
    int colMax = 12;  // Chaises droite
    int ligMin = (int)(pHaut / TILE_SIZE);
    int ligMax = (int)(pBas / TILE_SIZE);
    
    // Bornes de sécurité
    if (ligMin < 0) ligMin = 0;
    if (ligMax >= MAP_HEIGHT) ligMax = MAP_HEIGHT - 1;
    
    // === Parcourir toutes les cases de la table ===
    for (int lig = ligMin; lig <= ligMax; lig++) {
        for (int col = colMin; col <= colMax; col++) {
            if (col < 0 || col >= MAP_WIDTH) continue;
            
            int tuile = maps[currentLevel][lig][col];
            
            // --- COLLISION NOURRITURE : petite hitbox 4x4 ---
            if (estNourriture(tuile)) {
                float oX, oY, oW, oH;
                getHitboxObjet(tuile, col, lig, &oX, &oY, &oW, &oH);
                
                if (jDroite > oX && jGauche < oX + oW &&
                    jBas > oY && jHaut < oY + oH) {
                    return 1;
                }
            }
            
            // --- COLLISION CHAISES : hitbox complète du joueur ---
            if (estChaise(tuile)) {
                float cX = col * TILE_SIZE;
                float cY = lig * TILE_SIZE;
                
                // Vérifier si le sprite du joueur touche la tuile chaise
                if (pDroite > cX && pGauche < cX + TILE_SIZE &&
                    pBas > cY && pHaut < cY + TILE_SIZE) {
                    return 1;
                }
            }
        }
    }
    
    return 0;
}

// SYS DE PROJECTILE POUR SALLE a manger 


void InitProjectiles(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = 0;
    }
    projectileSpawnTimer = 0;
}

void SpawnProjectile(void) {
    if (currentLevel != 4) return;
    
    // Si salle déjà réussie, arrêter projectiles
    if (dialogue_chambre_parents >= 1) {
        return;
    }
    
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].active = 1;
            
            // Le projectile apparaît sur une rangée de la table, près du joueur
            // 70% de chance d'apparaître à ±3 rangées du joueur, 30% aléatoire sur la table
            int spawnRow;
            int playerRow = (int)(player.y / TILE_SIZE);
            if (rand() % 10 < 7) {
                // Proche du joueur (±3 rangées)
                int offset = (rand() % 7) - 3;  // -3 à +3
                spawnRow = playerRow + offset;
            } else {
                // Aléatoire sur toute la table
                spawnRow = TABLE_TOP_ROW + rand() % (TABLE_BOT_ROW - TABLE_TOP_ROW + 1);
            }
            // Clamper aux limites de la table
            if (spawnRow < TABLE_TOP_ROW) spawnRow = TABLE_TOP_ROW;
            if (spawnRow > TABLE_BOT_ROW) spawnRow = TABLE_BOT_ROW;
            projectiles[i].y = spawnRow * TILE_SIZE;
            
            // Difficulté progressive : vitesse augmente quand le joueur monte
            float playerProgress = 1.0f - (player.y / (float)(MAP_HEIGHT * TILE_SIZE));
            float baseSpeed = 0.8f;
            float maxSpeed = 1.8f;
            float currentSpeed = baseSpeed + (maxSpeed - baseSpeed) * playerProgress * playerProgress;
            
            float variation = 0.9f + ((rand() % 20) / 100.0f);
            currentSpeed *= variation;
            
            int fromLeft = rand() % 2;
            
            // Les projectiles partent des bords de la salle et traversent la table
            if (fromLeft) {
                projectiles[i].x = 2 * TILE_SIZE;       // Bord gauche de la salle
                projectiles[i].vx = currentSpeed;        // Vers la droite
            } else {
                projectiles[i].x = 17 * TILE_SIZE;      // Bord droit de la salle
                projectiles[i].vx = -currentSpeed;       // Vers la gauche
            }
            
            projectiles[i].type = rand() % 2;  // 0 = assiette (239), 1 = couteau (237/238)
            
            break;
        }
    }
}

void UpdateProjectiles(void) {
    // Ne met à jour les projectiles que dans le niveau cauchemar
    if (currentLevel != 4 || dialogue_chambre_parents >= 1) return;
    
    // Génération de nouveaux projectiles (fréquence réduite pour meilleure jouabilité)
    float playerProgress = 1.0f - (player.y / (float)(MAP_HEIGHT * TILE_SIZE));
    int spawnRate = (int)(55 - 25 * playerProgress);  // De 55 frames au début à 30 frames à la fin
    if (spawnRate < 25) spawnRate = 25;
    
    projectileSpawnTimer++;
    if (projectileSpawnTimer >= spawnRate) {
        SpawnProjectile();
        projectileSpawnTimer = 0;
    }
    
    // Mise à jour des projectiles existants
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            projectiles[i].x += projectiles[i].vx;
            
            // Désactive le projectile s'il sort de la salle (murs gauche/droit)
            if (projectiles[i].x < 1 * TILE_SIZE || projectiles[i].x > 18 * TILE_SIZE) {
                projectiles[i].active = 0;
            }
        }
    }
}

void DrawCircleLight(SDL_Renderer *renderer, int cx, int cy, int rayon) {
    for (int dy = -rayon; dy <= rayon; dy++) {
        for (int dx = -rayon; dx <= rayon; dx++) {
            int dist2 = dx*dx + dy*dy;
            if (dist2 <= rayon*rayon) {
                float dist = sqrtf((float)dist2);
                float alpha = 1.0f - (dist / (float)rayon);
                if (alpha < 0.0f) alpha = 0.0f;
                if (alpha > 1.0f) alpha = 1.0f;
                int a = (int)(alpha * 120); // 120 = intensité max du halo
                SDL_SetRenderDrawColor(renderer, 255, 255, 200, a);
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

void SauvegarderPartie(int isAuto) {
    SaveData data;
    data.currentLevel = currentLevel;
    data.playerX = player.x;
    data.playerY = player.y;
    data.playerDir = playerDir;
    data.isPlayerMoving = isPlayerMoving;
    data.max_objets = max_objets;
    data.hasDoudou = hasDoudou;
    data.has_water = has_water;
    data.has_drawing = has_drawing;
    data.has_truc_vert = has_truc_vert;
    data.has_spider = has_spider;
    data.has_pain = has_pain;
    data.has_heart = has_heart;
    data.has_eye = has_eye;
    data.has_os = has_os;
    data.has_coeur_rouge = has_coeur_rouge;
    data.has_soupe = has_soupe;
    data.has_pain_chagrin = has_pain_chagrin;
    data.whichTableauPiece = whichTableauPiece;
    data.statue_has_water = statue_has_water;
    data.statue_has_drawing = statue_has_drawing;
    data.hasTelecommande = hasTelecommande;
    data.teleOn = teleOn;
    data.papaReveil = papaReveil;
    data.forceSleep = forceSleep;
    data.chaudron_has_truc_vert = chaudron_has_truc_vert;
    data.chaudron_has_spider = chaudron_has_spider;
    data.chaudron_has_pain = chaudron_has_pain;
    data.chaudron_has_heart = chaudron_has_heart;
    data.chaudron_has_eye = chaudron_has_eye;
    data.chaudron_has_os = chaudron_has_os;
    data.chaudron_has_coeur_rouge = chaudron_has_coeur_rouge;
    data.plat_pret_a_servir = plat_pret_a_servir;
    data.bouche_has_soupe = bouche_has_soupe;
    data.bouche_has_pain = bouche_has_pain;
    data.cpt_piece_tableau = cpt_piece_tableau;

    // Copie de toutes les cases de la map modifiées
    for (int l = 0; l < NB_LEVELS; l++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                data.maps[l][y][x] = maps[l][y][x];
            }
        }
    }
    if (isAuto) {
        SaveGame("savegame_auto.bin", data);
    } else {
        SaveGame("savegame_manuel.bin", data);
    }
}

int ChargerPartie(int isAuto) {
    SaveData data;
    int success = 0;

    if (isAuto) success = LoadGame("savegame_auto.bin", &data);
    else success = LoadGame("savegame_manuel.bin", &data);
    
    if (success) {
        currentLevel = data.currentLevel;
        player.x = data.playerX;
        player.y = data.playerY;
        playerDir = data.playerDir;
        isPlayerMoving = data.isPlayerMoving;
        max_objets = data.max_objets;
        hasDoudou = data.hasDoudou;
        has_water = data.has_water;
        has_drawing = data.has_drawing;
        has_truc_vert = data.has_truc_vert;
        has_spider = data.has_spider;
        has_pain = data.has_pain;
        has_heart = data.has_heart;
        has_eye = data.has_eye;
        has_os = data.has_os;
        has_coeur_rouge = data.has_coeur_rouge;
        has_soupe = data.has_soupe;
        has_pain_chagrin = data.has_pain_chagrin;
        whichTableauPiece = data.whichTableauPiece;
        statue_has_water = data.statue_has_water;
        statue_has_drawing = data.statue_has_drawing;
        hasTelecommande = data.hasTelecommande;
        teleOn = data.teleOn;
        papaReveil = data.papaReveil;
        forceSleep = data.forceSleep;
        chaudron_has_truc_vert = data.chaudron_has_truc_vert;
        chaudron_has_spider = data.chaudron_has_spider;
        chaudron_has_pain = data.chaudron_has_pain;
        chaudron_has_heart = data.chaudron_has_heart;
        chaudron_has_eye = data.chaudron_has_eye;
        chaudron_has_os = data.chaudron_has_os;
        chaudron_has_coeur_rouge = data.chaudron_has_coeur_rouge;
        plat_pret_a_servir = data.plat_pret_a_servir;
        bouche_has_soupe = data.bouche_has_soupe;
        bouche_has_pain = data.bouche_has_pain;
        cpt_piece_tableau = data.cpt_piece_tableau;
        dialogueStep = 0;         
        dialogueStep_sortie1 = 0;   
        dialogue_entree_labyrinthe = 0;

        // Restaure la map
        for (int l = 0; l < NB_LEVELS; l++) {
            for (int y = 0; y < MAP_HEIGHT; y++) {
                for (int x = 0; x < MAP_WIDTH; x++) {
                    maps[l][y][x] = data.maps[l][y][x];
                }
            }
        }
    }
        return 1;
    }
void DrawProjectiles(SDL_Renderer *renderer) {
    if (currentLevel != 4 || dialogue_chambre_parents >= 1) return;
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            int px = (int)projectiles[i].x;
            int py = (int)projectiles[i].y;
            int projGridX = px / TILE_SIZE;
            // Visible uniquement sur la table (colonnes 8 à 11)
            if (projGridX < 8 || projGridX > 11) continue;
            int tileIndex;
            if (projectiles[i].type == 0) {
                tileIndex = 239;
            } else {
                tileIndex = (projectiles[i].vx > 0) ? 237 : 238;
            }
            SDL_SetTextureColorMod(tilesetTexture, 255, 255, 255);
            SDL_Rect srcProjectile = { TILE_SIZE * tileIndex, 0, TILE_SIZE, TILE_SIZE };
            SDL_Rect destProjectile = { px, py, TILE_SIZE, TILE_SIZE };
            SDL_RenderCopy(renderer, tilesetTexture, &srcProjectile, &destProjectile);
        }
    }
}

int CheckProjectileCollision(float px, float py, int pw, int ph) {
    // Ne vérifie les collisions que dans le niveau cauchemar
    if (currentLevel != 4 || dialogue_chambre_parents >= 1) return 0;
    
    // Vérifie collision pour chaque projectile actif
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            // Collision simple : les deux rectangles se touchent ?
            float projX = projectiles[i].x;
            float projY = projectiles[i].y;
            int projW = TILE_SIZE;
            int projH = TILE_SIZE;
            
            // Est-ce que le joueur et le projectile se chevauchent ?
            if (px < projX + projW && px + pw > projX &&
                py < projY + projH && py + ph > projY) {
                // Collision ! Désactive le projectile
                projectiles[i].active = 0;
                return 1;
            }
        }
    }
    return 0;
}
