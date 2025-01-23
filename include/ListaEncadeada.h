#pragma once

#include <Mapa.h>

typedef struct MapNode {
  char arquivo_mapa[16];
  Map* map;
  struct MapNode* prev;
  struct MapNode* next;
} MapNode;

MapNode* adicionar_mapa(const char* arquivo_mapa, MapNode* head);

MapNode* criar_lista_de_mapas();

MapNode* selecionar_mapa(int n, MapNode* head);

void remover_mapas(MapNode* head);