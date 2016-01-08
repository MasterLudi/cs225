/**
 * @file avl_tree.h
 * Declaraction of the avl_tree class. You will probably need to modify this
 *  file to add helper functions.
 * @author Daniel Hoodin in Spring 2008.
 * Modified by Kyle Johnson in Fall 2010.
 * Modified by Sean Massung in Summer 2012:
 *  - added templates
 *  - CRTP tree printing
 *  - doxygen
 */

#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <memory>

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

/**
 * The avl_tree class represents a templated linked-memory tree data structure.
 */
template <class K, class V>
class avl_tree
{
  private:
    /**
     * Represents a tree node; that is, an element in a avl_tree.
     * It has a data element and pointers to its left and right children.
     */
    struct node
    {
        K key;
        V value;
        std::unique_ptr<node> left;
        std::unique_ptr<node> right;
        int64_t height;

        /**
         * node element constructor; sets children to point to NULL.
         * @param k The object to use as a key
         * @param v The templated data element that the constructed
         *  node will hold.
         */
        node(K k, V v)
            : key{std::move(k)}, value{std::move(v)}, height{0}
        {
            // nothing
        }
    };

  public:
    /**
     * Constructor to create an empty tree.
     */
    avl_tree();

    /**
     * Copy constructor.
     */
    avl_tree(const avl_tree& other);

    /**
     * Move constructor.
     */
    avl_tree(avl_tree&& other) = default;

    /**
     * Destructor; frees all nodes associated by this tree.
     */
    ~avl_tree() = default;

    /**
     * Assignment operator.
     * @param rhs The tree to make a copy of
     * @return A reference to the current tree
     */
    avl_tree& operator=(avl_tree rhs);

    /**
     * Swaps the current avl_tree with the parameter.
     * @param other The tree to swap with
     */
    void swap(avl_tree& other);

    /**
     * Inserts into the avl_tree.
     * @param key The key to insert
     * @param value The value for the key to insert
     */
    void insert(K key, V value);

    /**
     * Finds an element in the AVL tree.
     * @param key The element to search for
     * @return the value stored for that key
     */
    const V& find(const K& key) const;

    /**
     * Prints the avl_tree to a stream (default stdout).
     * @param out The stream to print to
     */
    void print(ostream& out = cout) const;

    /**
     * This is a function used for grading.
     * @param newOut The stream to print to.
     */
    void setOutput(ostream& newOut);

  private:
    std::unique_ptr<node> root_;

    /**
     * Private helper function for the public insert function.
     * @param subtree The current node in the recursion
     * @param key The key to insert
     * @param value The value for the key to insert
     */
    void insert(std::unique_ptr<node>& subtree, K key, V value);

    /**
     * Finds an element in the AVL tree.
     * @param subtree The node to search from (current subroot)
     * @param key The element to search for
     * @return the value stored for that key
     */
    const V& find(const node* node, const K& key) const;

    /**
     * Checks if a subtree needs rebalanced, and invokes the correct
     * helper functions to fix the imbalance, if one exists.
     */
    void rebalance(std::unique_ptr<node>& subroot);

    /**
     * Fixes an imbalance when the balance factor is positive.
     */
    void rebalance_left(std::unique_ptr<node>& subroot);

    /**
     * Fixes an imbalance when the balance factor is negative.
     */
    void rebalance_right(std::unique_ptr<node>& subroot);

    /**
     * Rotates the tree right (there is an imbalance on the left side).
     */
    void rotate_right(std::unique_ptr<node>& t);

    /**
     * Rotates the tree left (there is an imbalance on the right side).
     */
    void rotate_left(std::unique_ptr<node>& t);

    /**
     * A right left rotation.
     * This function should simply call rotateRight and rotateLeft.
     */
    void rotate_right_left(std::unique_ptr<node>& node);

    /**
     * A left right rotation.
     * This function should simply call rotateLeft and rotateRight.
     * @param node The node to rotate
     */
    void rotate_left_right(std::unique_ptr<node>& node);

    /**
     * @param node The node's height to check
     * @return the height of the node if it's non-NULL or -1 if it is NULL
     */
    int64_t heightOrNeg1(const node* node) const;

    /**
     * Helper function for operator= and cctor.
     * @param subRoot The current node in the recursion
     */
    std::unique_ptr<node> copy(const node* subRoot);

    /**
     * Private helper function for clear that clears beneath the parameter node.
     * @param subRoot The current node in the recursion
     */
    void clear(node* subRoot);

    /** member variable used for grading */
    ostream* _out;
};

#include "avl_tree_given.tcc"
#include "avl_tree.tcc"
#endif
