/*
    - Materia: Supercómputo
    - Semestre: 2023-2024/I
    - Nombre del alumno: Isai Vicente Moreno Lara
    - Clave del alumno: 324591
    - Carrera: Ing. Sistemas Inteligentes
    - Nombre de tarea o programa: Suma de elementos de un arreglo
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

int main(int argc, char **argv)
{
    srand(time(NULL));
    int idProc, numProc;
    int N = 15, suma = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &idProc);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);

    int nDatos = N / (numProc - 1);
    int nDatosU = nDatos + N % (numProc - 1);

    if (idProc == 0)
    {
        int *a;
        int iProc;
        crearArreglo(&a, N);
        llenarArreglo(a, N);
        printf("Arreglo: ");
        imprimirArreglo(a, N);

        for (iProc = 1; iProc <= numProc - 2; iProc++)
        {
            MPI_Send(&a[(iProc - 1) * nDatos], nDatos, MPI_INT, iProc, 0, MPI_COMM_WORLD);
        }

        MPI_Send(&a[(numProc - 2) * nDatos], nDatosU, MPI_INT, numProc - 1, 0, MPI_COMM_WORLD);

        for (iProc = 1; iProc <= numProc - 1; iProc++)
        {
            int sumaTemp;
            MPI_Recv(&sumaTemp, 1, MPI_INT, iProc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            suma += sumaTemp;
        }

        printf("\nLa suma total es: %d\n", suma);
        free(a);
    }
    else
    {
        if (idProc == numProc - 1)
        {
            nDatos = nDatosU;
        }

        int *a;
        crearArreglo(&a, nDatos);
        MPI_Recv(a, nDatos, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int i, local_sum = 0;
        for (i = 0; i < nDatos; i++)
        {
            local_sum += a[i];
        }

        MPI_Send(&local_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(a);
    }

    MPI_Finalize();
    return 0;
}

void crearArreglo(int **arr, int tam)
{
    *arr = (int *)malloc(sizeof(int) * tam);
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
        printf("%d ", arr[i]);
    }
}