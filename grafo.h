#ifndef GRAFO_H
#define GRAFO_H

#define MAX_BAIRROS 10000

typedef struct NoAresta {
    int destino;
    struct NoAresta *proximo;
} NoAresta;

typedef struct {
    int quantidade;
    char nomes[MAX_BAIRROS][32];
    NoAresta *vizinhos[MAX_BAIRROS];
} Grafo;

void grafo_iniciar(Grafo *g);
void grafo_destruir(Grafo *g);
int grafo_adicionar_bairro(Grafo *g, const char *nome);
int grafo_adicionar_rua(Grafo *g, int origem, int destino);
int grafo_menor_caminho(const Grafo *g, int origem, int destino, int *caminho);

#endif
