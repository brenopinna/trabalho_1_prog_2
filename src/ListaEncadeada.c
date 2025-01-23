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

MapNode* criar_lista_de_mapas() {
  FILE *f;
  MapNode* head = NULL;

  f = fopen("map_1.txt", "r");
  if (f != NULL) {
    head = adicionar_mapa("map_1.txt", NULL);
    fclose(f);
  }

  MapNode* p = head;
  char arquivo_mapa[32];
  int i = 2;

  do {
    sprintf(arquivo_mapa, "map_%d.txt", i);
    f = fopen(arquivo_mapa, "r");
    if (f != NULL) {
      fclose(f);
      p->next = adicionar_mapa(arquivo_mapa, p);
      p = p->next;
      i++;
    }
  } while(f != NULL);

  return head;
}

void remover_mapas(MapNode* head) {
  if (head != NULL) {
    remover_mapas(head->next);
    free(head);
  }
}