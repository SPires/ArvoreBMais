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
  char nome[101];
} TREG;

TABM* primeiraFolha(TABM* a);
TREG* criaReg(int mat, float cr, int tranc, int ch_aprov, int periodos, int cur, char *nome);

/* Impressão da árvore
 * parametros : 
 * a: nó raiz da arvore
 * andar: nível de impressão da árvore. Segundo o código dela, sempre passar como 0.
 *
 * retorno: nenhum */
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

/* Geração da árvore
 * parametros : 
 * t: fator de ramificação
 *
 * retorno: nova árvore */
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

/* Liberação da árvore
 * parametros : 
 * a: árvore pronta
 *
 * retorno: nada */
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

/* busca verifica se uma dada chave esta numa arvore
 * parametros : 
 * x: nó raiz da arvore em que será feita a busca
 * ch: chave a ser procurada
 *
 * retorno: o nó em que foi encontrada a chave ou NULL, caso nao encontre  */
TABM *Busca(TABM* x, int ch){
	TABM *resp = NULL;
	if(!x) return resp;
	int i=0;
	if (x->folha) {
		while ((i < x->nchaves)&&(ch > x->chave[i])) i++;
		if (ch == x->chave[i]) return x;
	}
	while ((i < x->nchaves)&&(ch > x->chave[i])) i++;
	if (ch == x->chave[i]) return Busca(x->filho[i+1],ch);
	return Busca(x->filho[i],ch);	
}
   
TABM* remover(TABM* arv, int ch, int t);

/* Chamada segura à retirada
 * parametros : 
 * arv: árvore em uso
 * k: a chave principal do registro a ser retirado
 * t: fator de ramificação
 *
 * retorno: nova árvore */
TABM* retira(TABM* arv, int k, int t){
  if(!arv || !Busca(arv, k)){
	printf("Árvore não carregada ou matrícula não encontrada.\n");	
	return arv;
  }
  return remover(arv, k, t);
}    

/* Aplicação dos casos de remoção
 * parametros : 
 * arv: árvore em uso
 * ch: chave principal a ser removida
 * t: fator de ramificação
 *
 * retorno: nova árvore */ 
TABM* remover(TABM* arv, int ch, int t){
	if(!arv) {
		printf("Árvore vazia ou não carregada.");
		return arv;
	}
	int i=0;
	printf("Removendo %d...\n", ch);
	for(i = 0; i<arv->nchaves && arv->chave[i] < ch; i++);
	if(arv->folha){ //CASO 1
      		printf("\nAplicando o CASO 1\n");
      		int j;
      		for(j=i; j<arv->nchaves-1;j++){
			  arv->chave[j] = arv->chave[j+1];
			  arv->info[j] = arv->info[j+1];
			  arv->info[j+1] = NULL;
		}
		arv->nchaves--;
		return arv;
      		      
	}
	TABM *y = arv->filho[i], *z = NULL, *anterior = NULL;	
	
	if (y->nchaves == t-1){
		
		//Caso 3A
		
		if((i < arv->nchaves) && (arv->filho[i+1]->nchaves >=t)){
			printf("\nAplicando o CASO 3A.\n");
			z = arv->filho[i+1];
			if (y->folha){
				y->chave[t-1] = z->chave[0];
				y->info[t-1] = z->info[0];
				z->info[0] = NULL;
				arv->chave[i] = z->chave[1];
				y->nchaves++;
				int k;
				for (k=1; k < z->nchaves; k++){
					z->chave[k-1] = z->chave[k];
					z->info[k-1] = z->info[k];
					z->info[k] = NULL;
				}
				z->nchaves--;
				printf("\nUma chamada recursiva.\n");
				arv->filho[i] = remover(arv->filho[i], ch, t);
				return arv;
			}
			y->chave[t-1] = arv->chave[i];
			y->nchaves++;
			arv->chave[i] = z->chave[0];     //dar a arv uma chave de z
			int j;
			for(j=0; j < z->nchaves-1; j++)  //ajustar chaves de z
				z->chave[j] = z->chave[j+1];
			y->filho[y->nchaves] = z->filho[0]; //enviar ponteiro menor de z para o novo elemento em y
			for(j=0; j < z->nchaves; j++)       //ajustar filhos de z
				z->filho[j] = z->filho[j+1];
			z->nchaves--;
			printf("\nUma chamada recursiva.\n");
			arv->filho[i] = remover(arv->filho[i], ch, t);
			return arv;
    }
		if((i > 0) && (!z) && (arv->filho[i-1]->nchaves >=t)){
			printf("\nAplicando o CASO 3A.\n");
			z = arv->filho[i-1];
			if (y->folha){
				int k;
				y->nchaves++;
				for (k=y->nchaves; k > 0; k--){
					y->chave[k] = y->chave[k-1];
					y->info[k] = y->info[k-1];
					y->info[k-1] = NULL;
				}
				y->chave[0] = z->chave[z->nchaves-1];
				y->info[0] = z->info[z->nchaves-1];
				z->info[z->nchaves-1] = NULL;
				arv->chave[i] = y->chave[0];
				z->nchaves--;
				printf("\nUma chamada recursiva.\n");
				arv->filho[i] = remover(y, ch, t);
				return arv;
			}
			int j;
			for(j = y->nchaves; j>0; j--)               //encaixar lugar da nova chave
				y->chave[j] = y->chave[j-1];
			for(j = y->nchaves+1; j>0; j--)             //encaixar lugar dos filhos da nova chave
				y->filho[j] = y->filho[j-1];
			y->chave[0] = arv->chave[i-1];              //dar a y a chave i da arv
			y->nchaves++;
			arv->chave[i-1] = z->chave[z->nchaves-1];   //dar a arv uma chave de z
			y->filho[0] = z->filho[z->nchaves];         //enviar ponteiro de z para o novo elemento em y
			z->nchaves--;
			printf("\nUma chamada recursiva.\n");
			arv->filho[i] = remover(y, ch, t);
			return arv;
		}
		
		//Caso 3B
		
		if(!z){ //CASO 3B
		  if(i < arv->nchaves && arv->filho[i+1]->nchaves == t-1){
			printf("\nAplicando o CASO 3B.\n");
			z = arv->filho[i+1];
			if (y->folha){
				int k;
				for (k=0; k < z->nchaves; k++){
					y->chave[t-1+k] = z->chave[k];
					y->info[t-1+k] = z->info[k];
				}
				y->nchaves += z->nchaves;
				y->prox = z->prox;
				for (k=i; k < arv->nchaves; k++){
					arv->chave[k] =	arv->chave[k+1];
					arv->filho[k+1] = arv->filho[k+2];
				}
				arv->nchaves--;
				arv->filho[i] = remover (arv->filho[i], ch, t);
				return arv;
			}
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
			for(j=i; j < arv->nchaves-1; j++){ //limpar referências de i
			  arv->chave[j] = arv->chave[j+1];
			  arv->filho[j+1] = arv->filho[j+2];
			}
			arv->nchaves--;
			printf("\nUma chamada recursiva.\n");
			arv = remover(arv, ch, t);
			return arv;
		  }
		  if((i > 0) && (arv->filho[i-1]->nchaves == t-1)){ 
			printf("\nCASO 3B: i igual a nchaves\n");
			z = arv->filho[i-1];
			if (y->folha){
				printf("Não rola");
				return arv;
			}
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
			printf("\nUma chamada recursiva.\n");
			arv = remover(arv, ch, t);
			return arv;
		  }
		}
	}
	printf("\nExecutando uma chamada recursiva no filho %d.\n", i);
	arv->filho[i+1] = remover(arv->filho[i+1], ch, t);
	return arv;
}

/* Aplicar divisão em nós cheios
 *parametros
 *x: nó pai
 *i: posição onde entra a chave vinda de um dos filhos
 *y: a nova folha esquerda
 *t: o fator de ramificação 
 *
 * retorno: uma nova árvore*/
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
	z->prox = y->prox;
	y->prox = z;
  }
  for(j=0;j<t-1;j++) z->chave[j] = y->chave[j+t];
  if(!y->folha){
    for(j=0;j<t;j++){
      z->filho[j] = y->filho[j+t];
      y->filho[j+t] = NULL;
    }
  }
  if (y-> folha) {
	y->nchaves -= t-1;
  } else {
	y->nchaves = t-1;
  }
  for(j=x->nchaves; j>=i; j--) x->filho[j+1]=x->filho[j];
  x->filho[i] = z;
  for(j=x->nchaves; j>=i; j--) x->chave[j] = x->chave[j-1];
  if (y->folha){
	x->chave[i-1] = z->chave[0];
  } else {
	x->chave[i-1] = y->chave[t-1];
  }
  x->nchaves++;
  return x;
}

/* Inserir dado em um nó não completo
 * parametros : 
 * x: árvore em uso
 * k: chave a ser inserida
 * t: fator de ramificação
 * dado: informação associada à chave a ser inserida
 *
 * retorno: nova árvore */
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

/* Inserção
 * parametros
 * a: arvore na qual será inserido o novo registro
 * k: matricula do registro a ser inserido
 * t: ordem da arvore a
 * dado: o dado a ser inserido 
 *
 * retorna: nova árvore*/
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
	return a;
}

TABM * removeFormandos (TABM *a, int t);
TABM * removePeloTempoDeCurso (TABM *a, int t);

/* Chamar as funções que eliminam dados da árvore
 * parametros : 
 * t: fator de ramificação
 *
 * retorno: nova árvore */
TABM * otimizaArvore(TABM *a, int t){
    if(!a) return NULL;
    a = removeFormandos(a,t); 
    a = removePeloTempoDeCurso(a,t);
    return a;
   }

TABM * removeFormandos(TABM * a, int t){
    if(!a) return NULL;
    TABM * aux = (TABM *) malloc (sizeof(TABM));
    TABM * folha = NULL;
    if (!a->folha) folha = primeiraFolha(a);
    else folha=a;
    TREG * dado = (TREG *) malloc (sizeof(TREG));
    int i;
    while (folha){
        for (i=0;i<a->nchaves;i++){
            dado = a->info[i];
            if (dado->cur == 1 && dado->ch_aprov == 2955) {a=remover(a,dado->mat,t); return removeFormandos(a,t);}
            if (dado->cur == 2 && dado->ch_aprov == 3524) {a=remover(a,dado->mat,t); return removeFormandos(a,t);}
            if (dado->cur == 3 && dado->ch_aprov == 3200) {a=remover(a,dado->mat,t); return removeFormandos(a,t);}
        }
        folha = folha->prox;
    }
    free(dado);
    free(aux);
    return a;
  }  

TABM * removePeloTempoDeCurso(TABM * a, int t){
    if(!a) return NULL;
    TABM * aux = (TABM *) malloc (sizeof(TABM));
    if (!a->folha) aux = primeiraFolha(a);
    else aux=a;
    TREG * dado = (TREG *) malloc (sizeof(TREG));
    int i;
    while (aux){
        for (i=0;i<a->nchaves;i++){
            dado = aux->info[i];
            if (dado->cur == 1){
              if (dado->periodos == 16 && dado->ch_aprov !=2955)
	      	{a=remover(aux,dado->mat,t); return removePeloTempoDeCurso(a,t);}
              if (dado->periodos == 8 && dado->ch_aprov < 1477)
                 {a=remover(aux,dado->mat,t); return removePeloTempoDeCurso(a,t);}
            }
            if (dado->cur == 2){
              if (dado->periodos == 12 && dado->ch_aprov !=3524)
                 {a=remover(aux,dado->mat,t); return removePeloTempoDeCurso(a,t);}
              if (dado->periodos == 8 && dado->ch_aprov < 1762)
                 {a=remover(aux,dado->mat,t); return removePeloTempoDeCurso(a,t);}
            }
            if (dado->cur == 3){
              if (dado->periodos == 12 && dado->ch_aprov !=3200)
                 {a=remover(aux,dado->mat,t); return removePeloTempoDeCurso(a,t);}
              if (dado->periodos == 8 && dado->ch_aprov < 1600)
                 {a=remover(aux,dado->mat,t); return removePeloTempoDeCurso(a,t);}
            }
        }
        aux = aux->prox;
    }
    free(dado);
    free(aux);
    return a;
  }

/*Aponta para a folha mais à esquerda da árvore.
 *parametros
 *t: nó ancestral do nó folha procurado ou o proprio nó folha
 *
 *retorno: NULL se a arvore for nula, e t caso t seja folha
 */
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
  int i=0;
  while (mat > aux->chave[i]) i++;
  aux->info[i]->cr = novocr;
  return a;
}

TABM *alteraCH (TABM *a, int t, int mat, int novaCH){
  if (!a) return NULL;
  TABM *aux = (TABM*) malloc (sizeof(TABM));
  aux = Busca(a,mat);
  if (!aux) return NULL;
  int i=0;
  while (mat > aux->chave[i]) i++;
  aux->info[i]->ch_aprov = novaCH;
  return a;
}

TABM * alteraTranc (TABM *a, int t, int mat, int ntranc){
  if (!a) return NULL;
  TABM *aux = (TABM*) malloc (sizeof(TABM));
  aux = Busca(a,mat);
  if (!aux) return NULL;
  int i=0;
  while (mat > aux->chave[i]) i++;
  aux->info[i]->tranc = ntranc;
  return a;
}

TABM * alteraPeriodo (TABM *a, int t, int mat, int nperi){
  if (!a) return NULL;
  TABM *aux = (TABM*) malloc (sizeof(TABM));
  aux = Busca(a,mat);
  if (!aux) return NULL;
  int i=0;
  while (mat > aux->chave[i]) i++;
  aux->info[i]->periodos = nperi;
  return a;
}

TREG* criaReg(int mat, float cr, int tranc, int ch_aprov, int periodos, int cur, char *nome){
	TREG *r=(TREG*)malloc(sizeof(TREG));
	r->mat=mat;
	r->cr=cr;
	r->ch_aprov=ch_aprov;
	r->tranc = tranc;
	r->cur=cur;
	r->periodos = periodos;
	strcpy(r->nome,nome);
	return r;
}

/* Gerar uma árvore a partir de dados de um arquivo
 * parametros : 
 * t: fator de ramificação
 *
 * retorno: nova árvore */
TABM * novaArv (char *nome, int t){
	//criaReg(int mat, float cr, int tranc, int ch_aprov, int periodos, int cur, char *nome)
   int mat,tranc,ch_aprov,cur,periodos,ctrl = 1,i;
   float cr;
   char c;
   FILE *fp = fopen(nome,"rt");
   if (!fp) exit(1);
   TABM *a = Cria(t);
   TREG *aux;
   while(ctrl>0){
		ctrl = fscanf(fp,"%d %f %d %d %d %d ", &mat, &cr, &tranc, &ch_aprov, &periodos, &cur);
		if (ctrl != 6) break;
		i = 0;
		while(i<100){
			c = fgetc(fp);
			if(c == '\n' || c == EOF)break;
			nome[i] = c;
			i++;
		}
		nome[i]='\0';
		if(mat>=0){
			aux = criaReg(mat, cr, tranc, ch_aprov, periodos, cur, nome);
			//TABM *Insere(TABM *a, int k, int t, TREG *dado)
			a = Insere(a, mat, t,aux);
		}
	}
   fclose(fp);
   return a;
}

/* Enviar os dados da árvore para um arquivo
 * parametros : 
 * t: fator de ramificação
 *
 * retorno: nova árvore */
int gravarDados (TABM *a, char *saida){
  if (!a) {
     printf("%s","Árvore não existe.\n");
     return 0;
  }
  FILE *fp = fopen(saida,"wt+");
  if (!fp) exit(1);
  if (!a->folha) a = primeiraFolha(a);
  TREG * aux = (TREG*) malloc (sizeof(TREG));
  int i;
  while (a) {
    for (i=0;i<a->nchaves;i++){
       aux = a->info[i];
       fprintf(fp, "%d ",aux->mat);
       fprintf(fp, "%f ",aux->cr);
       fprintf(fp, "%d ",aux->tranc);
       fprintf(fp, "%d ",aux->ch_aprov);
       fprintf(fp, "%d ",aux->periodos);
       fprintf(fp, "%d ",aux->cur);
       fprintf(fp, "%s\n",aux->nome);
    }
    a = a->prox;
  }
  fclose(fp);
  Libera(a);
  a=NULL;
  printf("\n\n---------------\n");
  printf("Árvore enviada para o arquivo e liberada.");
  printf("\n---------------\n\n\n");
  return 1;
}

/* Exibir dados de um aluno específico
 * parametros : 
 * t: fator de ramificação
 *
 * retorno: nova árvore */
void imprimeinfo(TABM* a, int mat){
    TABM *aux = (TABM*) malloc (sizeof(TABM));
    aux = Busca(a,mat);
    if (!aux){
	printf("\n\n---------------\n");
	printf("Matrícula não existe.");
	printf("\n---------------\n\n");
    	return;
    }
    int i=0;
    while ((i < aux->nchaves) && (mat > aux->chave[i])) i++;
    printf("------------------------\n\n");
    printf("Matricula: %d\n",aux->info[i]->mat);
    printf("CR: %f\n",aux->info[i]->cr);
    printf("Trancamentos: %d\n",aux->info[i]->tranc);
    printf("CH total: %d\n", aux->info[i]->ch_aprov);
    printf("Períodos cursados: %d\n",aux->info[i]->periodos);
    printf("Currículo: %d\n",aux->info[i]->cur);
    printf("Nome: %s\n\n",aux->info[i]->nome);
    printf("------------------------\n\n\n");
    free(aux);
}
  
int main () {
  int op = -1;
  int t = -1;
  TABM *arvore = NULL;
  
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
      printf("7 - exibir informações\n");
      printf("9 - escrever arvore no arquivo\n");
      printf("Opção: ");
      scanf("%d", &op);
      fflush(stdin);
    }while(op<0||(op!=9&&op>7));
    switch(op)
    {
      case 0:
	{
	printf("\n\n-------------\n");
	printf("\n\n É hora de dar tchau! \n\n");
	printf("\n-------------\n\n");
        return 0;
        break;
	}
      case 1:
		{
        	char nome[501];
        	printf("\nDigite o nome do arquivo(no maximo 500 caracteres, os excedentes serao ignorados) : ");
 	      	scanf("%s",nome);
        	printf("Digite o t : ");
        	scanf("%d",&t);
        	if(arvore)
           		Libera(arvore);
        	arvore=novaArv(nome,t);
		printf("Carregando a arvore:\n");
		printf("\n\n------------------------\n\n");
        	Imprime(arvore,0);
		printf("\n\n------------------------\n\n");
        	break;
		}
      case 2:
		{
		if (!arvore) {
			printf("\nBase de dados não carregada.\n\n");
			break;
		}
        	TREG* novo=(TREG*)calloc(1,sizeof(TREG));
        	int mat=-1;
        	float cr=-1;
        	int tranc=0;
        	int ch_aprov, periodos, cur;    //carga horária com aprovação, número de períodos e currículo.
        	char nomeCandidato[101];
		int i;
		for (i=0;i<101;i++) nomeCandidato[i] = '\0';
        	if(!novo || !nomeCandidato)
        	{
          		fprintf(stderr,"%s:%d alocacao de memoria falhou, libere memoria \n", __FILE__, __LINE__);
          		break;
        	}
        	printf("\nDigite a matricula : ");
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
		scanf(" %100[^\n]",nomeCandidato);
        	novo->mat=mat;
        	novo->cr=cr;
        	novo->tranc=tranc;
        	novo->ch_aprov=ch_aprov;
        	novo->periodos=periodos;
        	novo->cur=cur;
        	strcpy(novo->nome,nomeCandidato);
        	arvore=Insere(arvore,mat,t,novo);
		printf("\n\n------------------------\n\n");        	
		Imprime(arvore,0);
		printf("\n\n------------------------\n\n");
        	break;
		}
      case 3:
		{
		if (!arvore) {
			printf("\nBase de dados não carregada.\n\n");
			break;
		}
        	int mat;
        	printf("\nDigite a matricula a remover : ");
        	scanf("%d", &mat);
        	arvore=retira(arvore,mat,t);
		printf("\n\n------------------------\n\n");
        	Imprime(arvore,0);
		printf("\n\n------------------------\n\n");
        	break;
		}
      case 4:
	{	
		if (!arvore) {
			printf("\nÁrvore não carregada.\n\n");
			break;
		}
		arvore=otimizaArvore(arvore,t);
		printf("\n\n------------------------\n\n");
		Imprime(arvore,0);
		printf("\n\n------------------------\n\n");
		break;
	}
      case 5:
		{
			if (!arvore) {
				printf("\nÁrvore não carregada.\n\n");
				break;
			}
			int op;
			int mat;
			printf("\n1- cr; 2- carga horária; 3- trancamento; 4- períodos;");
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
								printf("\n\n------------------------\n\n");
								Imprime(arvore,0);
								printf("\n\n------------------------\n\n");
								break;
							}
					case 2:
							{
								int carOraria;
								printf("Digite a Carga Horaria : ");
								scanf("%d", &carOraria);
								arvore=alteraCH(arvore,t,mat,carOraria);
								printf("\n\n------------------------\n\n");			
								Imprime(arvore,0);
								printf("\n\n------------------------\n\n");
								break;	
							}
					case 3:
							{
									int tranc;
									printf("Digite o numero de trancamentos : ");
									scanf("%d", &tranc);
									arvore=alteraTranc(arvore,t,mat,tranc);
									printf("\n\n------------------------\n\n");
									Imprime(arvore,0);
									printf("\n\n------------------------\n\n");
									break;
							}
					case 4:			
							{
									int periodos;
									printf("Digite o numero de periodos : ");
									scanf("%d", &periodos);
									arvore=alteraPeriodo(arvore,t,mat,periodos);
									printf("\n\n------------------------\n\n");
									Imprime(arvore,0);
									printf("\n\n------------------------\n\n");
									break;
							}
			}
        	// 1- cr; 2- carga horária; 3- trancamento; 4- períodos;
			break;
		}
      case 6:
	{	
		if (!arvore) {
			printf("Árvore não carregada.\n\n");
			break;
		}
		printf("\n\n-----------------------\n\n");
		Imprime(arvore,0);
		printf("\n\n-----------------------\n\n");
		break;
	}
      case 7:
	{
	  if (!arvore) {
		printf("\n\n---------------\n");		
		printf("\nÁrvore não carregada.\n");
		printf("\n---------------\n\n");
		break;
	  }
	  printf("Digite a matrícula desejada: ");
	  int mat;
	  scanf("%d",&mat);
	  imprimeinfo(arvore,mat);
	  break;
	}
      case 9:
		{
		if (!arvore) {
			printf("Base de dados não carregada.\n\n");
			break;
		}
        	//char nomeArquivo[501];
        	//printf("Digite o nome do arquivo(no maximo 500 caracteres, os excedentes serao ignorados) : ");
        	//scanf("%500[^\n]",nomeArquivo);
        	//fflush(stdin);
		char *saida = "saida.txt";
        	gravarDados(arvore,saida);
		arvore = NULL;
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
// 7- exibir dados específicos.
// 9- escrever árvore no arquivo.
return 0;
}
