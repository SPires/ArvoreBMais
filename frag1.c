#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void Imprime(TABM *a, int andar){
  if(a){
    int i,j;
    for(i=0; i<=a->nchaves-1; i++){
      Imprime(a->filho[i],andar+1);
      for(j=0; j<=andar; j++) printf("   ");
      printf("%d\n", a->chave[i]);
    }
    Imprime(a->filho[i],andar+1);
  }
}

void Libera(TABM *a){
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
  }
}

TABM *Busca(TABM* x, int ch){
  TABM *resp = NULL;
  if(!x) return resp;
  int i=0;
  if (x->folha==1){
  	for(i=0;i<x->nchaves;i++)
		if (x->chave[i]==ch)
			resp = x;
	return resp;
  }
  while(ch > x->chave[i] && i<x->nchaves) i++;
  return Busca(x->filho[i],ch);
}

TABM *Cria(int t){
  TABM* novo = (TABM *) malloc (sizeof(TABM));
  novo->nchaves = 0;
  novo->chave = (int *) malloc (sizeof(int)*((t*2)-1));
  novo->folha = 1;
  novo->info = (TREG **) malloc (sizeof(TREG*)*((t*2)-1));
  novo->filho = (TABM **) malloc (sizeof(TABM*)*t*2);
  novo->prox = (TABM *) malloc (sizeof(TABM));
  int i;
  for(i=0; i<(t*2); i++) novo->filho[i] = NULL;
  for(i=0; i<((2*t)-1); i++) novo->info[i] = NULL;
  novo->prox = NULL;
  return novo;
}

TABM *Divisao(TABM *x, int i, TABM* y, int t){
  TABM *z = Cria(t);
  z->nchaves = t-1;
  z->folha = y->folha;
  int j;
  if (y->folha){
	for(j=0;j<t-1;j++){
	  z->chave[j] = y->chave[j+t];
	  z->info[j] = y->info[j+t];
	  y->info[j+t] = NULL;
    }
	y->prox = z;
  }
  for(j=0;j<t-1;j++) z->chave[j] = y->chave[j+t];
  if(!y->folha){
    for(j=0;j<t;j++){
      z->filho[j] = y->filho[j+t];
      y->filho[j+t] = NULL;
    }
  }
  y->nchaves = t-1;
  for(j=x->nchaves; j>=i; j--) x->filho[j+1]=x->filho[j];
  x->filho[i] = z;
  for(j=x->nchaves; j>=i; j--) x->chave[j] = x->chave[j-1];
  x->chave[i-1] = z->chave[0];
  x->nchaves++;
  return x;
}

TABM *Insere_Nao_Completo(TABM *x, int k, int t, TREG *dado){
  int i = x->nchaves-1;
  if(x->folha){
    while((i>=0) && (k<x->chave[i])){
      x->chave[i+1] = x->chave[i];
	  x->info[i+1] = x->info[i];
	  x->info[i] = NULL;
      i--;
    }
    x->chave[i+1] = k;
	x->info[i+1] = dado;
    x->nchaves++;
    return x;
  }
  while((i>=0) && (k<x->chave[i])) i--;
  i++;
  if(x->filho[i]->nchaves == ((2*t)-1)){
    x = Divisao(x, (i+1), x->filho[i], t);
    if(k>x->chave[i]) i++;
  }
  x->filho[i] = Insere_Nao_Completo(x->filho[i], k, t, dado);
  return x;
}

TABM *Insere(TABM *a, int k, int t, TREG *dado){
	if (!a) return NULL;
	TABM *aux = Busca(a,k);
	if (aux){
		int i=0;
		while ((i<aux->nchaves) && (k>aux->chave[i])) i++;
		aux->info[i] = dado;
		return a;
	}
	if(a->nchaves == (2*t)-1){
		TABM *S = Cria(t);
		S->nchaves = 0;
		S->folha = 0;
		S->filho[0] = a;
		S = Divisao(S,1,a,t);
		S = Insere_Nao_Completo(S,k,t,dado);
		return S;
	}
	a = Insere_Nao_Completo(a,k,t,dado);
	printf("Insere não completo terminado\n");
	return a;
}

#define MAXTAMLINE 1001

int parseFile(FILE* fp, int* mat, float* cr,int* tranc,int* ch_aprov, int* periodos, int* cur, char** nome){
	char linha[MAXTAMLINE+1+1];
	char *aux;
	fgets(linha,MAXTAMLINE+1,fp);
	aux=strtok(linha," ");
	if(aux==NULL) return 0;
	*mat=atoi(aux);
	aux=strtok(NULL," ");
	*cr=atof(aux);
	aux=strtok(NULL," ");
	*tranc=atoi(aux);
	aux=strtok(NULL," ");
	*ch_aprov=atoi(aux);
	aux=strtok(NULL," ");
	*periodos=atoi(aux);
	aux=strtok(NULL, " ");
	*cur=atoi(aux);
	*nome=strtok(NULL,"\n");
	return 7;
}

TABM * novaArv (char *nome, int t){
   FILE *fp = fopen(nome,"rt");
   if (!fp) exit(1);
   printf("fp existe\n");
   TABM *a = Cria(t);
   printf("a é criada com valor %p\n", a);
   TREG *aux = (TREG *) malloc (sizeof(TREG));
   printf("aux é alocado com valor %p\n", aux);
   int r = parseFile(fp,&aux->mat,&aux->cr,&aux->tranc,&aux->ch_aprov,&aux->periodos,&aux->cur,&aux->nome);
   printf("Terminou de Parsear a primeira linha com r = %d\n", r);
   while (r == 7){
      Imprime(a,0);
      a = Insere(a,aux->mat,t,aux);
      printf("Inseri Porra com a = %p\n", a);
      r = parseFile(fp,&aux->mat,&aux->cr,&aux->tranc,&aux->ch_aprov,&aux->periodos,&aux->cur,&aux->nome);
      printf("parseei de novo com r = %d\n", r);
   }
   printf("Começo do fim da função\n");
   free(aux);
   fclose(fp);
   printf("FIm da função\n");
   return a;
}

int main () {
	int op = -1;
	int t = -1;
	TABM *arvore = NULL;
	char nome[501];
	printf("Digite o nome do arquivo(no maximo 500 caracteres, os excedentes serao ignorados) : ");
	scanf("%s",nome);
	printf("Digite o t : ");
	scanf("%d",&t);
	if(arvore) Libera(arvore);
	arvore=novaArv(nome,t);
	printf("Carreguei a arvore\n");
	Imprime(arvore,0);
	return 0;
}