#include <stdio.h>
#include "pedido.h"

void pedido_imprimir(const Pedido *p, const char *bairro) {
    int ano = p->data / 10000;
    int mes = (p->data / 100) % 100;
    int dia = p->data % 100;
    const char *status[] = {"Cadastrado", "Na fila", "Despachado"};
    printf("Codigo: %d | Data: %02d/%02d/%04d | %s | Bairro: %s | Status: %s\n",
           p->codigo, dia, mes, ano, p->urgente ? "Urgente" : "Normal",
           bairro, status[p->status]);
}
