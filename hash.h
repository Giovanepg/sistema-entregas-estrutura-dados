#ifndef HASH_H
#define HASH_H
#include <stddef.h>
#include "pedido.h"

typedef struct {
    Pedido **posicoes;
    size_t capacidade;
    size_t quantidade;
} Hash;

int hash_iniciar(Hash *h);
void hash_destruir(Hash *h); /* pedidos pertencem ao vetor */
Pedido *hash_buscar(const Hash *h, int codigo);
int hash_inserir(Hash *h, Pedido *p);

#endif
