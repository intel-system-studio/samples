/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright (C) Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#include "Matrix.h"
#include "tbb/atomic.h"
#include <vector>

enum OpKind {
    // Use Cell's value
    OP_VALUE,
    // Unary negation
    OP_NEGATE,
    // Addition
    OP_ADD,
    // Subtraction
    OP_SUB,
    // Multiplication
    OP_MUL
};

static const int ArityOfOp[] = {0,1,2,2,2};

class Cell {
public:
    //! Operation for this cell
    OpKind op;

    //! Inputs to this cell
    Cell* input[2];
   
    //! Type of value stored in a Cell
    typedef Matrix value_type;

    //! Value associated with this Cell
    value_type value;

    //! Set of cells that use this Cell as an input
    std::vector<Cell*> successor;

    //! Reference count of number of inputs that are not yet updated.
    tbb::atomic<int> ref_count;

    //! Update the Cell's value.
    void update();

    //! Default constructor
    Cell() {}
};

//! A directed graph where the vertices are Cells.
class Graph {
    std::vector<Cell> my_vertex_set;
public:
    //! Create a random acyclic directed graph
    void create_random_dag( size_t number_of_nodes );

    //! Print the graph
    void print();

    //! Get set of cells that have no inputs.
    void get_root_set( std::vector<Cell*>& root_set );
};

