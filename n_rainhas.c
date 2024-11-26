#include "janela.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Parte I: Função para verificar ameaças (mantida igual)
int n_rainhas(int n, char tab[]) {
    int count_rainhas = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (tab[i * n + j] == 'X') {
                count_rainhas++;
                for (int k = 0; k < n; k++) {
                    if ((k != j && tab[i * n + k] == 'X') || (k != i && tab[k * n + j] == 'X')) {
                        return 1;
                    }
                }
                for (int d = 1; d < n; d++) {
                    if ((i + d < n && j + d < n && tab[(i + d) * n + (j + d)] == 'X') ||
                        (i - d >= 0 && j - d >= 0 && tab[(i - d) * n + (j - d)] == 'X') ||
                        (i + d < n && j - d >= 0 && tab[(i + d) * n + (j - d)] == 'X') ||
                        (i - d >= 0 && j + d < n && tab[(i - d) * n + (j + d)] == 'X')) {
                        return 1;
                    }
                }
            }
        }
    }
    if (count_rainhas < n) return 0;
    return 2;
}

// Parte II: Função para desenhar o tabuleiro
void desenha_tabuleiro(int n, char tab[], int lin, int col) {
    tamanho_t tamanho_janela = {800, 800};
    float tamanho_casa = tamanho_janela.largura / (n + 2);
    ponto_t pos_tabuleiro = {tamanho_casa, tamanho_casa};
    cor_t cor_borda, cor_casa_clara, cor_casa_escura, cor_destaque, cor_rainha;

    int status = n_rainhas(n, tab);
    if (status == 1) cor_borda = (cor_t){1, 0, 0, 1};        // Vermelho
    else if (status == 2) cor_borda = (cor_t){0, 1, 0, 1};   // Verde
    else cor_borda = (cor_t){1, 1, 0, 1};                    // Amarelo

    cor_casa_clara = (cor_t){0.9, 0.9, 0.9, 1};
    cor_casa_escura = (cor_t){0.3, 0.3, 0.3, 1};
    cor_destaque = (cor_t){0, 0, 1, 1}; // Azul
    cor_rainha = (cor_t){1, 0.5, 0, 1}; // Laranja

    // Desenha o contorno do tabuleiro
    j_retangulo((retangulo_t){pos_tabuleiro, {n * tamanho_casa, n * tamanho_casa}}, 4, cor_borda, (cor_t){0, 0, 0, 0});

    // Desenha cada casa do tabuleiro
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ponto_t pos = {pos_tabuleiro.x + j * tamanho_casa, pos_tabuleiro.y + i * tamanho_casa};
            cor_t cor_casa = ((i + j) % 2 == 0) ? cor_casa_clara : cor_casa_escura;

            if (i == lin && j == col) cor_casa = cor_destaque;

            j_retangulo((retangulo_t){pos, {tamanho_casa, tamanho_casa}}, 1, cor_casa, cor_casa);

            if (tab[i * n + j] == 'X') {
                j_circulo((circulo_t){{pos.x + tamanho_casa / 2, pos.y + tamanho_casa / 2}, tamanho_casa / 3}, 0, cor_rainha, cor_rainha);
            }
        }
    }
}

// Parte III: Função para tratar entradas do mouse
bool trata_mouse(int n, char tab[], ponto_t pos_tabuleiro, float tamanho_casa, ponto_t *casa_destacada) {
    rato_t rato = j_rato();

    if (rato.clicado[0]) {
        float x = rato.posicao.x - pos_tabuleiro.x;
        float y = rato.posicao.y - pos_tabuleiro.y;

        int lin = (int)(y / tamanho_casa);
        int col = (int)(x / tamanho_casa);

        if (lin >= 0 && lin < n && col >= 0 && col < n) {
            int index = lin * n + col;
            tab[index] = (tab[index] == 'X') ? ' ' : 'X';
            return false;
        }
    }

    casa_destacada->x = (int)((rato.posicao.x - pos_tabuleiro.x) / tamanho_casa);
    casa_destacada->y = (int)((rato.posicao.y - pos_tabuleiro.y) / tamanho_casa);

    return false;
}

// Função para desenhar o cursor do mouse
void desenha_mouse() {
    rato_t rato = j_rato();  // Obtém o estado atual do mouse

    // Cor do cursor
    cor_t cor_cursor = {1.0, 0.0, 0.0, 1.0}; // Vermelho
    float tamanho_cursor = 5.0; // Raio do cursor

    // Desenha um pequeno círculo na posição do mouse
    j_circulo((circulo_t){{rato.posicao.x, rato.posicao.y}, tamanho_cursor}, 0, cor_cursor, cor_cursor);
}

// Parte IV: Programa principal
int main() {
    int n;
    printf("Digite o tamanho do tabuleiro: ");
    scanf("%d", &n);

    tamanho_t tamanho_janela = {800, 800};
    t_inicializa(tamanho_janela, "N-Rainhas");

    char *tab = malloc(n * n * sizeof(char));
    for (int i = 0; i < n * n; i++) tab[i] = ' ';

    clock_t inicio = clock();
    ponto_t casa_destacada = {-1, -1};

    while (true) {
        desenha_tabuleiro(n, tab, casa_destacada.y, casa_destacada.x);

        if (trata_mouse(n, tab, (ponto_t){tamanho_janela.largura / (n + 2), tamanho_janela.altura / (n + 2)}, tamanho_janela.largura / (n + 2), &casa_destacada)) {
            break;
        }

        desenha_mouse();

        if (n_rainhas(n, tab) == 2) {
            desenha_tabuleiro(n, tab, -1, -1);
            clock_t fim = clock();
            double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

            char texto_tempo[50];
            sprintf(texto_tempo, "Tempo: %.2f segundos", tempo);
            ponto_t pos_msg = {tamanho_janela.largura / 3, tamanho_janela.altura - 50};
            j_texto(pos_msg, 24, (cor_t){0, 1, 0, 1}, "Parabéns! Tabuleiro completo.");
            j_texto((ponto_t){pos_msg.x, pos_msg.y + 30}, 24, (cor_t){1, 1, 1, 1}, texto_tempo);

            j_atualiza();
            j_cochila(5);
            break;
        }

        j_atualiza();
    }

    j_finaliza();
    free(tab);
    return 0;
}
