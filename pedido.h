#ifndef PEDIDO_H
#define PEDIDO_H

typedef enum {
    CADASTRADO = 0,
    NA_FILA = 1,
    DESPACHADO = 2
} StatusPedido;

typedef struct {
    int codigo;
    int data;              /* formato AAAAMMDD */
    int urgente;           /* 1: urgente, 0: normal */
    int bairro;            /* indice do bairro no grafo */
    unsigned long chegada; /* desempate da fila */
    StatusPedido status;
} Pedido;

void pedido_imprimir(const Pedido *pedido, const char *nome_bairro);

#endif
