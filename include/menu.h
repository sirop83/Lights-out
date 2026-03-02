#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void InitMenu(SDL_Renderer *renderer);
// Retourne : 0 (Rien), 1 (Jouer), 2 (Quitter)
int UpdateMenu(SDL_Event *event);
void DrawMenu(SDL_Renderer *renderer, TTF_Font *fontTitre, TTF_Font *fontOptions);

void dessinerTexteMenu(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int y, int selected);
void dessinerCroix(SDL_Renderer *renderer, int x, int y);

#endif