CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -pedantic -O3
LDLIBS = -lm -lallegro -lallegro_font -lallegro_ttf -lallegro_image -lallegro_primitives -lallegro_acodec -lallegro_audio

SRC_DIR = src
OBJ_DIR = object
BIN_DIR = bin

SRC_FILES = main.c src/Jogo.c src/Player.c src/Mapa.c src/Entity.c src/Texto.c src/Goal.c src/ListaEncadeada.c

OBJ_FILES = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRC_FILES)))

# Somente para Windows #####################
# Descomente a linha abaixo.
ALLEGRO = allegro64
ifdef ALLEGRO
CFLAGS += -I${ALLEGRO}/include
LDFLAGS = -L${ALLEGRO}/lib 
endif
############################################

# Alvo principal.
all: $(BIN_DIR)/main

# Linkagem final.
$(BIN_DIR)/main: $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS) $(CFLAGS)

# Regra genérica (src/*.c).
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $< -c -o $@ $(CFLAGS)

# Regra especial para a main.c.
$(OBJ_DIR)/main.o: main.c | $(OBJ_DIR)
	$(CC) $< -c -o $@ $(CFLAGS)

# Cria os diretórios caso eles não existam.
$(OBJ_DIR):
	mkdir $@

$(BIN_DIR):
	mkdir $@

# Comandos de limpeza para Windows e Unixes, respectivamente.
clean:
ifdef ALLEGRO
	del /q $(OBJ_DIR) $(BIN_DIR)
else
	rm $(OBJ_DIR)/* $(BIN_DIR)/*
endif