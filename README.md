# 🌙 Lights Out

**Lights Out** est un jeu d'exploration et de survie horrifique en 2D Pixel Art, développé entièrement en langage C avec la bibliothèque SDL2 dans le cadre d'un projet étudiant.

Plongez dans le cauchemar d'un enfant. La maison a changé et l'obscurité est oppressante. Retrouvez votre doudou 🧸, résolvez des énigmes 🧩 et tentez de résoudre le mystère !

---

## 🎮 Contrôles du jeu

* **Flèches directionnelles** : Se déplacer
* **Touche [E]** : Interagir (Ouvrir, Récupérer un objet, Parler)
* **Touche [Entrée]** : Valider un choix / Cuisiner au chaudron / Sauter une animation / Dialogue suivant
* **Touche [Échap]** : Mettre le jeu en pause
* **Touche [F11]** : Basculer en Plein Écran (Fullscreen)

---

### 💾 Système de Sauvegarde (Auto-save)

Le jeu intègre un système de sauvegarde complet pour ne jamais perdre votre progression :
* **Sauvegarde Automatique :** Votre partie est automatiquement sauvegardée si vous quittez le jeu brutalement (croix rouge de la fenêtre) ou si vous retournez au menu principal via le menu Pause.
* **Sauvegarde Manuelle :** Vous pouvez sauvegarder à tout moment en appuyant sur `ECHAP` puis en sélectionnant `SAUVEGARDER`.
* **Chargement :** Au lancement du jeu, sélectionnez simplement **CHARGER** dans le menu principal pour reprendre exactement là où vous vous étiez arrêté (position, inventaire, état de la maison).

---

## 🛠️ Fonctionnalités Techniques

Ce projet a été construit de zéro (sans moteur de jeu préexistant comme Unity ou Godot) et met en œuvre plusieurs concepts algorithmiques :

* **Machine à états (State Machine) :** Gestion robuste du flux du programme via une énumération (Menu, Intro, Jeu, Pause, Options, Fin) permettant des transitions fluides sans fuite de mémoire.
* **Système d'Éclairage Dynamique :** Implémentation d'un système de champ de vision (Flashlight) avec calcul de distance euclidienne. L'obscurité s'adapte en temps réel à la position du joueur et aux sources de lumière (lampes, bougies).
* **Intelligence Artificielle (Pathfinding) :** L'ennemi (le Fantôme) utilise l'algorithme **BFS (Breadth-First Search)** pour calculer le chemin le plus court vers le joueur à travers le labyrinthe. Il possède également un système d'états (Patrouille aléatoire / Chasse / Perte de vue).
* **Moteur d'Animation Asynchrone :** Les animations du joueur (Marche, Idle, Respiration) et de l'environnement (Chaudron, Horloge, Télévision) sont basées sur le temps (`SDL_GetTicks`) via des modulos mathématiques, évitant l'usage de fonctions bloquantes.
* **Pixel Perfect Rendering :** Utilisation de l'`Integer Scaling` pour garantir que le Pixel Art ne subisse aucune déformation ou flou bilinéaire, quelle que soit la résolution de l'écran du joueur.
* **Système d'Événements et Puzzles :** Gestion d'un inventaire invisible et de conditions complexes (recettes de cuisine, interrupteurs, dialogues contextuels).

---

## 🚀 Comment jouer ?

### 🪟 Pour les joueurs sous Windows

Aucune installation n'est requise !
1. Téléchargez l'archive `.zip` du jeu.
2. **⚠️ TRÈS IMPORTANT :** Faites un Clic Droit sur l'archive et choisissez **"Extraire tout..."** (Ne lancez pas le jeu depuis l'aperçu du fichier ZIP).
3. Ouvrez le dossier extrait et double-cliquez sur `Light_out.exe`.
4. profitez :) !

### 🐧 Pour les joueurs sous Linux (Compilation depuis les sources)

**Prérequis :**
Assurez-vous d'avoir GCC, Make et les bibliothèques SDL2 installées :
```bash
sudo apt-get update
sudo apt-get install gcc make libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev
```

**Compilation et exécution :**
Placez-vous à la racine du projet et utilisez simplement la commande suivante pour compiler et lancer le jeu automatiquement :

```bash
make run
```

### 🪟 Compilation croisée pour Windows (Depuis Linux/WSL)
Le projet inclut déjà les dossiers mingw_dev/ nécessaires pour compiler la version Windows directement depuis un terminal Linux.

**Prérequis :**
Assurez-vous d'avoir le compilateur MinGW installé :

```bash
sudo apt-get install mingw-w64
```

**Compilation :**
```bash
make windows
```

L'exécutable Light_out.exe sera généré dans le dossier executable/. Pour y jouer sous Windows, assurez-vous que ce dossier contient bien le sous-dossier assets/ et les fichiers .dll associés.

### 🧹 Nettoyage des fichiers compilés
Pour nettoyer l'environnement de travail :
```bash
make clean
```

### 👥 Crédits 
🛠️Développement & Programmation : Mathys PHILIPPIN, Rayen ZAIRI, Lou ANSART, Théo ROMOND-DUPUIS

👩‍🎨Graphismes / Tuiles : Lou ANSART (utilisation de l'inteligence artificielle generative (gemini) pour la realisation des images (livre, menu principal, screamer).)

🎶Musiques et Bruitages : Mathys PHILIPPIN (sources : https://pixabay.com/fr/sound-effects/search/)

👨‍🏫Supervision / Professeur : Amaury AUGUSTE, Projet jeu retro en C - L2 algorithmique


## Développé avec ❤️ en utilisant C et SDL2.