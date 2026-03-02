#include "options.h"
#include <SDL2/SDL_mixer.h>

int globalVolumeMusique = 32;
int globalVolumeBruitages = 32;

int isMusicMuted = 0;       // 0 = Son activé, 1 = Son coupé
int isSfxMuted = 0;
int prevVolumeMusique = 32; // Pour restaurer le volume quand on unmute
int prevVolumeSfx = 32;

static int optionSelection = 0; // 0 = Musique, 1 = SFX, 2 = Retour

void InitOptions(void) {
    optionSelection = 0;
}

int UpdateOptions(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                optionSelection--;
                if (optionSelection < 0) optionSelection = 2; // 2 = Retour
                break;

            case SDLK_DOWN:
                optionSelection++;
                if (optionSelection > 2) optionSelection = 0;
                break;
            
            case SDLK_LEFT: 
                if (optionSelection == 0 && !isMusicMuted) { 
                    globalVolumeMusique -= 8;
                    if (globalVolumeMusique < 0) globalVolumeMusique = 0;
                    Mix_VolumeMusic(globalVolumeMusique); 
                } else if (optionSelection == 1 && !isSfxMuted) { 
                    globalVolumeBruitages -= 8;
                    if (globalVolumeBruitages < 0) globalVolumeBruitages = 0;
                    Mix_Volume(-1, globalVolumeBruitages); 
                }
                break;
                
            case SDLK_RIGHT: 
                if (optionSelection == 0 && !isMusicMuted) {
                    globalVolumeMusique += 8;
                    if (globalVolumeMusique > 128) globalVolumeMusique = 128; 
                    Mix_VolumeMusic(globalVolumeMusique);
                } else if (optionSelection == 1 && !isSfxMuted) { 
                    globalVolumeBruitages += 8;
                    if (globalVolumeBruitages > 128) globalVolumeBruitages = 128;
                    Mix_Volume(-1, globalVolumeBruitages);
                }
                break;
                
            case SDLK_RETURN:
            case SDLK_KP_ENTER: 
                if (optionSelection == 0) { // Mute Musique
                    isMusicMuted = !isMusicMuted;
                    if (isMusicMuted) {
                        prevVolumeMusique = globalVolumeMusique; // On sauvegarde
                        globalVolumeMusique = 0;                 // On coupe
                    } else {
                        globalVolumeMusique = prevVolumeMusique; // On restaure
                    }
                    Mix_VolumeMusic(globalVolumeMusique);
                } 
                else if (optionSelection == 1) { // Mute Bruitages
                    isSfxMuted = !isSfxMuted;
                    if (isSfxMuted) {
                        prevVolumeSfx = globalVolumeBruitages;
                        globalVolumeBruitages = 0;
                    } else {
                        globalVolumeBruitages = prevVolumeSfx;
                    }
                    Mix_Volume(-1, globalVolumeBruitages);
                }
                if (optionSelection == 2) {
                    return 1; 
                }
                break;
            case SDLK_ESCAPE:
                return 1;
        }
    }
    return 0; 
}

static void DrawTextOptions(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dest = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &dest);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

static void DrawVolumeBar(SDL_Renderer *renderer, int x, int y, int volume) {
    // Fond sombre de la jauge
    SDL_Rect bgRect = {x, y, 128, 12};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &bgRect);

    // Barre blanche (remplissage)
    SDL_Rect fgRect = {x, y, volume, 12}; 
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderFillRect(renderer, &fgRect);

    // Contour blanc
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bgRect);
}

void DrawOptions(SDL_Renderer *renderer, TTF_Font *fontGrand, TTF_Font *fontPetit) {
    // Fond noir
    SDL_SetRenderDrawColor(renderer, 15, 15, 20, 255); 
    SDL_RenderClear(renderer);

    SDL_Color cBlanc = {255, 255, 255, 255};
    SDL_Color cGris = {120, 120, 120, 255};
    SDL_Color cDore = {255, 215, 0, 255}; // Couleur quand c'est sélectionné

    // Titre
    DrawTextOptions(renderer, fontGrand, "- OPTIONS -", 90, 30, cBlanc);

    // 1. Musique
    SDL_Color cMusique = (optionSelection == 0) ? cDore : cGris;
    DrawTextOptions(renderer, fontPetit, "Musique", 30, 90, cMusique);
    
    // Si ce n'est pas muté, on dessine les flèches et la jauge normalement
    if (!isMusicMuted) {
        if (optionSelection == 0) DrawTextOptions(renderer, fontPetit, "<", 115, 90, cDore);
        DrawVolumeBar(renderer, 130, 91, globalVolumeMusique);
        if (optionSelection == 0) DrawTextOptions(renderer, fontPetit, ">", 265, 90, cDore);
    } else {
        // Si c'est muté, on dessine une barre vide et grise pour montrer que c'est désactivé
        DrawVolumeBar(renderer, 130, 91, 0); 
    }

    // Dessin de la Checkbox Mute (à droite)
    if (isMusicMuted) {
        DrawTextOptions(renderer, fontPetit, "[X]", 285, 90, (optionSelection == 0) ? cDore : cGris);
    } else {
        DrawTextOptions(renderer, fontPetit, "[ ]", 285, 90, (optionSelection == 0) ? cDore : cGris);
    }


    
    // 2. Bruitages (SFX)
    SDL_Color cSFX = (optionSelection == 1) ? cDore : cGris;
    DrawTextOptions(renderer, fontPetit, "Bruitages", 30, 130, cSFX);
    
    if (!isSfxMuted) {
        if (optionSelection == 1) DrawTextOptions(renderer, fontPetit, "<", 115, 130, cDore);
        DrawVolumeBar(renderer, 130, 131, globalVolumeBruitages);
        if (optionSelection == 1) DrawTextOptions(renderer, fontPetit, ">", 265, 130, cDore);
    } else {
        DrawVolumeBar(renderer, 130, 131, 0);
    }

    // Dessin de la Checkbox Mute (à droite)
    if (isSfxMuted) {
        DrawTextOptions(renderer, fontPetit, "[X]", 285, 130, (optionSelection == 1) ? cDore : cGris);
    } else {
        DrawTextOptions(renderer, fontPetit, "[ ]", 285, 130, (optionSelection == 1) ? cDore : cGris);
    }
    SDL_Color cRetour = (optionSelection == 2) ? cDore : cGris;
    if (optionSelection == 2) DrawTextOptions(renderer, fontPetit, ">", 115, 190, cDore);
    DrawTextOptions(renderer, fontPetit, "Retour", 135, 190, cRetour);
}