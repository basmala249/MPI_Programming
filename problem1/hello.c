#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
int main (int argc,char**argv){
    MPI_Init(&argc,&argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
 if (rank==0){
int s ;
printf ("Hello from master process.\nNumber of slave processes is %d\n\n please enter the size of the array... \n",size-1);
scanf("%d",&s);
printf("please enter the elements of the array\n"); int arr[s];
for(int i =0;i<s;i++){
scanf("%d",&arr[i]);
}int smlSize= s/(size-1);
int max=arr[0];int mxprocess,mxidx,tempidx;
 for(int i=1;i<size;i++){
MPI_Send(&smlSize,1,MPI_INT,i,0,MPI_COMM_WORLD);
MPI_Send(arr+(i-1)*smlSize,smlSize,MPI_INT,i,0,MPI_COMM_WORLD);
int tempmx;
MPI_Recv(&tempmx,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
MPI_Recv(&mxidx,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
if (tempmx>max){
max=tempmx;
mxprocess=i;
tempidx=mxidx;}
printf("Hello from slave#%d Max number in my partition is %d and index is %d\n",i,tempmx,mxidx);
 }
int idx;
idx=(mxprocess*(tempidx+1))-1; 
if(s%(size-1)!=0){
    for(int i = 0;i<s-smlSize*(size-1);i++){
    if (arr[smlSize*(size-1)+i]>max){max=arr[smlSize*(size-1)+i];
idx= smlSize*(size-1)+i;
} 
}}
printf("master process announce the final max which is %d and its index is %d\nthanks for using our program.",max,idx);
}
else if (rank>0) {int s;

MPI_Recv(&s,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
int arr[s];
MPI_Recv(arr,s,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
int max=arr[0];
int mxidx=0; 
for(int i=0;i<s;i++){
if (arr[i]>max){max=arr[i];mxidx=i;}
}
MPI_Send(&max,1,MPI_INT,0,0,MPI_COMM_WORLD);
MPI_Send(&mxidx,1,MPI_INT,0,0,MPI_COMM_WORLD);
}
    MPI_Finalize();
    return 0;
}


