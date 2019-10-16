//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================
// Original code written by Christian Bienia

// The code in this file defines the interface for a memory pool of Cell structures.
// It serves three purposes:
//   1.) To minimize calls to malloc and free as much as possible
//   2.) To reuse cell structures as much as possible
//   3.) To eliminate unnecessary synchronization for memory allocation

#ifndef CELL_POOLAN_H
#define CELL_POOLAN_H

#include "fluid_animateAN.h"

//A simple list of data blocks headers, required for internal memory management of pool
//NOTE: Do not add additional member variables or the padding might be wrong.
struct datablockhdr {
  struct datablockhdr *next;
  //NOTE: This form of padding will break if additional variables are added to the structure
  //because it does not account for compiler-inserted padding between structure members.
  char padding[c_cacheline_size - sizeof(datablockhdr *) % c_cacheline_size];
};

//The memory pool data structure
//The allocated memory is accessible twice: As a linked list of cells and also as a linked
//list of data blocks. The data blocks preserve the original block structure as returned
//by malloc, the cell list breaks the blocks down into smaller cell structures which can
//be used by the program as needed.
struct cellpoolAN {
  //linked list of available cells
  CellAN *cells;
  //number of cells allocated so far (NOT number of cells currently available in pool)
  int alloc;
  //linked list of allocated data blocks (required for free operation)
  datablockhdr *datablocks;

  //Initialize the memory pool
  //particles is used to determine the initial capacity and should correspond to the
  //number of particles that the pool is expected to manage
  cellpoolAN() {};

  //Destroy the memory pool
  ~cellpoolAN();

  // Initialize a certain number of cells based on particles
  void initialize(int particles);

  //Get a Cell structure from the memory pool
  CellAN *get_cell();

  //Return a Cell structure to the memory pool
  void return_cell(CellAN *cell);

private:
	datablockhdr *alloc_block(int cells);
};

#endif // CELL_POOLAN_H
