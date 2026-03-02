#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =========================
   Mapa: Árvore binária
   ========================= */

typedef struct Sala {
    char nome[64];
    const char *pista;            
    struct Sala *esq;
    struct Sala *dir;
} Sala;

/* criarSala() – cria dinamicamente um cômodo. */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        printf("Erro: falha ao alocar memoria (Sala).\n");
        exit(1);
    }
    strncpy(s->nome, nome, sizeof(s->nome) - 1);
    s->nome[sizeof(s->nome) - 1] = '\0';
    s->pista = pista;
    s->esq = NULL;
    s->dir = NULL;
    return s;
}

void liberarSalas(Sala *raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esq);
    liberarSalas(raiz->dir);
    free(raiz);
}

/* =========================
   BST: pistas coletadas
   ========================= */

typedef struct PistaNode {
    char pista[128];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

int bstBuscar(PistaNode *raiz, const char *pista) {
    if (!raiz || !pista) return 0;
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0) return 1;
    if (cmp < 0) return bstBuscar(raiz->esq, pista);
    return bstBuscar(raiz->dir, pista);
}

/* inserirPista()/adicionarPista() – insere a pista na BST. */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (!pista || pista[0] == '\0') return raiz;

    if (!raiz) {
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
    if (cmp < 0) raiz->esq = inserirPista(raiz->esq, pista);
    else if (cmp > 0) raiz->dir = inserirPista(raiz->dir, pista);
    else {
        /* repetida: ignora */
    }
    return raiz;
}

/* emOrdem()/exibirPistas() – imprime pistas em ordem alfabética (BST in-order). */
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

/* =========================
   Hash: pista -> suspeito
   (encadeamento separado)
   ========================= */

#define HASH_TAM 101

typedef struct HashEntry {
    char pista[128];
    char suspeito[64];
    struct HashEntry *prox;
} HashEntry;

typedef struct HashTable {
    HashEntry *buckets[HASH_TAM];
} HashTable;

unsigned long hashStringSimple(const char *s) {
    /* Hash simples por soma ASCII (aceito no enunciado); índice = soma % M [web:131] */
    unsigned long sum = 0;
    while (s && *s) sum += (unsigned char)(*s++);
    return sum;
}

void inicializarHash(HashTable *ht) {
    for (int i = 0; i < HASH_TAM; i++) ht->buckets[i] = NULL;
}

/* inserirNaHash() – insere associação pista/suspeito na tabela hash. */
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    if (!pista || !suspeito) return;

    unsigned long idx = hashStringSimple(pista) % HASH_TAM;
    HashEntry *cur = ht->buckets[idx];

    while (cur) {
        if (strcmp(cur->pista, pista) == 0) {
            strncpy(cur->suspeito, suspeito, sizeof(cur->suspeito) - 1);
            cur->suspeito[sizeof(cur->suspeito) - 1] = '\0';
            return;
        }
        cur = cur->prox;
    }

    HashEntry *e = (HashEntry*) malloc(sizeof(HashEntry));
    if (!e) {
        printf("Erro: falha ao alocar memoria (HashEntry).\n");
        exit(1);
    }
    strncpy(e->pista, pista, sizeof(e->pista) - 1);
    e->pista[sizeof(e->pista) - 1] = '\0';
    strncpy(e->suspeito, suspeito, sizeof(e->suspeito) - 1);
    e->suspeito[sizeof(e->suspeito) - 1] = '\0';

    e->prox = ht->buckets[idx];
    ht->buckets[idx] = e;
}

/* encontrarSuspeito() – consulta o suspeito correspondente a uma pista. */
const char* encontrarSuspeito(HashTable *ht, const char *pista) {
    if (!pista) return NULL;

    unsigned long idx = hashStringSimple(pista) % HASH_TAM;
    HashEntry *cur = ht->buckets[idx];

    while (cur) {
        if (strcmp(cur->pista, pista) == 0) return cur->suspeito;
        cur = cur->prox;
    }
    return NULL;
}

void liberarHash(HashTable *ht) {
    for (int i = 0; i < HASH_TAM; i++) {
        HashEntry *cur = ht->buckets[i];
        while (cur) {
            HashEntry *n = cur->prox;
            free(cur);
            cur = n;
        }
        ht->buckets[i] = NULL;
    }
}

/* =========================
   Utilitários
   ========================= */

void trim(char *s) {
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    size_t i = 0;
    while (s[i] && isspace((unsigned char)s[i])) i++;
    if (i > 0) memmove(s, s + i, strlen(s + i) + 1);
}

void toLowerStr(char *s) {
    for (; *s; s++) *s = (char)tolower((unsigned char)*s);
}

void lerLinha(const char *prompt, char *out, size_t outSize) {
    printf("%s", prompt);
    if (!fgets(out, outSize, stdin)) {
        out[0] = '\0';
        return;
    }
    out[strcspn(out, "\n")] = 0;
}

/* =========================
   Exploração + Análises
   ========================= */

/* explorarSalas() – navega pela árvore e ativa o sistema de pistas. */
void explorarSalas(Sala *hall, PistaNode **bstPistas, HashTable *hash) {
    Sala *atual = hall;

    while (atual) {
        printf("\nVoce esta em: %s\n", atual->nome);

        if (atual->pista) {
            printf("Pista encontrada: %s\n", atual->pista);

            if (!bstBuscar(*bstPistas, atual->pista)) {
                *bstPistas = inserirPista(*bstPistas, atual->pista);
                printf("Pista coletada!\n");
            } else {
                printf("Pista ja coletada.\n");
            }

            const char *sus = encontrarSuspeito(hash, atual->pista);
            if (sus) printf("Associacao: \"%s\" -> %s\n", atual->pista, sus);
            else printf("Aviso: pista sem suspeito cadastrado.\n");
        } else {
            printf("Nenhuma pista aqui.\n");
        }

        printf("Caminhos: ");
        if (atual->esq) printf("[e] ");
        if (atual->dir) printf("[d] ");
        printf("[s]\n");

        char buf[16];
        printf("Escolha: ");
        if (!fgets(buf, sizeof(buf), stdin)) break;

        char op = buf[0];
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
            printf("Opcao invalida.\n");
        }
    }
}

/* Coleta suspeitos a partir das pistas na BST */
typedef struct {
    char nome[64];
    int cont;
} ContSuspeito;

int contarSuspeitos(PistaNode *raiz, HashTable *hash, ContSuspeito *arr, int cap) {
    if (!raiz) return 0;

    int usados = 0;
    usados += contarSuspeitos(raiz->esq, hash, arr + usados, cap - usados);

    const char *sus = encontrarSuspeito(hash, raiz->pista);
    if (sus && usados < cap) {
        int achou = 0;
        for (int i = 0; i < usados; i++) {
            if (strcmp(arr[i].nome, sus) == 0) {
                arr[i].cont++;
                achou = 1;
                break;
            }
        }
        if (!achou && usados < cap) {
            strncpy(arr[usados].nome, sus, sizeof(arr[usados].nome) - 1);
            arr[usados].nome[sizeof(arr[usados].nome) - 1] = '\0';
            arr[usados].cont = 1;
            usados++;
        }
    }

    usados += contarSuspeitos(raiz->dir, hash, arr + usados, cap - usados);
    return usados;
}

/* Permitir consulta de todas as pistas relacionadas a cada suspeito. */
void listarPistasPorSuspeitoRec(PistaNode *raiz, HashTable *hash, const char *suspeito) {
    if (!raiz) return;
    listarPistasPorSuspeitoRec(raiz->esq, hash, suspeito);

    const char *sus = encontrarSuspeito(hash, raiz->pista);
    if (sus && strcmp(sus, suspeito) == 0) {
        printf("  - %s\n", raiz->pista);
    }

    listarPistasPorSuspeitoRec(raiz->dir, hash, suspeito);
}

void listarPistasPorSuspeito(PistaNode *bstPistas, HashTable *hash) {
    if (!bstPistas) {
        printf("(nenhuma pista coletada)\n");
        return;
    }

    ContSuspeito conts[32];
    for (int i = 0; i < 32; i++) {
        conts[i].nome[0] = '\0';
        conts[i].cont = 0;
    }

    int usados = contarSuspeitos(bstPistas, hash, conts, 32);
    if (usados == 0) {
        printf("(nenhuma associacao pista -> suspeito encontrada)\n");
        return;
    }

    for (int i = 0; i < usados; i++) {
        printf("\nSuspeito: %s (pistas: %d)\n", conts[i].nome, conts[i].cont);
        listarPistasPorSuspeitoRec(bstPistas, hash, conts[i].nome);
    }
}

/* Mostrar o suspeito mais citado ao final da análise. */
void mostrarMaisCitado(PistaNode *bstPistas, HashTable *hash) {
    if (!bstPistas) {
        printf("\nSuspeito mais citado: (nenhum)\n");
        return;
    }

    ContSuspeito conts[32];
    for (int i = 0; i < 32; i++) {
        conts[i].nome[0] = '\0';
        conts[i].cont = 0;
    }

    int usados = contarSuspeitos(bstPistas, hash, conts, 32);
    if (usados == 0) {
        printf("\nSuspeito mais citado: (nenhuma associacao)\n");
        return;
    }

    int best = 0;
    for (int i = 1; i < usados; i++) {
        if (conts[i].cont > conts[best].cont) best = i;
    }
    printf("\nSuspeito mais citado: %s (%d pista(s))\n", conts[best].nome, conts[best].cont);
}

int contarParaSuspeito(PistaNode *raiz, HashTable *hash, const char *suspeitoNorm) {
    if (!raiz) return 0;

    int total = 0;
    total += contarParaSuspeito(raiz->esq, hash, suspeitoNorm);

    const char *sus = encontrarSuspeito(hash, raiz->pista);
    if (sus) {
        char tmp[64];
        strncpy(tmp, sus, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = '\0';
        trim(tmp);
        toLowerStr(tmp);
        if (strcmp(tmp, suspeitoNorm) == 0) total++;
    }

    total += contarParaSuspeito(raiz->dir, hash, suspeitoNorm);
    return total;
}

/* verificarSuspeitoFinal() – conduz à fase de julgamento final. */
void verificarSuspeitoFinal(PistaNode *bstPistas, HashTable *hash) {
    char acusado[64];

    printf("\n===== JULGAMENTO FINAL =====\n");
    lerLinha("Digite o nome do suspeito acusado: ", acusado, sizeof(acusado));
    trim(acusado);

    if (acusado[0] == '\0') {
        printf("Acusacao vazia. Encerrando.\n");
        return;
    }

    char acusadoNorm[64];
    strncpy(acusadoNorm, acusado, sizeof(acusadoNorm) - 1);
    acusadoNorm[sizeof(acusadoNorm) - 1] = '\0';
    toLowerStr(acusadoNorm);

    int qtd = contarParaSuspeito(bstPistas, hash, acusadoNorm);

    printf("Pistas coletadas que apontam para \"%s\": %d\n", acusado, qtd);
    if (qtd >= 2) {
        printf("Desfecho: ACUSACAO SUSTENTADA (ha pistas suficientes).\n");
    } else {
        printf("Desfecho: ACUSACAO FRACA (pistas insuficientes).\n");
    }
}

int main(void) {
    /* Mapa fixo  */
    Sala *hall = criarSala("Hall de Entrada", "Pegadas molhadas no tapete");
    hall->esq = criarSala("Sala de Estar", "Carta rasgada perto da lareira");
    hall->dir = criarSala("Corredor Principal", NULL);

    hall->esq->esq = criarSala("Biblioteca", "Livro fora do lugar: Criptografia");
    hall->esq->dir = criarSala("Cozinha", "Faca recem-lavada");

    hall->dir->esq = criarSala("Escritorio", "Cofre aberto sem sinais de arrombamento");
    hall->dir->dir = criarSala("Jardim de Inverno", "Terra fresca em um vaso quebrado");

    /* Hash pista -> suspeito */
    HashTable hash;
    inicializarHash(&hash);

    inserirNaHash(&hash, "Pegadas molhadas no tapete", "Mordomo");
    inserirNaHash(&hash, "Carta rasgada perto da lareira", "Herdeira");
    inserirNaHash(&hash, "Livro fora do lugar: Criptografia", "Professor");
    inserirNaHash(&hash, "Faca recem-lavada", "Cozinheira");
    inserirNaHash(&hash, "Cofre aberto sem sinais de arrombamento", "Herdeira");
    inserirNaHash(&hash, "Terra fresca em um vaso quebrado", "Jardineiro");

    /* BST de pistas coletadas */
    PistaNode *bstPistas = NULL;

    /* Exploração */
    explorarSalas(hall, &bstPistas, &hash);

    /* Saídas finais */
    printf("\n===== PISTAS COLETADAS (ordem alfabetica) =====\n");
    if (!bstPistas) printf("(nenhuma)\n");
    else emOrdem(bstPistas);

    printf("\n===== PISTAS POR SUSPEITO (com base nas pistas coletadas) =====\n");
    listarPistasPorSuspeito(bstPistas, &hash);

    mostrarMaisCitado(bstPistas, &hash);

    verificarSuspeitoFinal(bstPistas, &hash);

    /* Limpeza */
    liberarPistas(bstPistas);
    liberarHash(&hash);
    liberarSalas(hall);
    return 0;
}
