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

#include "common.h"
#include "tbb/task.h"

class OptimizedSumTask: public tbb::task {
    Value* const sum;
    TreeNode* root;
    bool is_continuation;
    Value x, y;
public:
    OptimizedSumTask( TreeNode* root_, Value* sum_ ) : root(root_), sum(sum_), is_continuation(false) {
    }
    tbb::task* execute() /*override*/ {
        tbb::task* next = NULL;
        if( !is_continuation ) {
            if( root->node_count<1000 ) {
                *sum = SerialSumTree(root);
            } else {
                // Create tasks before spawning any of them.
                tbb::task* a = NULL;
                tbb::task* b = NULL;
                if( root->left )
                    a = new( allocate_child() ) OptimizedSumTask(root->left,&x);
                if( root->right )
                    b = new( allocate_child() ) OptimizedSumTask(root->right,&y);
                recycle_as_continuation();
                is_continuation = true;
                set_ref_count( (a!=NULL)+(b!=NULL) );
                if( a ) {
                    if( b ) spawn(*b);
                } else 
                    a = b;
                next = a;
            }
        } else {
            *sum = root->value;
            if( root->left ) *sum += x;
            if( root->right ) *sum += y;
        } 
        return next;
    }
};

Value OptimizedParallelSumTree( TreeNode* root ) {
    Value sum;
    OptimizedSumTask& a = *new(tbb::task::allocate_root()) OptimizedSumTask(root,&sum);
    tbb::task::spawn_root_and_wait(a);
    return sum;
}

