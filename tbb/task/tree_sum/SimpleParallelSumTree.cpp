/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright 2016 Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#include "common.h"
#include "tbb/task.h"

class SimpleSumTask: public tbb::task {
    Value* const sum;
    TreeNode* root;
public:
    SimpleSumTask( TreeNode* root_, Value* sum_ ) : root(root_), sum(sum_) {}
    task* execute() /*override*/ {
        if( root->node_count<1000 ) {
            *sum = SerialSumTree(root);
        } else {
            Value x, y;
            int count = 1; 
            tbb::task_list list;
            if( root->left ) {
                ++count;
                list.push_back( *new( allocate_child() ) SimpleSumTask(root->left,&x) );
            }
            if( root->right ) {
                ++count;
                list.push_back( *new( allocate_child() ) SimpleSumTask(root->right,&y) );
            }
            // Argument to set_ref_count is one more than size of the list,
            // because spawn_and_wait_for_all expects an augmented ref_count.
            set_ref_count(count);
            spawn_and_wait_for_all(list);
            *sum = root->value;
            if( root->left ) *sum += x;
            if( root->right ) *sum += y;
        }
        return NULL;
    }
};

Value SimpleParallelSumTree( TreeNode* root ) {
    Value sum;
    SimpleSumTask& a = *new(tbb::task::allocate_root()) SimpleSumTask(root,&sum);
    tbb::task::spawn_root_and_wait(a);
    return sum;
}

