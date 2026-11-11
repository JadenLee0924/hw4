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
    void rotateLeft(AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* n);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int8_t diff);

};

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* c = n->getLeft();
    AVLNode<Key, Value>* p = n->getParent();
    AVLNode<Key, Value>* t = c->getRight();

    c->setParent(p);
    if(p == NULL) {
        this->root_ = c;
    }
    else if(p->getLeft() == n) {
        p->setLeft(c);
    }
    else {
        p->setRight(c);
    }

    c->setRight(n);
    n->setParent(c);

    n->setLeft(t);
    if(t != NULL) {
        t->setParent(n);
    }
}

/*
 * Mirror of rotateRight: n's RIGHT child moves up, n becomes its left child.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* c = n->getRight();
    AVLNode<Key, Value>* p = n->getParent();
    AVLNode<Key, Value>* t = c->getLeft();

    c->setParent(p);
    if(p == NULL) {
        this->root_ = c;
    }
    else if(p->getLeft() == n) {
        p->setLeft(c);
    }
    else {
        p->setRight(c);
    }

    c->setLeft(n);
    n->setParent(c);

    n->setRight(t);
    if(t != NULL) {
        t->setParent(n);
    }
}

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;
    bool isLeft = false;

    while(curr != NULL) {
        if(new_item.first == curr->getKey()) {
            curr->setValue(new_item.second);
            return;
        }
        parent = curr;
        if(new_item.first < curr->getKey()) {
            isLeft = true;
            curr = curr->getLeft();
        }
        else {
            isLeft = false;
            curr = curr->getRight();
        }
    }

    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if(isLeft) {
        parent->setLeft(n);
    }
    else {
        parent->setRight(n);
    }
    n->setBalance(0);

    // Parent was already leaning the other way: it is now perfectly balanced
    // and its height did not change, so nothing above it can be affected.
    if(parent->getBalance() == -1 || parent->getBalance() == 1) {
        parent->setBalance(0);
        return;
    }

    // Parent was balanced: it now leans, its height grew, so walk upward
    parent->updateBalance(isLeft ? -1 : 1);
    insertFix(parent, n);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if(p == NULL || p->getParent() == NULL) {
        return;
    }
    AVLNode<Key, Value>* g = p->getParent();

    if(g->getLeft() == p) {
        g->updateBalance(-1);

        if(g->getBalance() == 0) {
            return;
        }
        if(g->getBalance() == -1) {
            insertFix(g, p);
            return;
        }
        // g is now -2
        if(p->getLeft() == n) {             // zig-zig
            rotateRight(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {                              // zig-zag
            rotateLeft(p);
            rotateRight(g);
            if(n->getBalance() == -1) {
                p->setBalance(0);  g->setBalance(1);  n->setBalance(0);
            }
            else if(n->getBalance() == 0) {
                p->setBalance(0);  g->setBalance(0);  n->setBalance(0);
            }
            else {
                p->setBalance(-1); g->setBalance(0);  n->setBalance(0);
            }
        }
    }
    else {
        g->updateBalance(1);

        if(g->getBalance() == 0) {
            return;
        }
        if(g->getBalance() == 1) {
            insertFix(g, p);
            return;
        }
        // g is now +2
        if(p->getRight() == n) {            // zig-zig
            rotateLeft(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {                              // zig-zag
            rotateRight(p);
            rotateLeft(g);
            if(n->getBalance() == 1) {
                p->setBalance(0);  g->setBalance(-1); n->setBalance(0);
            }
            else if(n->getBalance() == 0) {
                p->setBalance(0);  g->setBalance(0);  n->setBalance(0);
            }
            else {
                p->setBalance(1);  g->setBalance(0);  n->setBalance(0);
            }
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
    AVLNode<Key, Value>* n =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(n == NULL) {
        return;
    }

    // Two children: swap with the PREDECESSOR, then n has at most one child
    if(n->getLeft() != NULL && n->getRight() != NULL) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    AVLNode<Key, Value>* p = n->getParent();
    int8_t diff = 0;
    if(p != NULL) {
        diff = (p->getLeft() == n) ? 1 : -1;
    }

    AVLNode<Key, Value>* child = (n->getLeft() != NULL) ? n->getLeft() : n->getRight();
    if(child != NULL) {
        child->setParent(p);
    }
    if(p == NULL) {
        this->root_ = child;
    }
    else if(p->getLeft() == n) {
        p->setLeft(child);
    }
    else {
        p->setRight(child);
    }
    delete n;

    removeFix(p, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int8_t diff)
{
    if(n == NULL) {
        return;
    }

    // Work out the next call's arguments BEFORE any rotation moves n
    AVLNode<Key, Value>* p = n->getParent();
    int8_t ndiff = 0;
    if(p != NULL) {
        ndiff = (p->getLeft() == n) ? 1 : -1;
    }

    if(diff == -1) {
        if(n->getBalance() + diff == -2) {
            AVLNode<Key, Value>* c = n->getLeft();      // taller child

            if(c->getBalance() == -1) {                 // zig-zig
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0) {             // zig-zig, height unchanged
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
                return;
            }
            else {                                      // zig-zag
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if(g->getBalance() == 1) {
                    n->setBalance(0);  c->setBalance(-1); g->setBalance(0);
                }
                else if(g->getBalance() == 0) {
                    n->setBalance(0);  c->setBalance(0);  g->setBalance(0);
                }
                else {
                    n->setBalance(1);  c->setBalance(0);  g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if(n->getBalance() + diff == -1) {
            n->setBalance(-1);
            return;
        }
        else {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if(diff == 1) {
        if(n->getBalance() + diff == 2) {
            AVLNode<Key, Value>* c = n->getRight();     // taller child

            if(c->getBalance() == 1) {                  // zig-zig
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0) {             // zig-zig, height unchanged
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
                return;
            }
            else {                                      // zig-zag
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if(g->getBalance() == -1) {
                    n->setBalance(0);  c->setBalance(1);  g->setBalance(0);
                }
                else if(g->getBalance() == 0) {
                    n->setBalance(0);  c->setBalance(0);  g->setBalance(0);
                }
                else {
                    n->setBalance(-1); c->setBalance(0);  g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if(n->getBalance() + diff == 1) {
            n->setBalance(1);
            return;
        }
        else {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
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


#endif
