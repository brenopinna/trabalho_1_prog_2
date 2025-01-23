#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Mapa.h>
#include <ListaEncadeada.h>

MapNode* adicionar_mapa(const char* arquivo_mapa, MapNode* head) {
  MapNode* node = malloc(sizeof(MapNode));
  strcpy(node->arquivo_mapa, arquivo_mapa);
  node->prev = head;
  node->next = NULL;
  if (head != NULL) head->next = node;
  return node;
}

void remover_mapas(MapNode* head) {
  if (head != NULL) {
    remover_mapas(head->next);
    free(head);
  }
}