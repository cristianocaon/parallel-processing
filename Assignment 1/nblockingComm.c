#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define generate_data(i,j)

int main( int argc, char **argv)
{
  int i, j, pid, np, mtag, count;
  double t0, t1;
  int data[100][100], row_sum[100];
  MPI_Status status;
  MPI_Request req_s, req_r;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  if(pid == 0) { // generate data[]
    for(i=0; i<50; i++)
      for(j=0; j<100; j++)
        data[i][j] = i + j * j;
    mtag = 1;

    MPI_Isend(data, 5000, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_s);

    for(i=50; i<100; i++)
      for(j=0; j<100; j++)
        data[i][j] = i + j * j;

    for(i=50; i<100; i++) {
      row_sum[i] = 0;
      for(j=0; j<100; j++)
         row_sum[i] += data[i][j];
    }

    MPI_Wait(&req_s, &status);

    // Receiving the first 25 rows calculated in pid 1
    mtag = 2;
    MPI_Recv(&row_sum[0], 25, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);
    
    // Receiving the other 25 rows calculated in pid 1
    mtag = 3;
    MPI_Recv(&row_sum[25], 25, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);

    for(i=0; i<100; i++) {
       printf(" %d  ", row_sum[i]);
       if(i%5 == 0) printf("\n");
    }
  } else { /*** pid == 1 ***/
    mtag = 1;
    // Receiving data from pid 0
    MPI_Recv(data, 5000, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);

    // Generating first 25 rows
    mtag = 2;
    for(i=0; i<25; i++) {
      row_sum[i] = 0;
      for(j=0; j<100; j++)
        row_sum[i] += data[i][j];
    }

    // Sending first 25 rows with non-blocking Isend method
    MPI_Isend(row_sum, 25, MPI_INT, 0, mtag, MPI_COMM_WORLD, &req_r);

    // Simultaneously with sending the first 25 rows
    // Generating the rest of the other 25 rows
    mtag = 3;
    for(i=25; i<50; i++) {
      row_sum[i] = 0;
      for(j=0; j<100; j++)
        row_sum[i] += data[i][j];
    }

    // Waiting for non-blocking Isend method to finish
    MPI_Wait(&req_r, &status);

    // Send the rest of the 25 rows
    MPI_Send(row_sum, 25, MPI_INT, 0, mtag, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 1;
} /****************** End of function main() ********************/
