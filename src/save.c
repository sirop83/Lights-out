#include <stdio.h>
#include <stdlib.h>
#include "save.h"

int SaveGame(const char* filepath, SaveData data) {

    FILE* file = fopen(filepath, "wb"); 

    if (file == NULL) {
        printf("Erreur : Impossible de créer le fichier de sauvegarde.\n");
        return 0; 
    }


    size_t written = fwrite(&data, sizeof(SaveData), 1, file); 


    fclose(file);


    if (written == 1) {
        printf("Sauvegarde réussie !\n");
        return 1; 
    } else {
        printf("Erreur lors de l'écriture des données.\n");
        return 0; 
    }
}

int LoadGame(const char* filepath, SaveData* data) {
    FILE* file = fopen(filepath, "rb"); 

    if (file == NULL) {
        printf("Save introuvable : %s\n", filepath);
        return 0; 
    }

    size_t read = fread(data, sizeof(SaveData), 1, file);

    fclose(file);

    if (read == 1) {
        printf("Chargement réussi depuis %s\n", filepath);
        return 1; 
    } else {
        printf("Erreur\n");
        return 0; 
    }
}