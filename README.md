# Sistema de Entregas — Projeto de Estrutura de Dados

Trabalho da 3ª Avaliação — Cenário C (Entregas). Código em C, com separação simples em arquivos `.h` (declarações) e `.c` (funções).

## Compilação

Na pasta do projeto, no Linux Mint ou no terminal do VS Code:

```bash
gcc -std=c11 -O2 -Wall -Wextra -pedantic main.c pedido.c vetor.c hash.c heap.c pilha.c grafo.c testes.c -o entregas
./entregas
```

No Windows (MinGW), substitua o executável final por `entregas.exe` e execute `./entregas.exe`.

## Estruturas e demandas

| Demanda | Escolha no sistema | Comparação nos testes |
|---|---|---|
| D1: buscar pedido por código | Tabela hash (sondagem linear) | Busca linear em vetor |
| D2: listar por data | Vetor + Merge Sort | Insertion Sort |
| D3: urgentes primeiro; empate por chegada | Heap com `urgente` e `chegada` | Vetor mantido em ordem |
| D4: desfazer/refazer último despacho | Duas pilhas encadeadas | Pilha implementada com vetor |
| D5: menos ruas entre dois bairros | Grafo com lista de adjacência + BFS | Matriz de adjacência + BFS |

Todas as estruturas estão implementadas manualmente, sem biblioteca de estruturas prontas. Não usamos `qsort`.

## O que significam os arquivos?

- `pedido.h` / `pedido.c`: define e mostra os dados de um pedido.
- `vetor.h` / `vetor.c`: guarda pedidos e ordena por data.
- `hash.h` / `hash.c`: procura pedidos por código.
- `heap.h` / `heap.c`: controla quem será despachado primeiro.
- `pilha.h` / `pilha.c`: permite desfazer/refazer despachos.
- `grafo.h` / `grafo.c`: representa ruas entre bairros e usa BFS.
- `main.c`: apresenta o menu e chama as funções.
- `testes.h` / `testes.c`: gera casos de teste e compara tempos.

## Uso das opções 5, 6 e 7

- Para pedidos cadastrados pela opção 1, **use a opção 4 para colocar cada pedido na fila**. Cadastrar não significa despachar.
- A opção **5** retira o próximo pedido da fila, respeitando urgência e chegada.
- A opção **6** desfaz um despacho já executado pela 5, devolvendo o pedido à fila.
- A opção **7** refaz o último despacho desfeito pela 6.
- A opção **9** agora gera pedidos e já coloca esses pedidos na fila para permitir testes imediatos.
- A nova opção **11** exibe os contadores e os próximos 10 pedidos na ordem de prioridade.
- Se você executar um novo despacho após desfazer, o histórico de refazer é apagado (comportamento normal de desfazer/refazer).

## Teste manual simples

1. Opção `1`: pedido `101`, data `20261116`, urgência `0`, bairro `0`.
2. Opção `1`: pedido `102`, data `20261117`, urgência `1`, bairro `1`.
3. Opção `4`: adicione os códigos `101` e `102` à fila.
4. Opção `5`: o código `102` deve ser despachado primeiro, por ser urgente.
5. Opção `6`: desfaz o despacho, devolvendo `102` à fila.
6. Opção `7`: refaz o despacho de `102`.
7. Opção `8`: encontre a rota entre bairro `0` e bairro `5`.
8. Opção `9`: gere 100, 1.000 ou 10.000 pedidos.
9. Opção `10`: mede e compara as estruturas. Cria o arquivo `resultados.csv`.

## Testes para a entrega

A opção `10` usa **semente fixa 12345**, faz comparações com **n = 100, 1.000 e 10.000** e, para as estruturas em que a ordem de entrada faz sentido, usa **ordem crescente, aleatória e quase ordenada**.

- D1 mede construção da hash e buscas por hash e por varredura linear, nos mesmos dados.
- D2 mede a ordenação com Merge Sort e Insertion Sort. As datas de teste são chaves numéricas sintéticas crescentes, para controlar a ordem e comparar os algoritmos; o menu utiliza datas no formato AAAAMMDD.
- D3 mede inserções e retiradas do heap e de um vetor ordenado; nos testes o vetor retira o primeiro elemento deslocando os demais.
- D4 mede empilhar e desempilhar n registros, tanto em pilha encadeada quanto em vetor.
- D5 mede BFS em lista e matriz com n bairros (100, 1.000 e 10.000) e a mesma rede simulada. Como a topologia independe da ordem dos pedidos, são apenas três casos de grafo.

**Importante:** os tempos de execução dependem do computador. Valores muito pequenos podem ser exibidos como 0 porque `clock()` tem resolução limitada. O arquivo CSV salva valores em segundos com nove casas decimais, mas não é garantia de precisão de nove casas. Rode os testes no computador da equipe e use esses resultados reais no relatório.

## Explicação das escolhas

- **Hash:** busca em média rápida com chave única; vetor linear é simples, mas pode precisar percorrer tudo.
- **Merge Sort:** tempo O(n log n) em comparação ao Insertion Sort, que pode exigir O(n²); o Merge Sort usa vetor auxiliar.
- **Heap:** mantém a prioridade, inclusive desempate por ordem de chegada, com inserção e remoção O(log n); vetor ordenado tem inserção O(n) e, nesta implementação, remoção O(n).
- **Pilhas:** o último despacho entra no topo e é o primeiro a sair (LIFO); duas pilhas permitem desfazer e refazer. A alternativa com vetor também tem custo O(1) para essas operações quando há espaço.
- **Grafo em lista:** evita guardar milhares de células sem ruas; matriz facilita verificar se uma rua existe, mas ocupa O(V²) memória. BFS minimiza **quantidade de ruas**, não quilômetros.

## Observações

- Dados em memória, sem banco de dados, como permite o enunciado.
- Bairros e ruas do menu são fictícios.
- O vetor principal é responsável pela memória dos pedidos. As demais estruturas só apontam para eles.
- Ainda é necessário escrever o **relatório final de no máximo quatro páginas** com decisões e medições e empacotar o projeto para entregar no Gennera.

## Sobre `resultados_exemplo.csv`

Este arquivo foi produzido durante a validação neste ambiente e é apenas **exemplo**. Gere `resultados.csv` executando a opção `10` no computador em que vocês vão apresentar o trabalho e utilize as medições dessa execução no relatório.
