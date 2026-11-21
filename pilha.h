#ifndef PILHA_H
#define PILHA_H
#include <stddef.h>
#include "pedido.h"

typedef struct NoPilha {
    Pedido *pedido;
    struct NoPilha *proximo;
} NoPilha;

typedef struct {
    NoPilha *topo;
    size_t tamanho;
} Pilha;

void pilha_iniciar(Pilha *p);
int pilha_empilhar(Pilha *p, Pedido *pedido);
Pedido *pilha_desempilhar(Pilha *p);
void pilha_limpar(Pilha *p);

#endif
