#ifndef HEAP_H
#define HEAP_H
#include <stddef.h>
#include "pedido.h"

typedef struct {
    Pedido **itens;
    size_t tamanho;
    size_t capacidade;
} Heap;

int heap_iniciar(Heap *h);
void heap_destruir(Heap *h);
int heap_adicionar(Heap *h, Pedido *p);
Pedido *heap_retirar_proximo(Heap *h);
Pedido *heap_remover_codigo(Heap *h, int codigo); /* utilizado ao refazer */

#endif
