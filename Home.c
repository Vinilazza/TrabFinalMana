#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define TAMANHO_TABELA 53
#define TAMANHO_PALAVRA 50

typedef struct Nodo {
    char *palavra;
    struct Nodo *proximo;
    struct Nodo *anterior;
} Nodo;
typedef struct TabelaHash {
    Nodo *slots[TAMANHO_TABELA];
} TabelaHash;

int hash(char palavra[]);
void inicializarTabelaHash(TabelaHash *tabela);
void inserirFunction(TabelaHash *tabela, char palavra[]);
void insercaoInicial(TabelaHash *tabela);
void insercaoPalavras(TabelaHash *tabela);
void excluirDaTabela(TabelaHash *tabela);
void imprimirTabelaHash(TabelaHash *tabela);
void buscarNaTabela(TabelaHash *tabela);
void option();
void trocarPalavras(Nodo *a, Nodo *b);
Nodo *organizar(Nodo *inicio, Nodo *fim);
void quickSort(Nodo *inicio, Nodo *fim);
void sortSlot(TabelaHash *tabela, int indiceSlot);
void sortTabela(TabelaHash *tabela);
void imprimirSlot(TabelaHash *tabela);

int main() {
    TabelaHash minhaTabela;
    inicializarTabelaHash(&minhaTabela);
    insercaoInicial(&minhaTabela);

    int opcao;
    do {
        option();
        printf("Digite qual acao voce quer executar: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                insercaoPalavras(&minhaTabela);
                break;

            case 2:
                excluirDaTabela(&minhaTabela);
                break;

            case 3:
                imprimirTabelaHash(&minhaTabela);
                break;

            case 4:
                buscarNaTabela(&minhaTabela);
                break;

            case 5:
                imprimirSlot(&minhaTabela);
                break;
            case 6:
                printf("Saindo do programa.\n");
                break;

            default:
                printf("Opção inválida. Tente Nodovamente.\n");
        }
    } while (opcao != 6);
    return 0;
}

int hash(char palavra[]) {
    int soma = 0;
    for (int i = 0; palavra[i] != '\0'; i++) {
        soma += palavra[i];
    }
    return soma % TAMANHO_TABELA;
}

void inicializarTabelaHash(TabelaHash *tabela) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        tabela->slots[i] = NULL;
    }
}

void inserirFunction(TabelaHash *tabela, char palavra[]) {
    int indice = hash(palavra);

    Nodo *NodovoNodo = (Nodo *) malloc(sizeof(Nodo));

    if (NodovoNodo == NULL) {
        fprintf(stderr, "Erro na alocação de memória.\n");
        exit(EXIT_FAILURE);
    }

    NodovoNodo->palavra = strdup(palavra);

    if (NodovoNodo->palavra == NULL) {
        fprintf(stderr, "Erro na alocação de memória.\n");
        free(NodovoNodo);
        exit(EXIT_FAILURE);
    }

    NodovoNodo->proximo = tabela->slots[indice];
    NodovoNodo->anterior = NULL;

    if (tabela->slots[indice] != NULL) {
        tabela->slots[indice]->anterior = NodovoNodo;
    }

    tabela->slots[indice] = NodovoNodo;
}

void insercaoInicial(TabelaHash *tabela) {

    FILE *fptr = fopen("../nomes.txt", "r");
    if (fptr == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        exit(EXIT_FAILURE);
    }

    char palavra[TAMANHO_PALAVRA];

    while (fgets(palavra, sizeof(palavra), fptr) != NULL) {

        size_t len = strlen(palavra);
        if (len > 0 && palavra[len - 1] == '\n') {
            palavra[len - 1] = '\0';
        }

        inserirFunction(tabela, palavra);
    }

    fclose(fptr);

    sortTabela(tabela);
}

void insercaoPalavras(TabelaHash *tabela) {
    int tamanho;

    printf("\nDigite quantas palavras deseja adicionar: ");
    scanf("%d", &tamanho);

    if (tamanho <= 0) {
        printf("Você não pode adicionar 0 palavras.\n");
        return;
    }

    for (int i = 0; i < tamanho; i++) {
        char palavra[TAMANHO_PALAVRA];
        printf("Digite a palavra %d: ", i + 1);
        scanf("%s", palavra);

        // Convertendo a palavra para maiúsculas
        for (int j = 0; palavra[j] != '\0'; j++) {
            palavra[j] = toupper(palavra[j]);
        }

        // Calculando o hash uma vez para evitar repetição
        int indice = hash(palavra);

        // Inserindo na tabela hash
        inserirFunction(tabela, palavra);

        // Ordenando apenas o slot correspondente
        sortSlot(tabela, indice);
    }

    printf("Palavras adicionadas!\n");
}


void excluirDaTabela(TabelaHash *tabela) {

    char palavra[TAMANHO_PALAVRA];

    printf("\nDigite a palavra a ser excluida:");
    scanf("%s", palavra);

    for (int i = 0; palavra[i] != '\0'; i++) {
        palavra[i] = toupper(palavra[i]);
    }

    int indice = hash(palavra);

    if (indice < 0 || indice >= TAMANHO_TABELA) {
        fprintf(stderr, "Índice fora dos limites da tabela.\n");
        exit(EXIT_FAILURE);
    }

    Nodo *atual = tabela->slots[indice];

    while (atual != NULL) {
        if (strcmp(atual->palavra, palavra) == 0) {
            if (atual->anterior != NULL) {
                atual->anterior->proximo = atual->proximo;
            } else {
                tabela->slots[indice] = atual->proximo;
            }

            if (atual->proximo != NULL) {
                atual->proximo->anterior = atual->anterior;
            }

            free(atual->palavra);
            free(atual);

            printf("Palavra \"%s\" excluída com sucesso Nodo slot %d.\n", palavra, indice);
            return;
        }

        atual = atual->proximo;
    }

    printf("Palavra \"%s\" não encontrada na tabela.\n", palavra);
}

void imprimirTabelaHash(TabelaHash *tabela) {
    printf("\n");
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        int contador = 0;
        Nodo *atual = tabela->slots[i];
        while (atual != NULL) {
            contador++;
            atual = atual->proximo;
        }
        printf("Slot %d: %d palavras\n", i, contador);
    }
}

void buscarNaTabela(TabelaHash *tabela) {

    char palavra[TAMANHO_PALAVRA];

    printf("\nDigite a palavra a ser buscada: ");
    scanf("%s", palavra);

    for (int i = 0; palavra[i] != '\0'; i++) {
        palavra[i] = toupper(palavra[i]);
    }

    int indice = hash(palavra);
    int posicaoSlot = 0;

    if (indice < 0 || indice >= TAMANHO_TABELA) {
        fprintf(stderr, "indice não foi encontrado nos limites da tabela.\n");
        exit(EXIT_FAILURE);
    }

    Nodo *atual = tabela->slots[indice];

    while (atual != NULL) {
        if (strcmp(atual->palavra, palavra) == 0) {
            printf("A palavra '%s' foi achada Nodo slot '%d' na posicao '%d'\n", palavra, indice, posicaoSlot);
            return;
        }

        atual = atual->proximo;
        posicaoSlot++;
    }
    printf("Palavra não foi econtrada\n");
}

void option() {
    printf("\nMenu:\n");
    printf("1. Inserir palavras\n");
    printf("2. Excluir palavra\n");
    printf("3. Imprimir tabela\n");
    printf("4. Buscar na tabela\n");
    printf("5. Imprimir slot da tabela\n");
    printf("7. Sair\n");
}

void trocarPalavras(Nodo *a, Nodo *b) {
    char *temp = a->palavra;
    a->palavra = b->palavra;
    b->palavra = temp;
}

Nodo *organizar(Nodo *inicio, Nodo *fim) {
    char *pivo = fim->palavra;
    Nodo *i = inicio->anterior;

    for (Nodo *j = inicio; j != fim; j = j->proximo) {
        if (strcmp(j->palavra, pivo) < 0) {
            i = (i == NULL) ? inicio : i->proximo;
            trocarPalavras(i, j);
        }
    }

    i = (i == NULL) ? inicio : i->proximo;
    trocarPalavras(i, fim);

    return i;
}

void quickSort(Nodo *inicio, Nodo *fim) {
    if (fim != NULL && inicio != fim && inicio != fim->proximo) {
        Nodo *pivo = organizar(inicio, fim);
        quickSort(inicio, pivo->anterior);
        quickSort(pivo->proximo, fim);
    }
}

void sortSlot(TabelaHash *tabela, int indiceSlot) {
    Nodo **slot = &(tabela->slots[indiceSlot]);

    Nodo *fim = *slot;
    while (fim != NULL && fim->proximo != NULL) {
        fim = fim->proximo;
    }

    quickSort(*slot, fim);
}

void sortTabela(TabelaHash *tabela) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        sortSlot(tabela, i);
    }
}

void imprimirSlot(TabelaHash *tabela) {
    int indiceSlot;

    printf("\nDigite qual slot deseja imprimir: ");
    scanf("%d", &indiceSlot);

    if (indiceSlot < 0 || indiceSlot >= TAMANHO_TABELA) {
        printf("Índice de slot inválido.\n");
        return;
    }

    Nodo *atual = tabela->slots[indiceSlot];

    printf("Palavras Nodo slot %d:\n", indiceSlot);

    if(atual ==  NULL){
        printf("Nenhuma palavra encontrada.\n");
    } else {
        while (atual != NULL) {
            printf("%s\n", atual->palavra);
            atual = atual->proximo;
        }
    }
}

