#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void caesar(char *str, int n, int shift) {
    for (int i = 0; i < n; i++) {
        str[i] = str[i] + shift;
    }
}

int main(int argc, char *argv[]) {
    int rank, size, len, shift;
    char *text = NULL;
    char mode;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) {
        printf("Enter 'e' to encrypt or 'd' to decrypt: ");
        fflush(stdout);
        scanf(" %c", &mode);
        shift = (mode == 'e') ? 3 : -3;
        printf("Enter string (max 100): ");
        fflush(stdout);
        char input[101];
        scanf("%s", input);
        len = strlen(input);
        text = input;
        int base = len / (size - 1), rem = len % (size - 1), start = 0;
        for (int i = 1; i < size; i++) {
            int count = base + (i <= rem ? 1 : 0);
            MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(text + start, count, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            MPI_Send(&shift, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            start += count;
        }
        char *result = (char *)malloc(len + 1);
        start = 0;
        for (int i = 1; i < size; i++) {
            int count = base + (i <= rem ? 1 : 0);
            MPI_Recv(result + start, count, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            start += count;
        }
        result[len] = '\0';
        printf("Result: %s\n", result);
        fflush(stdout);
        free(result);
    } 
    else {
        int count;
        MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        char *sub = (char *)malloc(count);
        MPI_Recv(sub, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&shift, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        caesar(sub, count, shift);
        printf("Hello from slave #%d: processed %d characters.\n", rank, count);
        fflush(stdout);
        MPI_Send(sub, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        free(sub);
    }
    MPI_Finalize();
    return 0;
}
