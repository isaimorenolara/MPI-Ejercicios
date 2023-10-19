#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

void crearArreglo(int **a, int n);
void llenarArreglo(int *a, int n);
void imprimirArreglo(int *a, int n);
void inicializaArreglo(int *a, int n);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int idProc, numProc;
    int N = 15;

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
        printf("Array A: ");
        imprimirArreglo(a, N);
        printf("\n");

        crearArreglo(&b, N);
        llenarArreglo(b, N);
        printf("Array B: ");
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
            int *partial_sum = (int *)malloc(nDatosU * sizeof(int));
            MPI_Recv(partial_sum, nDatosU, MPI_INT, iProc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (i = 0; i < (iProc == numProc - 1 ? nDatosU : nDatos); i++)
            {
                c[(iProc - 1) * nDatos + i] = partial_sum[i];
            }

            free(partial_sum);
        }

        free(a);
        free(b);
        printf("Array C: ");
        imprimirArreglo(c, N);
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
            // printf("local_sum[%d] = %d\n", i, local_sum[i]);
        }

        MPI_Send(local_sum, nDatos, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(a);
        free(b);
        free(local_sum);
    }

    MPI_Finalize();
    return 0;
}

void crearArreglo(int **a, int n)
{
    *a = (int *)malloc(n * sizeof(int));
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
        printf("%d \t", a[i]);
    }
}

void inicializaArreglo(int *a, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        a[i] = 0;
    }
}