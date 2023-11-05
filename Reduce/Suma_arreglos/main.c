#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

void crearArreglo(int **arr, int tam);
void llenarArreglo(int *arr, int tam);
void imprimirArreglo(int *arr, int tam);
void inicializaArreglo(int *arr, int tam);

int main(int argc, char **argv)
{
    srand(time(NULL));
    int idProc, numProc;
    int N = 10;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &idProc);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);

    int nDatos = N / (numProc - 1);
    int nDatosU = nDatos + N % (numProc - 1);

    int *a, *b;

    crearArreglo(&a, N);
    llenarArreglo(a, N);
    imprimirArreglo(a, N);
    printf("\n");

    crearArreglo(&b, N);
    inicializaArreglo(b, N);

    MPI_Reduce(a, b, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (idProc == 0)
    {
        printf("\n\n");
        imprimirArreglo(b, N);
    }

    free(a);
    free(b);

    MPI_Finalize();
    return 0;
}

void crearArreglo(int **arr, int tam)
{
    *arr = (int *)malloc(tam * sizeof(int));
}

void llenarArreglo(int *arr, int tam)
{
    int i;
    for (i = 0; i < tam; i++)
    {
        arr[i] = 1 + rand() % 5;
    }
}

void imprimirArreglo(int *arr, int tam)
{
    int i;
    for (i = 0; i < tam; i++)
    {
        printf("%d \t", arr[i]);
    }
}

void inicializaArreglo(int *arr, int tam)
{
    int i;
    for (i = 0; i < tam; i++)
    {
        arr[i] = 0;
    }
}