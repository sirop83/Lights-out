#ifndef FIN_JEU_H
#define FIN_JEU_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

extern int choix;

void lancer_scene_fin(SDL_Event event, int *continuer_menu);
void afficher_menu_fin(SDL_Renderer *renderer, TTF_Font *font);

void DrawTexte(char *texteAffiche, SDL_Renderer *renderer, TTF_Font *font, int x, int y, int w, int h);
void DrawEllipse(SDL_Renderer *renderer, TTF_Font *font);

#endif

