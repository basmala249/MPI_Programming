#include <mpi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#define charSize 26
char temp[] = {
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z'
};

int getIndex(int a, int b){
  return (((((a % charSize) + (b % charSize)) % charSize) + charSize) % charSize);
}
int FindChar(char c , int val){
  int i ;
  for(i = 0 ; i < charSize ; ++i){
     if(temp[i] == c){
        return getIndex(i , val);    
     }
  }
  return 0;
}

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
      
      int Choice;
      printf("Read From File Enter 1 ; From Console Enter 2...\n");
      scanf("%d" , &Choice);
      
      int size = 0 , Num;
      int capacity = 100;
      char *sentence = malloc(capacity * sizeof(sentence));
      if(Choice == 1){// From File
          FILE *file;
          char ch;
          char filename[100];
          printf("Enter the file name: ");
          fflush(stdout);
          scanf("%s", filename);
          file = fopen(filename , "r");

          if (file == NULL) {
              printf("Error: Could not open file.\n");
              return 1;
          }
          bool found = false;
          int cnt = 0;
          while ((ch = fgetc(file)) != EOF) {
                 if(ch == '$')found = true;
                 if(ch == '\n')continue;
                 if(!found){
                    if (size >= capacity - 1) {
                      capacity *= 2;
                      char *temp = realloc(sentence, capacity);
                      sentence = temp;
                      free(temp);
                    }
                    sentence[size++] = ch;
                }
                else {
                  cnt++;
                  if(cnt == 9){
                     if(ch == 'E' || ch == 'e')Num = 1;
                     else Num = 2;
                     break;
                  }
                  
                }
            }
            sentence[size] = '\0';

          fclose(file);
      }
      else if(Choice == 2){// From Console
          // Reading String Of Unknown Size From User
          getchar();
          printf("Enter text...\n");
          char ch;
          while ((ch = getchar()) != '\n' && ch != EOF) {
              if (size >= capacity - 1) {
                  capacity *= 2;
                  char *temp = realloc(sentence, capacity);
                  sentence = temp;
              }
              sentence[size++] = ch;
          }
          sentence[size] = '\0';

          printf("For Encryption Enter 1 ; For Decryption Enter 2...\n");
          scanf("%d", &Num);
      }
      
      /*  Processing Part */
      
      int num_of_chars_per_processor = size / (sz - 1);
      int remainder = size % (sz - 1) , i, start = 0 , tot;
      // Concatenation of Strings 
      char result[size] ; 
      memset(result, 0, size);
      for(i = 1 ; i < sz ; ++i){
         tot = num_of_chars_per_processor + (i <= remainder ? 1 : 0);
         MPI_Send(&Num , 1 , MPI_INT , i  , 0 , MPI_COMM_WORLD);
         MPI_Send(&size , 1 , MPI_INT , i  , 0 , MPI_COMM_WORLD);
         MPI_Send(&sentence[start] , tot , MPI_CHAR , i , 0 , MPI_COMM_WORLD);
         char buffer[tot + 1];
         MPI_Recv(buffer, tot, MPI_CHAR, i, 0, MPI_COMM_WORLD, 0);
         buffer[tot] = '\0';
         strcat(result, buffer);
         start += tot;
      }
      
      char res[] = "Encoded";
      if(Num == 2) {
         res[0] = 'D';
         res[1] = 'e';
      }

      printf("Master Process announce the Final %s sentence is %s ." , res , result);
      printf("\n\nThanks for using our program...\n");
      
      free(sentence);
    }
    else {
         int arrSize , i , En_De;
         MPI_Recv(&En_De , 1 , MPI_INT , i  , 0 , MPI_COMM_WORLD , 0);
         
         if(En_De == 2)En_De = -1;
         En_De *= 3;
         
         MPI_Recv(&arrSize , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD , 0);
         int num_of_chars_of_string = (arrSize / (sz - 1)) + (rank <= (arrSize % (sz - 1)) ? 1 : 0);
         
         char buffer[num_of_chars_of_string] , Text[num_of_chars_of_string + 1];
         MPI_Recv(buffer , num_of_chars_of_string , MPI_CHAR , 0 , 0 , MPI_COMM_WORLD , 0);
         
         for(i = 0 ; i < num_of_chars_of_string ; ++i){
            if(buffer[i] == '\n'){
               Text[i] = buffer[i];
               continue;
            }
            Text[i] = temp[FindChar(buffer[i] , En_De)];
         }
         Text[num_of_chars_of_string] = '\0';
         printf("Hello from slave# %d the final sentence %s \n" , rank , Text);
         fflush(stdout);
         MPI_Send(Text, num_of_chars_of_string , MPI_CHAR , 0 , 0 , MPI_COMM_WORLD); 
    }
    
    
    
    MPI_Finalize();
    return 0;
}
