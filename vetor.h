#ifndef VETOR_H
#define VETOR_H
#include <stddef.h>
#include "pedido.h"

typedef struct {
    Pedido **itens;
    size_t tamanho;
    size_t capacidade;
} Vetor;

int vetor_iniciar(Vetor *v);
void vetor_destruir(Vetor *v); /* libera tambem os pedidos */
int vetor_adicionar(Vetor *v, Pedido *pedido);
Pedido **vetor_ordenado_por_data(const Vetor *v); /* copia a lista de ponteiros */

#endif
