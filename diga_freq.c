/*
Giovanni dos Santos 13695341
Guilherme Augusto Fincatti da Silva 13676986
Marcelo Eduardo Reginato 13676965
Pedro Guilherme de Barros Zenatte 13676919
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

typedef struct tipo_dado{
    int c; // Código ASCII do caracter
    int freq; // Frequência deste caracter na string
} elemento;

#define T 4

char* ler_linha_dinamica(int *tamanho_linhas) {
    size_t tamanho = 0;
    size_t capacidade = 16;
    char* linha = malloc(capacidade);
    if (!linha) 
        return NULL;

    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (tamanho + 1 >= capacidade) {
            capacidade *= 2;
            linha = realloc(linha, capacidade);
            if (!linha) return NULL;
        }
        linha[tamanho++] = c;
    }

    if (c == EOF && tamanho == 0) {
        free(linha);
        return NULL; // nada foi lido
    }

    linha[tamanho] = '\0'; // finaliza a string
    return linha;
}

int main() {
    char** matriz_linhas = NULL;
    size_t quantidade = 0;
    int contador_linhas = 0;
    int *tamanho_linhas = malloc(sizeof(int));
    int codigo;

    char* linha;
    while ((linha = ler_linha_dinamica(tamanho_linhas)) != NULL) {
        matriz_linhas = realloc(matriz_linhas, (quantidade + 1) * sizeof(char*));
        matriz_linhas[quantidade++] = linha;
        contador_linhas++;
    }

    #pragma omp parallel num_threads(T)
    {
        #pragma omp for
            for(int i = 0; i < contador_linhas; i++){
                for(int j = 0; j < strlen(matriz_linhas[i]) - 1; j++){
                    
                }
            }
    }

    free(matriz_linhas);

    return 0;
}
