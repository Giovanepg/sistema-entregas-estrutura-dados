
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pedido.h"
#include "vetor.h"
#include "hash.h"
#include "heap.h"
#include "pilha.h"
#include "grafo.h"
#include "testes.h"

/* Gerador simples, sempre com a mesma semente. */
static unsigned int semente = 12345;

static unsigned int aleatorio(void) {
    semente = semente * 1664525u + 1013904223u;
    return semente;
}

/* 0 = ordenado, 1 = aleatorio, 2 = quase ordenado */
static void fazer_codigos(int *codigos, int n, int tipo) {
    for (int i = 0; i < n; i++)
        codigos[i] = i + 1;

    if (tipo == 0)
        return;

    int trocas = tipo == 1 ? n : n / 20;

    for (int i = 0; i < trocas; i++) {
        int a = aleatorio() % n;
        int b = aleatorio() % n;

        int temp = codigos[a];
        codigos[a] = codigos[b];
        codigos[b] = temp;
    }
}

/* Calcula o tempo em segundos. */
static double segundos(clock_t comeco) {
    return (double)(clock() - comeco) / CLOCKS_PER_SEC;
}

/* Mostra o resultado e salva no CSV. */
static void salvar(FILE *arquivo, const char *demanda,
                   const char *caso, int n,
                   const char *metodo, double tempo) {

    printf("%-3s %-14s n=%-5d %-18s %.6f s\n",
        demanda, caso, n, metodo, tempo);

    if (arquivo) {
        fprintf(arquivo, "%s,%s,%d,%s,%.9f\n",
                demanda, caso, n, metodo, tempo);
    }
}

/* D1: busca linear para comparar com a hash. */
static Pedido *busca_linear(Pedido *pedidos, int n, int codigo) {
    for (int i = 0; i < n; i++) {
        if (pedidos[i].codigo == codigo)
            return &pedidos[i];
    }

    return NULL;
}

/* D2: Insertion Sort para comparar com Merge Sort. */
static void ordenar_insercao(Pedido **v, int n) {
    for (int i = 1; i < n; i++) {
        Pedido *atual = v[i];
        int j = i - 1;

        while (j >= 0 &&
            (v[j]->data > atual->data ||
            (v[j]->data == atual->data &&
                v[j]->codigo > atual->codigo))) {

            v[j + 1] = v[j];
            j--;
        }

        v[j + 1] = atual;
    }
}

/* Decide qual pedido tem maior prioridade. */
static int primeiro(Pedido *a, Pedido *b) {
    if (a->urgente != b->urgente)
        return a->urgente > b->urgente;

    return a->chegada < b->chegada;
}

/* D3: vetor ordenado para comparar com heap. */
static void inserir_ordenado(Pedido **lista,
                            int *total, Pedido *p) {
    int i = *total;

    while (i > 0 && primeiro(p, lista[i - 1])) {
        lista[i] = lista[i - 1];
        i--;
    }

    lista[i] = p;
    (*total)++;
}

/* D5: busca em largura usando matriz de adjacencia. */
static int bfs_matriz(unsigned char *matriz, int n,
                    int origem, int destino,
                    int *fila, int *distancia) {

    for (int i = 0; i < n; i++)
        distancia[i] = -1;

    int inicio = 0;
    int fim = 0;

    fila[fim++] = origem;
    distancia[origem] = 0;

    while (inicio < fim) {
        int atual = fila[inicio++];

        if (atual == destino)
            return distancia[atual];

        for (int j = 0; j < n; j++) {
            if (matriz[(size_t)atual * n + j] &&
                distancia[j] == -1) {

                distancia[j] = distancia[atual] + 1;
                fila[fim++] = j;
            }
        }
    }

    return -1;
}

/* Executa os testes de D1, D2, D3 e D4. */
static void testar_pedidos(FILE *arquivo, int n,
                           int tipo, const char *caso) {

    int *codigos = malloc(n * sizeof(int));
    Pedido *pedidos = calloc(n, sizeof(Pedido));
    Pedido **alternativa = malloc(n * sizeof(Pedido *));

    if (!codigos || !pedidos || !alternativa) {
        puts("Memoria insuficiente nos testes.");
        free(codigos);
        free(pedidos);
        free(alternativa);
        return;
    }

    semente = 12345;
    fazer_codigos(codigos, n, tipo);

    for (int i = 0; i < n; i++) {
        pedidos[i].codigo = codigos[i];
        pedidos[i].data = codigos[i];
        pedidos[i].urgente = codigos[i] % 2;
        pedidos[i].chegada = i + 1;
        alternativa[i] = &pedidos[i];
    }

    clock_t inicio;
    double tempo;
    volatile int achados = 0;

    /* D1: Hash contra busca linear. */
    Hash hash;

    if (!hash_iniciar(&hash))
        goto fim;

    inicio = clock();

    for (int i = 0; i < n; i++)
        hash_inserir(&hash, &pedidos[i]);

    salvar(arquivo, "D1", caso, n,
        "hash_inserir", segundos(inicio));

    inicio = clock();

    for (int i = 0; i < n; i++) {
        if (hash_buscar(&hash, codigos[i]))
            achados++;
    }

    salvar(arquivo, "D1", caso, n,
        "hash_buscar", segundos(inicio));

    inicio = clock();

    for (int i = 0; i < n; i++) {
        if (busca_linear(pedidos, n, codigos[i]))
            achados++;
    }

    salvar(arquivo, "D1", caso, n,
        "busca_linear", segundos(inicio));

    hash_destruir(&hash);

    /* D2: Merge Sort contra Insertion Sort. */
    Vetor vetor;

    if (!vetor_iniciar(&vetor))
        goto fim;

    inicio = clock();

    for (int i = 0; i < n; i++)
        vetor_adicionar(&vetor, &pedidos[i]);

    salvar(arquivo, "D1", caso, n,
           "vetor_inserir", segundos(inicio));

    inicio = clock();

    Pedido **ordenados = vetor_ordenado_por_data(&vetor);
    tempo = segundos(inicio);

    if (ordenados) {
        salvar(arquivo, "D2", caso, n,
               "merge_sort", tempo);
        free(ordenados);
    }

    inicio = clock();

    ordenar_insercao(alternativa, n);

    salvar(arquivo, "D2", caso, n,
           "insertion_sort", segundos(inicio));

    vetor.tamanho = 0;
    vetor_destruir(&vetor);

    /* D3: Heap contra vetor ordenado. */
    Heap heap;

    if (!heap_iniciar(&heap))
        goto fim;

    inicio = clock();

    for (int i = 0; i < n; i++)
        heap_adicionar(&heap, &pedidos[i]);

    salvar(arquivo, "D3", caso, n,
           "heap_inserir", segundos(inicio));

    inicio = clock();

    for (int i = 0; i < n; i++) {
        if (heap_retirar_proximo(&heap))
            achados++;
    }

    salvar(arquivo, "D3", caso, n,
           "heap_retirar", segundos(inicio));

    int total = 0;

    inicio = clock();

    for (int i = 0; i < n; i++)
        inserir_ordenado(alternativa, &total, &pedidos[i]);

    salvar(arquivo, "D3", caso, n,
           "vetor_inserir", segundos(inicio));

    inicio = clock();

    while (total > 0) {
        if (alternativa[0])
            achados++;

        for (int j = 1; j < total; j++)
            alternativa[j - 1] = alternativa[j];

        total--;
    }

    salvar(arquivo, "D3", caso, n,
           "vetor_retirar", segundos(inicio));

    heap_destruir(&heap);

    /* D4: Pilha encadeada contra pilha com vetor. */
    Pilha pilha;
    pilha_iniciar(&pilha);

    inicio = clock();

    for (int i = 0; i < n; i++)
        pilha_empilhar(&pilha, &pedidos[i]);

    for (int i = 0; i < n; i++) {
        if (pilha_desempilhar(&pilha))
            achados++;
    }

    salvar(arquivo, "D4", caso, n,
           "pilha_encadeada", segundos(inicio));

    pilha_limpar(&pilha);

    int topo = 0;
    inicio = clock();

    for (int i = 0; i < n; i++)
        alternativa[topo++] = &pedidos[i];

    for (int i = 0; i < n; i++) {
        if (alternativa[--topo])
            achados++;
    }

    salvar(arquivo, "D4", caso, n,
           "pilha_vetor", segundos(inicio));

    if (achados < 0)
        puts("Erro inesperado.");

fim:
    free(codigos);
    free(pedidos);
    free(alternativa);
}

/* D5: compara lista e matriz de adjacencia. */
static void testar_grafo(FILE *arquivo, int n,
                         const char *caso) {

    Grafo *grafo = malloc(sizeof(Grafo));

    unsigned char *matriz =
        calloc((size_t)n * n, sizeof(unsigned char));

    int *fila = malloc(n * sizeof(int));
    int *distancia = malloc(n * sizeof(int));
    int *caminho = malloc(MAX_BAIRROS * sizeof(int));

    if (!grafo || !matriz || !fila ||
        !distancia || !caminho) {

        puts("Nao foi possivel alocar o grafo de teste.");

        free(grafo);
        free(matriz);
        free(fila);
        free(distancia);
        free(caminho);
        return;
    }

    grafo_iniciar(grafo);

    for (int i = 0; i < n; i++)
        grafo_adicionar_bairro(grafo, "Bairro");

    /* Liga cada bairro ao proximo. */
    for (int i = 0; i < n - 1; i++) {
        grafo_adicionar_rua(grafo, i, i + 1);

        matriz[(size_t)i * n + i + 1] = 1;
        matriz[(size_t)(i + 1) * n + i] = 1;
    }

    /* Adiciona alguns atalhos entre bairros. */
    for (int i = 0; i < n - 7; i += 7) {
        grafo_adicionar_rua(grafo, i, i + 7);

        matriz[(size_t)i * n + i + 7] = 1;
        matriz[(size_t)(i + 7) * n + i] = 1;
    }

    /* Repete as buscas para melhorar a medicao. */
    int repeticoes = n == 10000 ? 5 : 100;

    int resultado_lista = -1;
    int resultado_matriz = -1;

    clock_t inicio = clock();

    for (int i = 0; i < repeticoes; i++) {
        resultado_lista =
            grafo_menor_caminho(grafo, 0, n - 1, caminho);
    }

    salvar(arquivo, "D5", caso, n,
        "lista_BFS", segundos(inicio) / repeticoes);

    inicio = clock();

    for (int i = 0; i < repeticoes; i++) {
        resultado_matriz =
            bfs_matriz(matriz, n, 0, n - 1, fila, distancia);
    }

    salvar(arquivo, "D5", caso, n,
           "matriz_BFS", segundos(inicio) / repeticoes);

    if (resultado_lista != resultado_matriz)
        puts("ERRO: caminhos diferentes!");

    grafo_destruir(grafo);

    free(grafo);
    free(matriz);
    free(fila);
    free(distancia);
    free(caminho);
}

/* Funcao principal dos testes. */
void executar_testes(void) {
    int tamanhos[] = {100, 1000, 10000};

    const char *casos[] = {
        "ordenado",
        "aleatorio",
        "quase_ordenado"
    };

    FILE *arquivo = fopen("resultados.csv", "w");

    if (arquivo)
        fprintf(arquivo, "demanda,entrada,n,metodo,segundos\n");

    puts("\nCOMPARACOES (tempos medidos nesta execucao)");
    puts("Obs.: tempos muito pequenos podem aparecer como 0.000000.");

    for (int t = 0; t < 3; t++) {
        for (int tipo = 0; tipo < 3; tipo++) {

            printf("\n--- %s, n=%d ---\n",
                   casos[tipo], tamanhos[t]);

            testar_pedidos(arquivo, tamanhos[t],
                           tipo, casos[tipo]);
        }

        printf("\n--- grafo com %d bairros ---\n",
               tamanhos[t]);

        testar_grafo(arquivo, tamanhos[t], "grafo_fixo");
    }

    if (arquivo) {
        fclose(arquivo);
        puts("Resultados salvos em resultados.csv");
    } else {
        puts("Aviso: nao foi possivel criar resultados.csv");
    }
}
