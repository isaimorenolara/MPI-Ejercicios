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
        int *a, *b, *c, *sendCounts, *displs;

        crearArreglo(&a, N);
        llenarArreglo(a, N);

        crearArreglo(&b, N);
        llenarArreglo(b, N);

        crearArreglo(&c, N);
        inicializaArreglo(c, N);

        crearArreglo(&sendCounts, numProc);
        crearArreglo(&displs, numProc);

        // Calcula sendCounts y displs para Scatterv y Gatherv
        for (int i = 0; i < numProc; i++)
        {
            sendCounts[i] = i < numProc - 1 ? nDatos : nDatosU;
            displs[i] = i * nDatos;
        }

        MPI_Scatterv(a, sendCounts, displs, MPI_INT, a, sendCounts[idProc], MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(b, sendCounts, displs, MPI_INT, b, sendCounts[idProc], MPI_INT, 0, MPI_COMM_WORLD);

        for (int i = 0; i < sendCounts[idProc]; i++)
        {
            c[i] = a[i] + b[i];
        }

        MPI_Gatherv(c, sendCounts[idProc], MPI_INT, c, sendCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

        imprimirArreglo(a, N);
        printf("\n");
        imprimirArreglo(b, N);
        printf("\n");
        imprimirArreglo(c, N);

        free(a);
        free(b);
        free(c);
        free(sendCounts);
        free(displs);
    }
    else
    {
        int *local_a, *local_b, *local_c;
        int i;

        crearArreglo(&local_a, nDatosU);
        crearArreglo(&local_b, nDatosU);
        crearArreglo(&local_c, nDatosU);

        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, local_a, nDatosU, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, local_b, nDatosU, MPI_INT, 0, MPI_COMM_WORLD);

        for (i = 0; i < nDatosU; i++)
        {
            local_c[i] = local_a[i] + local_b[i];
        }

        MPI_Gatherv(local_c, nDatosU, MPI_INT, NULL, NULL, NULL, MPI_INT, 0, MPI_COMM_WORLD);

        free(local_a);
        free(local_b);
        free(local_c);
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