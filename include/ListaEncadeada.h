#pragma once

#include <Mapa.h>

typedef struct MapNode {
  char nome_do_arquivo[256];
  Map* map;
  struct MapNode* prev;
  struct MapNode* next;
} MapNode;

MapNode* adicionar_mapa(char nome_do_arquivo[256], MapNode* head);

void remover_mapas(MapNode* head);