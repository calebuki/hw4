#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    void removeFix(AVLNode<Key,Value>* parent, int diff);
    void rotateLeft(AVLNode<Key,Value>* g);
    void rotateRight(AVLNode<Key,Value>* g);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    Key key = new_item.first;
    Value val = new_item.second;
    
    //if empty tree -> new node becomes root
    if (this->root_ == nullptr){
        this->root_ = new AVLNode<Key,Value>(key, val, nullptr);
        return;
    }

    //otherwise insert n normally (like BST)
    AVLNode<Key,Value> *curr = static_cast<AVLNode<Key,Value>*>(this->root_);
    AVLNode<Key,Value> *parent = nullptr;

    while (curr != nullptr){
        parent = curr;
        if (key < curr->getKey()){
            curr = curr->getLeft();
        } else if (key > curr->getKey()){
            curr = curr->getRight();
        } else {
            curr->setValue(val);
            return;
        }
    }

    //put in new node
    AVLNode<Key,Value> *node = new AVLNode<Key,Value>(key, val, parent);

    //look at parent
    //if b(p) == -1, then b(p) = 0; DONE
    //if b(p) == 1, then b(p) = 0; DONE
    //if b(p) == 0, then update b(p) and insert-fix(parent, node)

    //if key is left child
    int oldB = parent->getBalance();
    if (key < parent->getKey()){
        parent->setLeft(node);

        if (oldB == 0){
            parent->setBalance(-1);
            insertFix(parent, node);

        } else if (oldB == 1){
            parent->setBalance(0);

        //oldB == -1
        } else {
            parent->setBalance(0);
        }
    //if key is right child
    } else {
        parent->setRight(node);

        if (oldB == 0){
            parent->setBalance(1);
            insertFix(parent, node);

        } else if (oldB == -1){
            parent->setBalance(0);

        //oldB == 1
        } else {
            parent->setBalance(0);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    
    //first find the node
    AVLNode<Key,Value> *node = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));

    //not found:
    if (node == nullptr){
        return;
    }

    //if node has 2 children, swap with predecessor
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {

        //get predecessor + swap
        AVLNode<Key,Value> *pred = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key,Value>::predecessor(node));
        nodeSwap(node, pred);
    }

    //theres max 1 child (find it):
    AVLNode<Key,Value> *child = nullptr;
    if (node->getLeft() != nullptr){
        child = node->getLeft();
    } else {
        child = node->getRight();
    }

    //connect the parent to child
    AVLNode<Key,Value> *parent = node->getParent();
    int diff = 0;

    if (child != nullptr){
        child->setParent(parent);
    }
    if (parent == nullptr){
        this->root_ = child;
    }
    else if (parent->getLeft() == node){
        parent->setLeft(child);
        diff = 1;
    }
    else{
        parent->setRight(child);
        diff = -1; //redundant but visibility
    }

    delete node;
    
    if (parent != nullptr){
        removeFix(parent, diff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

//adopted from slides
template<class Key, class Value>
void AVLTree<Key,Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node){

    if (parent == nullptr){
        return;
    }
    
    AVLNode<Key,Value>* g = parent->getParent();
    if (g == nullptr){
        return;
    }

    //parent is LEFT child of g
    if (parent == g->getLeft()){
        g->updateBalance(-1);

        //case 1: b(g) == 0:
        if (g->getBalance() == 0){
            return;
        }

        //case 2: b(g) == -1, recurse:
        else if (g->getBalance() == -1){
            insertFix(g, parent);
            return;
        }

        //case 3: b(g) == -2, rotate
        else if (g->getBalance() == -2){
            
            //zig-zig (parent is left of g and n is left of p)
            if (node == parent->getLeft()){
                rotateRight(g);
                g->setBalance(0);
                parent->setBalance(0);
            }

            //zig-zag (node is right child of parent)
            else{
                rotateLeft(parent);
                rotateRight(g);

                //balance case a:
                if (node->getBalance() == -1){
                    parent->setBalance(0);
                    g->setBalance(1);
                //case b:
                } else if (node->getBalance() == 0){
                    parent->setBalance(0);
                    g->setBalance(0);
                //case c:
                } else {
                    parent->setBalance(-1);
                    g->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    
    //parent is RIGHT child of g
    } else {
        g->updateBalance(1);

        //case 1: b(g) == 0:
        if (g->getBalance() == 0){
            return;
        }

        //case 2: b(g) == 1, recurse:
        else if (g->getBalance() == 1){
            insertFix(g, parent);
            return;
        }

        //case 3: b(g) == 2, rotate
        else if (g->getBalance() == 2){
            
            //zig-zig
            if (node == parent->getRight()){
                rotateLeft(g);
                g->setBalance(0);
                parent->setBalance(0);
            }

            //zig-zag
            else{
                rotateRight(parent);
                rotateLeft(g);

                //balance case a:
                if (node->getBalance() == 1){
                    parent->setBalance(0);
                    g->setBalance(-1);
                //case b:
                } else if (node->getBalance() == -1){
                    parent->setBalance(1);
                    g->setBalance(0);
                //case c:
                } else {
                    parent->setBalance(0);
                    g->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key,Value>::removeFix(AVLNode<Key,Value>* parent, int diff){

    if (parent == nullptr){
        return;
    }

    parent->updateBalance(diff);
    AVLNode<Key,Value>* g = parent->getParent();

    //need to remember which side of g the subtree is on, before rotating
    bool parentIsLeftChild = false;
    if (g != nullptr && parent == g->getLeft()){
        parentIsLeftChild = true;
    }

    //LEFT removal
    if (diff == 1){
        
        //case a: b(p) + diff = 1
        if (parent->getBalance() == 1){
            return;
        }

        //case b: b(p) + diff = 0
        else if (parent->getBalance() == 0){
            if (g != nullptr){
                if (parentIsLeftChild){
                    removeFix(g, 1);
                } else {
                    removeFix(g, -1);
                }
            }
        }

        //case c: b(p) + diff = 2 (rotate)
        else if (parent->getBalance() == 2){
            AVLNode<Key,Value>* r = parent->getRight();

            //zig-zig:
            if (r->getBalance() >= 0){
                rotateLeft(parent);

                if (r->getBalance() == 0){
                    parent->setBalance(1);
                    r->setBalance(-1);
                    return;
                
                //r->getBalance() == 1
                } else {
                    parent->setBalance(0);
                    r->setBalance(0);

                    if (g != nullptr){
                        if (parentIsLeftChild){
                            removeFix(g, 1);
                        } else {
                            removeFix(g, -1);
                        }
                    }
                }
            }

            //zig ZAG
            else {
                AVLNode<Key,Value>* rl = r->getLeft();
                rotateRight(r);
                rotateLeft(parent);

                if (rl->getBalance() == 1){
                    parent->setBalance(-1);
                    r->setBalance(0);
                }

                else if (rl->getBalance() == 0){
                    parent->setBalance(0);
                    r->setBalance(0);
                }

                // -1
                else {
                    parent->setBalance(0);
                    r->setBalance(1);
                }
                
                rl->setBalance(0);
                
                if(g != nullptr){

                    if (parentIsLeftChild){
                        removeFix(g, 1);
                    } else {
                        removeFix(g, -1);
                    }
                }
            }
        }
    }


    //RIGHT removal (same thing p much), but switch +1 to -1 and vice-versa

    else if (diff == -1){
        
        //case a: b(p) + diff = 1
        if (parent->getBalance() == -1){
            return;
        }

        //case b: b(p) + diff = 0
        else if (parent->getBalance() == 0){
            if (g != nullptr){
                if (parentIsLeftChild){
                    removeFix(g, 1);
                } else {
                    removeFix(g, -1);
                }
            }
        }

        //case c:
        else if (parent->getBalance() == -2){
            AVLNode<Key,Value>* l = parent->getLeft();

            //zig-zig:
            if (l->getBalance() <= 0){
                rotateRight(parent);

                if (l->getBalance() == 0){
                    parent->setBalance(-1);
                    l->setBalance(1);
                    return;
                
                //r->getBalance() == -1
                } else {
                    parent->setBalance(0);
                    l->setBalance(0);

                    if (g != nullptr){
                        if (parentIsLeftChild){
                            removeFix(g, 1);
                        } else {
                            removeFix(g, -1);
                        }
                    }
                }
            }

            //zig ZAG
            else {
                AVLNode<Key,Value>* lr = l->getRight();
                rotateLeft(l);
                rotateRight(parent);

                if (lr->getBalance() == -1){
                    parent->setBalance(1);
                    l->setBalance(0);
                }

                else if (lr->getBalance() == 0){
                    parent->setBalance(0);
                    l->setBalance(0);
                }

                // 1
                else {
                    parent->setBalance(0);
                    l->setBalance(-1);
                }
                
                lr->setBalance(0);
                
                if(g != nullptr){

                    if (parentIsLeftChild){
                        removeFix(g, 1);
                    } else {
                        removeFix(g, -1);
                    }
                }
            }
        }
    }
}

//ROTATE LEFT helper function
template<class Key, class Value>
void AVLTree<Key,Value>::rotateLeft(AVLNode<Key,Value>* g){

    //base cases
    if (g == nullptr){
        return;
    }
    AVLNode<Key,Value>* parent = g->getRight();
    if (parent == nullptr){
        return;
    }

    AVLNode<Key,Value>* oldParent = g->getParent();

    //move parent's left subtree to g's right
    g->setRight(parent->getLeft());

    if (parent->getLeft() != nullptr){
        parent->getLeft()->setParent(g);
    }

    //parent becomes the parent to g
    parent->setLeft(g);
    g->setParent(parent);

    //connect parent to g's old parent
    parent->setParent(oldParent);
    
    if (oldParent != nullptr){

        if (oldParent->getLeft() == g){
            oldParent->setLeft(parent);
        } else {
            oldParent->setRight(parent);
        }
    } else {
        //g was the root, so now parent is the root
        this->root_ = parent;
    }
}

//ROTATE RIGHT helper function
template<class Key, class Value>
void AVLTree<Key,Value>::rotateRight(AVLNode<Key,Value>* g){

    //base cases
    if (g == nullptr){
        return;
    }
    AVLNode<Key,Value>* parent = g->getLeft();
    if (parent == nullptr){
        return;
    }

    AVLNode<Key,Value>* oldParent = g->getParent();

    //move parent's left subtree to g's right
    g->setLeft(parent->getRight());

    if (parent->getRight() != nullptr){
        parent->getRight()->setParent(g);
    }

    //parent becomes the parent to g
    parent->setRight(g);
    g->setParent(parent);

    //connect parent to g's old parent
    parent->setParent(oldParent);
    
    if (oldParent != nullptr){

        if (oldParent->getLeft() == g){
            oldParent->setLeft(parent);
        } else {
            oldParent->setRight(parent);
        }
    } else {
        //g was the root, so now parent is the root
        this->root_ = parent;
    }
}

#endif
