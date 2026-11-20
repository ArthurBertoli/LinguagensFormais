#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ESTADOS 100
#define MAX_TRANSICOES 100
#define MAX_PILHA 100
#define MAX_FITA 100
#define MAX_NOME 10
#define MAX_PROFUNDIDADE 1000 // limite de passos da simulação (evita recursão sem fim)

typedef struct {
    char nome[MAX_NOME];
    int inicial;
    int final;
} Estado;

typedef struct {
    Estado *origem;
    Estado *destino;
    char simbolo;
    char desempilha;
    char empilha;
} Transicao;

typedef struct {
    Estado estados[MAX_ESTADOS];
    int numEstados;
    Estado *estadoInicial;
    Estado *estadosFinais[MAX_ESTADOS];
    int numFinais;
    Transicao transicoes[MAX_TRANSICOES];
    int numTransicoes;
    char alfabetoEntrada[MAX_ESTADOS];
    char alfabetoPilha[MAX_ESTADOS];
} Automato;

typedef struct {
    char pilha[MAX_PILHA];
    int topo;
} Pilha;

Automato automato;
Pilha pilha;

// Configurações do caminho atual da simulação, usadas para detectar ciclos de transições vazias
Estado *caminhoEstado[MAX_PROFUNDIDADE + 1];
int caminhoPos[MAX_PROFUNDIDADE + 1];
int caminhoTopo[MAX_PROFUNDIDADE + 1];
char caminhoPilha[MAX_PROFUNDIDADE + 1][MAX_PILHA];
int limiteAtingido = 0;

void inicializaPilha(void) {
    pilha.topo = -1;
}

// Retorna 1 se conseguiu empilhar e 0 se a pilha está cheia
int empilhar(char c) {
    if (pilha.topo < MAX_PILHA - 1) {
        pilha.pilha[++pilha.topo] = c;
        return 1;
    }
    return 0;
}

char desempilhar(void) {
    if (pilha.topo >= 0) {
        return pilha.pilha[pilha.topo--];
    }
    return '\0';
}

Estado *buscaEstado(const char *nome) {
    for (int i = 0; i < automato.numEstados; i++) {
        if (strcmp(automato.estados[i].nome, nome) == 0) {
            return &automato.estados[i];
        }
    }
    return NULL;
}

void CriaEstado(const char *nome, int inicial, int final) {
    if (automato.numEstados >= MAX_ESTADOS) {
        printf("Limite de %d estados atingido.\n", MAX_ESTADOS);
        return;
    }
    if (buscaEstado(nome) != NULL) {
        printf("Já existe um estado chamado %s.\n", nome);
        return;
    }
    if (inicial && automato.estadoInicial != NULL) {
        printf("Aviso: %s substitui %s como estado inicial.\n", nome, automato.estadoInicial->nome);
        automato.estadoInicial->inicial = 0;
    }

    Estado *e = &automato.estados[automato.numEstados++];
    strncpy(e->nome, nome, MAX_NOME - 1);
    e->nome[MAX_NOME - 1] = '\0';
    e->inicial = inicial;
    e->final = final;
    if (inicial) {
        automato.estadoInicial = e;
    }
    if (final) {
        automato.estadosFinais[automato.numFinais++] = e;
    }
}

void CriaTransicao(const char *origem, const char *destino, char simbolo, char desempilha, char empilha) {
    if (automato.numTransicoes >= MAX_TRANSICOES) {
        printf("Limite de %d transições atingido.\n", MAX_TRANSICOES);
        return;
    }

    Transicao t;
    t.origem = buscaEstado(origem);
    t.destino = buscaEstado(destino);
    if (t.origem == NULL || t.destino == NULL) {
        printf("Estado de origem ou destino inexistente. Crie os estados antes da transição.\n");
        return;
    }
    t.simbolo = (simbolo == '-') ? '\0' : simbolo;
    t.desempilha = (desempilha == '-') ? '\0' : desempilha;
    t.empilha = (empilha == '-') ? '\0' : empilha;
    automato.transicoes[automato.numTransicoes++] = t;
}

// Imprime o símbolo ou, se for vazio ('\0'), o texto indicado ("ε" ou "-")
void imprimeSimbolo(FILE *saida, char c, const char *vazio) {
    if (c == '\0') {
        fputs(vazio, saida);
    } else {
        fputc(c, saida);
    }
}

void mostraFuncPrograma(void) {
    printf("Função Programa (Transições):\n");
    for (int i = 0; i < automato.numTransicoes; i++) {
        Transicao *t = &automato.transicoes[i];
        printf("δ(%s, ", t->origem->nome);
        imprimeSimbolo(stdout, t->simbolo, "ε");
        printf(", ");
        imprimeSimbolo(stdout, t->desempilha, "-");
        printf(") = (%s, ", t->destino->nome);
        imprimeSimbolo(stdout, t->empilha, "-");
        printf(")\n");
    }
}

// Verifica se a configuração atual (estado, posição, pilha) já apareceu no caminho
// sem que nenhum símbolo tenha sido lido desde então (ciclo de transições vazias)
int configuracaoRepetida(Estado *estadoAtual, int pos, int profundidade) {
    for (int k = profundidade - 1; k >= 0 && caminhoPos[k] == pos; k--) {
        if (caminhoEstado[k] == estadoAtual && caminhoTopo[k] == pilha.topo &&
            memcmp(caminhoPilha[k], pilha.pilha, (size_t)(pilha.topo + 1)) == 0) {
            return 1;
        }
    }
    return 0;
}

int leFitaRec(Estado *estadoAtual, const char *fita, int pos, int profundidade) {
    if (profundidade > MAX_PROFUNDIDADE) {
        limiteAtingido = 1;
        return 0;
    }
    if (configuracaoRepetida(estadoAtual, pos, profundidade)) {
        return 0;
    }
    caminhoEstado[profundidade] = estadoAtual;
    caminhoPos[profundidade] = pos;
    caminhoTopo[profundidade] = pilha.topo;
    memcpy(caminhoPilha[profundidade], pilha.pilha, (size_t)(pilha.topo + 1));

    if (fita[pos] == '\0') {
        for (int i = 0; i < automato.numFinais; i++) {
            if (automato.estadosFinais[i] == estadoAtual && pilha.topo == -1) {
                return 1; // Aceita
            }
        }
    }

    char simbolo = fita[pos];
    for (int i = 0; i < automato.numTransicoes; i++) {
        Transicao *t = &automato.transicoes[i];
        if (t->origem != estadoAtual) continue;

        int consome = (t->simbolo != '\0');
        if (consome && (simbolo == '\0' || t->simbolo != simbolo)) continue;

        // Só desempilha se o topo for o símbolo esperado (antes desempilhava mesmo sem casar)
        char desempilhado = '\0';
        if (t->desempilha != '\0') {
            if (pilha.topo < 0 || pilha.pilha[pilha.topo] != t->desempilha) continue;
            desempilhado = desempilhar();
        }

        int empilhou = 0;
        if (t->empilha != '\0') {
            if (!empilhar(t->empilha)) { // pilha cheia: desfaz e ignora a transição
                if (desempilhado != '\0') empilhar(desempilhado);
                continue;
            }
            empilhou = 1;
        }

        int novoPos = consome ? pos + 1 : pos;
        if (leFitaRec(t->destino, fita, novoPos, profundidade + 1)) {
            return 1;
        }

        // Retrocesso: desfaz o empilhamento e devolve o símbolo desempilhado
        if (empilhou) {
            desempilhar();
        }
        if (desempilhado != '\0') {
            empilhar(desempilhado);
        }
    }
    return 0;
}

void plot(void) {
    FILE *file = fopen("pda_graph.dot", "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo de plotagem.\n");
        return;
    }

    fprintf(file, "digraph PDA {\n");
    fprintf(file, "  rankdir=LR;\n");  // Direção do gráfico da esquerda para a direita
    fprintf(file, "  node [shape=circle];\n");

    // Estados
    for (int i = 0; i < automato.numEstados; ++i) {
        if (automato.estados[i].inicial) {
            fprintf(file, "  \"%s\" [shape=point];\n", "start");
            fprintf(file, "  \"%s\" -> \"%s\";\n", "start", automato.estados[i].nome);
        }
        fprintf(file, "  \"%s\" [shape=%s];\n", automato.estados[i].nome, automato.estados[i].final ? "doublecircle" : "circle");
    }

    // Transições
    for (int i = 0; i < automato.numTransicoes; ++i) {
        Transicao *t = &automato.transicoes[i];
        fprintf(file, "  \"%s\" -> \"%s\" [label=\"", t->origem->nome, t->destino->nome);
        imprimeSimbolo(file, t->simbolo, "ε");
        fputc(',', file);
        imprimeSimbolo(file, t->desempilha, "-");
        fputc('/', file);
        imprimeSimbolo(file, t->empilha, "-");
        fprintf(file, "\"];\n");
    }

    fprintf(file, "}\n");
    fclose(file);
    printf("Arquivo de plotagem gerado: pda_graph.dot\n");
}

int leFita(const char *fita) {
    if (automato.estadoInicial == NULL) {
        printf("Nenhum estado inicial definido.\n");
        return 0;
    }
    inicializaPilha();
    limiteAtingido = 0;
    int resultado = leFitaRec(automato.estadoInicial, fita, 0, 0);
    if (!resultado && limiteAtingido) {
        printf("Aviso: limite de %d passos atingido durante a simulação.\n", MAX_PROFUNDIDADE);
    }
    return resultado;
}

int aceita(const char *fita) {
    return leFita(fita);
}

// Lê um inteiro; descarta a linha se a entrada for inválida e encerra no fim da entrada
int leInteiro(int *valor) {
    int r = scanf("%d", valor);
    if (r == 1) return 1;
    if (r == EOF) exit(0);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    return 0;
}

void fimDaEntrada(int lidos) {
    if (lidos != 1) exit(0);
}

int main(void) {
    automato.numEstados = 0;
    automato.numFinais = 0;
    automato.numTransicoes = 0;
    automato.estadoInicial = NULL;

    int opcao, inicial, final;
    char nome[MAX_NOME], origem[MAX_NOME], destino[MAX_NOME], simbolo, desempilha, empilha;
    char fita[MAX_FITA];

    while (1) {
        printf("\nMenu:\n");
        printf("1. Criar Estado\n");
        printf("2. Criar Transição\n");
        printf("3. Mostrar Função Programa\n");
        printf("4. Verificar Palavra\n");
        printf("5. Gerar gráfico\n");
        printf("6. Sair\n");
        printf("Escolha uma opção: ");
        if (!leInteiro(&opcao)) {
            printf("Opção inválida!\n");
            continue;
        }

        switch (opcao) {
            case 1:
                printf("Nome do estado: ");
                fimDaEntrada(scanf("%9s", nome));
                printf("É inicial (1 para sim, 0 para não): ");
                if (!leInteiro(&inicial)) inicial = 0;
                printf("É final (1 para sim, 0 para não): ");
                if (!leInteiro(&final)) final = 0;
                CriaEstado(nome, inicial, final);
                break;
            case 2:
                printf("Estado de origem: ");
                fimDaEntrada(scanf("%9s", origem));
                printf("Estado de destino: ");
                fimDaEntrada(scanf("%9s", destino));
                printf("Símbolo de transição (use '-' para vazio): ");
                fimDaEntrada(scanf(" %c", &simbolo));
                printf("Símbolo a desempilhar (use '-' para nenhum): ");
                fimDaEntrada(scanf(" %c", &desempilha));
                printf("Símbolo a empilhar (use '-' para nenhum): ");
                fimDaEntrada(scanf(" %c", &empilha));
                CriaTransicao(origem, destino, simbolo, desempilha, empilha);
                break;
            case 3:
                mostraFuncPrograma();
                break;
            case 4:
                printf("Digite a palavra a ser verificada (use '-' para a palavra vazia): ");
                fimDaEntrada(scanf("%99s", fita));
                if (strcmp(fita, "-") == 0) {
                    fita[0] = '\0';
                }
                if (aceita(fita)) {
                    printf("Palavra aceita.\n");
                } else {
                    printf("Palavra rejeitada.\n");
                }
                break;
            case 5:
                plot();
                break;
            case 6:
                exit(0);
                break;
            default:
                printf("Opção inválida!\n");
        }
    }

    return 0;
}
