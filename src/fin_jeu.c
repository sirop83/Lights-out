#include "fin_jeu.h"
#include "game.h"
#include "intro.h"
#include "map_reveille.h"

int choix = 0;
int nbOptions = 2;

void lancer_scene_fin(SDL_Event event, int *continuer_menu) { //regarde le choix du joueur
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                choix--;
                if (choix < 0) choix = nbOptions - 1; //choix 0
                break;
            case SDLK_DOWN:
                choix++;
                if (choix >= nbOptions) choix = 0;
                break;
            case SDLK_RETURN: // Touche Entrée pour valider
                if (choix == 0) {
                    printf("Action : Le joueur décide de PARTIR.\n");
                } else {
                    printf("Action : Le joueur décide de RESTER.\n");
                }
                *continuer_menu = 0; // sort de la boucle menu
                break;
        }
    }
}

void DrawEllipse(SDL_Renderer *renderer, TTF_Font *font) {
    // fond Noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    dessinerTexteInterne(renderer, font, "20 ans plus tard...", 110);

}

// void afficher_menu_fin(SDL_Renderer *renderer, TTF_Font *font) {

//     // On dessine juste les deux boutons (DrawTexte fait déjà des belles boîtes)
//     // On les place au centre (X=110)
//     DrawTexte("Partir", renderer, font, 110, 80, 100, 30);
//     DrawTexte("Rester", renderer, font, 110, 120, 100, 30);

//     // Dessiner la flèche (=>)
//     // La flèche se place à côté du bouton (X=70)
//     int flecheY = (choix == 0) ? 80 : 120;
//     DrawTexte("=>", renderer, font, 70, flecheY, 35, 30);
// }

void afficher_menu_fin(SDL_Renderer *renderer, TTF_Font *font) {

    // 1. Définir la boîte de choix (plus petite et plus basse)
    int boxX = 230; // Encore plus à droite
    int boxY = 135; // Plus basse, collée au-dessus du dialogue
    int boxW = 65;  // Boîte moins large
    int boxH = 40;  // Boîte moins haute

    SDL_Rect rectBulle = { boxX, boxY, boxW, boxH };

    // Fond blanc
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150); 
    SDL_RenderFillRect(renderer, &rectBulle);

    // Bordure noire
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rectBulle);

    // 2. Préparer la couleur du texte
    SDL_Color colorNoir = {0, 0, 0, 255}; 
    // 3. Dessiner "Partir" (plus resserré)
    SDL_Surface *sPartir = TTF_RenderText_Solid(font, "Partir", colorNoir);
    if (sPartir) {
        SDL_Texture *tPartir = SDL_CreateTextureFromSurface(renderer, sPartir);
        SDL_Rect rPartir = { boxX + 20, boxY + 6, sPartir->w, sPartir->h }; // X+20, Y+6
        SDL_RenderCopy(renderer, tPartir, NULL, &rPartir);
        SDL_FreeSurface(sPartir);
        SDL_DestroyTexture(tPartir);
    }

    // 4. Dessiner "Rester"
    SDL_Surface *sRester = TTF_RenderText_Solid(font, "Rester", colorNoir);
    if (sRester) {
        SDL_Texture *tRester = SDL_CreateTextureFromSurface(renderer, sRester);
        SDL_Rect rRester = { boxX + 20, boxY + 22, sRester->w, sRester->h }; // X+20, Y+22
        SDL_RenderCopy(renderer, tRester, NULL, &rRester);
        SDL_FreeSurface(sRester);
        SDL_DestroyTexture(tRester);
    }

    // 5. Dessiner le curseur (>)
    int flecheY = (choix == 0) ? (boxY + 6) : (boxY + 22); // S'aligne avec les textes

    SDL_Surface *sFleche = TTF_RenderText_Solid(font, ">", colorNoir);
    if (sFleche) {
        SDL_Texture *tFleche = SDL_CreateTextureFromSurface(renderer, sFleche);
        SDL_Rect rFleche = { boxX + 6, flecheY, sFleche->w, sFleche->h }; // Curseur en X+6
        SDL_RenderCopy(renderer, tFleche, NULL, &rFleche);
        SDL_FreeSurface(sFleche);
        SDL_DestroyTexture(tFleche);
    }
}
