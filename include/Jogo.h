#pragma once
#include <stdbool.h>

typedef struct Jogo Jogo;

Jogo *novo_jogo();

void finalizar_jogo(Jogo *J);

bool jogo_rodando(Jogo *J);

void atualizar_jogo(Jogo *J);


