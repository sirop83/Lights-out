#ifndef SONS_H
#define SONS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

Mix_Music *chargement_son_ambiance();
Mix_Music *chargement_son_exterieur();
Mix_Music *chargement_son_maison_reveil();
Mix_Music *chargement_son_maison_reveil_interieur();

Mix_Chunk *chargement_son_pas();
Mix_Chunk *chargement_son_transition();
Mix_Chunk *chargement_son_item_pick_up();
Mix_Chunk *chargement_son_door_open();
Mix_Chunk *chargement_son_door_close();
Mix_Chunk *chargement_son_screamer();
Mix_Chunk *chargement_son_pendule();

#endif 