/*
    - Materia: Supercómputo
    - Semestre: 2023-2024/I
    - Nombre del alumno: Isai Vicente Moreno Lara
    - Clave del alumno: 324591
    - Carrera: Ing. Sistemas Inteligentes
    - Nombre de tarea o programa: Suma de 2 arreglos
    - Avance logrado (0 a 100%): 100%
*/
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

    if (idProc == 0)
    {
        int *a, *b, *c;
        int iProc, i;
        crearArreglo(&a, N);
        llenarArreglo(a, N);
        printf("Arreglo A: ");
        imprimirArreglo(a, N);
        printf("\n");

        crearArreglo(&b, N);
        llenarArreglo(b, N);
        printf("Arreglo B: ");
        imprimirArreglo(b, N);
        printf("\n");

        crearArreglo(&c, N);
        inicializaArreglo(c, N);

        for (iProc = 1; iProc <= numProc - 2; iProc++)
        {
            MPI_Send(&a[(iProc - 1) * nDatos], nDatos, MPI_INT, iProc, 0, MPI_COMM_WORLD);
            MPI_Send(&b[(iProc - 1) * nDatos], nDatos, MPI_INT, iProc, 0, MPI_COMM_WORLD);
        }

        MPI_Send(&a[(numProc - 2) * nDatos], nDatosU, MPI_INT, numProc - 1, 0, MPI_COMM_WORLD);
        MPI_Send(&b[(numProc - 2) * nDatos], nDatosU, MPI_INT, numProc - 1, 0, MPI_COMM_WORLD);

        for (iProc = 1; iProc <= numProc - 1; iProc++)
        {
            MPI_Recv(&c[(iProc - 1) * nDatos], (iProc == numProc - 1) ? nDatosU : nDatos, MPI_INT, iProc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        printf("Arreglo C: ");
        imprimirArreglo(c, N);
        free(a);
        free(b);
        free(c);
    }
    else
    {
        if (idProc == numProc - 1)
        {
            nDatos = nDatosU;
        }

        int *a, *b, *local_sum;
        int i;
        crearArreglo(&a, nDatos);
        crearArreglo(&b, nDatos);
        crearArreglo(&local_sum, nDatos);
        inicializaArreglo(local_sum, nDatos);

        MPI_Recv(a, nDatos, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(b, nDatos, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (i = 0; i < nDatos; i++)
        {
            local_sum[i] = a[i] + b[i];
        }

        MPI_Send(local_sum, nDatos, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(a);
        free(b);
        free(local_sum);
    }

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