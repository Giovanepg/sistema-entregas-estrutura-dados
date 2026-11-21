#include <stdlib.h>
#include <string.h>
#include "grafo.h"

void grafo_iniciar(Grafo *g) {
    g->quantidade = 0;
    for (int i = 0; i < MAX_BAIRROS; i++) g->vizinhos[i] = NULL;
}

void grafo_destruir(Grafo *g) {
    for (int i = 0; i < g->quantidade; i++) {
        NoAresta *no = g->vizinhos[i];
        while (no) {
            NoAresta *prox = no->proximo;
            free(no);
            no = prox;
        }
        g->vizinhos[i] = NULL;
    }
}

int grafo_adicionar_bairro(Grafo *g, const char *nome) {
    if (g->quantidade == MAX_BAIRROS) return -1;
    int indice = g->quantidade++;
    strncpy(g->nomes[indice], nome, sizeof(g->nomes[indice]) - 1);
    g->nomes[indice][sizeof(g->nomes[indice]) - 1] = '\0';
    return indice;
}

static int existe_rua(const Grafo *g, int a, int b) {
    for (NoAresta *no = g->vizinhos[a]; no; no = no->proximo)
        if (no->destino == b) return 1;
    return 0;
}

int grafo_adicionar_rua(Grafo *g, int a, int b) {
    if (a < 0 || b < 0 || a >= g->quantidade || b >= g->quantidade || a == b) return 0;
    if (existe_rua(g, a, b)) return 1;
    NoAresta *ab = malloc(sizeof(NoAresta));
    NoAresta *ba = malloc(sizeof(NoAresta));
    if (!ab || !ba) { free(ab); free(ba); return 0; }
    ab->destino = b; ab->proximo = g->vizinhos[a];
    ba->destino = a; ba->proximo = g->vizinhos[b];
    g->vizinhos[a] = ab;
    g->vizinhos[b] = ba;
    return 1;
}

int grafo_menor_caminho(const Grafo *g, int origem, int destino, int *caminho) {
    if (origem < 0 || destino < 0 || origem >= g->quantidade || destino >= g->quantidade)
        return -1;
    int fila[MAX_BAIRROS], inicio = 0, fim = 0;
    int anterior[MAX_BAIRROS];
    for (int i = 0; i < g->quantidade; i++) anterior[i] = -1;
    anterior[origem] = origem;
    fila[fim++] = origem;
    while (inicio < fim && anterior[destino] == -1) {
        int atual = fila[inicio++];
        for (NoAresta *no = g->vizinhos[atual]; no; no = no->proximo) {
            int prox = no->destino;
            if (anterior[prox] == -1) {
                anterior[prox] = atual;
                fila[fim++] = prox;
            }
        }
    }
    if (anterior[destino] == -1) return -1;
    int invertido[MAX_BAIRROS], total = 0;
    for (int atual = destino; atual != origem; atual = anterior[atual])
        invertido[total++] = atual;
    invertido[total++] = origem;
    for (int i = 0; i < total; i++) caminho[i] = invertido[total - 1 - i];
    return total - 1; /* numero de ruas, nao distancia em km */
}
