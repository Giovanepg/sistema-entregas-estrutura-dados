#include <stdio.h>
#include <stdlib.h>
#include "pedido.h"
#include "vetor.h"
#include "hash.h"
#include "heap.h"
#include "pilha.h"
#include "grafo.h"
#include "testes.h"

/* Registro compartilhado entre as estruturas. Somente o vetor libera os pedidos. */
typedef struct {
    Vetor pedidos;
    Hash indice;
    Heap despacho;
    Pilha desfazer;
    Pilha refazer;
    Grafo bairros;
    unsigned long proxima_chegada;
} Sistema;

static unsigned int semente = 12345;
static unsigned int sorteio(void) {
    semente = semente * 1664525u + 1013904223u;
    return semente;
}

static int ler_inteiro(const char *mensagem, int *valor) {
    char linha[100], resto;
    printf("%s", mensagem);
    if (!fgets(linha, sizeof(linha), stdin)) return 0;
    return sscanf(linha, " %d %c", valor, &resto) == 1;
}

static void bairros_padrao(Grafo *g) {
    const char *nomes[] = {"Centro", "Norte", "Sul", "Leste", "Oeste", "Industrial"};
    for (int i = 0; i < 6; i++) grafo_adicionar_bairro(g, nomes[i]);
    /* Ruas ficticias entre bairros; cada aresta conta uma rua. */
    grafo_adicionar_rua(g, 0, 1);
    grafo_adicionar_rua(g, 0, 2);
    grafo_adicionar_rua(g, 1, 3);
    grafo_adicionar_rua(g, 2, 4);
    grafo_adicionar_rua(g, 3, 5);
    grafo_adicionar_rua(g, 4, 5);
}

static int sistema_iniciar(Sistema *s) {
    if (!vetor_iniciar(&s->pedidos)) return 0;
    if (!hash_iniciar(&s->indice)) {
        vetor_destruir(&s->pedidos);
        return 0;
    }
    if (!heap_iniciar(&s->despacho)) {
        hash_destruir(&s->indice);
        vetor_destruir(&s->pedidos);
        return 0;
    }
    pilha_iniciar(&s->desfazer);
    pilha_iniciar(&s->refazer);
    grafo_iniciar(&s->bairros);
    bairros_padrao(&s->bairros);
    s->proxima_chegada = 0;
    return 1;
}

static void sistema_destruir(Sistema *s) {
    pilha_limpar(&s->desfazer);
    pilha_limpar(&s->refazer);
    heap_destruir(&s->despacho);
    hash_destruir(&s->indice);
    grafo_destruir(&s->bairros);
    vetor_destruir(&s->pedidos);
}

static int data_valida(int data) {
    int ano = data / 10000;
    int mes = data / 100 % 100;
    int dia = data % 100;
    if (ano < 2020 || ano > 2100 || mes < 1 || mes > 12 || dia < 1) return 0;
    int dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes == 2 && ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0))
        return dia <= 29;
    return dia <= dias[mes - 1];
}

static int cadastrar(Sistema *s, int codigo, int data, int urgente, int bairro) {
    if (codigo <= 0 || !data_valida(data) || (urgente != 0 && urgente != 1) ||
        bairro < 0 || bairro >= s->bairros.quantidade || hash_buscar(&s->indice, codigo))
        return 0;
    Pedido *p = malloc(sizeof(Pedido));
    if (!p) return 0;
    p->codigo = codigo;
    p->data = data;
    p->urgente = urgente;
    p->bairro = bairro;
    p->chegada = 0;
    p->status = CADASTRADO;
    if (!vetor_adicionar(&s->pedidos, p)) { free(p); return 0; }
    if (!hash_inserir(&s->indice, p)) {
        s->pedidos.tamanho--;
        free(p);
        return 0;
    }
    return 1;
}

static void mostrar_bairros(const Grafo *g) {
    for (int i = 0; i < g->quantidade; i++)
        printf("%d - %s  ", i, g->nomes[i]);
    printf("\n");
}

static void menu_cadastrar(Sistema *s) {
    int codigo, data, urgente, bairro;
    if (!ler_inteiro("Codigo: ", &codigo) ||
        !ler_inteiro("Data (AAAAMMDD): ", &data) ||
        !ler_inteiro("Urgente? (1=sim, 0=nao): ", &urgente)) {
        puts("Entrada invalida."); return;
    }
    mostrar_bairros(&s->bairros);
    if (!ler_inteiro("Numero do bairro: ", &bairro)) {
        puts("Entrada invalida."); return;
    }
    puts(cadastrar(s, codigo, data, urgente, bairro) ?
         "Pedido cadastrado! Use a opcao 4 para colocar na fila." : "Nao foi possivel cadastrar (verifique codigo e dados).");
}

static void menu_buscar(Sistema *s) {
    int codigo;
    if (!ler_inteiro("Codigo para buscar: ", &codigo)) { puts("Codigo invalido."); return; }
    Pedido *p = hash_buscar(&s->indice, codigo);
    if (p) pedido_imprimir(p, s->bairros.nomes[p->bairro]);
    else puts("Pedido nao encontrado.");
}

static void menu_listar(Sistema *s) {
    if (s->pedidos.tamanho == 0) { puts("Nenhum pedido cadastrado."); return; }
    Pedido **ordenados = vetor_ordenado_por_data(&s->pedidos);
    if (!ordenados) { puts("Memoria insuficiente."); return; }
    for (size_t i = 0; i < s->pedidos.tamanho; i++)
        pedido_imprimir(ordenados[i], s->bairros.nomes[ordenados[i]->bairro]);
    free(ordenados);
}

static void menu_enfileirar(Sistema *s) {
    int codigo;
    if (!ler_inteiro("Codigo para entrar na fila: ", &codigo)) { puts("Codigo invalido."); return; }
    Pedido *p = hash_buscar(&s->indice, codigo);
    if (!p || p->status != CADASTRADO) {
        puts("Pedido inexistente ou ja adicionado/despachado."); return;
    }
    p->chegada = ++s->proxima_chegada;
    if (!heap_adicionar(&s->despacho, p)) { puts("Memoria insuficiente."); return; }
    p->status = NA_FILA;
    puts("Pedido adicionado a fila de despacho.");
}

static void menu_despachar(Sistema *s) {
    Pedido *p = heap_retirar_proximo(&s->despacho);
    if (!p) { puts("Fila vazia! Cadastre um pedido (1) e adicione-o a fila (4), ou gere dados (9)."); return; }
    if (!pilha_empilhar(&s->desfazer, p)) {
        heap_adicionar(&s->despacho, p);
        puts("Memoria insuficiente."); return;
    }
    p->status = DESPACHADO;
    pilha_limpar(&s->refazer); /* novo despacho invalida refazer anterior */
    printf("Despachado pedido %d (%s, chegada %lu).\n", p->codigo,
           p->urgente ? "urgente" : "normal", p->chegada);
}

static void menu_desfazer(Sistema *s) {
    if (!s->desfazer.topo) { puts("Nada para desfazer! Primeiro execute um despacho pela opcao 5."); return; }
    Pedido *p = s->desfazer.topo->pedido;
    if (!heap_adicionar(&s->despacho, p)) { puts("Memoria insuficiente."); return; }
    if (!pilha_empilhar(&s->refazer, p)) {
        heap_remover_codigo(&s->despacho, p->codigo);
        puts("Memoria insuficiente."); return;
    }
    pilha_desempilhar(&s->desfazer);
    p->status = NA_FILA;
    printf("Desfeito despacho do pedido %d. Ele voltou para a fila.\n", p->codigo);
}

static void menu_refazer(Sistema *s) {
    if (!s->refazer.topo) { puts("Nada para refazer! Primeiro desfaça um despacho pela opcao 6."); return; }
    Pedido *p = s->refazer.topo->pedido;
    if (!pilha_empilhar(&s->desfazer, p)) { puts("Memoria insuficiente."); return; }
    if (!heap_remover_codigo(&s->despacho, p->codigo)) {
        pilha_desempilhar(&s->desfazer);
        puts("O pedido nao esta mais na fila; nao foi possivel refazer."); return;
    }
    pilha_desempilhar(&s->refazer);
    p->status = DESPACHADO;
    printf("Refeito despacho do pedido %d.\n", p->codigo);
}

static void menu_caminho(Sistema *s) {
    int origem, destino, caminho[MAX_BAIRROS];
    mostrar_bairros(&s->bairros);
    if (!ler_inteiro("Bairro de origem: ", &origem) ||
        !ler_inteiro("Bairro de destino: ", &destino)) {
        puts("Entrada invalida."); return;
    }
    int ruas = grafo_menor_caminho(&s->bairros, origem, destino, caminho);
    if (ruas < 0) { puts("Bairros invalidos ou sem caminho."); return; }
    printf("Menor caminho: %d rua(s). Rota: ", ruas);
    for (int i = 0; i <= ruas; i++) {
        if (i) printf(" -> ");
        printf("%s", s->bairros.nomes[caminho[i]]);
    }
    printf("\n");
}

static void menu_gerar(Sistema *s) {
    int quantidade;
    if (!ler_inteiro("Quantos pedidos gerar? (100, 1000 ou 10000): ", &quantidade) ||
        (quantidade != 100 && quantidade != 1000 && quantidade != 10000)) {
        puts("Escolha 100, 1000 ou 10000."); return;
    }
    semente = 12345u; /* semente fixa para testes reproduziveis */
    int gerados = 0;
    int codigo = 100000;
    while (gerados < quantidade) {
        while (hash_buscar(&s->indice, codigo)) codigo++;
        int data = 20260101 + (int)(sorteio() % 28u) +
                   (int)(sorteio() % 12u) * 100;
        int urgente = (int)(sorteio() % 2u);
        int bairro = (int)(sorteio() % (unsigned)s->bairros.quantidade);
        if (!cadastrar(s, codigo, data, urgente, bairro)) break;
        /* Pedidos gerados entram automaticamente na fila para facilitar os testes. */
        Pedido *p = hash_buscar(&s->indice, codigo);
        p->chegada = ++s->proxima_chegada;
        if (heap_adicionar(&s->despacho, p)) p->status = NA_FILA;
        else {
            puts("Memoria insuficiente para adicionar todos os pedidos a fila.");
            gerados++;
            break;
        }
        codigo++;
        gerados++;
    }
    printf("Gerados %d pedidos. Total cadastrado: %zu.\n", gerados, s->pedidos.tamanho);
    puts("Os pedidos gerados ja foram colocados na fila. Use 5 para despachar e 11 para conferir a fila.");
    puts("Use a opcao 10 para executar as comparacoes.");
}

static void menu_estado(Sistema *s) {
    int cadastrados = 0, na_fila = 0, despachados = 0;
    for (size_t i = 0; i < s->pedidos.tamanho; i++) {
        Pedido *p = s->pedidos.itens[i];
        if (p->status == CADASTRADO) cadastrados++;
        else if (p->status == NA_FILA) na_fila++;
        else despachados++;
    }
    printf("Pedidos cadastrados (ainda fora da fila): %d\n", cadastrados);
    printf("Pedidos na fila: %d\n", na_fila);
    printf("Pedidos despachados: %d\n", despachados);
    printf("Despachos que podem ser desfeitos: %zu\n", s->desfazer.tamanho);
    printf("Despachos que podem ser refeitos: %zu\n", s->refazer.tamanho);
    /* Mostra os primeiros pedidos na ordem correta sem alterar o heap verdadeiro. */
    Heap copia;
    if (!heap_iniciar(&copia)) return;
    for (size_t i = 0; i < s->despacho.tamanho; i++) {
        if (!heap_adicionar(&copia, s->despacho.itens[i])) {
            heap_destruir(&copia);
            puts("Memoria insuficiente para mostrar a fila.");
            return;
        }
    }
    int maximo = na_fila < 10 ? na_fila : 10;
    for (int i = 0; i < maximo; i++) {
        Pedido *p = heap_retirar_proximo(&copia);
        printf("%d. Pedido %d (%s, chegada %lu)\n", i + 1, p->codigo,
               p->urgente ? "urgente" : "normal", p->chegada);
    }
    if (na_fila > 10) puts("Mostrando apenas os 10 proximos pedidos.");
    heap_destruir(&copia);
}

static void imprimir_menu(void) {
    puts("\n======= SISTEMA DE ENTREGAS =======");
    puts("1 - Cadastrar pedido");
    puts("2 - Buscar pedido pelo codigo (hash)");
    puts("3 - Listar pedidos por data (vetor)");
    puts("4 - Adicionar pedido a fila (heap)");
    puts("5 - Despachar proximo pedido (heap)");
    puts("6 - Desfazer ultimo despacho (pilha)");
    puts("7 - Refazer ultimo despacho (pilha)");
    puts("8 - Menor caminho entre bairros (grafo)");
    puts("9 - Gerar dados com semente fixa");
    puts("10 - Comparar estruturas e medir tempos");
    puts("11 - Ver fila e situacao dos pedidos");
    puts("0 - Sair");
}

int main(void) {
    Sistema s;
    if (!sistema_iniciar(&s)) {
        puts("Erro ao iniciar as estruturas.");
        return 1;
    }
    int opcao;
    while (1) {
        imprimir_menu();
        if (!ler_inteiro("Opcao: ", &opcao)) break;
        switch (opcao) {
            case 1: menu_cadastrar(&s); break;
            case 2: menu_buscar(&s); break;
            case 3: menu_listar(&s); break;
            case 4: menu_enfileirar(&s); break;
            case 5: menu_despachar(&s); break;
            case 6: menu_desfazer(&s); break;
            case 7: menu_refazer(&s); break;
            case 8: menu_caminho(&s); break;
            case 9: menu_gerar(&s); break;
            case 10: executar_testes(); break;
            case 11: menu_estado(&s); break;
            case 0: sistema_destruir(&s); puts("Encerrado."); return 0;
            default: puts("Opcao invalida.");
        }
    }
    sistema_destruir(&s);
    return 0;
}
