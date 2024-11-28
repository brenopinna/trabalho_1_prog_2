CC = gcc

CFLAGS = -Iinclude

# Para Windows
ALLEGRO = allegro64

ifdef ALLEGRO
CFLAGS = -I${ALLEGRO}/include -Iinclude
LDFLAGS = -L${ALLEGRO}/lib 
endif

LDLIBS = -lallegro -lallegro_font -lallegro_ttf -lallegro_image -lallegro_primitives -lallegro_acodec -lallegro_audio

main: object/main.o object/Jogo.o | bin
	gcc -c main.c -o object/main.o ${CFLAGS}
	gcc -c src/Jogo.c -o object/Jogo.o ${CFLAGS}
	gcc object/main.o object/Jogo.o ${LDLIBS} -o bin/main ${LDFLAGS}

object/main.o: main.c | object

object/Jogo.o: src/Jogo.c | object

object:
	mkdir -p object

bin:
	mkdir -p bin

# TODO: consertar o clean para Windows
clean:
	rm bin/*
	rm object/*