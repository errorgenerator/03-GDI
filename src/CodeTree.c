#include "CodeTree.h"
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
#include<malloc.h>
#include<stdio.h>

static void clear_codetreenode(ctreenode* root)
//Function:             Frees all memory allocated to the tree's 
//                      nodes recursively
{
    if (root != 0)
    {
        if (root->left != 0) clear_codetreenode(root->left);
        if (root->right != 0) clear_codetreenode(root->right);
        free(root);
    }

}
void clear_codetree(codetree* ctree)
//Function:             Frees all memory allocated to the tree's 
//                      nodes recursively
{
    clear_codetreenode(ctree->root);
    ctree->root = 0;
    ctree->nodecount = 0;
    ctree->lfncount = 0;
}

static void print_ctreenode(ctreenode* cnode)
{
    if(cnode == 0) return;
    if(cnode->left == 0 && cnode->right == 0) printf(" %c\n",cnode->val);
    if(cnode->left != 0)
    {
        putchar('0');
        print_ctreenode(cnode->left);
    }
    if(cnode->right != 0)
    {
        putchar('1');
        print_ctreenode(cnode->right);
    }
}

void print_codetree(codetree* ctree)
//Function:             Prints the code tree
{
    print_ctreenode(ctree->root);
}

ctreenode* codetree_insert_next(codetree* ctree, ctreenode* prev, int side, char val)
//Function:             If prev->left or prev->right as specified by side is == 0
//                      then function appends a new node to that side, stores val in it
//                      and returns a pointer to it. 
//
//                      If prev->left or prev->right depending in side is not zero, val is ignored, 
//                      and the corresponding pointer of prev->left or prev->right is returned
//
//Usage:                use this function to sequentially traverse the the tree and insert new nodes
//                      so start at ctree->root on the first call and then feed the return value of 
//                      this function back into itself via the prev argument!
//
//Return value:         if this function returns a null pointer, it means malloc failed, always test for this case!
//                      before calling the function again!
{
    //if prev == ctree->root == 0, aka tree is zero initialized and has no nodes yet
    if(prev == 0)
    {
        ctree->root = (ctreenode*)malloc(sizeof(ctreenode));
        if(ctree->root == 0) return 0; //malloc failed!
        ctree->root->val = val;
        ctree->root->left = 0;
        ctree->root->right = 0;
        ++ctree->nodecount;
        return ctree->root;
    }
    else if(side == 0) // look at the left branch
    {
        if(prev->left != 0) return prev->left;
        else
        {
            //the left branch is 0 so create a new node, store val, initialize its branches and return it
            prev->left = (ctreenode*)malloc(sizeof(ctreenode));
            if(prev->left == 0) return 0; //malloc failed!
            prev->left->left = 0;
            prev->left->right = 0;
            prev->left->val = val;
            ++ctree->nodecount;  
            return prev->left;
        }
    
    }
    else if (side == 1) // look at the right branch
    {
        if (prev->right != 0) return prev->right;
        else
        {
            //the left branch is 0 so create a new node, store val, initialize its branches and return it
            prev->right = (ctreenode*)malloc(sizeof(ctreenode));
            if (prev->right == 0) return 0; //malloc failed!
            prev->right->left = 0;
            prev->right->right = 0;
            prev->right->val = val;
            ++ctree->nodecount;
            return prev->right;
        }

    }
    return 0;   //the value passed for the branching direction is invalid, return null pointer, caller beware!

}
