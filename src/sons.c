#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "intro.h"
#include "menu.h"
#include "game.h"
#include "sons.h"

Mix_Music *chargement_son_ambiance(){
	Mix_Music *bgm = Mix_LoadMUS("assets/sound/scary-ambience.ogg");
	// Mix_Music *bgm = Mix_LoadMUS("assets/sound/horror-background-atmosphere.ogg");
	// Mix_Music *bgm = Mix_LoadMUS("assets/sound/horror-ambient.ogg");
	if (!bgm) printf("Erreur chargement musique : %s\n", Mix_GetError());

	// Mettre le volume des bruitages à 50%
	// Mix_Volume(-1, 64);
	return bgm;
}

Mix_Music *chargement_son_exterieur(){
	Mix_Music *bgm = Mix_LoadMUS("assets/sound/scary-night-ambience.ogg");
	if (!bgm) printf("Erreur chargement musique : %s\n", Mix_GetError());

	// Mettre le volume des bruitages à 50%
	// Mix_Volume(-1, 64);
	return bgm;
}

Mix_Music *chargement_son_maison_reveil(){
	Mix_Music *bgm_reveil = Mix_LoadMUS("assets/sound/Ambiance_maison.wav");
	if(!bgm_reveil)printf("Erreur chargement musique : %s\n", Mix_GetError());
	return bgm_reveil;
}

Mix_Music *chargement_son_maison_reveil_interieur(){
	Mix_Music *bgm_reveil_maison = Mix_LoadMUS("assets/sound/Musique_fond_pokemon.ogg");
	if(!bgm_reveil_maison)printf("Erreur chargement musique : %s\n", Mix_GetError());
	return bgm_reveil_maison;
}

Mix_Chunk *chargement_son_pendule(){
	Mix_Chunk *sonPendule = Mix_LoadWAV("assets/sound/videoplayback.wav");
	if(!sonPendule) printf("Erreur chargement WAV: %s \n", Mix_GetError());
	return sonPendule;
}

Mix_Chunk *chargement_son_pas(){
	Mix_Chunk *sonPas = Mix_LoadWAV("assets/sound/foot-step.wav");

    if (!sonPas) printf("Erreur chargement WAV: %s\n", Mix_GetError());

    return sonPas;
}

Mix_Chunk *chargement_son_transition(){
	Mix_Chunk *sonTransition = Mix_LoadWAV("assets/sound/transition-whoosh.wav");

	if (!sonTransition) printf("Erreur chargement WAV: %s\n", Mix_GetError());

	return sonTransition;
}

Mix_Chunk *chargement_son_screamer(){
	Mix_Chunk *sonScreamer = Mix_LoadWAV("assets/sound/screamer-jumpscare.wav");

	if (!sonScreamer) printf("Erreur chargement WAV: %s\n", Mix_GetError());

	return sonScreamer;
}

Mix_Chunk *chargement_son_item_pick_up(){
	Mix_Chunk *sonPickUp = Mix_LoadWAV("assets/sound/item-pick-up.wav");
	if (!sonPickUp) printf("Erreur chargement WAV: %s\n", Mix_GetError());

	return sonPickUp;
}

Mix_Chunk *chargement_son_door_open(){
	Mix_Chunk *sonOpenDoor = Mix_LoadWAV("assets/sound/door_open.wav");
	if (!sonOpenDoor) printf("Erreur chargement WAV: %s\n", Mix_GetError());

	return sonOpenDoor;
}

Mix_Chunk *chargement_son_door_close(){
	Mix_Chunk *sonCloseDoor = Mix_LoadWAV("assets/sound/door_close.wav");
	if (!sonCloseDoor) printf("Erreur chargement WAV: %s\n", Mix_GetError());

	return sonCloseDoor;
}