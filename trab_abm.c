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
  novo->info = (TREG **) malloc (sizeof(TREG*)*((t*2)-1)); //hmmmmm
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
  int i = 0;
  if(x->folha){ //chegou na folha e agora vai verificar todas até encontrar a chave
    if(x->chave == ch)return x;
    while(i < x->nchaves){
      x = x->prox;
      if(x->chave == ch) return x;
  }    
  i = 0;
  while(i < x->nchaves && ch > x->chave[i]) i++;
  return Busca(x->filho[i], ch);
}
    
    
TABM* retira(TABM* arv, int k, int t){
  if(!arv || !Busca(arv, k)) return arv;
  return remover(arv, k, t);
}    
    
TABM* remover(TABM* arv, int ch, int t){
  if(!arv) return arv;
  int i;
  printf("Removendo %d...\n", ch);
  for(i = 0; i<arv->nchaves && arv->chave[i] < ch; i++);
  
  if(i < arv->nchaves && ch == arv->chave[i]){ //   CASO 1
    if(arv->folha){ 
      printf("\nCASO 1\n");
      int j;
      for(j=i; j<arv->nchaves-1;j++) arv->chave[j] = arv->chave[j+1];
      arv->nchaves--;
      return arv;      
    }     
  }
  //ok até aqui !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
  //falta ver a partir daqui
  TABM *y = arv->filho[i], *z = NULL;
  if (y->nchaves == t-1){ //CASOS 3A e 3B
    if((i < arv->nchaves) && (arv->filho[i+1]->nchaves >=t)){ //CASO 3A
      printf("\nCASO 3A: i menor que nchaves\n");
      z = arv->filho[i+1];
      y->chave[t-1] = z->chave[0];   //dar a y a chave i da arv
      y->nchaves++;
      arv->chave[i] = z->chave[0];     //O VALOR DO PAI FICA O MESMO, OU PASSA A SER IGUAL AO QUE FOI EMPRESTADO?
      int j;
      for(j=0; j < z->nchaves-1; j++)  //ajustar chaves de z
        z->chave[j] = z->chave[j+1];
      //z->chave[j] = 0; Rosseti
      y->filho[y->nchaves] = z->filho[0]; //enviar ponteiro menor de z para o novo elemento em y
      for(j=0; j < z->nchaves; j++)       //ajustar filhos de z
        z->filho[j] = z->filho[j+1];
      z->nchaves--;
      arv->filho[i] = remover(arv->filho[i], ch, t);
      return arv;
    }
    if((i > 0) && (!z) && (arv->filho[i-1]->nchaves >=t)){ //CASO 3A
      printf("\nCASO 3A: i igual a nchaves\n");
      z = arv->filho[i-1];
      int j;
      for(j = y->nchaves; j>0; j--)               //encaixar lugar da nova chave
        y->chave[j] = y->chave[j-1];
      for(j = y->nchaves+1; j>0; j--)             //encaixar lugar dos filhos da nova chave
        y->filho[j] = y->filho[j-1];
      //PAREI AQUI
      y->chave[0] = arv->chave[i-1];              //dar a y a chave i da arv
      y->nchaves++;
      arv->chave[i-1] = z->chave[z->nchaves-1];   //dar a arv uma chave de z
      y->filho[0] = z->filho[z->nchaves];         //enviar ponteiro de z para o novo elemento em y
      z->nchaves--;
      arv->filho[i] = remover(y, ch, t);
      return arv;
    }
    
    if(!z){ //CASO 3B
      if(i < arv->nchaves && arv->filho[i+1]->nchaves == t-1){
        printf("\nCASO 3B: i menor que nchaves\n");
        z = arv->filho[i+1];
        y->chave[t-1] = arv->chave[i];     //pegar chave [i] e coloca ao final de filho[i]
        y->nchaves++;
        int j;
        for(j=0; j < t-1; j++){
          y->chave[t+j] = z->chave[j];     //passar filho[i+1] para filho[i]
          y->nchaves++;
        }
        if(!y->folha){
          for(j=0; j<t; j++){
            y->filho[t+j] = z->filho[j];
          }
        }
        for(j=i; j < arv->nchaves-1; j++){ //limpar referÃªncias de i
          arv->chave[j] = arv->chave[j+1];
          arv->filho[j+1] = arv->filho[j+2];
        }
        arv->nchaves--;
        arv = remover(arv, ch, t);
        return arv;
      }
      if((i > 0) && (arv->filho[i-1]->nchaves == t-1)){ 
        printf("\nCASO 3B: i igual a nchaves\n");
        z = arv->filho[i-1];
        if(i == arv->nchaves)
          z->chave[t-1] = arv->chave[i-1]; //pegar chave[i] e poe ao final de filho[i-1]
        else
          z->chave[t-1] = arv->chave[i];   //pegar chave [i] e poe ao final de filho[i-1]
        z->nchaves++;
        int j;
        for(j=0; j < t-1; j++){
          z->chave[t+j] = y->chave[j];     //passar filho[i+1] para filho[i]
          z->nchaves++;
        }
        if(!z->folha){
          for(j=0; j<t; j++){
            z->filho[t+j] = y->filho[j];
          }
        }
        arv->nchaves--;
        arv->filho[i-1] = z;
        arv = remover(arv, ch, t);
        return arv;
      }
    }
  }  
  arv->filho[i] = remover(arv->filho[i], ch, t);
  return arv;
}
    
  
  // obssssssss
 TABM *Insere(TAMB *T, int k, int t, reg *info){
  if(Busca(T,k)) return T;
  if(!T){
    T=Cria(t);
    T->chave[0] = k;
    T->nchaves=1;
    T->info = info;
    return T;
  }
  if(T->nchaves == (2*t)-1){
    TABM *S = Cria(t);
    S->nchaves=0;
    S->folha = 0;
    S->filho[0] = T;
    S = Divisao(S,1,T,t);
    S = Insere_Nao_Completo(S,k,t,info);
    return S;
  }
  T = Insere_Nao_Completo(T,k,t,info);
  return T;
}   
  
  
  
  
  //função que vai chamar as outras funções que vão executar algum tipo atualização na árvore
  void otimizaArvore(TABM *T){
    if(T){
      removeFormandos(T); 
      removePeloTempoDeCurso(T);
    }
    
  //remove todos os registros com: - CHCS = TNC e CHCS < 50% de CHT
  //                               - CHCS = NTOTPER e CHCS < CHT  
  void removeFormandos(TABM * T){
    if(T){
      //
    }  
  }  
    
  //remove todos os registros com NPU NTOTPER 
  void removePeloTempoDeCurso(TABM * T){
    if(T){
      //      
    }  
  }
  
 
TABM* primeiraFolha (TABM* t){
  if (!t) return NULL;
  if (t->folha) return t;
  return primeiraFolha(t->filho[0]);
} 
    
TABM * alteraCR (TABM *a, int t, int mat, float novocr){
  if (!a) return NULL;
  TABM *aux = (TABM*) malloc (sizeof(TABM*));
  aux = Busca(a,mat);
  if (!aux) return NULL;
  int i;
  while (mat > aux->chave[i]) i++;
  TREG *aluno = (TREG *) malloc (sizeof(TREG*));
  aluno = aux->info[i];
  aluno->cr = novocr;
  a = Insere(a,mat,t,aluno);
  free(aluno);
  free(aux);
  return a;
}
  



int main (int argc, char** argv) {

}
