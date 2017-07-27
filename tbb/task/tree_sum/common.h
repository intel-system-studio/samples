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

typedef float Value;

struct TreeNode {
    //! Pointer to left subtree
    TreeNode* left; 
    //! Pointer to right subtree
    TreeNode* right;
    //! Number of nodes in this subtree, including this node.
    long node_count;
    //! Value associated with the node.
    Value value;
};

Value SerialSumTree( TreeNode* root );
Value SimpleParallelSumTree( TreeNode* root );
Value OptimizedParallelSumTree( TreeNode* root );
