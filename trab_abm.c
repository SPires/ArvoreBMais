//diretivas de compilação
#include<stdio.h>
#include<stdlib.h>

typedef struct arvbm {
  int *chave , nchaves, folha;
  struct reg **info;                //vetor de ponteiros para as informações
  struct arvbm **filho, *prox;
} TABM;

typedef struct reg {
  int mat;
  float cr;
  int tranc;
  int ch_aprov, periodos, cur;    //carga horária com aprovação, número de períodos e currículo.
  char *nome;
} TREG;
  
TABM *Cria(int t){
  TAB* novo = (TAB *) malloc (sizeof(TAB));
  novo->nchaves = 0;
  novo->chave = (int *) malloc (sizeof(int*)*((t*2)-1));
  novo->folha = 1;
  novo->info = (TREG **) malloc (sizeof(TREG*)*((t*2)-1));
  novo->filho = (TAB **) malloc (sizeof(TAB*)*t*2);
  novo->prox = (TABM *) malloc (sizeof(TABM*));
  int i;
  for(i=0; i<(t*2); i++) novo->filho[i] = NULL;
  return novo;
}



int main (int argc, char** argv) {

}
