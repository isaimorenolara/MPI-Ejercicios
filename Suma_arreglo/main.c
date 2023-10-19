#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

void crearArreglo(int **a, int n);
void llenarArreglo(int *a, int n);
void imprimirArreglo(int *a, int n);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int idProc, numProc;
    int N = 15;
    int suma = 0;

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
        printf("Array: ");
        imprimirArreglo(a, N);

        for (iProc = 1; iProc <= numProc - 2; iProc++)
        {
            MPI_Send(&a[(iProc - 1) * nDatos], nDatos, MPI_INT, iProc, 0, MPI_COMM_WORLD);
        }

        MPI_Send(&a[(numProc - 2) * nDatos], nDatosU, MPI_INT, numProc - 1, 0, MPI_COMM_WORLD);
        free(a);

        for (iProc = 1; iProc <= numProc - 1; iProc++)
        {
            int sumap;
            MPI_Recv(&sumap, 1, MPI_INT, iProc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            suma += sumap;
        }

        printf("\nLa suma total es: %d\n", suma);
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

void crearArreglo(int **a, int n)
{
    *a = (int *)malloc(sizeof(int) * n);
}

void llenarArreglo(int *a, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        // a[i] = i + 1;
        a[i] = 1 + rand() % 10;
    }
}

void imprimirArreglo(int *a, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("%d ", a[i]);
    }
}