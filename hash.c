#include <stdlib.h>
#include <stdint.h>
#include "hash.h"

static size_t posicao(int codigo, size_t capacidade) {
    return (size_t)(uint32_t)codigo % capacidade;
}

int hash_iniciar(Hash *h) {
    h->capacidade = 32;
    h->quantidade = 0;
    h->posicoes = calloc(h->capacidade, sizeof(Pedido *));
    return h->posicoes != NULL;
}

void hash_destruir(Hash *h) {
    free(h->posicoes);
    h->posicoes = NULL;
    h->capacidade = h->quantidade = 0;
}

Pedido *hash_buscar(const Hash *h, int codigo) {
    if (!h->posicoes) return NULL;
    size_t i = posicao(codigo, h->capacidade);
    while (h->posicoes[i]) {
        if (h->posicoes[i]->codigo == codigo) return h->posicoes[i];
        i = (i + 1) % h->capacidade;
    }
    return NULL;
}

static void colocar(Pedido **posicoes, size_t capacidade, Pedido *p) {
    size_t i = posicao(p->codigo, capacidade);
    while (posicoes[i]) i = (i + 1) % capacidade;
    posicoes[i] = p;
}

int hash_inserir(Hash *h, Pedido *p) {
    if (hash_buscar(h, p->codigo)) return 0;
    if ((h->quantidade + 1) * 10 >= h->capacidade * 7) {
        size_t nova = h->capacidade * 2;
        Pedido **posicoes = calloc(nova, sizeof(Pedido *));
        if (!posicoes) return 0;
        for (size_t i = 0; i < h->capacidade; i++) {
            if (h->posicoes[i]) colocar(posicoes, nova, h->posicoes[i]);
        }
        free(h->posicoes);
        h->posicoes = posicoes;
        h->capacidade = nova;
    }
    colocar(h->posicoes, h->capacidade, p);
    h->quantidade++;
    return 1;
}
