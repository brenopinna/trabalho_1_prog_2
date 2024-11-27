CC = gcc

CFLAGS = -Iinclude

# ALLEGRO = allegro64 Para Windows

ifdef ALLEGRO
CFLAGS = -I${ALLEGRO}/include 
LDFLAGS = -L${ALLEGRO}/lib 
endif

LDLIBS = -lallegro -lallegro_font -lallegro_ttf -lallegro_image -lallegro_primitives -lallegro_acodec -lallegro_audio


main: object/main.o object/Jogo.o
	gcc -c main.c -o object/main.o ${CFLAGS}
	gcc -c src/Jogo.c -o object/Jogo.o ${CFLAGS}
	gcc object/main.o object/Jogo.o ${LDLIBS} -o bin/main

object/main.o: main.c

object/Jogo.o: src/Jogo.c

clean:
	rm bin/*
	rm object/*