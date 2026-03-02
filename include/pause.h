#ifndef PAUSE_H
#define PAUSE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int UpdatePause(SDL_Event *event);

void DrawPause(SDL_Renderer *renderer, TTF_Font *fontTitre, TTF_Font *fontOptions);

#endif