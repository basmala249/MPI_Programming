#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int rank, size, n, *array = NULL;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    if (rank == 0) {
        printf("Hello from master process.\n");
        printf("Number of slave processes is %d\n", size - 1);
        printf("Please enter size of array\n");
        scanf("%d", &n);
        array = (int*)malloc(n * sizeof(int));
        printf("Please enter array elements\n");
        for (int i = 0; i < n; i++)  scanf("%d", &array[i]);
        int base = n / (size - 1), rem = n % (size - 1), start = 0;
        for (int i = 1; i < size; i++) {
            int count = base + (i <= rem ? 1 : 0);
            MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(array + start, count, MPI_INT, i, 0, MPI_COMM_WORLD);
            start += count;
        }
        int global_max = array[0], global_index = 0, max_val, idx, offset = 0;
        start = 0;
        for (int i = 1; i < size; i++) {
            int count = base + (i <= rem ? 1 : 0);
            MPI_Recv(&max_val, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&idx, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (max_val > global_max) {
                global_max = max_val;
                global_index = start + idx;
            }
            start += count;
        }
        printf("Master process announce the final max which is %d and its index is %d.\n", global_max, global_index);
        free(array);
    } 
    else {
        int count;
        MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int* sub_array = (int*)malloc(count * sizeof(int));
        MPI_Recv(sub_array, count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int max = sub_array[0], index = 0;
        for (int i = 1; i < count; i++) {
            if (sub_array[i] > max) {
                max = sub_array[i];
                index = i;
            }
        }
        printf("Hello from slave#%d Max number in my partition is %d and index is %d.\n", rank, max, index);
        MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(sub_array);
    }
    MPI_Finalize();
    return 0;
}
