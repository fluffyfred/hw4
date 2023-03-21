#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root)
{
    // Add your code below
    // We are essentially checking if the left and the right subtrees of the root have the same height
    // If they have the same height then return true, otherwise return false
    // But what if the root only has one child node? Do we return true or false? I think true, because we only have one path from the leaf node to the root
    if(root == NULL) {
			return true;
		}
		// else if( (root->left == NULL && root->right != NULL) || (root->left != NULL && root->right == NULL) ) {
    //     return true;
    // }
		else if( (root->left == NULL && root->right != NULL && root->right->left == NULL && root->right->right == NULL) || (root->left != NULL && root->right == NULL && root->left->left == NULL && root->left->right == NULL) ) {
			return true;
		}
    else {
        return height(root->left) == height(root->right);
    }
}

int height(Node* node)
{
    // pre-order traversal
    if (node == NULL) {
        return 0;
    }
    int left_height = height(node->left);
    int right_height = height(node->right);
    if(left_height > right_height) {
        return 1 + left_height;
    }
    else {
        return 1 + right_height;
    }
}
