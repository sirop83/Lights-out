#include "game.h"
#include "map.h"
#include <stdio.h>
#include <math.h>

Joueur player;
Fantome fantome;

float FANTOME_SPEED;

typedef struct {
    int x, y;
} Point;

// Fonction BFS pour trouver la direction vers le joueur
// Retourne la direction (0: Haut, 1: Bas, 2: Gauche, 3: Droite, -1: Stop)
int ObtenirDirectionBFS(int startX, int startY, int targetX, int targetY) {
    if (startX == targetX && startY == targetY) return -1;

    // Tableaux pour suivre les parents (d'où on vient) pour reconstruire le chemin
    // predecesseur[y][x] stocke la direction qu'on a prise pour arriver sur la case x,y
    static int predecesseur[MAP_HEIGHT][MAP_WIDTH]; 
    static int visited[MAP_HEIGHT][MAP_WIDTH];

    // Reset des tableaux
    for(int y=0; y<MAP_HEIGHT; y++) {
        for(int x=0; x<MAP_WIDTH; x++) {
            visited[y][x] = 0;
            predecesseur[y][x] = -1;
        }
    }

    // File d'attente pour le BFS (taille max = taille carte)
    Point queue[MAP_WIDTH * MAP_HEIGHT];
    int head = 0;
    int tail = 0;

    // On ajoute le point de départ
    queue[tail++] = (Point){startX, startY};
    visited[startY][startX] = 1;

    int found = 0;

    // Directions : 0=Haut, 1=Bas, 2=Gauche, 3=Droite
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    // --- ALGORITHME DE PROPAGATION (INONDATION) ---
    while (head < tail) {
        Point current = queue[head++];

        // Si on a atteint la cible (le joueur)
        if (current.x == targetX && current.y == targetY) {
            found = 1;
            break;
        }

        // On teste les 4 voisins
        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            // Vérifications : dans la carte ? pas un mur ? pas déjà visité ?
            if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
                int type = maps[currentLevel][ny][nx];
                int isWalkable = (type == 0 || type == 1 || type == 82 || type == 115 || type == 116 || type == 117 || type == 118);

                if (isWalkable && !visited[ny][nx]) {
                    visited[ny][nx] = 1;
                    predecesseur[ny][nx] = i; // On note la direction prise pour arriver ici
                    queue[tail++] = (Point){nx, ny};
                }
            }
        }
    }

    // --- RECONSTRUCTION DU CHEMIN ---
    if (found) {
        // On part de la cible (le joueur) et on remonte jusqu'au fantôme
        int currX = targetX;
        int currY = targetY;
        int directionFinale = -1;

        while (currX != startX || currY != startY) {
            int dirToGetHere = predecesseur[currY][currX];
            
            // C'est la direction que le fantôme devra prendre
            directionFinale = dirToGetHere;

            // On remonte au parent (inverse de la direction)
            if (dirToGetHere == 0) currY++;      // Si on est venu du haut, on redescend
            else if (dirToGetHere == 1) currY--; // Si on est venu du bas, on remonte
            else if (dirToGetHere == 2) currX++; // Si on est venu de gauche, on va droite
            else if (dirToGetHere == 3) currX--; // Si on est venu de droite, on va gauche
        }
        return directionFinale;
    }

    return -1; // Pas de chemin trouvé
}

void SpawnFantomeRandom() {
    int validPosition = 0;
    int maxEssais = 100; // Sécurité pour ne pas boucler à l'infini

    while (!validPosition && maxEssais > 0) {
        // 1. On choisit une case au hasard (en évitant les bords extérieurs)
        int rx = (rand() % (MAP_WIDTH - 2)) + 1;
        int ry = (rand() % (MAP_HEIGHT - 2)) + 1;

        // 2. On vérifie le type de la case dans le niveau actuel
        int type = maps[currentLevel][ry][rx];

        if (type == 82) {
            
            // 3. On vérifie que ce n'est pas trop près du joueur (Spawn kill)
            float fx = rx * TILE_SIZE;
            float fy = ry * TILE_SIZE;
            
            float dx = player.x - fx;
            float dy = player.y - fy;
            float dist = sqrt(dx*dx + dy*dy);

            // Si on est à plus de 5 tuiles (80 pixels) du joueur
            if (dist > 80) {
                fantome.x = fx;
                fantome.y = fy;
                fantome.direction = 0; // Reset direction
                fantome.timer = 0;     // Reset cerveau
                validPosition = 1;     // C'est bon, on sort de la boucle
            }
        }
        maxEssais--;
    }
    
    // Si après 100 essais on n'a pas trouvé (très improbable), on le met au milieu par défaut
    if (!validPosition) {
        fantome.x = 10 * TILE_SIZE;
        fantome.y = 7 * TILE_SIZE;
    }
}

int isWallSimple(float x, float y) {
    int caseX = (int)(x / TILE_SIZE);
    int caseY = (int)(y / TILE_SIZE);

    // 1. Sécurité : Si hors de la carte, c'est un mur
    if (caseX < 0 || caseX >= MAP_WIDTH || caseY < 0 || caseY >= MAP_HEIGHT) {
        return 1;
    }

    // 2. Lecture de la tuile
    int type = maps[currentLevel][caseY][caseX];

    if (type == 0 || type == 1 || type == 82 || type == 115 || type == 116 || type == 117 || type == 118) {
        return 0; // C'est libre
    }

    return 1; // C'est un mur
}

// Fonction utilitaire collision (avec marge de sécurité +1 pixel)
int CheckCollisionFantome(float x, float y) {
    // On vérifie les 4 coins du fantôme
    if (isWallSimple(x + 1, y + 1)) return 0;
    if (isWallSimple(x + fantome.w - 1, y + 1)) return 0;
    if (isWallSimple(x + 1, y + fantome.h - 1)) return 0;
    if (isWallSimple(x + fantome.w - 1, y + fantome.h - 1)) return 0;
    
    return 1; // 1 = La voie est libre 
}

// Vérifie si un mur bloque la vue entre (x1,y1) et (x2,y2)
int CheckLineOfSight(float x1, float y1, float x2, float y2) {
    int x0 = (int)(x1 / TILE_SIZE);
    int y0 = (int)(y1 / TILE_SIZE);
    int xEnd = (int)(x2 / TILE_SIZE);
    int yEnd = (int)(y2 / TILE_SIZE);

    int dx = abs(xEnd - x0);
    int dy = -abs(yEnd - y0);
    int sx = x0 < xEnd ? 1 : -1;
    int sy = y0 < yEnd ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        if (x0 < 0 || x0 >= MAP_WIDTH || y0 < 0 || y0 >= MAP_HEIGHT) return 0;
        
        int type = maps[currentLevel][y0][x0];
        if (type != 0 && type != 1 && type != 82) return 0; // Mur détecté !

        if (x0 == xEnd && y0 == yEnd) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    return 1; // Vue dégagée
}

void ActionFantome(int rayonDetection) {
    static int isChasing = 0; 
    
    float dx = player.x - fantome.x;
    float dy = player.y - fantome.y;
    double distance = sqrt(dx*dx + dy*dy);

    // Reset si téléportation
    if (distance > 200) { 
        isChasing = 0;
        fantome.timer = 0;
    }

    // --- GESTION ÉTAT CHASSE ---
    if (isChasing) {
        FANTOME_SPEED = 0.5f;
        // Si trop loin, on arrête la chasse
        if (distance > (6 * TILE_SIZE) || currentLevel < 4) {
            isChasing = 0;
            fantome.timer = 0; 
            FANTOME_SPEED = 0.25f;
        }
    } 
    else {
        FANTOME_SPEED = 0.25f;
        if (distance < rayonDetection && currentLevel >= 4) {
            if (CheckLineOfSight(fantome.x + fantome.w/2, fantome.y + fantome.h/2, 
                                 player.x + player.w/2, player.y + player.h/2)) {
                isChasing = 1;
                fantome.timer = 0; 
            }
        }
    }

    // --- COLLISIONS ---
    float testX = fantome.x;
    float testY = fantome.y;
    // Prédiction simple
    if (fantome.direction == 0) testY -= FANTOME_SPEED;
    else if (fantome.direction == 1) testY += FANTOME_SPEED;
    else if (fantome.direction == 2) testX -= FANTOME_SPEED;
    else if (fantome.direction == 3) testX += FANTOME_SPEED;

    int hitWall = !CheckCollisionFantome(testX, testY);
    fantome.timer--;

    // Est-ce qu'on est parfaitement centré sur une case ?
    // C'est important pour le BFS de ne changer de direction qu'aux intersections
    int onGrid = (((int)fantome.x % TILE_SIZE == (TILE_SIZE - fantome.w)/2) && 
                  ((int)fantome.y % TILE_SIZE == (TILE_SIZE - fantome.h)/2));
    
    // Si on chasse et qu'on est sur la grille (ou bloqué), on recalcule le chemin
    if (isChasing && (onGrid || hitWall)) {
        fantome.timer = 0;
    }

    // --- CERVEAU ---
    if (hitWall || fantome.timer <= 0) {
        
        int bestDir = -1;
        float marge = 4.0f;

        if (isChasing) {
            // Conversion coordonnées pixels -> coordonnées grille
            int gx = (int)((fantome.x + fantome.w/2) / TILE_SIZE);
            int gy = (int)((fantome.y + fantome.h/2) / TILE_SIZE);
            int px = (int)((player.x + player.w/2) / TILE_SIZE);
            int py = (int)((player.y + player.h/2) / TILE_SIZE);

            // On demande au GPS la direction à prendre
            bestDir = ObtenirDirectionBFS(gx, gy, px, py);

            // On met un timer pour ne pas recalculer à chaque frame (optimisation)
            // Mais assez court pour réagir si le joueur bouge vite
            fantome.timer = (int)(TILE_SIZE / FANTOME_SPEED); 
        }
        
        // >>> MODE PATROUILLE (ALEATOIRE) <<<
        // Si le BFS échoue (pas de chemin) ou si on ne chasse pas
        if (bestDir == -1) { 
            int dirs[4]; int nbDirs = 0;
            if (CheckCollisionFantome(fantome.x, fantome.y - marge)) dirs[nbDirs++] = 0;
            if (CheckCollisionFantome(fantome.x, fantome.y + marge)) dirs[nbDirs++] = 1;
            if (CheckCollisionFantome(fantome.x - marge, fantome.y)) dirs[nbDirs++] = 2;
            if (CheckCollisionFantome(fantome.x + marge, fantome.y)) dirs[nbDirs++] = 3;

            if (nbDirs > 0) {
                int oppose = -1;
                if (fantome.direction == 0) oppose = 1; else if (fantome.direction == 1) oppose = 0;
                else if (fantome.direction == 2) oppose = 3; else if (fantome.direction == 3) oppose = 2;

                int validDirs[4]; int nbValid = 0;
                for(int i=0; i<nbDirs; i++) if (dirs[i] != oppose) validDirs[nbValid++] = dirs[i];

                if (nbValid > 0) bestDir = validDirs[rand() % nbValid];
                else bestDir = dirs[0];
            }
            fantome.timer = 30 + rand() % 60; 
        }

        // 4. APPLICATION DE LA DIRECTION
        if (bestDir != -1) {
            fantome.direction = bestDir;
            
            // Snapping (Centrage très important pour que le BFS marche bien)
            // Cela aligne le fantôme parfaitement au centre du couloir
            if (fantome.direction <= 1) { // Vertical
                int col = (int)((fantome.x + fantome.w/2) / TILE_SIZE);
                fantome.x = (float)(col * TILE_SIZE + (TILE_SIZE - fantome.w)/2.0f);
            } else { // Horizontal
                int lig = (int)((fantome.y + fantome.h/2) / TILE_SIZE);
                fantome.y = (float)(lig * TILE_SIZE + (TILE_SIZE - fantome.h)/2.0f);
            }
        } else {
            fantome.timer = 10;
        }
    } 
    
    // --- MOUVEMENT FINAL ---
    float moveX = fantome.x;
    float moveY = fantome.y;

    if (fantome.direction == 0) moveY -= FANTOME_SPEED;
    else if (fantome.direction == 1) moveY += FANTOME_SPEED;
    else if (fantome.direction == 2) moveX -= FANTOME_SPEED;
    else if (fantome.direction == 3) moveX += FANTOME_SPEED;

    if (CheckCollisionFantome(moveX, moveY)) {
        fantome.x = moveX;
        fantome.y = moveY;
    }
}