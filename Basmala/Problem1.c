#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;
    int sz;
    MPI_Comm_size(MPI_COMM_WORLD, &sz);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank == 0){
      printf("Hello from master process.\n");
      printf("Number of slave processes is %d \n\n" , sz - 1);
      
      if(sz == 1){
        printf("No Available slave processes, Re-Run.\n");
        return 0;
      }
      printf("Please enter size of array...\n");
      int size ;
      scanf("%d" , &size);
      printf("\nPlease enter array elements...\n");
      int arr[size] , i , a;
      for(i = 0 ; i < size ; i++){
        scanf("%d" , &a);
        arr[i] = a;
      }
      int num_of_elemet_per_process = size / (sz - 1);
      int remindar = size % (sz - 1);
      int start = 0;
      int cnt;
      for(i = 1 ; i < sz ; ++i){
         MPI_Send(&size , 1 , MPI_INT , i , 0 , MPI_COMM_WORLD);
         cnt = num_of_elemet_per_process + (i <= remindar ? 1 : 0);
         MPI_Send( &arr[start] , cnt , MPI_INT , i , 0 , MPI_COMM_WORLD);
         start += num_of_elemet_per_process + (i <= remindar ? 1 : 0);
      }
      
      /* Recieve Portion  */
      int mx = INT_MIN , index = -1 ;
      start = 0 ;
      for(i = 1 ; i < sz ; ++i){
         int indx1 ;
         MPI_Recv(&indx1 , 1, MPI_INT , i , 0 , MPI_COMM_WORLD , 0);
         if(arr[start + indx1] > mx){
            mx = arr[start + indx1];
            index = start + indx1;
         }
         start += num_of_elemet_per_process + (i <= remindar ? 1 : 0);
      }
      printf("Master process announce the final max which is %d and its index is %d .\n\n" , mx , index);
      printf("Thanks for using our program.\n");
    }
    else {
       int arrSize ;
       MPI_Recv(&arrSize, 1, MPI_INT , 0 , 0 , MPI_COMM_WORLD , 0);
       int Size = (arrSize / (sz - 1)) + (rank <= (arrSize % (sz - 1)) ? 1 : 0);
       int buffer[Size];
       MPI_Recv(buffer , Size , MPI_INT , 0 , 0 , MPI_COMM_WORLD , 0);
       int mx = INT_MIN , indx = -1 , i;
       for(i = 0 ; i < Size; ++i){
          if(buffer[i] > mx){
            mx = buffer[i] ;
            indx = i;
          }     
       }
       printf("Hello from slave# %d Max number in my partition is %d and index is %d .\n" , rank , mx , indx );
       MPI_Send(&indx , 1, MPI_INT , 0 , 0 , MPI_COMM_WORLD);
    }
    
    
    
    
    MPI_Finalize();
    return 0;
}
