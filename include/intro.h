#ifndef INTRO_H
#define INTRO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
void InitIntro(void);
void SkipIntro(void);
void StartIntro(SDL_Renderer *renderer);
int UpdateIntroTimer(void);
void DrawIntro(SDL_Renderer *renderer, TTF_Font *font, TTF_Font *fontMini);

#endif