#include <stdlib.h>
#include "pilha.h"

void pilha_iniciar(Pilha *p) {
    p->topo = NULL;
    p->tamanho = 0;
}

int pilha_empilhar(Pilha *p, Pedido *pedido) {
    NoPilha *novo = malloc(sizeof(NoPilha));
    if (!novo) return 0;
    novo->pedido = pedido;
    novo->proximo = p->topo;
    p->topo = novo;
    p->tamanho++;
    return 1;
}

Pedido *pilha_desempilhar(Pilha *p) {
    if (!p->topo) return NULL;
    NoPilha *no = p->topo;
    Pedido *pedido = no->pedido;
    p->topo = no->proximo;
    free(no);
    p->tamanho--;
    return pedido;
}

void pilha_limpar(Pilha *p) {
    while (p->topo) pilha_desempilhar(p);
}
