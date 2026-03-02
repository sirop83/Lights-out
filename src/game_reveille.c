#include "game.h"
#include "game_reveille.h"
#include "map_reveille.h"

// int currentLevel = 0;

/*------- CHAMBRE ---------
444,445 = sol
446 = mur en brique 
447,448 = cubes seuls
449 = affiche lapin 
450 = livre 
451 = lampe 
452,453,454 = bas du tapis 
455,456,457 = haut tapis
458,459 = tapis rond
460,461 = haut lit 
462,463 = bas lit
464 = cars
465 = batman 
466,467 = petit train
468 = avion
469 = dino
470(haut),471 = commode pres du lit
472,473 = cubes
474,475,476,477 = bureau de travail
478 = haut de la chaise
479,480,481 = bas de la tente
482,483,484 = haut de la tente
485 = coussin
486,487 = coussins
501 = panier de basket
502 = ballon de basket
503(bas),504 = commode coin 
597,598,599 = bas tapis de course 
600, 601,602 = haut tapis de course
603 = legos jaunes
604 = carre marron secours (table)
606 ???
616,617 ???

------ TENTE -------
505,506,507,508,509 = bas tapis 
510,511,512,513,514 = mid v1
515,516,517,518,519 = mid v2
520,521,522,523,524 = haut tapis 
557 = duolingo
558 = lampe éteinte
559 = coockie
560 = mur 

------ COULOIR ------
488,489 = bas tapis 
490,491 = mid tapis
492,493 = fin tapis
494(bas),495 = commode avec broc
496 = vase
497,498 = lumiere couloir a refaire 
499, 500= limiere couloir a refaire v2
607 = bougie

------ HALL D'ENTREE ------
525,526 = bas tableau
527,528 = haut tableau
529,530,531,532,533,534 = bas tapis
535,536,537,538,539,540 = haut tapis
541 = tableau 
542(haut),543 = commode de coté
544(g),545 = haut de la table 
546,547 = bas de la table
548 = sol
549,550 = bas mirroir
551,552 = haut mirroir
553,554 = pot de la plante
555,556 = haut de la plante
608(gauche), 609(droite) 610(haut g), 611 = fauteuil de coté
612(bas), 613,614(haut g),615  = fauteuil de face
657,658 = bas de meuble sans le chat

------ CUISINE -------
561 = sol 
562-568 = table
569,570 = lavabo 
571,572 = plaque cuisson
573 = bloc seul
574 = pain 
575 = tirroir mur gauche
576,577,578 = bas table
579(haut),580 = chaise
581,582 = bas du four
583,584 = haut du four
585(bas g), 586, 587(haut d)= table bordel
588 = petite plante 
589 = grande plante
618(bas),619 = decors meuble de droite
620,621 = sacs 
622 = poele
643, 644, 645 = bas commode
646, 647, 648 = mid commode
649, 650, 651 = fin commode

------ SALLON -------
590,591 = haut cheminée 
592,593,594,595 = tableau 
596 = lampe
605 = commode livres
623 = haut de la lampe 2
624 = bas de la lampe 2
625(bas g),626(bas d),627,628 = radio 
629(haut),630, 631= haut billard
632,633,634 = bas billard
635,636,637 = pieds billard
638(bas),639(mid),640 = horloge vecna
652(bas),653(mid),654 = horloge vecna v2

------ JARDIN ---------
641 = terre 
642 = plantes
659 = fleurs blanches
660 = fleurs rouges
661 = buisson
662 = terre chemin 
663 = fin du chemin de terre
664 = pousse
665 = carottes 
666 = tomates
667 = herbe
668 = radis
669,670 = marre 

------ SALLE A MANGER ------
655 = bougie 
656 = broc

-----------------
671,672 = maman réveil

*/

// Sons
static Mix_Music *MusicInterior_Reveil = NULL;
static Mix_Music *MusicInterior_Reveil_Maison = NULL;

// Variables pour la maman

static float mamanX = 8.0f * 16.0f; 
static float mamanY = 3.0f * 16.0f; 
static int mamanAction = 0;         
static Uint32 mamanTimer = 0;

// Autres variables

static int playerDir = 0;       // 0=Bas, 1=Gauche, 2=Droite, 3=Haut
static int isPlayerMoving = 0;  // 0=Immobile, 1=Marche

static SDL_Texture *tilesetTexture = NULL;

static int toucheE_Relache = 1;

int showInteractPromptTente2 = 0;

int maps_reveille[NB_LEVELS][MAP_HEIGHT][MAP_WIDTH] = {
	{      //carte 1 (chambre)
        {2,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0, 2,  2,449,  2,  8,  9,  2, 2}, // Trou en haut
        {2,  2,  2,464,465,  2,  2,  2,  0,  0,  0,  0, 2,  2,469,  2, 10, 11,  2, 2},
        {2,  1,  0,460,461,451,  0,  1,  0,  1,  0,  0, 0,230,  0,  1,  0,  1,  0, 2},
        {2,  1,  0,462,463,  1,  0,  1,  0,  1, 447,  1, 0,  1,  0,  1,  0,603,  0, 2},
        {2,  1,458,459,  0,  1,  0,  1,  0,  1,  0,  1, 0,  1,  0,  1,  0,  1,  0, 2},
        {2,450,  0,  1,  0,  1,  0,  1,600,601,602,  1, 0,  1,  0,  1,  0,  1,  0, 2},
        {2,470,  0,  1,  0,  1,  0,  1,597,598,599, 448, 0,  1,  0,  1,482,483,484, 2},
        {2,471,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1, 0,  1,  0,  1,479,480,481, 2},
        {2,  1,  0,  1,  0,472,473,  1,  0,  1,  0,  1, 0,  1,  0,485,  0,486,487, 2}, 
        {2,  1,  0,468,  0,   1,447,  1,  0,217,  0,  1, 0,  1,  0,  1,  0,  1,  0, 2},
        {2,  1,  0,  1,466,467,  0,  1,  0,  1,  0,  1, 0,  1,  0,  1,468,  1,  0, 2},
        {2,501,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1, 0,455,456,457,  0,  1,504, 2},
        {2,  1,502,  1,  0,  1,478,  1,  0,  1,  0,  1, 0,452,453,454,  0,  1,503, 2}, // Bas fermé
        {2,  1,  0,  1,  0,  1,474,475,476,477,  0,  1, 0, 22, 23,  0,  0,230,  0, 2},
        {2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2,  2,  2,  2,  2, 2} // Bas fermé
    },
    // Couloir niveau 1
    {
        {2, 2, 2, 2, 2, 2, 2,  2,   0,492,493,  0,  2,  2, 2, 2, 2, 2, 2, 2}, // Haut (Suite)
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},   // Toits cotés
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},   // Murs cotés
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,607,  2,  2, 2, 2, 2, 2, 2, 2}, // Vide (Sol)
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,376,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 50,  2,   0,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 50,  2, 495,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2, 494,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,496,  2,  2, 2, 2, 2, 2, 2, 2}, // Bas (Entrée)
        {2, 2, 2, 2, 2, 2, 2,  2,   0,490,491,  0,  2,  2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2,  2,   0,488,489,  0,  2,  2, 2, 2, 2, 2, 2, 2}
    }, 
    // CARTE 3 : HALL (Niveau 2)
    {
        {2,  2,  2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2, 2}, // Trou en haut (tout fermé en 2 ici ?)
        {2,  2,  2,  2,  2,  527, 528, 2,   0,   0,   0,   0,   2,   2,   541, 2,   551, 552, 2, 2},
        {2,  2,  2,  2,  2,  525, 526, 544, 545, 0,   0,   0,   2,   544, 545, 2,   549, 550, 2, 2},
        {2,  2,  2,  555, 556, 0,   0, 657, 658, 0,   0,   0,   0,   546, 547, 0,   0,   0,   2, 2},
        {2,  2,  2,  553, 554, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2, 2},
        {2,  2,  2,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2, 2},
        {2,  2,  2,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2, 2},
        {2,  2,  2,  535, 538, 538, 538, 538, 538, 538, 538, 538, 538, 538, 538, 538, 538, 540, 0, 0},
        {2,  2,  2,  529, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 532, 534, 0, 0},
        {2,  2,  2,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2, 2},
        {2,  2,  2,  0,   0,   614, 615, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2, 2},
        {2,  2,  2,  610, 611, 612, 613, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2, 2},
        {2,  2,  2,  608, 609, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   555, 556, 0,   2, 2}, // Bas fermé
        {2,  2,  2,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   553, 554, 0,   2, 2},
        {2,  2,  2,  2,   2,   2,   2,   2,   0,   0,   0,   0,   2,   2,   2,   2,   2,   2,   2, 2}
     },
    // CARTE 4 CUISINE (Niveau 3)
    {       
        {2,   2,   2,   2,   2,   2,   2,   2,   0, 427, 428, 0, 2, 649, 650, 651,   2,   2, 2, 2}, // Trou en haut
        {2,   2,   2,   0, 583, 584,   2, 587,   0, 425, 426, 0, 2, 646, 647, 648,   2,   2, 2, 2},
        {2,   2,   2,   0, 581, 582, 585, 586,   0, 423, 424, 0, 0, 643, 644, 645,   0,   0, 2, 2},
        {2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0, 620, 2, 2},
        {2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0, 621, 2, 2},
        {2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0, 2, 2},  
        {2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0,397,2},
        {2,   2,   0,   0, 579,   0, 239, 574,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0, 0, 0},
        {2,   2, 589,   0, 580,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0, 0, 0},
        {2,   2,   0,   0, 579, 179, 180,   0,   0, 239,   0, 0, 0,   0,   0,   0,   0,   0, 2, 2},
        {2,   2,   0,   0, 580, 576, 577, 577, 577, 578,   0, 0,257,258, 258, 259,   0,   0, 2, 2},
        {2,   2, 181,   0,   0,   0,   0,   0,   0,   0,   0, 0,254,255, 255, 256,   0, 619, 2, 2},
        {2,   2,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0, 618, 2, 2}, // Bas fermé
        {2,   2,   0, 569, 570, 571, 572,   0, 588,   0,   0, 0, 0,   0,   0,   0, 589,   0, 2, 2},
        {2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2, 2, 2,   2,   2,   2,   2,   2, 2, 2}
    },
    // CARTE 5 SALLE A MANGER (Niveau 4 - dernière du bloc précédent)
    {    
        {2, 2, 2, 2, 2, 2, 2, 2,  2,425,426,  2,  2, 2, 2, 2, 2, 2, 2, 2}, // Trou en haut
        {2, 2, 2, 2, 2, 2, 2, 2,  2,423,424,  2,  2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 0, 0, 0, 0,589, 0,  0,  0,  0,  0,  0, 0, 0,589, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0,555,556, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,  0,  0,  0,239,285, 0, 0, 0,553,554, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,283,239,  0,  0,286, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,284,  0,288,239,285, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 0, 0, 0, 0, 0, 0,283,239,656,  0,286, 0, 0,455,456,457, 0, 0},
        {2, 2, 0, 0, 0, 0, 0, 0,284,  0,655,239,285, 0, 0,452,453,454, 0, 0},
        {2, 2, 0, 0, 0, 0, 0, 0,283,239,287,  0,286, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,284,  0,  0,  0,  0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 2, 2}, // Bas fermé
        {2, 2, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2,  0,  0,  0,  0,  2, 2, 2, 2, 2, 2, 2, 2}
    },
       {      // Jardin Niveau 5
        {446, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642}, 
        {446,   0, 659, 660, 660,   0,   0, 642, 661,   0,   0,   0,   0, 664, 666, 664,   0,   0,   0, 642}, 
        {446,   0, 660, 659, 660,   0,   0, 642,   0,   0,   0, 664, 664,   0, 666,   0,   0, 661,   0, 642},
        {446,   0, 660, 660, 659,   0,   0, 642,   0,   0,   0, 665,   0, 666,   0, 668, 668, 664,   0, 642},
        {446,   0, 660, 659, 660,   0,   0, 642,   0,   0,   0,   0, 665, 666, 664,   0, 668, 664, 661, 642},
        {446, 661, 659, 660, 660,   0,   0, 642,   0,   0,   0,   0, 664,   0, 666, 668, 664,   0,   0, 642},
        {446,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 642},
        {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 642},
        {  0, 662, 662, 662, 662, 662, 663,   0,   0,   0,   0, 669, 670,   0,   0,   0,   0,   0,   0, 642},
        {446,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 642},
        {446,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 642},
        {446, 661,   0,   0,   0,   0,   0, 661,   0,   0,   0,   0,   0,   0,   0,   0,   0, 661,   0, 642},
        {446,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 661, 661,   0,   0,   0,   0, 642}, 
        {446, 661,   0,   0, 661,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 642},
        {446, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642, 642}  
    },
    // Chambre des parents (index 6)
    {    
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // Trou en haut
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2}, // Bas fermé
        {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
        {2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2}
    },
    // Salon (index 7)
    {    
        {2, 2, 2,594,595, 2,  2,  2,590, 591,  2,605, 2, 2,   2,   2,640,  2, 2}, // Trou en haut
        {2, 2, 2,592,593, 2,  2,  2,332, 333,  2,  2, 2, 2,   2,   2,639,  0,  2, 2},
        {2, 2, 0,  0,  0, 0,  0,   0,  0,  0,  0,  0, 0, 0,   0,   0,638,  0,  2, 2},
        {2, 2, 0,  0,  0, 0,623, 308,309,310,311,623, 0, 0, 304, 305,  0,  0,  2, 2},
        {2, 2,555,556,  0, 0,624, 312,313,314,315,624, 0, 0, 302, 303,  0,  0,  2, 2},
        {2, 2,553,554,  0, 0,  0, 520,521,522,523,524, 0, 0, 324, 325,  0,  0,  2, 2},
        {0, 0, 0,  0,  0, 0,  0, 515,516,517,518,519, 0, 0,   0,   0,555,556,  2, 2},
        {0, 0, 0,  0,  0, 0,  0, 510,511,512,513,514, 0, 0,   0,   0,553,554,  2, 2},
        {0, 0, 0,  0,  0, 0,  0, 515,516,517,518,519, 0, 0,   0,   0,  0,  0,  2, 2},
        {2, 2,627,628, 0, 0,  0, 510,511,512,513,514, 0, 0,   0,   0,  0,  0,  2, 2},
        {2, 2,625,626, 0, 0,  0, 505,506,507,508,509, 0, 0,   0,   0,614,615,  2, 2},
        {2, 2,324,325, 0,629,630,631,  0,  0,  0,  0, 0, 0, 610,611, 612,613,  2, 2},
        {2, 2, 0,  0,  0,632,633,634,  0,  0,  0,  0, 0,589,608,609,  0,  0,  2, 2}, // Bas fermé
        {2, 2, 0,  0,  0,635,636,637,  0,  0,  0,  0, 0, 0,   0,   0,  0,  0,  2, 2},
        {2, 2, 2,  2,  2, 2,  2,   2,  2,  2,  2,  2, 2, 2,   2,   2,  2,  2,  2, 2}
    },
    // TENTE (index 8)
    {
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50}, // Marge Haut
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50, 151, 152, 151, 152, 151, 152, 151, 152, 151, 50, 50, 50, 50, 50, 50}, // Début Rectangle
        {50, 50, 50, 50, 50, 366,   1,   0,   1,   0, 485, 469,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50, 366, 472,   0,   1,   0,   1,   0, 557, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,   0,   1,   0,   1,   0,   1,   0,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,   0,   1,   0, 558, 486, 487,   0,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,   0,   1,   0,   1,   0, 447, 559, 174, 364, 50, 50, 50, 50, 50, 50}, 
        {50, 50, 50, 50, 50, 366, 485,   0,   1,   0,   1, 468,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50, 366,   1,   0,   1,   0,   1,   0,   1, 364, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50, 365, 365, 365, 365, 365, 365, 365, 365, 364, 50, 50, 50, 50, 50, 50}, // Fin Rectangle
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50},          // Marge Bas
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50},
        {50, 50, 50, 50, 50,  50,  50,  50,  50,  50,  50,  50,  50,  50, 50, 50, 50, 50, 50, 50}},

};

// int TuilesNotSpecial[] = {0, 1, 2};
// int tailleTuilesNotSpecial = (int)sizeof(TuilesNotSpecial) / (int)sizeof(TuilesNotSpecial[0]);

int dialogueImpossibleChambreParents = 0;

int showInteractPromptMaman = 0;
int dialogueMamanReveil = 0;

int currentZoneStateReveil = -1;

int InitGameStepByStepReveille(SDL_Renderer *renderer){
	player.x = 80;
    player.y = 50; 
    player.w = 12; 
    player.h = 14;

    MusicInterior_Reveil = chargement_son_maison_reveil();
    MusicInterior_Reveil_Maison = chargement_son_maison_reveil_interieur();

    SDL_Surface *surface = SDL_LoadBMP("assets/tuille_into.bmp");
    if (surface) {
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
        tilesetTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }	
    return 0;
}

void ManageMusicReveille()
{
    // 1. Appliquer le volume global défini dans les options (en continu)
    Mix_VolumeMusic(globalVolumeMusique);
    
    // 2. Définition de la zone : 
    // 1 = Jardin (niveau 5)
    // 0 = Maison (tous les autres niveaux)
    int newZoneState = (currentLevel == 5) ? 1 : 0;

    // 3. Si le joueur change de zone 
    if (newZoneState != currentZoneStateReveil)
    {
        Mix_HaltMusic();
        
        // Si on entre dans le jardin
        if (newZoneState == 1) {
            if (MusicInterior_Reveil) {
                Mix_FadeInMusic(MusicInterior_Reveil, -1, 1000); 
            }
        }
        // Si on entre ou qu'on est dans la maison
        // else if (newZoneState == 0) {
        //     if (MusicInterior_Reveil_Maison) {
        //         Mix_FadeInMusic(MusicInterior_Reveil_Maison, -1, 1000); 
        //     }
        // }
        
        currentZoneStateReveil = newZoneState; 
    }
}

int IsWallMaman(float x, float y) {
    int caseX = (int)x / TILE_SIZE; 
    int caseY = (int)y / TILE_SIZE;

    if (caseX < 0 || caseX >= 20 || caseY < 0 || caseY >= 15) return 1;

    // On bloque la maman dans la zone centrale de la chambre 
    if (caseX < 5 || caseX > 15 || caseY < 2 || caseY > 10) {
        return 1; 
    }

    int type = maps_reveille[0][caseY][caseX];
    
    switch (type) {
        case 0: case 1:               // Sol en bois normal
        case 452: case 453: case 454: // Bas du grand tapis
        case 455: case 456: case 457: // Haut du grand tapis
        case 458: case 459:           // Petit tapis rond
        case 464: case 465:           // Voitures, Batman
        case 466: case 467:           // Petit train
        case 468: case 469:           // Avion, Dinosaure
        case 447: case 448:           // Cubes
        case 472: case 473:           // Autres cubes
        case 603: case 217:           // Legos (jaunes, rouges, verts)
        case 230:                     // Chaussette
        case 502:                     // Ballon de basket
        case 485: case 486: case 487: // Coussins
        case 600:case 601:case 602:case 597: case 598: case 599:// Le grand tapis
            return 0; 
            
        default:
            return 1; 
    }
}

int IsWallReveille(float x, float y)
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

    int type = maps_reveille[currentLevel][caseY][caseX];
    int type_pattern = maps_patern_reveille[currentLevel][caseY][caseX];

    // =========================================================
    // --- 1. GESTION DES MURS GLOBAUX (Couche Pattern) ---
    // =========================================================
    // 446 = Mur brique | 2 = Mur normal (parents) | 560 = Mur Tente | 642 = Buissons jardin
    if (type_pattern == 446 || type_pattern == 2 || type_pattern == 560 || type_pattern == 642)
    {
        int caseY_Below = caseY + 1;
        if (caseY_Below < MAP_HEIGHT)
        {
            int typeBelow_pattern = maps_patern_reveille[currentLevel][caseY_Below][caseX];
            if (typeBelow_pattern == 446 || typeBelow_pattern == 2 || typeBelow_pattern == 560 || typeBelow_pattern == 642)
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }

        // Si la case en dessous est praticable, on laisse le joueur "dépasser" de 4 pixels pour la perspective
        if (localY < 4)
        {
            return 1;
        }
        return 0;
    }

    // =========================================================
    // --- 2. GESTION DES MEUBLES SPÉCIFIQUES (Couche Pattern) ---
    // =========================================================
    
    // Le "604" a deux utilités différentes selon le niveau !
    if (type_pattern == 604) {
        
        // A. Table du couloir (Niveau 1)
        if (currentLevel == 1) {
            if (localX < 2) return 0; // On laisse glisser un peu sur la gauche
            
            // Si on est sur le 604 du BAS (vérifié par la présence d'un 604 au-dessus)
            if (caseY > 0 && maps_patern_reveille[currentLevel][caseY - 1][caseX] == 604) {
                if (localY > 11) return 0; // Laisse passer le bas des pieds
                return 1; 
            } 
            else {
                // On est sur le 604 du HAUT
                return 1; // 100% solide
            }
        }
        // B. Meuble mural droit de la cuisine (Niveau 3) -> Remplaçant de 388, 389, 374
        else if (currentLevel == 3) {
            if (localX < 3) return 0;
            return 1;
        }
    }

    // Grande table cuisine (160..166 -> 562..568)
    if(type_pattern >= 562 && type_pattern <= 568){
        if( (type_pattern == 563 || type_pattern == 565 || type_pattern == 568)){
            if(localY > 13) return 0;
            if(type_pattern == 565 && localX < 2 ) return 0;
            if(type_pattern == 563 && localX > 13) return 0;
        }
        if((type_pattern == 562 || type_pattern == 564) && localX < 2 ) return 0;
        if((type_pattern == 566 || type_pattern == 567 ) && localX > 13) return 0;
        return 1;
    }

    // Tiroirs / Blocs seuls cuisine (181, 171 -> 575, 573) - Peuvent être en pattern ou en map !
    if(type == 573 || type == 575 || type_pattern == 573 || type_pattern == 575){
        if(localX > 14) return 0;
        if((type == 575 || type_pattern == 573) && localY > 4) return 0;
        return 1;
    }

    // =========================================================
    // --- 3. GESTION DES OBJETS (Couche Maps) ---
    // =========================================================

    // Armoires (inchangées : 10, 11, 14, 15, 18, 19)
    if (type == 10 || type == 11 || type == 14 || type == 15 || type == 18 || type == 19) {
        if (localY < 4) return 1;
        return 0;
    }
    if (type == 22 || type == 23) {
        if (localY > 2) return 1;
        return 0;
    }

    int isTopCorner = (y < player.y + 8);

    // Livre (20 -> 450)
    if (type == 450) {
        if (localX >= 11) return 0; 
        if (isTopCorner) return 0;  
        if (localY < 3) return 0;
        return 1; 
    }

    // Lampe chambre (21 -> 451)
    if (type == 451) {
        if (localX >= 12) return 0; 
        if (isTopCorner && (localY >= 5) ) return 0;  
        return 1; 
    }

    // Commode chambre (42, 43 -> 470, 471)
    if (type == 470 || type == 471) {
        if (localX >= 11) return 0; 
        if (isTopCorner) return 0; 
        return 1;
    }

    // Lit (32..35 -> 460..463)
    if (type == 460 || type == 461 || type == 462 || type == 463) {
        if (isTopCorner) return 0; 
        if ( (type == 460 || type == 462 ) && localX < 4) return 0;
        if ( (type == 461 || type == 463) && localX > 11 ) return 0;
        return 1; 
    }

    // Tente bas (52..57 -> 479..484)
    if (type >= 479 && type <= 484) {
        if (type == 482) { // Haut gauche
            if (localX < 8) return 0; 
            return 1;
        }
        if (type == 483 || type == 484) return 1; // Haut milieu/droit
        if (type == 479 || type == 480 || type == 481) {
            if (isTopCorner) return 0; 
            return 1; 
        }
    }

    // Commode mur droit (80, 81 -> 503, 504)
    if (type == 503 || type == 504) {
        if (localX < 6) return 0; 
        if (isTopCorner) return 0; 
        return 1; 
    }

    // Bureau et chaise (46..49 -> 474..477 | 51 -> 478)
    if ((type >= 474 && type <= 477) || type == 478) {
        
        // 1. CHAISE (Le dossier de la chaise)
        if (type == 478) {
            if (localX < 8 || localX > 11) return 0; 
            if (isTopCorner) return 0;
            return 1; 
        }
        
        // 2. BUREAU
        if (type == 474 && localX < 3) return 0; 
        if (type == 477 && localX > 8) return 0; 

        if (localY < 2) {
            if (caseY > 0 && maps_reveille[currentLevel][caseY - 1][caseX] == 478) {
                if (localX < 8 || localX > 11) return 0; 
                return 1; 
            }
            return 0; 
        }
        
        if (isTopCorner) return 0; 
        return 1;
    }

    if(type == 624){
        if(isTopCorner)return 0;
        if(localX<4)return 0;
        return 1;
    }

    // Extrémités gauche Couloir (71, 72, 172, 173 -> 494, 495)
    if (type == 494 || type == 495 || type == 172 || type == 173) {
        if (localX >= 12) return 0;
        if (isTopCorner) return 0; 
        return 1;
    }

    // Extrémités droite Couloir (377)
    if (type == 377) {
        if (localX < 6) return 0; 
        if (isTopCorner) return 0; 
        return 1;
    }

    // Petit vase / Bougie (73 -> 496)
    if (type == 496) {
        if (localX < 4) return 0; 
        if (localY < 2) return 0;
        if (isTopCorner) return 0; 
        return 1;
    }

    // Fauteuils Hall (378..385 -> 608..615)
    if (type >= 608 && type <= 615) {
        if (type >= 612 && type <= 615) {
            if ((type == 612 ) && localX < 6) return 0; 
            if ((type == 613 ) && localX > 6) return 0;
            if (type == 612 || type == 613) {
                if (isTopCorner) return 0;
                return 1; 
            }
        }
        if (type >= 608 && type <= 611) {
            if ((type == 608 ) && localX < 2) return 0;
            if ((type == 609 ) && localX > 2) return 0; 
            if (type == 608 || type == 609) {
                if (isTopCorner) return 0; 
                return 1; 
            }
        }
    }
    
    // Pot de fleur (143, 144 -> 553, 554)
    if(type == 553 ){
        if(localX < 9 || isTopCorner) return 0;
        return 1;
    }
    if(type == 554){
        if(localX > 6 || isTopCorner) return 0;
        return 1;
    }
    
    // Pieds table sans chat (386, 387 -> 657, 658)
    if(type == 657 || type == 658){
        if(y < player.y + 10) return 0; 
        return 1;
    }
    
    // Haut table hall (134, 135 -> 544, 545)
    if (type == 544 || type == 545) {
        return 1; 
    }
    // Grosse plante (275 -> 589)
    if (type == 589) {
        
        // Augmente ce chiffre (13, 14 ou 15) pour bloquer le joueur plus vite vers le bas
        if (localY > 15) return 0;

        // L'astuce de la pendule : on regarde où est le centre du joueur
        float playerCenterX = player.x + (player.w / 2.0f);
        int playerCenterTileX = (int)playerCenterX / TILE_SIZE;

        if (playerCenterTileX == caseX) {
            // --- Le joueur est vraiment FACE à la plante ---
            if (isTopCorner) return 0; // La tête passe au-dessus
            return 1; // Le corps bloque
        } 
        else {
            // --- Le joueur FRÔLE la plante sur les côtés ---
            if (localX < 4 || localX > 11) return 0; // Laisse glisser sur les côtés
            
            if (isTopCorner) return 0; // La tête passe au-dessus
            return 1; // Le reste bloque
        }
    }

    // Statue mur gauche (84 -> Gardé si présent dans ta map finale)
    if(type == 84){
        if(isTopCorner) return 0;
        return 1;
    }

    // Pieds table hall (136, 137 -> 546, 547)
    if (type == 546 || type == 547) {
        if (y < player.y + 8) return 0; 
        return 1; 
    }

    // Chaudron (294 à 297) -> S'il ne reste pas dans ta map, ça n'affecte rien de le laisser
    if(type == 294 || type == 295){
        if(isTopCorner) return 0;
        if(type == 294 && localX < 4) return 0;
        if(type == 295 && localX > 12) return 0;
        return 1;
    }
    if(type == 296 || type == 297){
        if(localY < 14) return 0;
        if(type == 296 && localX < 4) return 0;
        if(type == 297 && localX > 12) return 0;
        return 1;
    }
    
    // Meuble mur de droite cuisine MAPS (388, 389, 374 -> 618, 619) 
    if(type == 618 || type == 619){
        if(localX < 3) return 0;
        return 1;
    }
    
    // Sacs cuisine (395, 396 -> 620, 621)
    if(type == 620 || type == 621){
        if(isTopCorner) return 0;
        if(localX < 2) return 0;
        return 1;
    }
    
    // Bas armoire cuisine (245..247 -> 643..645)
    if(type == 643 || type == 644 || type == 645){
        if(isTopCorner) return 0;
        if(type == 643 && localX < 3) return 0;
        if(type == 645 && localX > 12) return 0;
        return 1;
    }
    
    // Poêle, plaques, lavabo etc. (178, 169, 170, 167, 168, 243, 274 -> 571, 572, 569, 570, 588)
    if(type == 178 || type == 571 || type == 572 || type == 569 || type == 570 || type == 243 || type == 588){
        if(isTopCorner) return 0;
        if(type == 588){
            if(localY < 4) return 0;
            if(localX > 12) return 0;
            return 1;
        }
        return 1;
    }
    
    // Chaise cuisine (264 -> 580)
    if(type == 580){
        if(localX < 3) return 0;
        if(isTopCorner) return 0;
        return 1;
    }
    
    // Bas commode parents (207, 208, 209)
    if(type == 207 || type == 208 || type == 209){
        if(type == 207 && localX < 13) return 0;
        if(type == 209 && localX > 12) return 0;
        if(isTopCorner) return 0;
        return 1;
    }
    
    // Lit parents (187..206 & 219..222)
    if ((type >= 187 && type <= 206) || (type >= 219 && type <= 222)) {
        if (isTopCorner) return 0;
        return 1;
    }
    
    // Commode parents (185, 186, 218)
    if(type == 185 || type == 186 || type == 218){
        if(localX > 12) return 0;
        if(isTopCorner) return 0;
        return 1;
    }
    
    // Petite commode salon (399)
    if(type == 399){
        if(isTopCorner) return 0;
        if(localX < 4) return 0;
        return 1;
    }
    
    // Bas canapé salon (312 à 315)
    if(type >= 312 && type <= 315){
        if(isTopCorner) return 0;
        return 1;
    }
    
    // Horloges (413..415 -> 638..640 et 652..654)
    if ((type >= 638 && type <= 640) || (type >= 652 && type <= 654) ) {
        if (type == 640 || type == 654) {
            return 1; 
        }
        
        float playerCenterX = player.x + (player.w / 2.0f);
        int playerCenterTileX = (int)playerCenterX / TILE_SIZE;

        if (playerCenterTileX == caseX) {
            if (isTopCorner) return 0;
            return 1;
        } 
        else {
            if (localX < 4 || localX > 11) return 0;
            if (isTopCorner) return 0;
            return 1;
        }
    }
    
    // Télé/Radio sur meuble (302, 303, 324, 325, 625, 626)
    if((type >= 302 && type <= 303) || (type == 324 || type == 325) || type == 625 || type == 626){
        if((type == 302 || type == 324) && localX < 1) return 0;
        if( (type == 303 || type == 325 || type == 626) && localX > 14) return 0;
        if(isTopCorner) return 0;
        return 1;
    }
    
    // Bas billard (404..412 -> 629..637)
    if(type >= 629 && type <= 637){
        if( (type >= 635 && type <= 637) && localY > 6) return 0; // Pieds du billard
        if(isTopCorner) return 0;
        return 1;
    }
    
    // Porte fin (435 à 438)
    if(type >= 435 && type <= 438){
        if((type == 435 || type == 437) && localX < 4 ) return 0;
        if(isTopCorner) return 0;
        return 1;
    }
    
    // Bouche (156..159)
    if(type >= 156 && type <= 159){
        if((type == 156 || type == 158) && localX < 7) return 0;
        if( (type == 157 || type == 159) && localX > 10) return 0;
        if(isTopCorner) return 0;
        return 1;
    }

    return 0;
}

void UpdateGameReveille(void){
	const Uint8 *state = SDL_GetKeyboardState(NULL);

    dialogueImpossibleChambreParents = 0;
    if (dialogueImpossibleChambreParents > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheE_Relache)
            {
                dialogueImpossibleChambreParents = 0; // On ferme le dialogue
                toucheE_Relache = 0;
            }
        }
        else
        {
            toucheE_Relache = 1;
        }
        return;
    }

    if (dialogueMamanReveil > 0)
    {
        if (state[SDL_SCANCODE_RETURN])
        {
            if (toucheE_Relache)
            {
                dialogueMamanReveil = 0; 
                toucheE_Relache = 0;
            }
        }
        else
        {
            toucheE_Relache = 1;
        }
        return; 
    }

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

    if (dirX != 0 && dirY != 0)
    {
        // On multiplie par 0.707 (environ 1/racine(2)) pour ralentir
        dirX *= 0.7071f;
        dirY *= 0.7071f;
    }

    float nextX = player.x + (dirX * PLAYER_SPEED);
    float nextY = player.y + (dirY * PLAYER_SPEED);
    // Collision X
    int touchWallX = 0;
    if (IsWallReveille(nextX, player.y))
        touchWallX = 1;
    if (IsWallReveille(nextX + player.w, player.y))
        touchWallX = 1;
    if (IsWallReveille(nextX, player.y + player.h))
        touchWallX = 1;
    if (IsWallReveille(nextX + player.w, player.y + player.h))
        touchWallX = 1;

    if (!touchWallX)
        player.x = nextX;

    // Collision Y
    int touchWallY = 0;
    if (IsWallReveille(player.x, nextY))
        touchWallY = 1;
    if (IsWallReveille(player.x + player.w, nextY))
        touchWallY = 1;
    if (IsWallReveille(player.x, nextY + player.h))
        touchWallY = 1;
    if (IsWallReveille(player.x + player.w, nextY + player.h))
        touchWallY = 1;

    if (!touchWallY)
        player.y = nextY;

    float distance_tente;
    showInteractPromptTente2 = 0;
    if (IsLocationObjet(24, 0, 55, &distance_tente, -1, -1))
    {
        showInteractPromptTente2 = 1;
    }

    showInteractPromptMaman = 0;
    float distMaman = 9999.0f;
    if (currentLevel == 0) {
        float dxM = (player.x + player.w / 2.0f) - (mamanX + 8.0f);
        float dyM = (player.y + player.h / 2.0f) - (mamanY + 8.0f);
        distMaman = sqrtf(dxM * dxM + dyM * dyM);
        
        if (distMaman <= 24.0f) {
            showInteractPromptMaman = 1;
        }
    }

    if(state[SDL_SCANCODE_E]){
    	if(toucheE_Relache){
            if (showInteractPromptMaman) {
                dialogueMamanReveil = 1; // Ouvre le dialogue
            }
    		if (distance_tente <= 24 && currentLevel == 0 && maps[0][6][16] == 55)
            {
                currentLevel = 8;
                player.x = 7 * TILE_SIZE;
                player.y = 7 * TILE_SIZE;
            }
    		toucheE_Relache = 0;
    	}else{
    		toucheE_Relache = 1;
    	}
    }

    // --- GESTION DE LA MUSIQUE DU RÉVEIL ---
    ManageMusicReveille();

    // Changement tente <->chambre 
    if (IsLocationLeft(6, 10, 8, 6 * TILE_SIZE - 8))
    {
        player.x = 15 * TILE_SIZE;
        currentLevel = 0;
    }

    // Changement chambre <-> couloir
    if (IsLocationUp(8, 13, 0, 5))
    {
        currentLevel = 1;
        player.y = (MAP_HEIGHT * TILE_SIZE) - 20;
    }
	else if (IsLocationDown(8, 13, 1, 20))
    {
        currentLevel = 0; // On retourne à la CHAMBRE
        player.y = 10;    // On apparaît tout en HAUT de la chambre
    }

    // Changement couloir <-> Hall d'entrée
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

    // Changement Hall d'entrée <-> cuisine
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

    // Changement Hall d'entrée <-> jardin
    if (IsLocationRight(6, 10, 2, 20))
    {
        currentLevel = 5;
        player.x = 5;
    }
	else if (IsLocationLeft(6, 10, 5, 5))
    {
        currentLevel = 2;
        player.x = (MAP_WIDTH * TILE_SIZE) - 20;
    }

    // Changement Cuisine <-> salle à manger
    if (IsLocationUp(8, 13, 3, 10))
    {
        currentLevel =  4;
        player.y = (MAP_HEIGHT * TILE_SIZE) - 20;
    }
    else if (IsLocationDown(8, 13, 4, 20))
    {
        currentLevel = 3;
        player.y = 10;
    }

    // Changement salle à manger <-> chambre des parents
    if (IsLocationUp(8, 13, 4, 10))
    {
        dialogueImpossibleChambreParents = 1;
    }

    if (IsLocationUp(8, 13, 4, 5))
    {
        player.y = 5; 
    }

    else if (IsLocationDown(8, 13, 6, 20))
    {
        currentLevel = 4;
        player.y = 10;
    }

    // Changement salle à manger <-> salon
    if (IsLocationRight(6, 10, 4, 20))
    {
        currentLevel = 7;
        player.x = 5;
    }
	else if (IsLocationLeft(6, 10, 7, 5))
    {
        currentLevel = 4;
        player.x = (MAP_WIDTH * TILE_SIZE) - 20;
    }

    if (currentLevel == 0) { 
        Uint32 currentTime = SDL_GetTicks();
        
        // Toutes les X secondes, elle décide d'une nouvelle action
        if (currentTime > mamanTimer) {
            mamanAction = rand() % 5; // Donne un chiffre entre 0 (pause) et 4 (droite)
            
            // Elle garde cette action entre 1 et 2.5 secondes
            mamanTimer = currentTime + 1000 + (rand() % 1500); 
        }

        float mDX = 0;
        float mDY = 0;
        float mSpeed = 0.5f; 

        if (mamanAction == 1) mDY = -mSpeed; // Haut
        if (mamanAction == 2) mDY = mSpeed;  // Bas
        if (mamanAction == 3) mDX = -mSpeed; // Gauche
        if (mamanAction == 4) mDX = mSpeed;  // Droite

        if (mDX != 0 || mDY != 0) {
            float nextMX = mamanX + mDX;
            float nextMY = mamanY + mDY;

            int touchMX = IsWallMaman(nextMX + 2, mamanY + 2) || 
                          IsWallMaman(nextMX + 14, mamanY + 2) || 
                          IsWallMaman(nextMX + 2, mamanY + 15) || 
                          IsWallMaman(nextMX + 14, mamanY + 15);
            if (!touchMX) mamanX = nextMX;

            int touchMY = IsWallMaman(mamanX + 2, nextMY + 2) || 
                          IsWallMaman(mamanX + 14, nextMY + 2) || 
                          IsWallMaman(mamanX + 2, nextMY + 15) || 
                          IsWallMaman(mamanX + 14, nextMY + 15);
            if (!touchMY) mamanY = nextMY;
        }
    }


}

static inline int IsTuileDessusReveille(int index) {
    return (index == 501 || index == 607 || index == 555 || index == 556 
         || index == 610 || index == 611 || index == 614 || index == 615 
         || index == 579 || index == 623 || index == 308 || index == 309 
         || index == 310 || index == 311 || index == 304 || index == 305 
         || index == 627 || index == 628 || index == 639 || index == 640 
         || index == 653 || index == 654);
}

void DrawGameReveille(SDL_Renderer *renderer, TTF_Font *font, TTF_Font *fontMini)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (tilesetTexture)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                int lum = 255; 
                
                int type_pat = maps_patern_reveille[currentLevel][y][x];
                DrawTuiles(x, y, type_pat, renderer, lum); 

                int type_maps = maps_reveille[currentLevel][y][x];
                if (!IsTuileDessusReveille(type_maps) && IsTuileSpecial(type_maps))
                {
                    if (type_maps >= 219 && type_maps <= 222) continue; 
                    if (type_maps >= 187 && type_maps <= 190) continue; 
                    DrawTuiles(x, y, type_maps, renderer, lum); 
                }
            }
        }

        int indexJoueur = 7; 
        if (isPlayerMoving) {
            int etape = (SDL_GetTicks() / 120) % 4; 
            if (playerDir == 0) {
                if (etape == 0) indexJoueur = 356; 
                else if (etape == 1 || etape == 3) { 
                    if ((SDL_GetTicks() % 3000) < 150) indexJoueur = 355; 
                    else indexJoueur = 7; 
                } 
                else if (etape == 2) indexJoueur = 357; 
            }
            else if (playerDir == 1) {
                if (etape == 0)      indexJoueur = 345; 
                else if (etape == 1) indexJoueur = 347; 
                else if (etape == 2) indexJoueur = 350; 
                else if (etape == 3) indexJoueur = 347; 
            }
            else if (playerDir == 2) {
                if (etape == 0)      indexJoueur = 344; 
                else if (etape == 1) indexJoueur = 346; 
                else if (etape == 2) indexJoueur = 349; 
                else if (etape == 3) indexJoueur = 346; 
            }
            else if (playerDir == 3) {
                if (etape == 0)      indexJoueur = 352; 
                else if (etape == 1) indexJoueur = 348; 
                else if (etape == 2) indexJoueur = 351; 
                else if (etape == 3) indexJoueur = 348; 
            }
        } 
        else {
            int idleTime = SDL_GetTicks() % 2000;
            if (playerDir == 0) {
                if (idleTime < 1200) {
                    if ((SDL_GetTicks() % 3000) < 150) indexJoueur = 355; 
                    else indexJoueur = 7;   
                } 
                else if (idleTime < 1600) indexJoueur = 358; 
                else                      indexJoueur = 7;  
            }
            else if (playerDir == 1) {
                if (idleTime < 1200)      indexJoueur = 347; 
                else if (idleTime < 1600) indexJoueur = 361; 
                else                      indexJoueur = 363; 
            }
            else if (playerDir == 2) {
                if (idleTime < 1200)      indexJoueur = 346; 
                else if (idleTime < 1600) indexJoueur = 360; 
                else                      indexJoueur = 362; 
            }
            else if (playerDir == 3) {
                if (idleTime < 1200)      indexJoueur = 348; 
                else if (idleTime < 1600) indexJoueur = 359; 
                else                      indexJoueur = 348; 
            }
        }

        int dx = (int)roundf(player.x) - 2;
        int dy = (int)roundf(player.y) - 2;
        SDL_Rect srcPlayer = { indexJoueur * 16, 0, 16, 16 };
        SDL_Rect destPlayer = { dx, dy, 16, 16 };
        SDL_RenderCopy(renderer, tilesetTexture, &srcPlayer, &destPlayer);

        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                int type_maps = maps_reveille[currentLevel][y][x];
                if (IsTuileDessusReveille(type_maps))
                {
                    DrawTuiles(x, y, type_maps, renderer, 255); 
                }
            }
        }

    }
    if (showInteractPromptTente2 == 1)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Entrer", cBlanc);

        if (sText)
            DrawInteractions(renderer, sText);
    }
    if(dialogueImpossibleChambreParents == 1){
        char *texteAffiche = "Mon papa est entrain de dormir !";
        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }

    if (currentLevel == 0) {
            // Dessin du Haut de la Maman (Tête)
            SDL_Rect srcMamanHaut = { 672 * 16, 0, 16, 16 };
            SDL_Rect dstMamanHaut = { (int)mamanX - 2, (int)mamanY - 18, 16, 16 };
            SDL_RenderCopy(renderer, tilesetTexture, &srcMamanHaut, &dstMamanHaut);

            // Dessin du Bas de la Maman (Corps)
            SDL_Rect srcMamanBas = { 671 * 16, 0, 16, 16 };
            SDL_Rect dstMamanBas = { (int)mamanX - 2, (int)mamanY - 2, 16, 16 };
            SDL_RenderCopy(renderer, tilesetTexture, &srcMamanBas, &dstMamanBas);
    }

    if (showInteractPromptMaman == 1 && dialogueMamanReveil == 0)
    {
        SDL_Color cBlanc = {255, 255, 255, 255};
        SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "[E] Parler", cBlanc);
        if (sText) DrawInteractions(renderer, sText);
    }
    if (dialogueMamanReveil == 1) {
        char *texteAffiche = "Bonjour mon tresor ! Tu as bien dormi ?";
        DrawTexte(texteAffiche, renderer, font, 20, 180, 280, 50);
    }

}