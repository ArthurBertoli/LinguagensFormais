# Simulador de Autômato com Pilha

> Projeto da disciplina **Linguagens Formais e Autômatos** — Ciência da Computação, UDESC.

## Sobre

Programa interativo em C que permite montar um **Autômato com Pilha** (PDA) pelo terminal, definindo estados e transições, e depois verificar se uma palavra é aceita. Também exporta o autômato em formato DOT (Graphviz) para gerar uma visualização do grafo.

## Funcionalidades

Menu com as opções:

1. **Criar estado**: nome (até 9 caracteres) e indicação de estado inicial e/ou final. Só existe um estado inicial: criar outro substitui o anterior.
2. **Criar transição**: estado de origem, estado de destino, símbolo lido, símbolo desempilhado e símbolo empilhado; `-` representa vazio (ε). Os estados de origem e destino precisam existir.
3. **Mostrar função programa**: lista todas as transições no formato `δ(origem, símbolo, desempilha) = (destino, empilha)`.
4. **Verificar palavra**: simula o autômato por busca recursiva com retrocesso (backtracking) e informa se a palavra foi aceita ou rejeitada. A palavra é aceita quando a fita é toda lida, o estado atual é final e a pilha está vazia. Uma transição só desempilha quando o topo é o símbolo esperado, e o retrocesso restaura a pilha. Transições vazias (ε) são consideradas; ciclos de transições vazias são detectados e a simulação tem um limite de 1000 passos. Digite `-` para testar a palavra vazia.
5. **Gerar gráfico**: cria o arquivo `pda_graph.dot` com os estados (finais em círculo duplo, seta de entrada no inicial) e as transições rotuladas como `símbolo,desempilha/empilha`.
6. **Sair**.

Limites definidos no código: até 100 estados, 100 transições, pilha de 100 posições e palavras de até 99 caracteres.

## Tecnologias

- Linguagem C
- Graphviz (opcional, só para transformar o `.dot` gerado em imagem)

## Estrutura

```
├── 3automato.c     # código-fonte do simulador
├── automato.txt    # cópia do código-fonte em .txt (igual a 3automato.c)
├── TF-GAS-ABS.pdf  # relatório do trabalho
└── TF-GAS-ABS.docx # relatório do trabalho (versão editável)
```

## Como compilar e executar

```bash
gcc -Wall -std=c99 3automato.c -o automato
./automato
```

Para visualizar o grafo gerado pela opção 5 (com o Graphviz instalado):

```bash
dot -Tpng pda_graph.dot -o pda_graph.png
```

## Autores

- Guilherme Augusto Santos
- Arthur Bertoli Silva
