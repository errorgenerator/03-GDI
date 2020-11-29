#ifndef CODE_TREE_H
#define CODE_TREE_H
/*      GDI Chapter 3: Encoding exercise
*       29 - 11 - 2020
*       ...where are you dum dum.....
*/

/*
*       4. CodeTree.h CodeTree.c -> a binary tree that holds the symbols of a code table in its leaf nodes
*                                   it has functions to sequentially traverse from one level to the next
*                                   based on the juction value 0 or 1, 0 means selecting a left branch,
*                                   1 means selecting a right branch
*
*
*/
#include <inttypes.h>

typedef struct ctreenode ctreenode;
struct ctreenode
{
    char val;           //if your encoded symbols are not single characters, this needs to be a string
    ctreenode* left;
    ctreenode* right;

};

static inline ctreenode def_ctreenode()
//Funtion:              default initializer of a tree node
{
    ctreenode a;
    a.val = 0;
    a.left = 0;
    a.right = 0;
    return a;
}

typedef struct codetree
{
    size_t nodecount;   //total number of nodes
    size_t lfncount;    //number of leaf nodes
    ctreenode* root;    //root of the tree
}codetree;

static inline codetree def_codetree()
//Function:             default initializer for a tree
{
    codetree a;
    a.nodecount = 0;
    a.lfncount = 0;
    a.root = 0;
    return a;
}

void clear_codetree(codetree* ctree);
//Function:             Frees all memory allocated to the tree's 
//                      nodes recursively

ctreenode* codetree_insert_next(codetree* ctree, ctreenode* prev, int side, char val);
//Function:             If prev->left or prev->right as specified by side is == 0
//                      then function appends a new node to that side, stores val in it
//                      and returns a pointer to it. 
//
//                      If prev->left or prev->right depending in side is not zero, val is ignored, 
//                      and the corresponding pointer of prev->left or prev->right is returned
//
//Usage:                use this function to sequentially traverse the the tree and insert new nodes

void print_codetree(codetree* ctree);
//Function:             Prints the code tree


























#endif