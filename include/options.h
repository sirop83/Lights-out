#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Variables globales pour stocker le volume (de 0 à 128 max)
extern int globalVolumeMusique;
extern int globalVolumeBruitages;

void InitOptions(void);
int UpdateOptions(SDL_Event *event);
void DrawOptions(SDL_Renderer *renderer, TTF_Font *fontGrand, TTF_Font *fontPetit);

#endif