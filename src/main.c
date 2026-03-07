#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "intro.h"
#include "menu.h"
#include "game.h"
#include "sons.h"
#include "pause.h"
#include "fin_jeu.h"
#include "game_reveille.h"
#include "options.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720
#define LOGICAL_WIDTH 320
#define LOGICAL_HEIGHT 240
#define FPS 60

typedef enum {
    ETAT_MENU,
    ETAT_INTRO,
    ETAT_CHARGEMENT,
    ETAT_JEU,
    ETAT_OPTIONS,
    ETAT_PAUSE,
    ETAT_FIN,
    ETAT_JEU_REVEILLE,
    ETAT_CONFIRM_QUIT
} GameState;

SDL_Renderer* renderer;
TTF_Font* font;

int fin_jeu = 0;
int game_is_loading = 0;
int save_type_to_load = 0;

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    srand(time(NULL));

    
  // --- 1. CONFIGURATION ANTI-FLOU (À faire AVANT la fenêtre) ---
    #ifdef SDL_HINT_WINDOWS_DPI_AWARENESS
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2"); 
#endif
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");             // Force les pixels carrés (Nearest)

    // --- 2. CREATION DE LA FENETRE ---
    SDL_Window *window = SDL_CreateWindow("Lights Out", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    // --- 3. CREATION DU RENDU (Avec Mode Sans Echec) ---
    FILE *safemode = fopen("safe_mode.txt", "r");
    
    if (safemode != NULL) {
        fclose(safemode);
        printf("Mode Sans Echec active (Software Rendering).\n");
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    } else {

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); 
        
        if (renderer == NULL) {
            printf("Erreur carte graphique, passage en mode processeur (Software)...\n");
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        }
    }
    
    // Si la carte graphique refuse complètement, on force le mode logiciel (Software)
    if (renderer == NULL) {
        printf("Erreur carte graphique, passage en mode processeur (Software)...\n");
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    // --- 4. MISE A L'ECHELLE INTERNE DU JEU ---
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE); // je suis obligé de faire ça pour eviter le petit decalage meme si ça peut rendre moins bien sous windows
    SDL_RenderSetLogicalSize(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT); // Résolution interne 320x240


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // --- AJOUT IMPORTANT ---
    // On prépare SDL_Mixer pour lire du OGG et du MP3 proprement
    int flags = MIX_INIT_OGG | MIX_INIT_MP3;
    if ((Mix_Init(flags) & flags) != flags) {
        printf("Erreur Mix_Init: %s\n", Mix_GetError());
    }

    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur SDL_mixer : %s\n", Mix_GetError());
    }

    // Mix_Chunk *sonPasEnBoucle = chargement_son_pas();
    // Mix_PlayChannel(2, sonPasEnBoucle, -1); // -1 = boucle infinie
    // Mix_Volume(2, 0); // On commence volume à 0

    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (TTF_Init() < 0) return 1;


    SDL_RenderSetLogicalSize(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); //nearest pour une sorte de Vsync 

    TTF_Font *fontGrand = TTF_OpenFont("assets/font.ttf", 20);
    TTF_Font *fontPetit = TTF_OpenFont("assets/font.ttf", 12);
    TTF_Font *fontMini = TTF_OpenFont("assets/font.ttf", 11);

    font = fontMini;

    if (!fontGrand || !fontPetit || !fontMini) { printf("Erreur Font\n"); return 1; }

    GameState etat = ETAT_MENU;
    
    GameState etat_avant_pause = ETAT_JEU;
    GameState etat_avant_options = ETAT_MENU;

    int selectionQuit = 0; 
    GameState etat_avant_quit = ETAT_MENU;
    int destination_quit = 0;

    InitIntro();
    InitMenu(renderer);
    int vraiPourcentage = 0;

    int running = 1;
    SDL_Event event;

    Uint32 frameStart;
    int frameTime;
    const int frameDelay = 1000 / FPS;

    // --- BOUCLE PRINCIPALE ---
    while (running) {
        frameStart = SDL_GetTicks();
        
        // A. GESTION DES EVENEMENTS (Clavier / Souris)
        while (SDL_PollEvent(&event)) {

           if (event.type == SDL_QUIT) {
                if (etat == ETAT_JEU || etat == ETAT_JEU_REVEILLE || etat == ETAT_PAUSE) {
                    etat_avant_quit = etat;
                    etat = ETAT_CONFIRM_QUIT;
                    selectionQuit = 0;
                    destination_quit = 0;
                } else if (etat != ETAT_CONFIRM_QUIT) {
                    running = 0;
                }
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11) {
                // On récupère l'état actuel
                Uint32 flags = SDL_GetWindowFlags(window);
                
                // Si on est déjà en plein écran, on repasse en fenêtré (0)
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                    SDL_SetWindowFullscreen(window, 0);
                } else {
                    // Sinon, on passe en plein écran "Bureau" (Borderless)
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
            }

            if (etat == ETAT_MENU) {
                int action = UpdateMenu(&event);
                if (action == 1) { 
                    if(fin_jeu) etat = ETAT_JEU_REVEILLE;
                    else {
                        ResetGame();
                        etat = ETAT_CHARGEMENT; 
                        vraiPourcentage = 0;   
                    }
                }
                if (action == 2) {
                    etat = ETAT_OPTIONS;
                    InitOptions();
                }
                if (action == 3) running = 0;
                if (action == 4) { // CHARGER MANUEL
                    game_is_loading = 1;
                    save_type_to_load = 0;
                    etat = ETAT_CHARGEMENT; vraiPourcentage = 0;
                }
                if (action == 5) { // CHARGER AUTO
                    game_is_loading = 1;
                    save_type_to_load = 1;
                    etat = ETAT_CHARGEMENT; vraiPourcentage = 0;
                }
            }
            else if (etat == ETAT_OPTIONS) {
                int actionOptions = UpdateOptions(&event);
                if (actionOptions == 1) { // Si UpdateOptions renvoie 1 (Bouton Retour)
                    etat = etat_avant_options;     // On retourne au menu principal !
                }
            }
            else if (etat == ETAT_INTRO) {
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                    SkipIntro();
                    etat = ETAT_JEU; 
                }
            }
            // --- GESTION DE LA TOUCHE ECHAP ---
            else if (etat == ETAT_JEU || etat == ETAT_JEU_REVEILLE) {
                
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    etat_avant_pause = etat; // On sauvegarde si on était réveillé ou non
                    etat = ETAT_PAUSE;       // Et on passe en pause !
                }
            }
            // --- GESTION DU MENU PAUSE ---
            else if (etat == ETAT_PAUSE) {
                int actionPause = UpdatePause(&event);
                
                if (actionPause == 1) { // 1 = Continuer
                    etat = etat_avant_pause; // Restaure ETAT_JEU ou ETAT_JEU_REVEILLE
                }
                else if (actionPause == 2) { // 2 = Menu Principal
                    etat_avant_quit = ETAT_PAUSE;
                    etat = ETAT_CONFIRM_QUIT;
                    selectionQuit = 0;
                    destination_quit = 1; // 1 = Retourner au Menu
                }
                else if (actionPause == 3){
                    etat_avant_options = ETAT_PAUSE;
                    etat = ETAT_OPTIONS;
                    InitOptions();
                }
                else if (actionPause == 4) {
                    SauvegarderPartie(0);
                    etat = etat_avant_pause;
                }
                else if (actionPause == 5) { // 5 = Quitter
                    etat_avant_quit = ETAT_PAUSE;
                    etat = ETAT_CONFIRM_QUIT;
                    selectionQuit = 0;
                    destination_quit = 0; // 0 = Fermer le jeu
                }
            }
            // --- GESTION DE LA FIN DE JEU ---
            else if(etat == ETAT_FIN){
                lancer_scene_fin(event, &menu_fin);

                if(menu_fin == 0){
                    etat = ETAT_JEU;
                    dialogue_maman=0;

                    if(choix == 0){
                        printf("ON PART\n"); 
                        // Ton appel de fonction exactement comme tu le voulais :
                        InitGameStepByStepReveille(renderer); 
                        etat = ETAT_JEU_REVEILLE;
                        currentLevel = 0;
                        fin_jeu=1;
                        // Faudrait faire la logique où on se tp à la salle dans la vraie vie
                    }
                    else if(choix == 1){
                        printf("ON RESTE\n");
                    }
                }
            }
            else if (etat == ETAT_CONFIRM_QUIT) {
                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            selectionQuit--;
                            if (selectionQuit < 0) selectionQuit = 2;
                            break;
                        case SDLK_DOWN:
                            selectionQuit++;
                            if (selectionQuit > 2) selectionQuit = 0;
                            break;
                        case SDLK_RETURN:
                        case SDLK_KP_ENTER:
                            if (selectionQuit == 0) { // OUI (Sauvegarder)
                                SauvegarderPartie(1); 
                                if (destination_quit == 0) {
                                    running = 0; // Quitter le jeu
                                } else {
                                    // Retour au menu principal
                                    etat = ETAT_MENU;
                                    Mix_HaltMusic(); 
                                    Mix_HaltChannel(-1);
                                    ResetGame();
                                }
                            } else if (selectionQuit == 1) { // NON (Ne pas sauver)
                                if (destination_quit == 0) {
                                    running = 0; // Quitter le jeu
                                } else {
                                    // Retour au menu principal
                                    etat = ETAT_MENU;
                                    Mix_HaltMusic(); 
                                    Mix_HaltChannel(-1);
                                    ResetGame();
                                }
                            } else if (selectionQuit == 2) { // ANNULER
                                etat = etat_avant_quit;
                            }
                            break;
                    }
                }
            }
        }

       if (etat == ETAT_INTRO) {
            if (UpdateIntroTimer() == 1) {
                etat = ETAT_JEU;
            }
        }
        else if (etat == ETAT_CHARGEMENT) {
            int fini = InitGameStepByStep(renderer, &vraiPourcentage);
            if (fini == 1) {
            if (game_is_loading) {
                    // On nettoie d'abord tout par sécurité
                    ResetGame(); 
                    
                    if (ChargerPartie(save_type_to_load)) {
                        if (fin_jeu == 1) {
                           
                            float saveX = player.x;
                            float saveY = player.y;
                            
                        
                            InitGameStepByStepReveille(renderer); 
                            
                            
                            player.x = saveX;
                            player.y = saveY;
                            
                            etat = ETAT_JEU_REVEILLE;
                        } else {
                            etat = ETAT_JEU; 
                        }
                    } else {
                        // SI AUCUNE SAUVEGARDE : on force l'intro proprement
                        printf("Aucune sauvegarde trouvee, lancement nouvelle partie...\n");
                        ResetGame(); // Double sécurité
                        StartIntro(renderer); 
                        etat = ETAT_INTRO;
                    }
                    game_is_loading = 0;
                } else {
                    etat = ETAT_INTRO;
                    StartIntro(renderer);
                }
            }
        }
        else if (etat == ETAT_JEU)
        {
            UpdateGame();
            if(menu_fin == 1)etat = ETAT_FIN;
        }
        else if(etat == ETAT_JEU_REVEILLE){
            UpdateGameReveille();
        }

        // C. DESSIN
        if (etat == ETAT_MENU) {
            DrawMenu(renderer, fontGrand, fontPetit);
        }
        else if (etat == ETAT_OPTIONS) {
            DrawOptions(renderer, fontGrand, fontPetit);
        }
        else if(etat==ETAT_FIN){
            DrawGame(renderer, fontPetit, fontMini);
            afficher_menu_fin(renderer, fontMini);
        }
        else if (etat == ETAT_INTRO) {
            DrawIntro(renderer, fontPetit, fontMini);
        }
        else if (etat == ETAT_CHARGEMENT) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            int largeurMax = LOGICAL_WIDTH - 40; 
            int hauteurBarre = 4;                 
            int posX = 20;                        
            int posY = LOGICAL_HEIGHT - 20;       

            SDL_Color cGris = {150, 150, 150, 255}; 
            SDL_Surface *sText = TTF_RenderText_Solid(fontMini, "Chargement...", cGris);
            if (sText) {
                SDL_Texture *tText = SDL_CreateTextureFromSurface(renderer, sText);
                SDL_Rect rText = { posX + largeurMax - sText->w, posY - sText->h - 4, sText->w, sText->h };
                SDL_RenderCopy(renderer, tText, NULL, &rText);
                SDL_FreeSurface(sText);
                SDL_DestroyTexture(tText);
            }

            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); 
            SDL_Rect rFondBarre = { posX, posY, largeurMax, hauteurBarre };
            SDL_RenderFillRect(renderer, &rFondBarre);

            int remplissage = (vraiPourcentage * largeurMax) / 100; 
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
            SDL_Rect rRemplissage = { posX, posY, remplissage, hauteurBarre };
            SDL_RenderFillRect(renderer, &rRemplissage);
        }
        else if (etat == ETAT_JEU) {
            DrawGame(renderer,fontPetit, fontMini);
        }
        else if(etat == ETAT_JEU_REVEILLE){
            DrawGameReveille(renderer, fontPetit, fontMini);
        }
        // --- DESSIN DU MENU PAUSE SUR LA BONNE MAP ---
        else if (etat == ETAT_PAUSE) {
            if (etat_avant_pause == ETAT_JEU_REVEILLE) {
                DrawGameReveille(renderer, fontPetit, fontMini); // Fond de jour
            } else {
                DrawGame(renderer, fontPetit, fontMini);         // Fond de nuit
            }
            DrawPause(renderer, fontGrand, fontPetit);           // Par-dessus le jeu !
        }
        // --- DESSIN CONFIRMATION QUITTER ---
        else if (etat == ETAT_CONFIRM_QUIT) {
            // 1. Redessiner le fond en fonction d'où on vient
            if (etat_avant_quit == ETAT_PAUSE) {
                if (etat_avant_pause == ETAT_JEU_REVEILLE) DrawGameReveille(renderer, fontPetit, fontMini);
                else DrawGame(renderer, fontPetit, fontMini);
                DrawPause(renderer, fontGrand, fontPetit);
            } else if (etat_avant_quit == ETAT_JEU) {
                DrawGame(renderer, fontPetit, fontMini);
            } else if (etat_avant_quit == ETAT_JEU_REVEILLE) {
                DrawGameReveille(renderer, fontPetit, fontMini);
            }

            // 2. Assombrir l'écran
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            SDL_RenderFillRect(renderer, NULL);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

            // 3. Dessiner la boîte
            SDL_Rect box = { LOGICAL_WIDTH/2 - 90, LOGICAL_HEIGHT/2 - 45, 180, 90 };
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
            SDL_RenderFillRect(renderer, &box);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &box);

            // 4. Dessiner le titre
            SDL_Color cBlanc = {255, 255, 255, 255};
            SDL_Color cDore = {255, 215, 0, 255};
            SDL_Color cGris = {150, 150, 150, 255};

            SDL_Surface *sTitre = TTF_RenderText_Solid(fontPetit, "Voulez vous sauvegarder ?", cBlanc);
            if (sTitre) {
                SDL_Texture *tTitre = SDL_CreateTextureFromSurface(renderer, sTitre);
                SDL_Rect rTitre = { LOGICAL_WIDTH/2 - sTitre->w/2, box.y + 10, sTitre->w, sTitre->h };
                SDL_RenderCopy(renderer, tTitre, NULL, &rTitre);
                SDL_FreeSurface(sTitre);
                SDL_DestroyTexture(tTitre);
            }

            // 5. Dessiner les choix
            const char* opts[3] = {"OUI", "NON", "ANNULER"};
            for (int i = 0; i < 3; i++) {
                SDL_Color color = (i == selectionQuit) ? cDore : cGris;
                SDL_Surface *sOpt = TTF_RenderText_Solid(fontPetit, opts[i], color);
                if (sOpt) {
                    SDL_Texture *tOpt = SDL_CreateTextureFromSurface(renderer, sOpt);
                    SDL_Rect rOpt = { LOGICAL_WIDTH/2 - sOpt->w/2, box.y + 35 + (i * 15), sOpt->w, sOpt->h };
                    SDL_RenderCopy(renderer, tOpt, NULL, &rOpt);
                    SDL_FreeSurface(sOpt);
                    SDL_DestroyTexture(tOpt);
                    
                    if (i == selectionQuit) {
                        SDL_Surface *sCurs = TTF_RenderText_Solid(fontPetit, ">", color);
                        if (sCurs) {
                            SDL_Texture *tCurs = SDL_CreateTextureFromSurface(renderer, sCurs);
                            SDL_Rect rCurs = { rOpt.x - 15, rOpt.y, sCurs->w, sCurs->h };
                            SDL_RenderCopy(renderer, tCurs, NULL, &rCurs);
                            SDL_FreeSurface(sCurs);
                            SDL_DestroyTexture(tCurs);
                        }
                    }
                }
            }
        }

        // 4. Bordure
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect bordure = {2, 2, LOGICAL_WIDTH - 4, LOGICAL_HEIGHT - 4};
        SDL_RenderDrawRect(renderer, &bordure);

        SDL_RenderPresent(renderer);
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

     CleanGame(); 
    // Nettoyage sons
    Mix_CloseAudio();

    // Nettoyage...
    TTF_CloseFont(fontGrand);
    TTF_CloseFont(fontPetit);
    TTF_CloseFont(fontMini);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}