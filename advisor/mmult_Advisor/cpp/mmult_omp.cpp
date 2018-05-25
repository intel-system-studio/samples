//=========================================================================
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

//  Simple minded matrix multiply
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>

using namespace std;

//routine to initialize an array with data
void init_array(double row, double col, double off, int arrSize, double **array)
{
  int i,j;

  for (i=0; i<arrSize; i++) {
    for (j=0; j<arrSize; j++) {
      array[i][j] = row*i+col*j+off;
    }
  }
}


// routine to print out contents of small arrays
void print_array(char * name, int arrSize, double **array)
{
  int i,j;
	
  cout << endl << name << endl;
  for (i=0;i<arrSize;i++){
    for (j=0;j<arrSize;j++) {
      cout << "\t" << array[i][j];
    }
    cout << endl;
  }
}

// matrix multiply routine
void multiply_d(int arrSize, double **aMatrix, double **bMatrix, double **product)
{
#pragma omp parallel for schedule(dynamic)
  for(int i=0;i<arrSize;i++) {
    for(int j=0;j<arrSize;j++) {
      double sum = 0;
      for(int k=0;k<arrSize;k++) {
        sum += aMatrix[i][k] * bMatrix[k][j];
      }
      product[i][j] = sum;
    }
  }
}

int main(int argc, char*argv[])
{
  int num=0;

  if(argc !=2) {
    cerr << "Usage: 3_mmult_omp[_debug] arraySize [default is 1024].\n";
    num = 1024;
  } else {
    num = atoi(argv[1]);
    if (num < 2) {
      cerr << "Array dimensions must be greater than 1; setting it to 2. \n" << endl;
      num = 2;
    }
    if (num > 9000) {
      cerr << "Array dimensions must not be greater than 9000; setting it to 9000. \n" << endl;
      num = 9000;
    }
  }

  double** aMatrix = new double*[num];
  double** bMatrix = new double*[num];
  double** product = new double*[num];

  for (int i=0; i<num; i++) {
    aMatrix[i] = new double[num];
    bMatrix[i] = new double[num];
    product[i] = new double[num];
  }

// initialize the arrays with different data
  init_array(3,-2,1,num,aMatrix);
  init_array(-2,1,3,num,bMatrix);

// start timing the matrix multiply code
  cout << "Size: " << num <<" X " << num << endl;

  struct timeval startTime, endTime;
  gettimeofday(&startTime, NULL);
  multiply_d(num, aMatrix, bMatrix, product);
// stop timing the matrix multiply code
  gettimeofday(&endTime, NULL);

// print simple test case of data to be sure multiplication is correct
  if (num < 6) {
    print_array((char*)("aMatrix"), num, aMatrix);
    print_array((char*)("bMatrix"), num, bMatrix);
    print_array((char*)("product"), num, product);
  }

// print elapsed time
  double useconds = (endTime.tv_sec*1000000 + endTime.tv_usec) - (startTime.tv_sec*1000000 + startTime.tv_usec);
  double seconds = useconds/1000000;
  cout << endl << "Calculations took " << seconds << " sec.\n";

// cleanup
  for (int i=0; i<num; i++) {
    delete [] aMatrix[i];
    delete [] bMatrix[i];
    delete [] product[i];
  }

  delete [] aMatrix;
  delete [] bMatrix;
  delete [] product;

  return 0;
}
