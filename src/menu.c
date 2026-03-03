#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "menu.h"
#include <string.h>

#define LOGICAL_WIDTH 320
#define LOGICAL_HEIGHT 240

static SDL_Color NOIR = {255, 255, 255, 255}; 
static SDL_Color BLANC = {0, 0, 0, 255};      

static SDL_Texture *bgMenuTexture = NULL;

static int isLoadSubMenu = 0; 
static int selection = 0;

static const char *optionsMain[] = {"JOUER", "CHARGER", "OPTIONS", "QUITTER"};
static int nbOptionsMain = 4;

static const char *optionsLoad[] = {"SAUVEGARDE MANUELLE", "SAUVEGARDE AUTO", "RETOUR"};
static int nbOptionsLoad = 3;

void dessinerTexteMenu(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int y, int selected) {
    SDL_Color colorTexte = selected ? BLANC : NOIR;
    SDL_Surface *surface = TTF_RenderText_Solid(font, texte, colorTexte);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = { (LOGICAL_WIDTH - surface->w) / 2, y, surface->w, surface->h };

    if (selected) {
        SDL_SetRenderDrawColor(renderer, NOIR.r, NOIR.g, NOIR.b, 255);
        SDL_Rect bgRect = {rect.x - 4, rect.y - 2, rect.w + 8, rect.h + 4};
        SDL_RenderFillRect(renderer, &bgRect);
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void dessinerCroix(SDL_Renderer *renderer, int x, int y) {
    SDL_SetRenderDrawColor(renderer, NOIR.r, NOIR.g, NOIR.b, 255);
    SDL_Rect p1 = {x+2, y, 2, 6};   SDL_RenderFillRect(renderer, &p1);
    SDL_Rect p2 = {x, y+2, 2, 2};   SDL_RenderFillRect(renderer, &p2);
    SDL_Rect p3 = {x+4, y+2, 2, 2}; SDL_RenderFillRect(renderer, &p3);
}

void InitMenu(SDL_Renderer *renderer) {
    selection = 0;
    isLoadSubMenu = 0; 
    
    SDL_Surface *surf = SDL_LoadBMP("assets/menu.bmp");
    if (surf) {
        bgMenuTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    } else {
        printf("Erreur chargement fond menu : %s\n", SDL_GetError());
    }
}

int UpdateMenu(SDL_Event *event) {
    int currentNbOptions = isLoadSubMenu ? nbOptionsLoad : nbOptionsMain;

    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                selection--;
                if (selection < 0) selection = currentNbOptions - 1;
                break;
            case SDLK_DOWN:
                selection++;
                if (selection >= currentNbOptions) selection = 0;
                break;
            case SDLK_ESCAPE: 
                if (isLoadSubMenu) {
                    isLoadSubMenu = 0;
                    selection = 1; 
                }
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                if (!isLoadSubMenu) { // MENU PRINCIPAL 
                    if (selection == 0) return 1; 
                    if (selection == 1) {         
                        isLoadSubMenu = 1;
                        selection = 0;
                        return 0; 
                    }
                    if (selection == 2) return 2; 
                    if (selection == 3) return 3; 
                } else {              // SOUS-MENU CHARGER 
                    if (selection == 0) { 
                        isLoadSubMenu = 0; selection = 1;
                        return 4; 
                    }
                    if (selection == 1) { 
                        isLoadSubMenu = 0; selection = 1;
                        return 5; 
                    }
                    if (selection == 2) { 
                        isLoadSubMenu = 0; selection = 1; 
                        return 0;
                    }
                }
                break;
        }
    }
    return 0; 
}

void DrawMenu(SDL_Renderer *renderer, TTF_Font *fontTitre, TTF_Font *fontOptions) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (bgMenuTexture) SDL_RenderCopy(renderer, bgMenuTexture, NULL, NULL);

    int currentNbOptions = isLoadSubMenu ? nbOptionsLoad : nbOptionsMain;
    const char **currentOptions = isLoadSubMenu ? optionsLoad : optionsMain;

    int startY = 100;
    for (int i = 0; i < currentNbOptions; i++) {
        int itemY = startY + (i * 25);
        dessinerTexteMenu(renderer, fontOptions, currentOptions[i], itemY, (i == selection));
        
        if (i == selection) {
            int textW, textH;
            TTF_SizeText(fontOptions, currentOptions[i], &textW, &textH);
            int croixX = (LOGICAL_WIDTH - textW) / 2 - 15; 
            dessinerCroix(renderer, croixX, itemY + 2);
        }
    }
}