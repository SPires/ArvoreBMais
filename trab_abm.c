//diretivas de compilação
#include<stdio.h>
#include<stdlib.h>

typedef struct arvbm {
  int *chave , nchaves, folha;
  struct reg **info;
  struct arvbm **filho, *prox;
} TABM;

typedef struct reg {
  int mat;
  float cr;
  int tranc;
  int ch_aprov, periodos, cur;
  char *nome;
} TREG;
  

//funções a serem implementadas

int main (int argc, char** argv) {

}
