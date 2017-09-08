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

#include "tbb/parallel_do.h"
#include <vector>
#include <algorithm>
#include "Graph.h"


class Body {
public:
    Body() {};

    //------------------------------------------------------------------------
    // Following signatures are required by parallel_do
    //------------------------------------------------------------------------
    typedef Cell* argument_type;

    void operator()( Cell* c, tbb::parallel_do_feeder<Cell*>& feeder ) const {
        c->update();
        // Restore ref_count in preparation for subsequent traversal.
        c->ref_count = ArityOfOp[c->op];
        for( size_t k=0; k<c->successor.size(); ++k ) {
            Cell* successor = c->successor[k];
            // ref_count is used for inter-task synchronization.
            // Correctness checking tools might not take this into account, and report
            // data races between different tasks, that are actually synchronized.
            if( 0 == --(successor->ref_count) ) {
                feeder.add( successor );
            }
        }
    }
};

void ParallelPreorderTraversal( const std::vector<Cell*>& root_set ) {
    tbb::parallel_do(root_set.begin(), root_set.end(),Body());
}


