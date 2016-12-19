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
 

void imprime(TABM* a);

 
TABM *Cria(int t){
  TABM* novo = (TABM *) malloc (sizeof(TABM));
  novo->nchaves = 0;
  novo->chave = (int *) malloc (sizeof(int)*((t*2)-1));
  novo->folha = 1;
  novo->info = (TREG **) malloc (sizeof(TREG*)*((t*2)-1)); //hmmmmm
  novo->filho = (TABM **) malloc (sizeof(TABM*)*t*2);
  novo->prox = (TABM *) malloc (sizeof(TABM));
  int i;
  for(i=0; i<(t*2); i++) novo->filho[i] = NULL;
  return novo;
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
  int i;
  if (x->folha==1){
  	for(i=0;i<x->nchaves;i++)
		if (x->chave[i]==ch)
			resp = x;
	return resp;
  }
  while(ch > x->chave[i] && i<x->nchaves) i++;
  return Busca(x->filho[i],ch);
}
    
TABM* remover(TABM* arv, int ch, int t);

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
      for(j=i; j<arv->nchaves-1;j++) {
        arv->chave[j] = arv->chave[j+1];
        arv->info[j] = arv->info[j+1];
      }
      arv->nchaves--;
      return arv;      
    }     
  }
  TABM *y = arv->filho[i], *z = NULL;
	
  if (y->nchaves == t-1){ //CASOS 3A e 3B
    if((i < arv->nchaves) && (arv->filho[i+1]->nchaves >=t)){ //CASO 3A: Chave está no MEIO/ESQUERDA(y) = T-1, DIREITA(z) = T 
      printf("\nCASO 3A: i menor que nchaves\n");
      z = arv->filho[i+1];
      y->chave[t-1] = z->chave[0];   //dar a y a chave 0 de z
      y->info[t-1] = z->info[0];
      y->nchaves++;
      arv->chave[i] = z->chave[1]; //arv fica com o valor de chave igual ao primeiro elemento do filho da direita
      int j;
      for(j=0; j < z->nchaves-1; j++){ //ajusa as chaves de z, movendo seus elementos para a esquerda
        z->chave[j] = z->chave[j+1];
        z->info[j] = z->info[j+1];
      }
      y->filho[y->nchaves] = z->filho[0]; //enviar ponteiro menor de z para o novo elemento em y
      for(j=0; j < z->nchaves; j++)       //ajustar filhos de z
        z->filho[j] = z->filho[j+1];
      z->nchaves--;
      arv->filho[i] = remover(arv->filho[i], ch, t);
      return arv;
   }
    if((i > 0) && (!z) && (arv->filho[i-1]->nchaves >=t)){ //CASO 3A: ESQUERDA(z) = T, DIREITA/MEIO(y) = T-1
      printf("\nCASO 3A: i igual a nchaves\n");
      z = arv->filho[i-1];
      int j;
      for(j = y->nchaves; j>0; j--) {              //move as posições para abrir espaço pra nova chave
        y->chave[j] = y->chave[j-1];
        y->info[j] = y->info[j-1];
      }
      for(j = y->nchaves+1; j>0; j--)             //encaixar lugar dos filhos da nova chave
        y->filho[j] = y->filho[j-1];
      y->chave[0] = z->chave[z->nchaves-1];
      y->info[0] = z->info[z->nchaves-1];
      y->nchaves++;
      arv->chave[i-1] = y->chave[0]; 	          //recebe o valor da chave do filho da direita
      y->filho[0] = z->filho[z->nchaves];         //enviar ponteiro do último elemento de z para o novo elemento em y
      z->nchaves--;
      arv->filho[i] = remover(y, ch, t);
      return arv;
    }
    
    if(!z){ //CASO 3B
      if(i < arv->nchaves && arv->filho[i+1]->nchaves == t-1){  //filho da direita tem t-1 chaves
        printf("\nCASO 3B: i menor que nchaves\n");
        z = arv->filho[i+1];
        int j;
	y->chave[t-1] = z->chave[0]; 
	y->info[t-1] = z->info[0];
	y->nchaves++;
        for(j=0; j < t-1; j++){
          y->chave[t+j] = z->chave[j+1];     //passar chave do filho da direita pro nó do filho da esquerda
          y->info[t+j] = z->info[j+1];
	  y->nchaves++;
        }
        if(!y->folha){
          for(j=0; j<t; j++){
            y->filho[t-1+j] = z->filho[j];    //passa a referência dos filhos
          }
        }
        for(j=i; j < arv->nchaves-1; j++){    //hmmmm...... 
          arv->chave[j] = arv->chave[j+1];
          arv->filho[j+1] = arv->filho[j+2];
        }
        arv->nchaves--;
        arv = remover(arv, ch, t);
        return arv;
      }

      //acho que não entendi esse caso	    
      if((i > 0) && (arv->filho[i-1]->nchaves == t-1)){ //filho da esquerda tem t-1 chaves??     
        printf("\nCASO 3B: i igual a nchaves\n");
        z = arv->filho[i-1];
        //if(i == arv->nchaves)
        //  z->chave[t-1] = arv->chave[i-1]; //pegar chave[i] e poe ao final de filho[i-1]
        //else
        //  z->chave[t-1] = arv->chave[i];   //pegar chave [i] e poe ao final de filho[i-1]
        //z->nchaves++;
        int j;
        for(j=0; j < t-1; j++){
          z->chave[t-1+j] = y->chave[j];     //passar filho[i+1] para filho[i]
          z->info[t-1+j] = y->info[j];
	  z->nchaves++;
        }
        if(!z->folha){
          for(j=0; j<t; j++){
            z->filho[t-1+j] = y->filho[j];
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

TABM *Divisao(TABM *x, int i, TABM* y, int t){
  TABM *z=Cria(t);
  z->nchaves= t - 1;
  z->folha = y->folha;
  int j;
  if (y->folha){
	for(j=0;j<t-1;j++){
	  z->chave[j] = y->chave[j+t];
	  z->info[j] = y->info[j+t];
    }
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
  x->chave[i-1] = y->chave[t-1];
  x->nchaves++;
  return x;
}

TABM *Insere_Nao_Completo(TABM *x, int k, int t, TREG *dado){
  int i = x->nchaves-1;
  if(x->folha){
    while((i>=0) && (k<x->chave[i])){
      x->chave[i+1] = x->chave[i];
	  x->info[i+1] = x->info[i];
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
  if(Busca(a,k)){
    printf("Entrou na Busca\n");
    int i=0;
    while (k > a->chave[i]) i++;
    a->info[i] = dado;
    return a;
  }
  if(!a){
    printf("a é NULL\n");
    a = Cria(t);
    a->chave[0] = k;
    a->folha = 1;
    a->nchaves = 1;
    a->info[0]->mat=dado->mat;
    a->info[0]->cr=dado->cr;
    a->info[0]->tranc=dado->tranc;
    a->info[0]->ch_aprov=dado->ch_aprov;
    a->info[0]->periodos=dado->periodos;
    a->info[0]->cur=dado->cur;
    printf("copiei Inicio\n");
    strcpy(a->info[0]->nome,dado->nome);
    printf("copiei FInal\n");
    a->info[0] = dado;
    return a;
  }
  if(a->nchaves == (2*t)-1){
    printf("a ta cheio\n");
    TABM *S = Cria(t);
    S->nchaves=0;
    S->folha = 0;
    S->filho[0] = a;
    S = Divisao(S,1,a,t);
    S = Insere_Nao_Completo(S,k,t,dado);
    return S;
  }
  printf("Insere nao completo\n");
  a = Insere_Nao_Completo(a,k,t,dado);
  printf("Insere não completo terminado\n");
  return a;
}

TABM * removeFormandos (TABM *a, int t);
TABM * removePeloTempoDeCurso (TABM *a, int t);

TABM * otimizaArvore(TABM *a, int t){
    if(!a) return NULL;
    a = removeFormandos(a,t); 
    a = removePeloTempoDeCurso(a,t);
    return a;
   }

TABM* primeiraFolha(TABM* a);

TABM * removeFormandos(TABM * a, int t){
    if(!a) return NULL;
    TABM * aux = (TABM *) malloc (sizeof(TABM));
    if (!a->folha) a = primeiraFolha(a);
    TREG * dado = (TREG *) malloc (sizeof(TREG));
    int i;
    while (a){
        for (i=0;i<a->nchaves;i++){
            dado = a->info[i];
            if (dado->cur == 1 && dado->ch_aprov == 2955) remover(a,dado->mat,t);
            if (dado->cur == 2 && dado->ch_aprov == 3524) remover(a,dado->mat,t);
            if (dado->cur == 3 && dado->ch_aprov == 3200) remover(a,dado->mat,t);
        }
        a = a->prox;
    }
    free(dado);
    free(aux);
    return a;
  }  

TABM * removePeloTempoDeCurso(TABM * a, int t){
    if(!a) return NULL;
    TABM * aux = (TABM *) malloc (sizeof(TABM));
    if (!a->folha) aux = primeiraFolha(a);
    TREG * dado = (TREG *) malloc (sizeof(TREG));
    int i;
    while (aux){
        for (i=0;i<a->nchaves;i++){
            dado = aux->info[i];
            if (dado->cur == 1){
              if (dado->periodos == 16 && dado->ch_aprov !=2955)
                 remover(aux,dado->mat,t);
              if (dado->periodos == 8 && dado->ch_aprov < 1477)
                 remover(aux,dado->mat,t);
            }
            if (dado->cur == 2){
              if (dado->periodos == 12 && dado->ch_aprov !=3524)
                 remover(aux,dado->mat,t);
              if (dado->periodos == 8 && dado->ch_aprov < 1762)
                 remover(aux,dado->mat,t);
            }
            if (dado->cur == 3){
              if (dado->periodos == 12 && dado->ch_aprov !=3200)
                 remover(aux,dado->mat,t);
              if (dado->periodos == 8 && dado->ch_aprov < 1600)
                 remover(aux,dado->mat,t);
            }
        }
        aux = aux->prox;
    }
    free(dado);
    free(aux);
    return a;
  }

TABM * primeiraFolha (TABM* t){
  if (!t) return NULL;
  if (t->folha) return t;
  return primeiraFolha(t->filho[0]);
} 
    
TABM * alteraCR (TABM *a, int t, int mat, float novocr){
  if (!a) return NULL;
  TABM *aux = (TABM*) malloc (sizeof(TABM));
  aux = Busca(a,mat);
  if (!aux) return NULL;
  int i;
  while (mat > aux->chave[i]) i++;
  TREG *aluno = (TREG *) malloc (sizeof(TREG));
  aluno = aux->info[i];
  aluno->cr = novocr;
  a = Insere(a,mat,t,aluno);
  free(aluno);
  free(aux);
  return a;
}

TABM *alteraCH (TABM *a, int t, int mat, int novaCH){
  if (!a) return NULL;
  TABM *aux = (TABM*) malloc (sizeof(TABM));
  aux = Busca(a,mat);
  if (!aux) return NULL;
  int i;
  while (mat > aux->chave[i]) i++;
  TREG *aluno = (TREG *) malloc (sizeof(TREG));
  aluno = aux->info[i];
  aluno->ch_aprov = novaCH;
  a = Insere(a,mat,t,aluno);
  free(aluno);
  free(aux);
  return a;
}

TABM * alteraTranc (TABM *a, int t, int mat, int ntranc){
  if (!a) return NULL;
  TABM *aux = (TABM*) malloc (sizeof(TABM));
  aux = Busca(a,mat);
  if (!aux) return NULL;
  int i;
  while (mat > aux->chave[i]) i++;
  TREG *aluno = (TREG *) malloc (sizeof(TREG));
  aluno = aux->info[i];
  aluno->tranc = ntranc;
  a = Insere(a,mat,t,aluno);
  free(aluno);
  free(aux);
  return a;
}

TABM * alteraPeriodo (TABM *a, int t, int mat, int nperi){
  if (!a) return NULL;
  TABM *aux = (TABM*) malloc (sizeof(TABM));
  aux = Busca(a,mat);
  if (!aux) return NULL;
  int i;
  while (mat > aux->chave[i]) i++;
  TREG *aluno = (TREG *) malloc (sizeof(TREG));
  aluno = aux->info[i];
  aluno->periodos = nperi;
  a = Insere(a,mat,t,aluno);
  free(aluno);
  free(aux);
  return a;
}

#define MAXTAMLINE 1001

int parseFile(FILE* fp, int* mat, float* cr,int* tranc,int* ch_aprov, int* periodos, int* cur, char** nome)
{
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
      imprime(a);
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

int gravarDados (TABM *a, char *saida){
  if (!a) return 0;
  FILE *fp = fopen(saida,"wt+");
  if (!fp) exit(1);
  int i;
  TREG *aux = (TREG *) malloc (sizeof(TREG));
  if (a->folha){
    for (i=0;i<a->nchaves;i++){
       aux = a->info[i];
       fprintf(fp, "%d %f %d %d %d %d %s\n",a->info[i]->mat,a->info[i]->cr,a->info[i]->tranc,a->info[i]->ch_aprov,a->info[i]->periodos,a->info[i]->cur,a->info[i]->nome);
    }
  }
  a = primeiraFolha(a);
  while (aux) {
    for (i=0;i<a->nchaves;i++){
       aux = a->info[i];
       fprintf(fp, "%d %f %d %d %d %d %s\n",a->info[i]->mat,a->info[i]->cr,a->info[i]->tranc,a->info[i]->ch_aprov,a->info[i]->periodos,a->info[i]->cur,a->info[i]->nome);
    }
    a = a->prox;
  }
  fclose(fp);
  free(aux);
  Libera(a);
  a=NULL;
  return 1;
}

void imprimeinfo(TREG* t)
{
    printf("%d ",t->mat);
    printf("%f ",t->cr);
    printf("%d ",t->tranc);
    printf("%d %d %d ", t->ch_aprov,t->periodos,t->cur);
    printf("%s ",t->nome);
}
  
  
void imprime(TABM* t)
{
  if(t)
  {
    printf("(");
    if(t->folha)
    {
      int i;
      for(i=0; i<t->nchaves; i++)
      {
        imprimeinfo(t->info[i]);
        printf(" ");
      }
    }
    else
    {
      int i;
      for(i=0; i<t->nchaves; i++)
      {
        imprime(t->filho[i]);
        printf(":");
        printf("%d:",t->chave[i]);
        printf(":");
      
      }
      imprime(t->filho[i]);
    }
    printf(")");
  }
}
  
  
int main () {
  int op=-1;
  int t=-1;
  TABM* arvore=NULL;
  
  while(1)
  {
    do
    {
      printf("0 - sair\n");
      printf("1 - carregar a base de dados\n");
      printf("2 - inserir um dado\n");
      printf("3 - remover um dado\n");
      printf("4 - otimizar a árvore\n");
      printf("5 - alterar\n");
      printf("6 - mostrar arvore\n");
      printf("9 - escrever arvore no arquivo\n");
	  scanf("%d", &op);
	  fflush(stdin);
    }while(op<0||(op!=9&&op>6));
    switch(op)
    {
      case 0:
        return 0;
        break;
      case 1:
		{
        	char nome[501];
        	printf("Digite o nome do arquivo(no maximo 500 caracteres, os excedentes serao ignorados) : ");
 	      	scanf("%s",nome);
        	printf("Digite o t : ");
        	scanf("%d",&t);
        	if(arvore)
           		Libera(arvore);
        	arvore=novaArv(nome,t);
			printf("Carreguei a arvore\n");
        	imprime(arvore);
        	break;
		}
      case 2:
		{
        	TREG* novo=(TREG*)calloc(sizeof(TREG),1);
        	int mat=-1;
        	float cr=-1;
        	int tranc=0;
        	int ch_aprov, periodos, cur;    //carga horária com aprovação, número de períodos e currículo.
        	char *nomeCandidato=(char*)calloc(sizeof(char),101);
        	if(!novo || !nomeCandidato)
        	{
          		fprintf(stderr,"%s:%d alocacao de memoria falhou, libere memoria \n", __FILE__, __LINE__);
          		break;
        	}
        	printf("Digite a matricula : ");
        	scanf("%d",&mat);
        	printf("Digite o cr : ");
        	scanf("%f", &cr);
        	printf("Digite o numero de trancamentos : ");
        	scanf("%d", &tranc);
        	printf("Digite a carga horaria aprovada : ");
        	scanf("%d", &ch_aprov);
        	printf("Digite o numero de periodos : ");
        	scanf("%d", &periodos);
        	printf("Digite o numero do curriculo : ");
       		scanf("%d", &cur);
        	printf("Digite o nome do candidato (tamanho maximo de 100 caracteres, os excedentes serao ignorados : ");
        	scanf("%100[^\n]", nomeCandidato);
        	novo->mat=mat;
        	novo->cr=cr;
        	novo->tranc=tranc;
        	novo->ch_aprov=ch_aprov;
        	novo->periodos=periodos;
        	novo->cur=cur;
        	novo->nome=nomeCandidato;
        	arvore=Insere(arvore,mat,t,novo);
        	imprime(arvore);
        	break;
		}
      case 3:
		{
        	int mat;
        	printf("Digite a matricula a remover : ");
        	scanf("%d", &mat);
        	arvore=retira(arvore,mat,t);
        	imprime(arvore);

        	break;
		}
      case 4:
        arvore=otimizaArvore(arvore,t);
        imprime(arvore);
        break;
      case 5:
		{
			int op;
			int mat;
			printf("1- cr; 2- carga horária; 3- trancamento; 4- períodos;");
			do{
				printf("Digite o que deseja Alterar : ");
				scanf("%d", &op);
			}while(op<1||op>4);
			printf("Digite a matricula do aluno a ser alterado : ");
			scanf("%d", &mat);
			switch(op)
			{
					case 1:
							{
								float cr;
								printf("Digite o cr : ");
								scanf("%f", &cr);
								arvore=alteraCR(arvore,t,mat,cr);
								imprime(arvore);
								break;
							}
					case 2:
							{
								int carOraria;
								printf("Digite a Carga Horaria : ");
								scanf("%d", &carOraria);
								arvore=alteraCH(arvore,t,mat,carOraria);
								imprime(arvore);
								break;	
							}
					case 3:
							{
									int tranc;
									printf("Digite o numero de trancamentos : ");
									scanf("%d", &tranc);
									arvore=alteraTranc(arvore,t,mat,tranc);
									imprime(arvore);
									break;
							}
					case 4:			
							{
									int periodos;
									printf("Digite o numero de periodos : ");
									scanf("%d", &periodos);
									arvore=alteraPeriodo(arvore,t,mat,periodos);
									imprime(arvore);
									break;
							}
			}
        	// 1- cr; 2- carga horária; 3- trancamento; 4- períodos;
			// 
		}
      case 6:
        imprime(arvore);
        break;
      case 9:
		{
        	char nomeArquivo[501];
        	printf("Digite o nome do arquivo(no maximo 500 caracteres, os excedentes serao ignorados) : ");
        	scanf("%500[^\n]",nomeArquivo);
        	fflush(stdin);
        	gravarDados(arvore,nomeArquivo);
			break;
		}
    }
  }
//códigos para chama de funções
// 0 - sair
// 1- carregar a base de dados.
// 2- inserir um dado
// 3- remover um dado
// 4- otimizar a árvore
// 5- alterar: 1- cr; 2- carga horária; 3- trancamento; 4- períodos;
// 6- mostrar árvore
// 9- escrever árvore no arquivo.
return 0;
}
