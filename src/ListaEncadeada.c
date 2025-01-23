#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Mapa.h>
#include <ListaEncadeada.h>

MapNode* adicionar_mapa(char nome_do_arquivo[256], MapNode* head){
    MapNode* node = malloc(sizeof(MapNode));
    strcpy(node->nome_do_arquivo, nome_do_arquivo);
    node->prev = head;
    node->next = NULL;
    if(head != NULL) head->next = node;
    return node;
}

void remover_mapas(MapNode* head){
    if(head != NULL) {
        remover_mapas(head->next);
        free(head);
    }
}