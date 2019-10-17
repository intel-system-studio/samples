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

// The code in this file implements a memory pool of Cell structures.
// It serves three purposes:
//   1.) To minimize calls to malloc and free as much as possible
//   2.) To reuse cell structures as much as possible
//   3.) To eliminate unnecessary synchronization for memory allocation

#include <cstdlib>
#include <cassert>
#include <xmmintrin.h>
#ifndef __APPLE__
#include <cstdint>
#endif

#include "cell_pool.h"

/* *** REMINDER ***
 The following asserts were added to the serial program:
 1. assert struct Cell aligned
 2. assert struct Cell and struct Cell_aux same size
*/

// Allocate and initialize a new data block for `cells' number of cells
// Data blocks will have the following format:
//
//   | struct datablockhdr | struct Cell | struct Cell | ..... |
//
// The cells inside the block will be connected to a NULL-terminated linked list
// with the cell at the lowest memory location being the first of its elements.
datablockhdr* cellpool::alloc_block(int cells) {
	// allocate a full block
	assert(cells > 0);
	datablockhdr *block = (datablockhdr *)_mm_malloc(sizeof(datablockhdr) + sizeof(Cell)*cells, c_cacheline_size);

	// initialize header and cells
	block->next = NULL;
	Cell *temp1 = (Cell *)(block+1);
	for(int i=0; i<cells-1; ++i) {
		// If all structures are correctly padded then all pointers should also be correctly aligned,
		// but let's verify that nevertheless because the padding might change.
		assert((uint64_t)(temp1) % sizeof(void *) == 0);
		Cell *temp2 = temp1+1;
		temp1->next = temp2;
		temp1 = temp2;
	}
	// last Cell structure in block
	temp1->next = NULL;

	return block;
}

// Initialize the memory pool
// particles is used to determine the initial capacity and should correspond to the
// number of particles that the pool is expected to manage
void cellpool::initialize(int particles) {
	const int c_alloc_min_cells = 1024;
	assert(sizeof(datablockhdr) % c_cacheline_size == 0);
	assert(particles > 0);

	// Allocate the initial data, let's start with 4 times more cells than best case (ignoring statically allocated Cells structures)
	alloc = 4 * (particles/c_particles_per_cell);
	alloc = alloc < c_alloc_min_cells ? c_alloc_min_cells : alloc;
	datablocks = alloc_block(alloc);
	cells = (Cell *)(datablocks+1);
}

// Destroy the memory pool
cellpool::~cellpool() {
	while(datablocks) {
		datablockhdr *temp = datablocks;
		datablocks = datablocks->next;
		_mm_free(temp);
	}
	datablocks = 0;
}

// Get a Cell structure from the memory pool
Cell *cellpool::get_cell() {
	if(cells == NULL) {
		// double the number of cells
		datablockhdr *block = alloc_block(alloc);
		alloc *= 2;
		block->next = datablocks;
		datablocks = block;
		cells = (Cell *)(datablocks+1);
	}

	// return first cell in list
	Cell *temp = cells;
	cells = temp->next;
	temp->next = NULL;
	return temp;
}

// Return a Cell structure to the memory pool
void cellpool::return_cell(Cell *cell) {
	assert(cell);
	cell->next = cells;
	cells = cell;
}
