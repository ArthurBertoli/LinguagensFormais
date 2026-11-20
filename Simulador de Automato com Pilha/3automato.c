#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ESTADOS 100
#define MAX_TRANSICOES 100
#define MAX_PILHA 100
#define MAX_FITA 100

typedef struct {
    char nome[10];
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

void inicializaPilha() {
    pilha.topo = -1;
}

void empilhar(char c) {
    if (pilha.topo < MAX_PILHA - 1) {
        pilha.pilha[++pilha.topo] = c;
    }
}

char desempilhar() {
    if (pilha.topo >= 0) {
        return pilha.pilha[pilha.topo--];
    }
    return '\0';
}

void CriaEstado(char *nome, int inicial, int final) {
    Estado e;
    strcpy(e.nome, nome);
    e.inicial = inicial;
    e.final = final;
    automato.estados[automato.numEstados++] = e;
    if (inicial) {
        automato.estadoInicial = &automato.estados[automato.numEstados - 1];
    }
    if (final) {
        automato.estadosFinais[automato.numFinais++] = &automato.estados[automato.numEstados - 1];
    }
}

void CriaTransicao(char *origem, char *destino, char simbolo, char desempilha, char empilha) {
    Transicao t;
    for (int i = 0; i < automato.numEstados; i++) {
        if (strcmp(automato.estados[i].nome, origem) == 0) {
            t.origem = &automato.estados[i];
        }
        if (strcmp(automato.estados[i].nome, destino) == 0) {
            t.destino = &automato.estados[i];
        }
    }
    t.simbolo = (simbolo == '-') ? '\0' : simbolo;
    t.desempilha = (desempilha == '-') ? '\0' : desempilha;
    t.empilha = (empilha == '-') ? '\0' : empilha;
    automato.transicoes[automato.numTransicoes++] = t;
}

void mostraFuncPrograma() {
    printf("Função Programa (Transições):\n");
    for (int i = 0; i < automato.numTransicoes; i++) {
        printf("δ(%s, %c, %c) = (%s, %c)\n", automato.transicoes[i].origem->nome,
               (automato.transicoes[i].simbolo == '\0') ? 'ε' : automato.transicoes[i].simbolo,
               (automato.transicoes[i].desempilha == '\0') ? '-' : automato.transicoes[i].desempilha,
               automato.transicoes[i].destino->nome,
               (automato.transicoes[i].empilha == '\0') ? '-' : automato.transicoes[i].empilha);
    }
}

int leFitaRec(Estado *estadoAtual, char *fita, int pos) {
    if (pos == strlen(fita)) {
        for (int i = 0; i < automato.numFinais; i++) {
            if (automato.estadosFinais[i] == estadoAtual && pilha.topo == -1) {
                return 1; // Aceita
            }
        }
    }

    char simbolo = fita[pos];
    for (int i = 0; i < automato.numTransicoes; i++) {
        Transicao t = automato.transicoes[i];
        if (t.origem == estadoAtual && (t.simbolo == simbolo || t.simbolo == '\0')) {
            if (t.desempilha == '\0' || desempilhar() == t.desempilha) {
                if (t.empilha != '\0') {
                    empilhar(t.empilha);
                }
                int novoPos = t.simbolo == '\0' ? pos : pos + 1;
                if (leFitaRec(t.destino, fita, novoPos)) {
                    return 1;
                }
                if (t.empilha != '\0') {
                    desempilhar(); // Reverte empilhamento se a transição não levou à aceitação
                }
            }
        }
    }
    return 0;
}

void plot() {
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
        Transicao t = automato.transicoes[i];
        fprintf(file, "  \"%s\" -> \"%s\" [label=\"%c,%c/%c\"];\n",
                t.origem->nome,
                t.destino->nome,
                t.simbolo ? t.simbolo : 'ε',
                t.desempilha ? t.desempilha : '-',
                t.empilha ? t.empilha : '-');
    }

    fprintf(file, "}\n");
    fclose(file);
    printf("Arquivo de plotagem gerado: pda_graph.dot\n");
}

int leFita(char *fita) {
    inicializaPilha();
    return leFitaRec(automato.estadoInicial, fita, 0);
}

int aceita(char *fita) {
    return leFita(fita);
}

int main() {
    automato.numEstados = 0;
    automato.numFinais = 0;
    automato.numTransicoes = 0;

    int opcao, inicial, final;
    char nome[10], origem[10], destino[10], simbolo, desempilha, empilha;
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
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Nome do estado: ");
                scanf("%s", nome);
                printf("É inicial (1 para sim, 0 para não): ");
                scanf("%d", &inicial);
                printf("É final (1 para sim, 0 para não): ");
                scanf("%d", &final);
                CriaEstado(nome, inicial, final);
                break;
            case 2:
                printf("Estado de origem: ");
                scanf("%s", origem);
                printf("Estado de destino: ");
                scanf("%s", destino);
                printf("Símbolo de transição (use '-' para vazio): ");
                scanf(" %c", &simbolo);
                printf("Símbolo a desempilhar (use '-' para nenhum): ");
                scanf(" %c", &desempilha);
                if (desempilha == '-') desempilha = '\0';
                printf("Símbolo a empilhar (use '-' para nenhum): ");
                scanf(" %c", &empilha);
                if (empilha == '-') empilha = '\0';
                CriaTransicao(origem, destino, simbolo, desempilha, empilha);
                break;
            case 3:
                mostraFuncPrograma();
                break;
            case 4:
                printf("Digite a palavra a ser verificada: ");
                scanf("%s", fita);
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