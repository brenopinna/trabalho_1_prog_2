CC = gcc

CFLAGS = -Iinclude

# ALLEGRO = allegro64 Para Windows

ifdef ALLEGRO
CFLAGS = -I${ALLEGRO}/include 
LDFLAGS = -L${ALLEGRO}/lib 
endif

LDLIBS = -lallegro -lallegro_font -lallegro_ttf -lallegro_image -lallegro_primitives -lallegro_acodec -lallegro_audio

main: object/main.o object/Jogo.o object/Player.o | bin
	gcc -c main.c -o object/main.o ${CFLAGS}
	gcc -c src/Jogo.c -o object/Jogo.o ${CFLAGS}
	gcc -c src/Player.c -o object/Player.o ${CFLAGS}
	gcc object/main.o object/Jogo.o object/Player.o ${LDLIBS} -o bin/main

object/main.o: main.c | object

object/Jogo.o: src/Jogo.c | object

object/Player.o: src/Player.c | object

object:
	mkdir -p object

bin:
	mkdir -p bin

clean:
	rm bin/*
	rm object/*