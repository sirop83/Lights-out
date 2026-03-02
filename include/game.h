#ifndef GAME_H
#define GAME_H



 
// --- DEFINE ---
#define TILE_SIZE 16          
#define LOGICAL_WIDTH 320
#define LOGICAL_HEIGHT 240
#define MAP_WIDTH 20        
#define MAP_HEIGHT 15  
#define NB_LEVELS 12 
#define PLAYER_SPEED 2.0f // j'ai test pour les animation je trouve que 0.7 c'est une bonne vitesse A VOIR 


// #define FANTOME_SPEED 0.5f 



typedef struct {
    float x, y;     
    int w, h;       
} Joueur;

typedef struct{
    float x, y;
    int w, h;
    int direction; // 0: Haut, 1: Bas, 2: Gauche, 3: Droite
    int timer;     // Nombre de frames restantes avant de changer de direction
} Fantome;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <time.h>
#include "game.h"
#include "intro.h"
#include "sons.h"
#include "map.h"
#include "ia.h"
#include "fin_jeu.h"
#include "options.h"
#include <stdio.h>
#include <math.h>

extern Joueur player;
extern Fantome fantome;
extern int livreOuvert;

// extern SDL_Texture *tilesetTexture;

// -- VARIABLES AUTRE PART --
extern SDL_Renderer* renderer;
extern TTF_Font* font;

extern float FANTOME_SPEED;

extern int maps[NB_LEVELS][MAP_HEIGHT][MAP_WIDTH];

extern int menu_fin;
extern int dialogue_maman;

int InitGameStepByStep(SDL_Renderer *renderer, int *pourcentage);
void UpdateGame(void);
void DrawGame(SDL_Renderer *renderer, TTF_Font *font, TTF_Font *fontMini);
void ActionFantome();
void SpawnFantomeRandom();
void DrawTexte(char *texteAffiche, SDL_Renderer *renderer,TTF_Font *font, int x, int y, int w, int h);
void DrawInteractions(SDL_Renderer *renderer, SDL_Surface *sText);
void DrawTuiles(float x, float y, int indexTuile, SDL_Renderer *renderer, int luminosite);

int IsTuileSpecial(int index);

void TrouveCoordonnees(int *nvx, int *nvy, int indexObjet, int CurrLvl);
int IsLocationObjet(int rayon, int CurrLvl, int indexTuile, float *distance, int x, int y);
void GestionPapa();

int IsLocationUp(int xDebut, int xFin, int CurrLvl, int yDiff);
int IsLocationDown(int xDebut, int xFin, int CurrLvl, int yDiff);
int IsLocationRight(int yDebut, int yFin, int CurrLvl, int xDiff);
int IsLocationLeft(int yDebut, int yFin, int CurrLvl, int xDiff);

int IsDansTab(int tabIndexTuile[], int taille, int index);
void copieTableau (int src[MAP_HEIGHT][MAP_WIDTH], int dest[MAP_HEIGHT][MAP_WIDTH]);

void GestionMemoSalon();

void CleanGame(); // Nettoyage de tous les pointeurs

void ResetGame(void);

extern int currentLevel; 

#endif