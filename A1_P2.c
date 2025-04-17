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
    char mode, input_mode;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Master 👑
    if (rank == 0) {
        printf("Enter input mode (c for console, f for file): ");
        fflush(stdout);
        scanf(" %c", &input_mode);
        printf("Enter 'e' to encrypt or 'd' to decrypt: ");
        fflush(stdout);
        scanf(" %c", &mode);
        shift = (mode == 'e') ? 3 : -3;
        char buffer[1001];
        // read from file
        if (input_mode == 'f') {
            char filename[100];
            printf("Enter file name: ");
            fflush(stdout);
            scanf("%s", filename);
            FILE *fp = fopen(filename, "r");
            if (!fp) {
                printf("Error opening file.\n");
                fflush(stdout);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            fgets(buffer, sizeof(buffer), fp);
            fclose(fp);
        } 
        else {
            printf("Enter string (max 1000): ");
            fflush(stdout);
            scanf("%s", buffer);
        }
        len = strlen(buffer);
        text = buffer;
        int base = len / (size-1), rem = len % (size-1), indx = 0;
        for (int i = 1; i < size; i++) {
            int count = base + (i <= rem ? 1 : 0);
            MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(text + indx, count, MPI_CHAR, i, 0, MPI_COMM_WORLD); // points to the start of sub string
            MPI_Send(&shift, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            indx += count;
        }
        char *result = (char *)malloc(len + 1);
        indx = 0;
        for (int i = 1; i < size; i++) {
            int count = base + (i <= rem ? 1 : 0);
            MPI_Recv(result + indx, count, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            indx += count;
        }
        result[len] = '\0'; // null terminated
        printf("Result: %s\n", result);
        fflush(stdout);
        free(result);
    } 
    // ***** Slaves 👷🏻‍♂️ ****************
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
