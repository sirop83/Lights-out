#ifndef SAVE_H
#define SAVE_H

#include "game.h" 

typedef struct {
    // --- 1. Le Joueur ---
    int currentLevel;
    float playerX;
    float playerY;
    int playerDir;
    int isPlayerMoving;

    // --- 2. L'Inventaire ---
    int max_objets;
    int hasDoudou;
    int has_water;
    int has_drawing;

    int has_truc_vert;
    int has_spider;
    int has_pain;
    int has_heart;
    int has_eye;
    int has_os;
    int has_coeur_rouge;

    int has_soupe;
    int has_pain_chagrin;
    int whichTableauPiece;

    // --- 3. État du Monde ---
    int statue_has_water;
    int statue_has_drawing;
    int hasTelecommande;
    int teleOn;
    int papaReveil;
    int forceSleep;

    int chaudron_has_truc_vert;
    int chaudron_has_spider;
    int chaudron_has_pain;
    int chaudron_has_heart;
    int chaudron_has_eye;
    int chaudron_has_os;
    int chaudron_has_coeur_rouge;
    int plat_pret_a_servir;

    int bouche_has_soupe;
    int bouche_has_pain;
    int cpt_piece_tableau;

    // --- 4. La Carte ---
    int maps[NB_LEVELS][MAP_HEIGHT][MAP_WIDTH];

} SaveData;

int SaveGame(const char* filepath, SaveData data);
int LoadGame(const char* filepath, SaveData* data);

#endif