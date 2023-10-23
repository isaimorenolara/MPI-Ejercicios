#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

void crearMatriz(int ***matriz, int filas, int columnas);
void llenarMatriz(int **matriz, int filas, int columnas);
void inicializaMatriz(int **matriz, int filas, int columnas);
void imprimirMatriz(int **matriz, int filas, int columnas);
void liberarMatriz(int **matriz, int filas);

int main(int argc, char **argv)
{
    int idProc, numProc;
    int N = 5;
    int M = 5;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &idProc);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);

    int nDatos = N / (numProc - 1);
    int nDatosU = nDatos + N % (numProc - 1);
    int filasPorProceso = N / (numProc - 1);

    if (idProc == 0)
    {
        int **matrizA, **matrizB, **matrizC;
        int iProc, i;

        crearMatriz(&matrizA, N, M);
        llenarMatriz(matrizA, N, M);
        printf("\nMatriz A:\n");
        imprimirMatriz(matrizA, N, M);

        crearMatriz(&matrizB, N, M);
        llenarMatriz(matrizB, N, M);
        printf("\nMatriz B:\n");
        imprimirMatriz(matrizB, N, M);

        crearMatriz(&matrizC, N, M);
        inicializaMatriz(matrizC, N, M);

        for (i = 1; i < numProc; i++)
        {
            int filaInicio = (i - 1) * filasPorProceso;
            int filaFin = (i == numProc - 1) ? N : filaInicio + filasPorProceso;
            int numFilas = filaFin - filaInicio;

            MPI_Send(&(matrizA[filaInicio][0]), numFilas * N, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&(matrizB[0][0]), N * N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int fila = 0; fila < filasPorProceso; fila++)
        {
            for (int col = 0; col < N; col++)
            {
                matrizC[fila][col] = 0;
                for (int k = 0; k < N; k++)
                {
                    matrizC[fila][col] += matrizA[fila][k] * matrizB[k][col];
                }
            }
        }

        for (int i = 1; i < numProc; i++)
        {
            int filaInicio = (i - 1) * filasPorProceso;
            int filaFin = (i == numProc - 1) ? N : filaInicio + filasPorProceso;

            int numFilas = filaFin - filaInicio;

            MPI_Recv(&(matrizC[filaInicio][0]), numFilas * N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        printf("\nMatriz C:\n");
        imprimirMatriz(matrizC, N, M);

        liberarMatriz(matrizA, N);
        liberarMatriz(matrizB, N);
        liberarMatriz(matrizC, N);
    }
    else
    {
        int filaInicio = (idProc - 1) * filasPorProceso;
        int filaFin = (idProc == numProc - 1) ? N : filaInicio + filasPorProceso;

        int numFilas = filaFin - filaInicio;

        int **matrizA = malloc(numFilas * sizeof(int *));
        for (int i = 0; i < numFilas; i++)
        {
            matrizA[i] = malloc(N * sizeof(int));
        }

        MPI_Recv(&(matrizA[0][0]), numFilas * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int **matrizB = malloc(N * sizeof(int *));
        for (int i = 0; i < N; i++)
        {
            matrizB[i] = malloc(N * sizeof(int));
        }

        MPI_Recv(&(matrizB[0][0]), N * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int **matrizC = malloc(numFilas * sizeof(int *));
        for (int i = 0; i < numFilas; i++)
        {
            matrizC[i] = malloc(N * sizeof(int));
        }

        for (int fila = 0; fila < numFilas; fila++)
        {
            for (int col = 0; col < N; col++)
            {
                matrizC[fila][col] = 0;
                for (int k = 0; k < N; k++)
                {
                    matrizC[fila][col] += matrizA[fila][k] * matrizB[k][col];
                }
            }
        }

        MPI_Send(&(matrizC[0][0]), numFilas * N, MPI_INT, 0, 0, MPI_COMM_WORLD);

        liberarMatriz(matrizA, numFilas);
        liberarMatriz(matrizB, N);
        liberarMatriz(matrizC, numFilas);
    }

    MPI_Finalize();

    return 0;
}

void crearMatriz(int ***matriz, int filas, int columnas)
{
    *matriz = (int **)malloc(filas * sizeof(int *));

    if (*matriz == NULL)
    {
        printf("Error: No se pudo reservar memoria para las filas.\n");
        return;
    }

    for (int i = 0; i < filas; i++)
    {
        (*matriz)[i] = (int *)malloc(columnas * sizeof(int));
        if ((*matriz)[i] == NULL)
        {
            printf("Error: No se pudo reservar memoria para las columnas de la fila %d.\n", i);
            return;
        }
    }
}

void llenarMatriz(int **matriz, int filas, int columnas)
{
    int i, j;

    for (i = 0; i < filas; i++)
    {
        for (j = 0; j < columnas; j++)
        {
            matriz[i][j] = 1;
        }
    }
}

void inicializaMatriz(int **matriz, int filas, int columnas)
{
    int i, j;

    for (i = 0; i < filas; i++)
    {
        for (j = 0; j < columnas; j++)
        {
            matriz[i][j] = 0;
        }
    }
}

void imprimirMatriz(int **matriz, int filas, int columnas)
{
    int i, j;

    for (i = 0; i < filas; i++)
    {
        for (j = 0; j < columnas; j++)
        {
            printf("%d\t", matriz[i][j]);
        }
        printf("\n");
    }
}

void liberarMatriz(int **matriz, int filas)
{
    int i;

    for (i = 0; i < filas; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}