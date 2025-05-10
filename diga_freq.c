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

#define T 4
#define MAX_ASCII 126

typedef struct tipo_dado{
    int c; // Código ASCII do caracter
    int freq; // Frequência deste caracter na string
} elemento;

void merge(elemento *vetor, int inicio, int meio, int fim){
    int tamanho_esq = meio - inicio + 1;
    int tamanho_dir = fim - meio;

    elemento *esq = malloc(sizeof(elemento) * tamanho_esq);
    elemento *dir = malloc(sizeof(elemento) * tamanho_dir);

    for(int i = 0; i < tamanho_esq; i++)
        esq[i] = vetor[inicio + i];

    for(int i = 0; i < tamanho_dir; i++)
        dir[i] = vetor[meio + 1 + i];

    int i = 0, j = 0, k = inicio;
    while(i < tamanho_esq && j < tamanho_dir){
        if (esq[i].freq <= dir[j].freq){
            vetor[k++] = esq[i++];
        } 
        else{
            vetor[k++] = dir[j++];
        }
    }

    while(i < tamanho_esq){
        vetor[k++] = esq[i++];
    }

    while(j < tamanho_dir){
        vetor[k++] = dir[j++];
    }

    free(esq);
    free(dir);
}

void merge_sort_paralelo(elemento *vetor, int inicio, int fim, int profundidade_maxima){
    if(inicio >= fim)
        return;

    int meio = inicio + (fim - inicio) / 2;

    if(profundidade_maxima > 0){
        #pragma omp task
        merge_sort_paralelo(vetor, inicio, meio, profundidade_maxima - 1);

        #pragma omp task
        merge_sort_paralelo(vetor, meio + 1, fim, profundidade_maxima - 1);

        #pragma omp taskwait
    } 
    else{
        merge_sort_paralelo(vetor, inicio, meio, 0);
        merge_sort_paralelo(vetor, meio + 1, fim, 0);
    }

    merge(vetor, inicio, meio, fim);
}

char* ler_linha_dinamica(int *tamanho_linhas){
    size_t tamanho = 0;
    size_t capacidade = 16;
    char* linha = malloc(capacidade);
    if(!linha) 
        return NULL;

    int c;
    while((c = getchar()) != EOF && c != '\n'){
        if (tamanho + 1 >= capacidade) {
            capacidade *= 2;
            linha = realloc(linha, capacidade);
            if (!linha) return NULL;
        }
        linha[tamanho++] = c;
    }

    if(c == EOF && tamanho == 0){
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
    
    /* -------------------------- Leitura da entrada -------------------------------- */
    char* linha;
    while((linha = ler_linha_dinamica(tamanho_linhas)) != NULL){
        matriz_linhas = realloc(matriz_linhas, (quantidade + 1) * sizeof(char*));
        matriz_linhas[quantidade++] = linha;
        contador_linhas++;
    }

    /* -------------------- Alocacao da matriz de frequencias ------------------------ */

    int **matriz_freq = malloc(sizeof(int*) * contador_linhas);
    if(matriz_freq == NULL){
        printf("Erro de alocacao de memoria para a matriz de frequencias.\n");
        return 1;
    }

    for(int i = 0; i < contador_linhas; i++){
        matriz_freq[i] = calloc(MAX_ASCII, sizeof(int));
        if(matriz_freq[i] == NULL){
            printf("Erro de alocacao de memoria para a matriz de frequencias.\n");
            return 1;
        }
    }

    /* --------------------------- Inicio da regiao paralela ----------------------------- */

    double wtime = omp_get_wtime();

    #pragma omp parallel num_threads(T)
    {
        /* Cada thread cria sua matriz local */
        int **local_freq = malloc(sizeof(int*) * contador_linhas);
        for(int i = 0; i < contador_linhas; i++) {
            local_freq[i] = calloc(MAX_ASCII, sizeof(int));
        }

        /* Soma 1 na frequencia de cada caracter */
        #pragma omp for nowait
        for(int i = 0; i < contador_linhas; i++){
            for(int j = 0; j < strlen(matriz_linhas[i]) - 1; j++){
                unsigned char c = matriz_linhas[i][j];
                if (c < MAX_ASCII)
                    local_freq[i][c]++;
            }
        }

        /* Combina os resultados locais na matriz principal */
        #pragma omp for
        for(int i = 0; i < contador_linhas; i++){
            #pragma omp simd
            for (int j = 0; j < MAX_ASCII; j++){
                matriz_freq[i][j] += local_freq[i][j];
            }
        }

        // libera cópias locais
        for(int i = 0; i < contador_linhas; i++){
            free(local_freq[i]);
        }
        free(local_freq);
    }

    /* Ordenação dos vetores de frequêcia através do merge sort */

    elemento **vetores_ordenados = malloc(sizeof(elemento *) * contador_linhas);
    
    for(int i = 0; i < contador_linhas; i++){
        vetores_ordenados[i] = malloc(sizeof(elemento) * MAX_ASCII);
        for (int j = 0; j < MAX_ASCII; j++){
            vetores_ordenados[i][j].c = j;
            vetores_ordenados[i][j].freq = matriz_freq[i][j];
        }
    }

    #pragma omp parallel
    {
        #pragma omp single
        {
            for(int i = 0; i < contador_linhas; i++){
                #pragma omp task firstprivate(i)
                {
                    merge_sort_paralelo(vetores_ordenados[i], 0, MAX_ASCII - 1, 4); // 4 = profundidade de paralelismo
                }
            }
        }
    }

    wtime = omp_get_wtime() - wtime;

    /* ---------------------------- Fim da regiao paralela --------------------------------------------- */

    // Impressão da saída de forma sequencial
    for(int i = 0; i < contador_linhas; i++) {
        for(int j = 0; j < MAX_ASCII; j++) {
            if(vetores_ordenados[i][j].freq > 0) {
                printf("%d %d\n", vetores_ordenados[i][j].c, vetores_ordenados[i][j].freq);
            }
        }
        printf("\n");
    }

    /* Desalocação das matrizes utilizadas */
    printf("Tempo total: %lf\n", wtime);

    for(int i = 0; i < contador_linhas; i++){
        free(matriz_linhas[i]);
    }

    free(matriz_linhas);

    for(int i = 0; i < contador_linhas; i++){
        free(matriz_freq[i]);
    }

    free(matriz_freq);

    return 0;
}
