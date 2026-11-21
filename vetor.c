#include <stdlib.h>
#include "vetor.h"

int vetor_iniciar(Vetor *v) {
    v->tamanho = 0;
    v->capacidade = 16;
    v->itens = malloc(v->capacidade * sizeof(Pedido *));
    return v->itens != NULL;
}

void vetor_destruir(Vetor *v) {
    for (size_t i = 0; i < v->tamanho; i++) free(v->itens[i]);
    free(v->itens);
    v->itens = NULL;
    v->tamanho = v->capacidade = 0;
}

int vetor_adicionar(Vetor *v, Pedido *p) {
    if (v->tamanho == v->capacidade) {
        size_t nova = v->capacidade * 2;
        Pedido **aux = realloc(v->itens, nova * sizeof(Pedido *));
        if (!aux) return 0;
        v->itens = aux;
        v->capacidade = nova;
    }
    v->itens[v->tamanho++] = p;
    return 1;
}

static int antes(const Pedido *a, const Pedido *b) {
    if (a->data != b->data) return a->data < b->data;
    return a->codigo <= b->codigo;
}

static void mesclar(Pedido **lista, Pedido **aux, size_t ini, size_t meio, size_t fim) {
    size_t i = ini, j = meio, k = ini;
    while (i < meio && j < fim) {
        aux[k++] = antes(lista[i], lista[j]) ? lista[i++] : lista[j++];
    }
    while (i < meio) aux[k++] = lista[i++];
    while (j < fim) aux[k++] = lista[j++];
    for (k = ini; k < fim; k++) lista[k] = aux[k];
}

static void mergesort(Pedido **lista, Pedido **aux, size_t ini, size_t fim) {
    if (fim - ini <= 1) return;
    size_t meio = ini + (fim - ini) / 2;
    mergesort(lista, aux, ini, meio);
    mergesort(lista, aux, meio, fim);
    mesclar(lista, aux, ini, meio, fim);
}

Pedido **vetor_ordenado_por_data(const Vetor *v) {
    if (v->tamanho == 0) return NULL;
    Pedido **copia = malloc(v->tamanho * sizeof(Pedido *));
    Pedido **aux = malloc(v->tamanho * sizeof(Pedido *));
    if (!copia || !aux) { free(copia); free(aux); return NULL; }
    for (size_t i = 0; i < v->tamanho; i++) copia[i] = v->itens[i];
    mergesort(copia, aux, 0, v->tamanho);
    free(aux);
    return copia;
}
