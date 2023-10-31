/*
    - Materia: Supercómputo
    - Semestre: 2023-2024/I
    - Nombre del alumno: Isai Vicente Moreno Lara
    - Clave del alumno: 324591
    - Carrera: Ing. Sistemas Inteligentes
    - Nombre de tarea o programa: Impresión de arreglo con bcast
    - Avance logrado (0 a 100%): 100%
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

void crearArreglo(float **arr, int tam);
void llenarArreglo(float *arr, int tam);
void imprimirArreglo(float *arr, int tam);

int main(int argc, char **argv)
{
    srand(time(NULL));
    int idProc;
    int N = 10;
    float *a;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &idProc);

    crearArreglo(&a, N);

    if (idProc == 0)
    {
        llenarArreglo(a, N);
    }

    MPI_Bcast(a, N, MPI_FLOAT, 0, MPI_COMM_WORLD);
    imprimirArreglo(a, N);
    printf("\n");
    free(a);

    MPI_Finalize();
    return 0;
}

void crearArreglo(float **arr, int tam)
{
    *arr = (float *)malloc(tam * sizeof(float));
}

void llenarArreglo(float *arr, int tam)
{
    int i;
    for (i = 0; i < tam; i++)
    {
        arr[i] = 1.0 + (float)rand() / RAND_MAX * 5.0;
    }
}

void imprimirArreglo(float *arr, int tam)
{
    int i;
    for (i = 0; i < tam; i++)
    {
        printf("%0.3f \t", arr[i]);
    }
}