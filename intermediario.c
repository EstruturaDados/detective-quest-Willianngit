#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Sala {
    char nome[64];
    char pista[128];            /* string opcional; "" significa sem pista */
    struct Sala *esq;
    struct Sala *dir;
} Sala;

typedef struct PistaNode {
    char pista[128];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

/* criarSala() – cria dinamicamente um cômodo com ou sem pista. */
Sala* criarSala(const char *nome, const char *pistaOpcional) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        printf("Erro: falha ao alocar memoria (Sala).\n");
        exit(1);
    }

    strncpy(s->nome, nome, sizeof(s->nome) - 1);
    s->nome[sizeof(s->nome) - 1] = '\0';

    if (pistaOpcional && pistaOpcional[0] != '\0') {
        strncpy(s->pista, pistaOpcional, sizeof(s->pista) - 1);
        s->pista[sizeof(s->pista) - 1] = '\0';
    } else {
        s->pista[0] = '\0';
    }

    s->esq = NULL;
    s->dir = NULL;
    return s;
}

/* buscarPista() – busca uma pista na BST; retorna 1 se encontrar, 0 caso contrário. */
int buscarPista(PistaNode *raiz, const char *pista) {
    if (!raiz || !pista) return 0;

    int cmp = strcmp(pista, raiz->pista); /* <0, 0, >0 conforme relação entre strings */
    if (cmp == 0) return 1;
    if (cmp < 0) return buscarPista(raiz->esq, pista);
    return buscarPista(raiz->dir, pista);
}

/* inserirPista() – insere uma nova pista na árvore de pistas (BST). */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (!pista || pista[0] == '\0') return raiz;

    if (raiz == NULL) {
        PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
        if (!n) {
            printf("Erro: falha ao alocar memoria (PistaNode).\n");
            exit(1);
        }
        strncpy(n->pista, pista, sizeof(n->pista) - 1);
        n->pista[sizeof(n->pista) - 1] = '\0';
        n->esq = NULL;
        n->dir = NULL;
        return n;
    }

    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    } else if (cmp > 0) {
        raiz->dir = inserirPista(raiz->dir, pista);
    } else {
        /* Pista repetida: mantém apenas uma ocorrência. */
    }
    return raiz;
}

/* emOrdem() – imprime a BST de pistas em ordem alfabética (in-order). */
void emOrdem(PistaNode *raiz) {
    if (!raiz) return;
    emOrdem(raiz->esq);
    printf("- %s\n", raiz->pista);
    emOrdem(raiz->dir);
}

void liberarPistas(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

void liberarSalas(Sala *raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esq);
    liberarSalas(raiz->dir);
    free(raiz);
}

char lerOpcao(void) {
    char buf[32];

    printf("Escolha (e=esquerda, d=direita, s=sair): ");
    if (!fgets(buf, sizeof(buf), stdin)) return 's';

    buf[strcspn(buf, "\n")] = 0;
    if (buf[0] == '\0') return '\0';
    return buf[0];
}

/* explorarSalasComPistas() – controla a navegação entre salas e coleta de pistas. */
void explorarSalasComPistas(Sala *hall, PistaNode **bstPistas) {
    Sala *atual = hall;

    while (atual) {
        printf("\nVoce esta em: %s\n", atual->nome);

        if (atual->pista[0] != '\0') {
            if (!buscarPista(*bstPistas, atual->pista)) {
                printf("Pista encontrada e coletada: %s\n", atual->pista);
                *bstPistas = inserirPista(*bstPistas, atual->pista);
            } else {
                printf("Pista ja coletada anteriormente: %s\n", atual->pista);
            }
        } else {
            printf("Nenhuma pista aqui.\n");
        }

        printf("Caminhos disponiveis: ");
        if (atual->esq) printf("[e] ");
        if (atual->dir) printf("[d] ");
        printf("[s]\n");

        char op = lerOpcao();

        if (op == 's' || op == 'S') {
            printf("Exploracao encerrada.\n");
            break;
        } else if (op == 'e' || op == 'E') {
            if (atual->esq) atual = atual->esq;
            else printf("Nao ha caminho para a esquerda.\n");
        } else if (op == 'd' || op == 'D') {
            if (atual->dir) atual = atual->dir;
            else printf("Nao ha caminho para a direita.\n");
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }
}

int main(void) {
    /* Mapa fixo (pré-definido no código): árvore binária de salas. */
    Sala *hall = criarSala("Hall de Entrada", "Pegadas molhadas no tapete");

    hall->esq = criarSala("Sala de Estar", "Carta rasgada perto da lareira");
    hall->dir = criarSala("Corredor Principal", "");

    hall->esq->esq = criarSala("Biblioteca", "Livro fora do lugar: Criptografia");
    hall->esq->dir = criarSala("Cozinha", "Faca recem-lavada");

    hall->dir->esq = criarSala("Escritorio", "Cofre aberto sem sinais de arrombamento");
    hall->dir->dir = criarSala("Jardim de Inverno", "Terra fresca em um vaso quebrado");

    /* BST de pistas coletadas */
    PistaNode *bstPistas = NULL;

    explorarSalasComPistas(hall, &bstPistas);

    printf("\nPistas coletadas (ordem alfabetica):\n");
    if (!bstPistas) {
        printf("(nenhuma pista coletada)\n");
    } else {
        emOrdem(bstPistas);
    }

    liberarPistas(bstPistas);
    liberarSalas(hall);
    return 0;
}
