#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x)<(y)?(x):(y))

double* gen_matrix(int n, int m);
int mmult(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols);
void compare_matrix(double *a, double *b, int nRows, int nCols);

/** 
    Program to multiply a matrix times a matrix using both
    mpi to distribute the computation among nodes and omp
    to distribute the computation among threads.
*/

int main(int argc, char* argv[])
{
  int nrows, ncols;
  double *aa;	/* the A matrix */
  double *bb;	/* the B matrix */
  double *cc1;	/* A x B computed using the omp-mpi code you write */
  double *cc2;	/* A x B computed using the conventional algorithm */
  
  int myid, numprocs;
  double starttime, endtime;
  MPI_Status status;
  
  int numSent;
  int sender;
  int myID;
  int numProcesses;


  /* insert other global variables here */
  double *buff;
  double *result;
  int ansType, currentRow;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  if (argc > 1) {
    nrows = atoi(argv[1]);
    ncols = nrows;
    
    /* Input matrix row and col dimensions. Input 3 args since a cols needs to be
    equal to b rows */
    a_r = atoi(argv[1]);
    a_c = atoi(argv[2]);
    b_r = a_c;
    b_c = atoi(argv[3]);

    printf("The matrix dim: %d by %d and %d by $d \n", a_r, a_c, b,r, b_c);

    aa = gen_matrix(a_r, a_c);
    bb = gen_matrix(b_r, b_c);

    printf("\nPrinting Matrix A ....\n");
    for(int r = 0; r < a_r; r++){
      for(int c = 0; c < a_c; c++){
        printf("%.1 ", aa[i* a_c + c]);
      }
      printf("\n");
    }

    printf("Printing Matrix B ....\n");
    for(int r = 0; r < b_r; r++){
      for(int c = 0; c < b_c; c++){
        printf("%.1 ", bb[i * b_c + j]);    
      }
      printf("\n");
    }


    /* Resulting matrix dimensions */
    nrows = a_r;
    ncols = b_c;

    /* create buffer */
    buff = (double *)malloc(sizeof(double)*b_r);


    /* Parent check */
    if (myid == 0) {
      // Master Code goes here
      //aa = gen_matrix(nrows, ncols);
      //bb = gen_matrix(ncols, nrows);
      cc1 = malloc(sizeof(double) * nrows * nrows); 
      numSent = 0;
      starttime = MPI_Wtime();

      /* Insert your master code here to store the product into cc1 */
      MPI_Bcast(bb, b_r * b_c, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      
      for(int i = 0; i < min(numProcesses - 1, nrows); i++){
        for(int j = 0; j < b_r; j++){
          buff[j] = aa[i * a_c + j];
        }
        int k = i + 1;
        MPI_Send(bff, b_r, MPI_DOUBLE, k, k, MPI_COMM_WORLD);
        numSent++;
      } 

     double received = (double *)malloc(sizeof(double)*ncols);

      for (int i = 0; i < nrows; i++) {
        MPI_Recv(received, ncols, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        sender = status.MPI_SOURCE;
        ansType = status.MPI_TAG;

        for (int c = 0; c < ncols; c++){
          cc1[(ansType - 1) *ncols + c] = received[j];
        }

        if(numSent < ncols) {
          for(int c = 0; c < a_col; c++){
            buff[j] = aa[numSent * a_c + j];
          }
      
          MPI_Send(buff, a_c, MPI_DOUBLE, sender, numSent + 1, MPI_COMM_WORLD);
          numSent++;
        } else {
          MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
        }
      }

      endtime = MPI_Wtime();
      printf("%f\n",(endtime - starttime));
      cc2  = malloc(sizeof(double) * nrows * cols);
      mmult(cc2, aa, a_r, a_c, bb, b_r, b_c);
      compare_matrices(cc2, cc1, nrows, nrows);
    } else {
      // Slave Code goes here aka worker code
      
      bb = (double *)malloc(sizeof(double)*b_r*b_c);
      MPI_Bcast(bb, a_r*b_c, MPI_DOUBLE, 0, MPI_COMM_WORLD);

      if(myID < nrows + 1) {
        while(1){
          MPI_Recv(buff, b_r, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          
          /* get new row */
          c_r = status.MPI_TAG;

          /* loop exit condition */
          if (c_r == 0) break;
     
          result = (double *)malloc(sizeof(double)*ncols);

          for(int i = 0; i < ncols; i++){
            for(int j = 0; j < a_c; j++){
              result[i] += buff[j] * bb[j * ncols + 1];
            }
          } 

          MPI_Send(result, ncols, MPI_DOUBLE, 0, c_r, MPI_COMM_WORLD);
        }
      }
    }
  } else {
    fprintf(stderr, "Usage matrix_times_vector <size>\n");
  }
  MPI_Finalize();
  return 0;
}
