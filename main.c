#include "Jogo.h"

int main(){
   Jogo* J = novo_jogo();
  
   while(jogo_rodando(J))
       atualizar_jogo(J);
  
   finalizar_jogo(J);
}