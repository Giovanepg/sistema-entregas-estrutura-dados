#include <stdlib.h>
#include "heap.h"

static int maior_prioridade(const Pedido *a, const Pedido *b) {
    if (a->urgente != b->urgente) return a->urgente > b->urgente;
    return a->chegada < b->chegada;
}

static void trocar(Pedido **a, Pedido **b) {
    Pedido *tmp = *a; *a = *b; *b = tmp;
}

int heap_iniciar(Heap *h) {
    h->tamanho = 0;
    h->capacidade = 16;
    h->itens = malloc(h->capacidade * sizeof(Pedido *));
    return h->itens != NULL;
}

void heap_destruir(Heap *h) {
    free(h->itens);
    h->itens = NULL;
    h->tamanho = h->capacidade = 0;
}

static void subir(Heap *h, size_t i) {
    while (i > 0) {
        size_t pai = (i - 1) / 2;
        if (!maior_prioridade(h->itens[i], h->itens[pai])) break;
        trocar(&h->itens[i], &h->itens[pai]);
        i = pai;
    }
}

static void descer(Heap *h, size_t i) {
    while (1) {
        size_t esquerda = 2 * i + 1, direita = esquerda + 1, melhor = i;
        if (esquerda < h->tamanho && maior_prioridade(h->itens[esquerda], h->itens[melhor]))
            melhor = esquerda;
        if (direita < h->tamanho && maior_prioridade(h->itens[direita], h->itens[melhor]))
            melhor = direita;
        if (melhor == i) break;
        trocar(&h->itens[i], &h->itens[melhor]);
        i = melhor;
    }
}

int heap_adicionar(Heap *h, Pedido *p) {
    if (h->tamanho == h->capacidade) {
        size_t nova = h->capacidade * 2;
        Pedido **aux = realloc(h->itens, nova * sizeof(Pedido *));
        if (!aux) return 0;
        h->itens = aux;
        h->capacidade = nova;
    }
    h->itens[h->tamanho] = p;
    subir(h, h->tamanho);
    h->tamanho++;
    return 1;
}

static Pedido *retirar_indice(Heap *h, size_t i) {
    Pedido *p = h->itens[i];
    h->tamanho--;
    if (i != h->tamanho) {
        h->itens[i] = h->itens[h->tamanho];
        if (i > 0 && maior_prioridade(h->itens[i], h->itens[(i - 1) / 2]))
            subir(h, i);
        else
            descer(h, i);
    }
    return p;
}

Pedido *heap_retirar_proximo(Heap *h) {
    if (h->tamanho == 0) return NULL;
    return retirar_indice(h, 0);
}

Pedido *heap_remover_codigo(Heap *h, int codigo) {
    for (size_t i = 0; i < h->tamanho; i++) {
        if (h->itens[i]->codigo == codigo) return retirar_indice(h, i);
    }
    return NULL;
}
