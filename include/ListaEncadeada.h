#pragma once

#include <Mapa.h>

typedef struct MapNode {
  char arquivo_mapa[256];
  Map* map;
  struct MapNode* prev;
  struct MapNode* next;
} MapNode;

MapNode* adicionar_mapa(char arquivo_mapa[256], MapNode* head);

void remover_mapas(MapNode* head);