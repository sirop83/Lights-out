#include "intro.h"
#include "game.h" 
#include <stdio.h>
#include <stdlib.h>

extern int dialogueStep;

int isColorMode = 0; 
static Uint32 debutIntro = 0;
static SDL_Texture* renderTarget = NULL;

void InitIntro(void) {
    // Initialize intro state
    debutIntro = 0;
    dialogueStep = 0;
    isColorMode = 0;
    renderTarget = NULL;
}

void StartIntro(SDL_Renderer *renderer) {
    debutIntro = SDL_GetTicks();
    dialogueStep = 0; 
    currentLevel = 0;
    
    player.x = 5 * 16; 
    player.y = 3 * 16; 

    if (!renderTarget) {
        renderTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320, 240);
    }
}

void SkipIntro(void) {
    if (renderTarget) {
        SDL_DestroyTexture(renderTarget);
        renderTarget = NULL;
    }
    player.x = 5 * 16; 
    player.y = 3 * 16;
    isColorMode = 0;   
    dialogueStep = 2; 
}

int UpdateIntroTimer(void) {
    Uint32 temps = SDL_GetTicks() - debutIntro;


    if (temps < 3000) {
        player.x = 5 * 16; 
        player.y = 3 * 16; 
    }
    else if (temps < 10000) {
        player.x = 3 * 16 + 8; 
        player.y = 2 * 16 + 12; 
    }
    else if (temps < 16000) {
        player.x = 5 * 16; 
        player.y = 3 * 16;
        isColorMode = 0; 
    }
 
    else {
        SkipIntro();
        return 1; 
    }
    return 0;
}

void DrawIntro(SDL_Renderer *renderer, TTF_Font *font, TTF_Font *fontMini) {
    Uint32 temps = SDL_GetTicks() - debutIntro;
    if (temps < 10000) {
        isColorMode = 1; 
        
        float zoom = 1.0f;
        float progress = 0.0f;

        if (temps > 4000) {
            float t = (temps - 4000) / 6000.0f; 
            progress = t * t * t * t; 
            zoom = 1.0f + (3.0f * progress); 
        }

        float focusX = (3 * 16) + 8.0f; 
        float focusY = (2 * 16) + 12.0f; 

        float currentScreenX = focusX + (160.0f - focusX) * progress;
        float currentScreenY = focusY + (120.0f - focusY) * progress;

        int shakeX = 0;
        int shakeY = 0;
        if (temps > 8000) {
            float intensite = ((temps - 8000) / 2000.0f) * 6.0f; 
            shakeX = (rand() % (int)(intensite * 2 + 1)) - (int)intensite;
            shakeY = (rand() % (int)(intensite * 2 + 1)) - (int)intensite;
        }

        SDL_Rect destRect = {
            (int)(currentScreenX - (focusX * zoom)) + shakeX,
            (int)(currentScreenY - (focusY * zoom)) + shakeY,
            (int)(320.0f * zoom),
            (int)(240.0f * zoom)
        };

        SDL_SetRenderTarget(renderer, renderTarget);
        
        DrawGame(renderer, font, fontMini);
        
        if (temps > 3000 && temps < 10000) {
            SDL_Color cBlanc = {200, 200, 255, 255};
            int offsetAnim = (SDL_GetTicks() / 200) % 5;
            SDL_Surface *sZzz = TTF_RenderText_Solid(fontMini, "Zzz...", cBlanc);
            if (sZzz) {
                SDL_Texture *tZzz = SDL_CreateTextureFromSurface(renderer, sZzz);
                SDL_Rect rZzz = { (int)focusX - 5, (int)focusY - 15 - offsetAnim, sZzz->w, sZzz->h };
                SDL_RenderCopy(renderer, tZzz, NULL, &rZzz);
                SDL_FreeSurface(sZzz);
                SDL_DestroyTexture(tZzz);
            }
        }

        SDL_SetRenderTarget(renderer, NULL);
        
        SDL_RenderCopy(renderer, renderTarget, NULL, &destRect);

        int alpha = 0;
        if (temps > 2000 && temps <= 3000) {
            alpha = (temps - 2000) * 180 / 1000; 
        }
        else if (temps > 3000 && temps <= 8000) {
            alpha = 180;
        }
        else if (temps > 8000) {
            alpha = 180 + ((temps - 8000) * 75 / 2000); 
        }

        if (alpha > 255) alpha = 255;

        if (alpha > 0) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_RenderFillRect(renderer, NULL);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }
    else if (temps < 13000) {
        isColorMode = 0; 
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (temps > 10500 && temps < 12800) {
            SDL_Color cBlanc = {230, 230, 230, 255};
            SDL_Surface *sTexte = TTF_RenderText_Solid(font, "Maman ? Papa ? Il fait tout noir...", cBlanc);
            if (sTexte) {
                SDL_Texture *tTexte = SDL_CreateTextureFromSurface(renderer, sTexte);
                SDL_Rect rTexte = { (320 - sTexte->w) / 2, (240 - sTexte->h) / 2, sTexte->w, sTexte->h };
                SDL_RenderCopy(renderer, tTexte, NULL, &rTexte);
                SDL_FreeSurface(sTexte);
                SDL_DestroyTexture(tTexte);
            }
        }
    }
    else if (temps < 16000) {
        isColorMode = 0; 
        
        float t = (temps - 13000) / 3000.0f; 
        float invT = 1.0f - t;
        float progress = 1.0f - (invT * invT * invT);
        
        float zoom = 4.0f - (3.0f * progress);

        float focusX = (5 * 16) + 8.0f; 
        float focusY = (3 * 16) + 8.0f; 

        float currentScreenX = 160.0f + (focusX - 160.0f) * progress;
        float currentScreenY = 120.0f + (focusY - 120.0f) * progress;

        SDL_Rect destRect = {
            (int)(currentScreenX - (focusX * zoom)),
            (int)(currentScreenY - (focusY * zoom)),
            (int)(320.0f * zoom),
            (int)(240.0f * zoom)
        };

        SDL_SetRenderTarget(renderer, renderTarget);
        DrawGame(renderer, font, fontMini);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, renderTarget, NULL, &destRect);

        int alpha = 255 - (int)(255 * progress);
        if (alpha < 0) alpha = 0;

        if (alpha > 0) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_RenderFillRect(renderer, NULL);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }
}