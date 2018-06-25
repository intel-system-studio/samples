//=======================================================================

// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF END-USER LICENSE AGREEMENT FOR
// Intel® Advisor 2017.

// /* Copyright (C) 2010-2017 Intel Corporation. All Rights Reserved.
 
 // The source code, information and material ("Material") 
 // contained herein is owned by Intel Corporation or its 
 // suppliers or licensors, and title to such Material remains 
 // with Intel Corporation or its suppliers or licensors.
 // The Material contains proprietary information of Intel or 
 // its suppliers and licensors. The Material is protected by 
 // worldwide copyright laws and treaty provisions.
 // No part of the Material may be used, copied, reproduced, 
 // modified, published, uploaded, posted, transmitted, distributed 
 // or disclosed in any way without Intel's prior express written 
 // permission. No license under any patent, copyright or other
 // intellectual property rights in the Material is granted to or 
 // conferred upon you, either expressly, by implication, inducement, 
 // estoppel or otherwise. Any license under such intellectual 
 // property rights must be express and approved by Intel in writing.
 // Third Party trademarks are the property of their respective owners.
 // Unless otherwise agreed by Intel in writing, you may not remove 
 // or alter this notice or any other notice embedded in Materials 
 // by Intel or Intel's suppliers or licensors in any way.
 
// ========================================================================

#include "omp.h"
#include "mpi.h"
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>

using namespace std;

int  MAX_ARRAY_SIZE;
float prev_root = 0.0;

int main (int argc, char *argv[])
{ 
  if(argc !=2) {
    cerr << "Usage: default is 1000000.\n";
                MAX_ARRAY_SIZE = 1000000;
  } 
  else {
    MAX_ARRAY_SIZE = atoi(argv[1]);
    if (MAX_ARRAY_SIZE < 1) {
      cerr << "Boardsize should be bigger than 0. Setting to 1000000 \n" << endl;
      MAX_ARRAY_SIZE = 1000000;
    }
  }



  int i, namelen, rank, root_process = 0, size;
  char name[MPI_MAX_PROCESSOR_NAME];
  float *a;
  float *b;
  float *c;
  float *d;
  a = new float[MAX_ARRAY_SIZE];
  b = new float[MAX_ARRAY_SIZE];
  c = new float[MAX_ARRAY_SIZE];

  MPI_Status stat; 

  MPI_Init (&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(name, &namelen); 

  /* The root process will allocated array storage for gathering results from each of 
 the processes */

  if (rank == root_process) {
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    d = (float *) malloc(size * MAX_ARRAY_SIZE * sizeof(float));
  } 

#pragma omp parallel for
  for (i = 0; i < MAX_ARRAY_SIZE; i++){
     a[i] = sqrt(1 + rank);
     b[i] = sqrt(2 + rank);
     c[i] = sqrt(a[i] / b[i]);
     #pragma omp critical
     {
     c[i] += prev_root;
     prev_root=sqrt(c[i]);
     }
  }
                               	
  fprintf(stdout,"Process rank %d of %d running on %s ready to call MPI_Gather\n",
        rank,size,name); 

  /* Use the MPI Gather communication collective to gather the partial results */
  MPI_Gather(c, MAX_ARRAY_SIZE, MPI_INT, d, MAX_ARRAY_SIZE, MPI_INT, root_process, MPI_COMM_WORLD); 

  /* Print out the first and last result elements that were computed by each MPI process */
  if (rank == root_process) {
    for (i = 0; i < size; i++)
        fprintf(stdout,"Strided array elements d[%d] = %f; d[%d] = %f\n",i*MAX_ARRAY_SIZE,
                d[i*MAX_ARRAY_SIZE],i*MAX_ARRAY_SIZE+MAX_ARRAY_SIZE-1,
                d[i*MAX_ARRAY_SIZE+MAX_ARRAY_SIZE-1]);
  } 

  MPI_Finalize(); 


  return (0); 

}
