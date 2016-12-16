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

TABM *Libera(TABM *a){
  if(a){
    if(!a->folha){
      int i;
      for(i = 0; i <= a->nchaves; i++) Libera(a->filho[i]);
    }
    free(a->chave);
    free(a->filho);
    free(a->info);
    free(a->prox);
    free(a);
    return NULL;
  }
}

TABM *Busca(TABM* x, int ch){
  TABM *resp = NULL;
  if(!x) return resp;
  if(x->folha){ //chegou na folha e agora vai verificar todas até encontrar a chave
      if(x->chave == ch)return x;
      while(x->prox){
        x = x->prox;
        if(x->chave == ch) return x;
  
  //como fazer pra descer até as folhas?
  int i = 0;
  while(i < x->nchaves && ch > x->chave[i]) i++;
  return Busca(x->filho[i], ch);
}

int main (int argc, char** argv) {

}
