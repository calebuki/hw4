#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

//helper function, get's the depth of a node
int depth(Node* root){
  if (root == nullptr){
    return 0;
  }
  int leftDepth = depth(root->left);
  int rightDepth = depth(root->right);
  if (leftDepth > rightDepth){
    return 1 + leftDepth;
  } else {
    return 1 + rightDepth;
  }
}

bool equalPaths(Node * root)
{
    // Add your code below

    //empty tree is true
    if (root == nullptr){
      return true;
    }


    //if it's a leaf Node, true in itself
    if (root->left == nullptr && root->right == nullptr){
      return true;
    }

    //if only one child exists:
    if (root->left == nullptr){
      return equalPaths(root->right);
    }
    if (root->right == nullptr){
      return equalPaths(root->left);
    }

    //if both children exist, go left then right:
    int leftDepth = depth(root->left);
    int rightDepth = depth(root->right);

    //if everything returns true, return true lol
    if (leftDepth == rightDepth && equalPaths(root->left) && equalPaths(root->right)){
      return true;
    } else {
      return false;
    }

}

