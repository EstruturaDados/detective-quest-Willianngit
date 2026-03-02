#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Sala {
    char nome[64];
    struct Sala *esq;
    struct Sala *dir;
} Sala;


Sala* criarSala(const char *nome) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        printf("Erro: falha ao alocar memoria.\n");
        exit(1);
    }
    strncpy(s->nome, nome, sizeof(s->nome) - 1);
    s->nome[sizeof(s->nome) - 1] = '\0';
    s->esq = NULL;
    s->dir = NULL;
    return s;
}


void liberaArvore(Sala *raiz) {
    if (!raiz) return;
    liberaArvore(raiz->esq);
    liberaArvore(raiz->dir);
    free(raiz);
}


char lerOpcao(void) {
    char buf[32];

    printf("Escolha (e=esquerda, d=direita, s=sair): ");
    if (!fgets(buf, sizeof(buf), stdin)) {
        return 's';
    }
    buf[strcspn(buf, "\n")] = 0; 
    if (buf[0] == '\0') return '\0';
    return buf[0];
}


void explorarSalas(Sala *hall) {
    Sala *atual = hall;

    while (atual) {
        printf("\nVoce esta em: %s\n", atual->nome);

        if (!atual->esq && !atual->dir) {
            printf("Fim do caminho: voce chegou a um comodo sem saidas.\n");
            break;
        }

        printf("Caminhos disponiveis: ");
        if (atual->esq) printf("[e] ");
        if (atual->dir) printf("[d] ");
        printf("[s]\n");

        char op = lerOpcao();

        if (op == 's' || op == 'S') {
            printf("Exploracao encerrada.\n");
            break;
        } else if ((op == 'e' || op == 'E') && atual->esq) {
            atual = atual->esq;
        } else if ((op == 'd' || op == 'D') && atual->dir) {
            atual = atual->dir;
        } else {
            printf("Opcao invalida (ou caminho inexistente). Tente novamente.\n");
        }
    }
}


int main(void) {
    
    Sala *hall = criarSala("Hall de Entrada");

    hall->esq = criarSala("Sala de Estar");
    hall->dir = criarSala("Corredor Principal");

    hall->esq->esq = criarSala("Biblioteca");
    hall->esq->dir = criarSala("Cozinha");

    hall->dir->esq = criarSala("Escritorio");
    hall->dir->dir = criarSala("Jardim");

    
    explorarSalas(hall);

    liberaArvore(hall);
    return 0;
}
