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
    void insert_fix(AVLNode<Key, Value>* node_parent, AVLNode<Key, Value>* node);
		void remove_fix(AVLNode<Key, Value>* n, int diff);
    void rotateLeft(AVLNode<Key, Value>* input);
    void rotateRight(AVLNode<Key, Value>* input);
		// AVLNode<Key, Value>* AVLFind(const Key& k) const; // TODO
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == NULL) {
        // empty tree
        AVLNode<Key, Value>* inserted_node = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        this->root_ = inserted_node;
        return;
    }
    if(this->internalFind(new_item.first) != NULL) {
        // node already in the tree
        this->internalFind(new_item.first)->setValue(new_item.second);
        return;
    }
    // same structure as BST insertion 
    AVLNode<Key, Value>* temp = (AVLNode<Key, Value>*) (this->root_);
    while(temp != NULL) {
        if(temp->getKey() < new_item.first) {
            if(temp->getRight() == NULL) {
                AVLNode<Key, Value>* inserted_node = new AVLNode<Key, Value>(new_item.first, new_item.second, temp);
                temp->setRight(inserted_node);
								if(temp->getBalance() == 1 || temp->getBalance() == -1) {
										temp->setBalance(0);
										return;
								}
								else {
										temp->updateBalance(1); 
										this->insert_fix(temp, inserted_node);
										return;
								}
                return; 
            }
            temp = temp->getRight();
        }
        else { /* temp->getKey() > new_item.first */
            if(temp->getLeft() == NULL) {
                AVLNode<Key, Value>* inserted_node = new AVLNode<Key, Value>(new_item.first, new_item.second, temp);
                temp->setLeft(inserted_node);
								if(temp->getBalance() == 1 || temp->getBalance() == -1) {
										temp->setBalance(0);
										return;
								}
								else {
										temp->updateBalance(-1);
										this->insert_fix(temp, inserted_node);
										return;
								}
                return;
            }
            temp = temp->getLeft();
        }
    }
    // AVL Insert Pseudocodes
    // if(temp->getBalance() == 1 || temp->getBalance() == -1) {
    //     temp->setBalance(0);
    //     return;
    // }
    // else {
    //     temp->setBalance(get_height(temp->getRight()) - get_height(temp->getLeft()));
    //     this->insert_fix(temp, inserted_node);
    //     return;
    // }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
		AVLNode<Key, Value>* temp = (AVLNode<Key, Value>*) (this->internalFind(key));
		int diff = 0;
		if(temp == NULL) {
			return;
		}
		if(temp->getLeft() != NULL && temp->getRight() != NULL) {
			nodeSwap( temp, (AVLNode<Key, Value>*) (this->predecessor(temp)) );
		}
		AVLNode<Key, Value>* p = temp->getParent();
		if(p != NULL) {
			if(p->getLeft() == temp) {
				diff = 1;
			}
			else {
				diff = -1;
			}
		}

		// child_node = NULL;

		// if n's left and right child are both null:
		// 	child node is null
		// else if n's left child is not null
		// 	child node is n's left node
		// else if n's right child is not null
		// 	child node is n's right child


		// if(p == NULL) {

		// }
		// else {
			if(temp->getLeft() == NULL && temp->getRight() == NULL) {
			// if the node to be removed has no children then just delete it
			if(temp->getParent() == NULL) {
				delete temp;
				this->root_ = NULL;
			}
			else if(temp->getParent()->getLeft() == temp) {
				temp->getParent()->setLeft(NULL);
				delete temp;
			}
			else {
				temp->getParent()->setRight(NULL);
				delete temp;
			}
		}
		else if(temp->getLeft() == NULL && temp->getRight() != NULL) {
			// if the node to be removed only has a right child
			if(temp->getParent() == NULL) {
				this->root_ = temp->getRight();
				temp->getRight()->setParent(NULL);
				delete temp;
			}
			else if(temp->getParent()->getLeft() == temp) {
				temp->getRight()->setParent(temp->getParent());
				temp->getParent()->setLeft(temp->getRight());
				delete temp;
			}
			else {
				temp->getRight()->setParent(temp->getParent());
				temp->getParent()->setRight(temp->getRight());
				delete temp;
			}
		}
		else /*if(temp->getLeft() != NULL && temp->getRight() == NULL)*/ {
			// if the node to be removed only has a left child
			if(temp->getParent() == NULL) {
				this->root_ = temp->getLeft();
				temp->getLeft()->setParent(NULL);
				delete temp;
			}
			else if(temp->getParent()->getLeft() == temp) {
				temp->getLeft()->setParent(temp->getParent());
				temp->getParent()->setLeft(temp->getLeft());
				delete temp;
			}
			else {
				temp->getLeft()->setParent(temp->getParent());
				temp->getParent()->setRight(temp->getLeft());
				delete temp;
			}
		}
		remove_fix(p, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insert_fix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if(p == NULL || p->getParent() == NULL) {
        return;
    }
    else {
        AVLNode<Key, Value>* g = p->getParent();
				if(g->getLeft() == p) {
					          g->updateBalance(-1);
										if(g->getBalance() == 0) { // case 1
												return; 
										}
										else if(g->getBalance() == -1) { // case 2
												insert_fix(g, p);
										}
										else { // case 3
												// b(g) = -2
												if(g->getLeft() == p && p->getLeft() == n) {
													// zig-zig
													rotateRight(g);
													p->setBalance(0);
													g->setBalance(0);
													return;
												}
												else {
													// zig-zag
													rotateLeft(p);
													rotateRight(g);
													if(n->getBalance() == -1) {
														p->setBalance(0);
														g->setBalance(1);
														n->setBalance(0);
													}
													else if(n->getBalance() == 0) {
														p->setBalance(0);
														g->setBalance(0);
														n->setBalance(0);
													}
													else {
														// n->getBalance() == 1
														p->setBalance(-1);
														g->setBalance(0);
														n->setBalance(0);
													}
													return;
												}
										}
			  }
				else { // p is a right child 
					          g->updateBalance(1);
										if(g->getBalance() == 0) { // case 1
												return; 
										}
										else if(g->getBalance() == 1) { // case 2
												insert_fix(g, p);
										}
										else { // case 3
												// b(g) = 2
												if(g->getRight() == p && p->getRight() == n) {
													// zig-zig
													rotateLeft(g);
													p->setBalance(0);
													g->setBalance(0);
													return;
												}
												else {
													// zig-zag
													rotateRight(p);
													rotateLeft(g);
													if(n->getBalance() == 1) {
														p->setBalance(0);
														g->setBalance(-1);
														n->setBalance(0);
													}
													else if(n->getBalance() == 0) {
														p->setBalance(0);
														g->setBalance(0);
														n->setBalance(0);
													}
													else {
														// n->getBalance() == -1
														p->setBalance(1);
														g->setBalance(0);
														n->setBalance(0);
													}
													return;
												}
										}
				}
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove_fix(AVLNode<Key, Value>* n, int diff)
{
	int ndiff = 0;
	if(n == NULL) {
		return;
	}
	AVLNode<Key, Value>* p = n->getParent();
	if(p != NULL) {
		if(p->getLeft() == n) {
			ndiff = 1;
		}
		else {
			ndiff = -1;
		}
	}
	if(diff == -1) {
		        if(n->getBalance() + diff == -2) { // case 1 
							AVLNode<Key, Value>* c = n->getLeft();
							if(c->getBalance() == -1) {
								rotateRight(n);
								n->setBalance(0);
								c->setBalance(0);
								remove_fix(p, ndiff);
							}
							else if(c->getBalance() == 0) {
								rotateRight(n);
								n->setBalance(-1);
								c->setBalance(1);
								return;
							}
							else { // c->getBalance() = 1
								AVLNode<Key, Value>* g = c->getRight();
								rotateLeft(c);
								rotateRight(n);
								if(g->getBalance() == 1) {
									n->setBalance(0);
									c->setBalance(-1);
									g->setBalance(0);
								}
								else if(g->getBalance() == 0) {
									n->setBalance(0);
									c->setBalance(0);
									g->setBalance(0);
								}
								else { // g->getBalance() = -1
						      n->setBalance(1);
									c->setBalance(0);
									g->setBalance(0);
								}
								remove_fix(p, ndiff);
							}
						}
						else if(n->getBalance() + diff == -1) { // case 2 
						  n->setBalance(-1);
							return;
						}
						else { // case 3, n->getBalance() + diff = 0
						  n->setBalance(0);
							remove_fix(p, ndiff);
						}
	}
	else { // diff = 1
            if(n->getBalance() + diff == 2) { // case 1 
							AVLNode<Key, Value>* c = n->getRight();
							if(c->getBalance() == 1) {
								rotateLeft(n);
								n->setBalance(0);
								c->setBalance(0);
								remove_fix(p, ndiff);
							}
							else if(c->getBalance() == 0) {
								rotateLeft(n);
								n->setBalance(1);
								c->setBalance(-1);
								return;
							}
							else { // c->getBalance() = 1
								AVLNode<Key, Value>* g = c->getLeft();
								rotateRight(c);
								rotateLeft(n);
								if(g->getBalance() == -1) {
									n->setBalance(0);
									c->setBalance(1);
									g->setBalance(0);
								}
								else if(g->getBalance() == 0) {
									n->setBalance(0);
									c->setBalance(0);
									g->setBalance(0);
								}
								else { // g->getBalance() = 1
						      n->setBalance(-1);
									c->setBalance(0);
									g->setBalance(0);
								}
								remove_fix(p, ndiff);
							}
						}
						else if(n->getBalance() + diff == 1) { // case 2 
						  n->setBalance(1);
							return;
						}
						else { // case 3, n->getBalance() + diff = 0
						  n->setBalance(0);
							remove_fix(p, ndiff);
						}
	}
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* input)
{
	if(input->getRight() != NULL) {
		AVLNode<Key, Value>* temp = input->getRight();
		// check if temp->getLeft() exists and change its parent to avoid seg fault
		if(temp->getLeft() != NULL) {
			input->setRight(temp->getLeft());
		  temp->getLeft()->setParent(input);
		}
		else {
			input->setRight(NULL);
		}
		//--------------------
		temp->setLeft(input);
		if(input->getParent() == NULL) {
			temp->setParent(NULL);
			input->setParent(temp);
			this->root_ = temp;
		}
		else if(input->getParent()->getLeft() == input) {
			temp->setParent(input->getParent());
			input->getParent()->setLeft(temp);
			input->setParent(temp);
		}
		else { // input->getParent()->getRight() == input
			temp->setParent(input->getParent());
			input->getParent()->setRight(temp);
			input->setParent(temp);
		}
	}
	return;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* input)
{
	if(input->getLeft() != NULL) {
		AVLNode<Key, Value>* temp = input->getLeft();
		//--------------------
		if(temp->getRight() != NULL) {
			input->setLeft(temp->getRight());
			temp->getRight()->setParent(input);
		}
		else {
			input->setLeft(NULL);
		}
		//--------------------
		temp->setRight(input);
		if(input->getParent() == NULL) {
			temp->setParent(NULL);
			input->setParent(temp);
			this->root_ = temp;
		}
		else if(input->getParent()->getLeft() == input) {
			temp->setParent(input->getParent());
			input->getParent()->setLeft(temp);
			input->setParent(temp);
		}
		else { // input->getParent()->getRight() == input
			temp->setParent(input->getParent());
			input->getParent()->setRight(temp);
			input->setParent(temp);
		}
	}
	return;
}

// template<class Key, class Value>
// AVLNode<Key, Value>* AVLTree<Key, Value>::AVLFind(const Key& k) const
// {
// 	AVLNode<Key, Value>* temp = this->root_;
//     while(true) {
//         if(temp == NULL) {
//             // reached the leaf end of the tree and not yet found the node
//             return NULL;
//         }
//         else if(temp->getKey() == k) {
//             return temp;
//         }
//         else if(temp->getKey() < k) {
//             temp = temp->getRight();
//         }
//         else {
//             temp = temp->getLeft();
//         }
//     }
// }

#endif
