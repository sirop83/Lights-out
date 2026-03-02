#ifndef GAME_REVEILLE_H
#define GAME_REVEILLE_H

int InitGameStepByStepReveille(SDL_Renderer *renderer);
int IsWallReveille(float x, float y);
void UpdateGameReveille(void);
void DrawGameReveille(SDL_Renderer *renderer, TTF_Font *font, TTF_Font *fontMini);
int IsWallMaman(float x, float y);
void ManageMusicReveille();

#endif