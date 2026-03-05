CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf -lSDL2_mixer -lm

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

# 1. On cherche tous les fichiers .h
HEADERS = $(wildcard $(INC_DIR)/*.h)

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/game

.PHONY: all clean run directories windows

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# 2. On ajoute $(HEADERS) ici pour que la compilation dépende aussi des .h
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

# ==========================================
# --- CONFIGURATION WINDOWS ---
# ==========================================
CC_WIN = x86_64-w64-mingw32-gcc
WINDRES_WIN = x86_64-w64-mingw32-windres
RES = ressource.res

CFLAGS_WIN = -Wall -Wextra -std=c99 -Iinclude -Imingw_dev/include -Imingw_dev/include/SDL2
LDFLAGS_WIN = -Lmingw_dev/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lm -mwindows


$(RES): ressource.rc
	$(WINDRES_WIN) $< -O coff -o $@


windows: $(RES)
	@echo "Compilation pour Windows en cours..."
	mkdir -p Lights_out_Windows_dev
	$(CC_WIN) $(CFLAGS_WIN) src/*.c $(RES) -o Lights_out_Windows_dev/Lights_out.exe $(LDFLAGS_WIN)
	@echo "Terminé ! Le fichier est 'Lights_out_Windows_dev/Lights_out.exe'."


clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) Lights_out_Windows_dev/Lights_out.exe bin_win/ $(RES)
	@echo "Fichiers compilés et ressources supprimés."