/*
Giovanni dos Santos 13695341
Guilherme Augusto Fincatti da Silva 13676986
Marcelo Eduardo Reginato 13676965
Pedro Guilherme de Barros Zenatte 13676919
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* ler_linha_dinamica() {
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

    char* linha;
    while ((linha = ler_linha_dinamica()) != NULL) {
        matriz_linhas = realloc(matriz_linhas, (quantidade + 1) * sizeof(char*));
        matriz_linhas[quantidade++] = linha;
    }

    

    printf("%c %c", matriz_linhas[0][0], matriz_linhas[1][2]);

    free(matriz_linhas);

    return 0;
}
