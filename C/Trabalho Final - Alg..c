#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<locale.h>

#define TAM 256

// definindo a estrutura n�.

typedef struct no{ 
	unsigned char caracter;
	int frequencia;
	struct no *esq, *dir, *proximo;	
}No;

// definindo a estrutura lista.

typedef struct {
	No *inicio;
	int tam;
	
}Lista;

// tabela

void inicializa_tabela_com_zero(unsigned int tab[]) { // preenche toda a tabela com 0
	int i;
	for (i = 0; i < TAM; i++) {
		tab[i] = 0;
	} 
}

void preenche_tab_frequencia(unsigned char texto[], unsigned int tab[]) { // preenche a tabela de frequencia ate o caractere de fim de string '\0'
	int i = 0;
	while (texto[i] != '\0') {
		tab[texto[i]]++; 
		i++; 
	}
}

// lista ordenada

void criar_lista(Lista *lista) {  // cria uma lista utlizando-se da struct lista criada acima
	lista->inicio = NULL; 
	lista->tam = 0; 
}

void inserir_ordenado(Lista *lista, No *no) { // insere a frequencia dos caracteres na lista de maneira ordenada  
	No *aux; // cria um no auxiliar para ajudar a fazer a ordena��o
	if (lista->inicio == NULL) { // se o inicio da lista nao existir 
		lista->inicio = no; //  o inicio da lista sera o n� testado
	}
	else if(no->frequencia < lista->inicio->frequencia){ // se o no tiver frequencia menor que o inicio da lista
		no->proximo = lista->inicio; // o no em questao tera como proximo o antigo inicio da lista 
		lista->inicio = no; // o n� em quest�o se transformar� no inicio da lista.
	}
	else { // se o no tiver frequencia maior que o inicio da lista
		aux = lista->inicio; // a variavel auxiliar criada acima receber� o endere�o do inicio da lista
		while(aux->proximo && aux->proximo->frequencia <= no->frequencia) { // percorre a lista ate achar um elemento com frequencia maior ou igual a frequencia do n�
			aux = aux->proximo; // avan�a para o proximo n� na lista
		}
		no->proximo = aux->proximo; // o proximo do n� 'no' aponta para o n� que seria o pr�ximo na lista
		aux->proximo = no; // o pr�ximo do n� aux (anterior) aponta para o n� 'no'.

	}
	lista->tam++; // incrementa o tamanho da lista (aumenta em 1)
}

void preencher_lista(unsigned int tab[], Lista *lista) { //preencher_lista constr�i uma lista encadeada ordenada de n�s, onde cada n� representa um caractere do texto e sua frequ�ncia.
	int i;
	No *novo; 
	for (i = 0; i < TAM; i++) {
		if(tab[i] > 0) {
			novo = malloc(sizeof(No));
			if (novo) {
				novo->caracter = i;
				novo->frequencia = tab[i];
				novo->dir = NULL;
				novo->esq = NULL;
				novo->proximo = NULL;
				
				inserir_ordenado(lista, novo);
			}
			else {
				printf("Erro ao alocar mem�ria em preencher_lista!\n");
				break;
			}
		}
	}
}
// arvore de huffmann
//Verifica se a lista n�o est� vazia.
//Se a lista n�o estiver vazia, salva o primeiro n� da lista em aux.
//Desconecta o n� removido da lista, atribuindo NULL ao seu ponteiro proximo.

No* remove_no_inicio(Lista *lista) {
	No *aux = NULL;
	 
	if (lista->inicio){
		aux = lista->inicio;
		lista->inicio = aux->proximo;
		aux->proximo = NULL;
		lista->tam--;
	}
	
	return aux;
}
//os n�s com as menores frequ�ncias s�o removidos do in�cio da lista e combinados para formar um novo n� pai na �rvore.
//Esse novo n� � ent�o inserido ordenadamente de volta na lista. Esse processo � repetido at� que reste apenas um n� na lista, que se torna a raiz da �rvore.

No* montar_arvore(Lista *lista) {
	No *primeiro, *segundo, *novo;
	while (lista->tam > 1) {
		primeiro = remove_no_inicio(lista);
		segundo = remove_no_inicio(lista);
		novo = malloc(sizeof(No));
		if (novo) {
			novo->caracter = '+';
			novo->frequencia = primeiro->frequencia + segundo->frequencia;
			novo->esq = primeiro;
			novo->dir = segundo;
			novo->proximo = NULL;
			
			inserir_ordenado(lista, novo); 
		}
		else {
			printf("\n\tErro ao alocar mem�ria em montar_arvore!\n");
			break;
		}
	}
	return lista->inicio; // Se tornou a raiz
}

// montar o dicionario

//calcula recursivamente a altura de uma �rvore bin�ria. Se a �rvore � vazia (raiz nula), a altura � -1.
//Caso contr�rio, a altura � o m�ximo entre a altura da sub�rvore esquerda mais 1 e a altura da sub�rvore direita mais 1.
int altura_arvore(No *raiz) {
	int esq, dir;
	if(raiz == NULL)
		return -1;
	else {
		esq = altura_arvore(raiz->esq) + 1;
		dir = altura_arvore(raiz->dir) + 1;
		if(esq > dir)
			return esq;
		else
			return dir;
	}	
}
//aloca dinamicamente uma matriz de caracteres com TAM linhas e colunas colunas, onde TAM � uma constante definida anteriormente no c�digo. 
//Essa matriz ser� utilizada para armazenar o dicion�rio de Huffman, onde cada linha representa um caractere e cada coluna representa a sequ�ncia bin�ria correspondente
 
char** aloca_dicionario(int colunas) {
	char** dicionario;
	int i;
	dicionario = malloc(sizeof(char*) * TAM);
	for (i = 0; i < TAM; i++) {
		dicionario[i] = calloc(colunas, sizeof(char));
	}
	return dicionario;
}

void gerar_dicionario(char **dicionario, No *raiz, char *caminho, int colunas){
	char esquerda[colunas], direita[colunas];
	if(raiz->esq == NULL && raiz->dir == NULL) // Se a raiz � uma folha (n� sem filhos), atribui o caminho ao caractere correspondente.
		strcpy(dicionario[raiz->caracter], caminho);
	else { // Se a raiz n�o � uma folha, continua percorrendo a �rvore recursivamente.
		strcpy(esquerda, caminho);
		strcpy(direita, caminho);
		
		strcat(esquerda, "0"); // Adiciona '0' ao caminho para indicar a sub�rvore esquerda.
		strcat(direita, "1"); // Adiciona '1' ao caminho para indicar a sub�rvore direita.
		
		gerar_dicionario(dicionario, raiz->esq, esquerda, colunas); // Recursivamente gera o dicion�rio para a sub�rvore esquerda.
		gerar_dicionario(dicionario, raiz->dir, direita, colunas); // Recursivamente gera o dicion�rio para a sub�rvore direita.
	}
}

// codificar

//calcula o tamanho necess�rio para armazenar a string codificada, somando os comprimentos das representa��es bin�rias dos caracteres no texto.
int calcula_tamanho_string(char **dicionario, unsigned char *texto) {
	int i = 0, tam = 0;
	while(texto[i] != '\0') {
		tam = tam + strlen(dicionario[texto[i]]);
		i++;
	}
	return tam + 1;
}

char* codificar(char **dicionario, unsigned char *texto) {
	
	int i = 0, tam = calcula_tamanho_string(dicionario, texto);
	
	char *codigo = calloc(tam, sizeof(char)); // Aloca dinamicamente mem�ria para armazenar a string codificada
	
    // Loop para percorrer cada caractere do texto
	while (texto[i] != '\0') { // Concatena a representa��o bin�ria associada ao caractere atual no dicion�rio de Huffman � string codificada
		strcat(codigo, dicionario[texto[i]]);
		i++;
	}
	return codigo;  // Retorna a string codificada
}

// impressao do texto.txt

void imprime_texto(char **dicionario, unsigned char *codificado) {
	int i;
	FILE *arq;
	arq = fopen("texto.txt", "w"); // Abre o arquivo "texto.txt" para escrita
    if (arq == NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(stderr, "N�o foi poss�vel abrir o arquivo.\n");
    }
    fprintf(arq, "%s\n", codificado);  // Imprime o texto codificado no arquivo
    fprintf(arq, "Caractere; Bin�rio\n"); // Imprime o cabe�alho para o dicion�rio
    for(i = 0; i<TAM; i++)  { // Loop para imprimir cada caractere e sua representa��o bin�ria no dicion�rio
		if (strlen(dicionario[i]) > 0)
 			fprintf(arq, "    %c;      %s\n", i, dicionario[i]);
	}
    printf("Texto salvo com sucesso em texto.txt.\n");

    fclose(arq);
}

int main() {
    unsigned char texto[100]; 
	unsigned int tabela_frequencia[TAM];
	Lista lista;
	No *arvore;
	int colunas;
	char **dicionario;
	char *codificado;
    setlocale(LC_ALL, "Portuguese");
	printf("Digite uma frase: ");
    scanf("%99[^\n]", texto); // O ^\n significa "leia at� encontrar uma quebra de linha" e o %99 � para estabelecer o limite m�ximo de caracteres como 99.
    
    //cria��o da tabela
    
	inicializa_tabela_com_zero(tabela_frequencia);
	preenche_tab_frequencia(texto, tabela_frequencia);
	
	//cria��o da lista ordenada
	
	criar_lista(&lista);
	preencher_lista(tabela_frequencia, &lista);
	
	//montar a arvore de huffmann
	
	arvore = montar_arvore(&lista);
	
	//montar dicionario
	colunas = altura_arvore(arvore) + 1;
	dicionario = aloca_dicionario(colunas);
	gerar_dicionario(dicionario, arvore, "", colunas);
	
	// codificar	
	codificado = codificar(dicionario, texto);
	
	// gerar arquivo de texto
	
	imprime_texto(dicionario, codificado);
	
    return 0;
}
